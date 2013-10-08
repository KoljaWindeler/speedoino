/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2011 Kolja Windeler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "global.h"

speedo_gear::speedo_gear(){
	faktor_counter=0;
	faktor_flat=0;
	last_time_executed=0;
	neutral_set=false;
	gang=-1;

	for(unsigned int j=0;j<sizeof(n_gang)/sizeof(n_gang[0]);j++){ // 0..6
		n_gang[j]=0;
	};
};

speedo_gear::~speedo_gear(){
};

void speedo_gear::init(){
	KUPPLUNGS_DIRC&=~(1<<KUPPLUNGS_PIN); //ensure that its defined as input, pinMode(kupplungs_pin,INPUT);
	KUPPLUNGS_PORT|=1<<KUPPLUNGS_PIN;// digitalWrite(kupplungs_pin,HIGH); // input mit pull up
	last_time_executed=millis();
}

int speedo_gear::check_vars(){
	if(n_gang[1]+n_gang[2]==0){
		// gaenge einlesen
		int temp[7]={160,120,90,73,60,53,45};
		for(unsigned int j=0;j<sizeof(temp)/sizeof(temp[0]);j++){ // 0..6
			n_gang[j]=temp[j];
		};
		pDebug->sprintp(PSTR("gear failed"));
		return 1;
	}
	return 0;
};



/* prozedure wird alle 250 ms aufgerufen und ermittelt den gang, packt ihn in ein tiefpass und stellt das ergebniss zur verfügung */
void speedo_gear::calc(){
	if(millis()-last_time_executed>250){
		last_time_executed=millis();

		// faktor berechnen
		if(pSensors->get_speed(true)>0){ // ich fahre also mit min 4 km/h .. dadurch wird der faktor max 15000/4=3500 .. das mal 16 ist noch im INT bereich
			if(!digitalRead(kupplungs_pin) && faktor_counter!=0){//Kupplung gezogen, einmalig laut geben was seit dem letzten mal so an min und max und flat ausgekommen ist
				faktor_counter=0; // damit nach dem kuppeln die alten werte verworfen werden
			} else if(digitalRead(kupplungs_pin)) { // Kupplung nicht gezogen, also gang berechnen
				int faktor=pSensors->get_RPM(RPM_TYPE_DIRECT)/pSensors->get_speed(true); // wie ist denn wohl der faktor
				// changed from 8 to 4, to reduce delay while calculation //1.8.2012
				faktor_flat=pSensors->flatIt(faktor*10,&faktor_counter,4,faktor_flat); // mal sehen ob man so eine art tiefpass faktor sinnvoll nutzen kann


				//n_gang[1]=1350 // übersetzung gang 1
				//n_gang[2]=900  // übersetzung gang 2
				//n_gang[3]=725  // übersetzung gang 3
				//n_gang[4]=600  // übersetzung gang 4
				//n_gang[5]=518  // übersetzung gang 5
				//n_gang[6]=442  // übersetzung gang 6

				if(faktor_flat>=n_gang[6]*0.9 && faktor_flat<=n_gang[1]*1.3){ // 1755 - 397
					unsigned int abstand=n_gang[1]; // 1350
					for(int a=1; a<=6; a++){
						if( abs(faktor_flat-n_gang[a]) < abstand ){
							abstand=abs(faktor_flat-n_gang[a]);
							gang=a;
						}
					}
				};
			}
		} else { // wenn ich gar nicht fahre ..
			gang=-1;  // hier 0 einsetzen für "N" bei 0 km/h oder -1 für keine Ausgabe bei 0 km/h
		};
	};
}

int speedo_gear::get(){
	if(neutral_set){
		return 0; // show as "N" in speedo
	} else if(gang>=-1 && gang<=6){
		return gang; // show as 1-6 in speedo
	}
	return -1; // clears output -> failsave
}

void speedo_gear::set_neutral(bool is_neutral){
	neutral_set=is_neutral;
}

void speedo_gear::calibrate(){
	if(pSpeedo->disp_zeile_bak[0]!=2){ // just once
		pSpeedo->disp_zeile_bak[0]=2;

		// title bar
		pOLED->highlight_bar(0,0,128,8);
		pOLED->string_P(pSpeedo->default_font,PSTR("Gear"),5,0,15,0,0);
		char temp[6];
		sprintf(temp,"%i",int(floor(pMenu->state/10))%10);
		pOLED->string(pSpeedo->default_font,temp,10,0,15,0,0);

		// draw old value
		pOLED->string_P(pSpeedo->default_font,PSTR("Old Value:"),0,3);
		sprintf(temp,"%4i",pSensors->m_gear->n_gang[int(pSensors->m_gps->mod(pMenu->state,100))/10]);
		pOLED->string(pSpeedo->default_font,temp,11,3);

		// some text
		pOLED->string_P(pSpeedo->default_font,PSTR("Dont shift!! Drive"),0,5);
		pOLED->string_P(pSpeedo->default_font,PSTR("faster and slower"),0,6);
		pOLED->string_P(pSpeedo->default_font,PSTR("~ cancel        save"),0,7);
		sprintf(temp,"%c",127);
		pOLED->string(pSpeedo->default_font,temp,14,7);
	}

	_delay_ms(150); // 128 values for LP, 150ms per Value + calc time ~> 22sec
	faktor_flat=pSensors->flatIt(int((unsigned long)(10*pSensors->get_RPM(RPM_TYPE_DIRECT))/pSensors->get_speed(true)),&faktor_counter,127,faktor_flat);
	if(faktor_flat!=pSpeedo->disp_zeile_bak[1]){
		pSpeedo->disp_zeile_bak[1]=faktor_flat;

		// draw it
		pOLED->string_P(pSpeedo->default_font,PSTR("Value:"),0,2);
		char temp[5];
		sprintf(temp,"%4i",int(faktor_flat));
		pOLED->string(pSpeedo->default_font,temp,11,2);
	}
};

