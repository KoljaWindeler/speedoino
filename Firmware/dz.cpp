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

speedo_dz::speedo_dz(void){
}

void speedo_dz::counter(){
	if(DZ_DEBUG){
		Serial.print("DZ peak@");
		Serial.print(millis());
		Serial.print(" ");
		Serial.println(peak_count);
	};
	peak_count++; // läuft bis 65.536 dann auf 0
};

void speedo_dz::calc() {
	///// DZ BERECHNUNG ////////
	unsigned long now=millis(); 		// aktuelle zeit
	unsigned long differ=now-previous_time; // zeit seit dem letzte mal abholen der daten
	unsigned int  now_peaks=peak_count; // aktueller dz zähler stand, separate var damit der peakcount weiter verndert werden koennte
	if(now_peaks>15 && differ>250){ // max mit 10Hz, bei niedriger drehzahl noch seltener, 1400 rpm => 680 ms
		//now_peaks=now_peaks>>anzahl_shift;			// könnte ja sein das man weniger umdrehungen als funken hat, hornet hat 2 Funken je Umdrehun
		/* bei 15krpm = 25 peaks
		 * differ => 100 --> 60000/100=600
		 * 600 * 25 => 15.000
		 *
		 * bei 1400rpm => 685 ms fr 16 Peaks
		 * 60000/685 = 88
		 * 88 * 16 = 1408
		 */
		unsigned int dz=60000/differ*now_peaks/2; // Drehzahl berechnet (60.000 weil ms => min)
		if(dz>15000){ // wenn man über 15000 U/min => Abstand von 2 Zündungen = 60000[ms/min]/15000[U/min] = 4 [ms/U]
			dz=previous_dz; // alten Wert halten, kann nicht sein
		};

		peak_count=0;
		previous_time=now; // speichere wann ich zuletzt nachgesehen habe
		previous_dz=dz; // speichere die aktuelle dz

		exact=dz;
		rounded=250*round(dz/250); // auf 25er Runden
		///// DZ BERECHNUNG ////////

		//// schalt"blitz"
		if(exact>11000 && exact<12000 && blitz_en){ // zwischen 11 und 12 k blinken
			if(millis()-pOLED->disp_last_invert>100){ // maximal mit 10 hz blinken
				pOLED->disp_last_invert=millis();
				if(pOLED->disp_invert){
					pOLED->send_command(0XA4); // toggle to standart
					pOLED->disp_invert=false;
				} else {
					pOLED->send_command(0xA7); // toggle to inverted
					pOLED->disp_invert=true;
				};
			};
		} else if(pOLED->disp_invert){ // danach nur checken das es nicht gerade beim invertierten zustand stehen geblieben ist
			pOLED->send_command(0XA4);
			pOLED->disp_invert=false;
		};

		pSensors->m_gear->calc();// alle 250 ms, also mit 4Hz den Gang berechnen
	} else if(now-previous_time>1000){
		rounded=0;
		exact=0;
	};
	if(DEMO_MODE){ rounded=((millis()/300)%260)*70;   exact=rounded; pSensors->m_gear->calc(); };
	int RxPtr=0;
	char RxBuffer[10];
	while(Serial3.available()>0 && RxPtr<8){
		RxBuffer[RxPtr++]=Serial3.read();
	}
	if(strncmp(RxBuffer,"$k*",3)){ // TODO
		Serial3.print("$m");
		Serial3.print(exact);
		Serial3.print("*");
	}
};

void helper(){
	pSensors->m_dz->counter();
}


void speedo_dz::init() {
	attachInterrupt(0, helper, RISING ); // interrupt handler für signalwechsel 0=DigiPin 2
	previous_time=millis();
	rounded=0;                 // to show on display, rounded by 50
	exact=0;                 // real rotation speed
	peak_count=0;
	Serial.println("DZ init done");
	blitz_en=false;
};
