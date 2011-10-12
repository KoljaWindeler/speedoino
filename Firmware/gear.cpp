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
};

speedo_gear::~speedo_gear(){
};

void speedo_gear::init(){
	pinMode(kupplungs_pin,INPUT);
	digitalWrite(kupplungs_pin,HIGH); // input mit pull up
	faktor_counter=0;
	gang=-1;
}

/* prozedure wird alle 250 ms aufgerufen und ermittelt den gang, packt ihn in ein tiefpass und stellt das ergebniss zur verfügung */
void speedo_gear::calc(){
	// faktor berechnen
	if(pSensors->m_speed->get_mag_speed()>0){ // ich fahre also mit min 4 km/h .. dadurch wird der faktor max 15000/4=3500 .. das mal 16 ist noch im INT bereich
		if(!digitalRead(kupplungs_pin) && faktor_counter!=0){//Kupplung gezogen, einmalig laut geben was seit dem letzten mal so an min und max und flat ausgekommen ist
			faktor_counter=0; // damit nach dem kuppeln die alten werte verworfen werden
		} else if(digitalRead(kupplungs_pin)) { // Kupplung nicht gezogen, also gang berechnen
			int faktor=pSensors->m_dz->exact/pSensors->m_speed->get_mag_speed(); // wie ist denn wohl der faktor
			faktor_flat=pSensors->flatIt(faktor*10,&faktor_counter,8,faktor_flat); // mal sehen ob man so eine art tiefpass faktor sinnvoll nutzen kann


			//n_gang[1]=1350 // übersetzung gang 1
			//n_gang[2]=900  // übersetzung gang 2
			//n_gang[3]=725  // übersetzung gang 3
			//n_gang[4]=600  // übersetzung gang 4
			//n_gang[5]=518  // übersetzung gang 5
			//n_gang[6]=442  // übersetzung gang 6

			if(faktor_flat>=n_gang[6]*0.9 && faktor_flat<=n_gang[1]*1.3){ // 1755 - 397
				unsigned int abstand=n_gang[0]; // 1350
				for(int a=1; a<=6; a++){
					if( abs(faktor_flat-n_gang[a]) < abstand ){
						abstand=abs(faktor_flat-n_gang[a]);
						gang=a;
					}
				}
			};

			//			char a_faktor[5]; // XXXX[\0]
			//			sprintf(a_faktor,"%04i",faktor_flat%10000);
			//			pOLED->string(STD_SMALL_1X_FONT,a_faktor,17,1,0,DISP_BRIGHTNESS,0);

			//			if(faktor_flat>=n_gang[6] && faktor_flat<=n_gang[0]){ // faktor 		// 1700 - 0400 //
			//				if     (faktor_flat<n_gang[0] && faktor_flat>=n_gang[1]){  gang=1; }// 1700 - 1000 // 1350
			//				else if(faktor_flat<n_gang[1] && faktor_flat>=n_gang[2]){  gang=2; }// 1000 - 0800 // 900
			//				else if(faktor_flat<n_gang[2] && faktor_flat>=n_gang[3]){  gang=3; }// 0800 - 0650 // 725
			//				else if(faktor_flat<n_gang[3] && faktor_flat>=n_gang[4]){  gang=4; }// 0650 - 0550 // 600
			//				else if(faktor_flat<n_gang[4] && faktor_flat>=n_gang[5]){  gang=5; }// 0550 - 0485 // 518
			//				else if(faktor_flat<n_gang[5] && faktor_flat>=n_gang[6]){  gang=6; }// 0485 - 0400 // 442
			//			};

		}
	} else { // wenn ich gar nicht fahre ..
		gang=-1;  // hier 0 einsetzen für "N" bei 0 km/h oder -1 für keine Ausgabe bei 0 km/h
	};
}

int speedo_gear::get(){
	if(gang>=-1 && gang<=6)
		return gang;
	else
		return  -1;
}

void speedo_gear::calibrate(){
	if(pSpeedo->disp_zeile_bak[0]!=2){ // just once
		pSpeedo->disp_zeile_bak[0]=2;

		// title bar
		pOLED->highlight_bar(0,0,128,8);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Gear"),5,0,15,0,0);
		char temp[6];
		sprintf(temp,"%i",int(floor(pMenu->state/10))%10);
		pOLED->string(STD_SMALL_1X_FONT,temp,10,0,15,0,0);

		// draw old value
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Old Value:"),0,3);
		sprintf(temp,"%4i",pSensors->m_gear->n_gang[int(pSensors->m_gps->mod(pMenu->state,100))/10]);
		pOLED->string(STD_SMALL_1X_FONT,temp,11,3);

		// some text
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Dont shift!! Drive"),0,5);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("faster and slower"),0,6);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("~ cancel        save"),0,7);
		sprintf(temp,"%c",127);
		pOLED->string(STD_SMALL_1X_FONT,temp,14,7);
	}

	_delay_ms(150);
	pSensors->m_dz->calc(); // erst berechnen dann damit weiter rechnen
	faktor_flat=pSensors->flatIt(int((unsigned long)(10*pSensors->m_dz->exact)/pSensors->m_speed->get_mag_speed()),&faktor_counter,128,faktor_flat);
	if(faktor_flat!=pSpeedo->disp_zeile_bak[1]){
		pSpeedo->disp_zeile_bak[1]=faktor_flat;

		// draw it
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Value:"),0,2);
		char temp[5];
		sprintf(temp,"%4i",int(faktor_flat));
		pOLED->string(STD_SMALL_1X_FONT,temp,11,2);
	}
};

