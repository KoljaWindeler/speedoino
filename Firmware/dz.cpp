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
	unsigned long now=millis(); 											// aktuelle zeit
	unsigned long differ=now-previous_time; 								// zeit seit dem letzte mal abholen der daten
	unsigned int  now_peaks=peak_count; 									// aktueller dz zähler stand, separate var damit der peakcount weiter verndert werden koennte
	if(pAktors->m_stepper->init_steps_to_go!=0){
		if(pAktors->m_stepper->init_steps_to_go>=5){
			if(pAktors->m_stepper->get_pos()!=0){		//
				pAktors->m_stepper->go_to(0,FAST_ACCEL,FAST_SPEED);			// 240*8 = 1920
			} else {
				pAktors->m_stepper->init_steps_to_go=4; 					// nächsten schritt vorbereiten
			}
		} else if(pAktors->m_stepper->init_steps_to_go==4){
			if(pAktors->m_stepper->get_pos()!=MOTOR_OVERWRITE_END_POS){		// motor noch nicht am ende angekommen
				pAktors->m_stepper->go_to(MOTOR_OVERWRITE_END_POS,FAST_ACCEL,FAST_SPEED);// weiter dorthin scheuchen
			} else { 														// motor angekommen
				pAktors->m_stepper->init_steps_to_go=3; 					// nächsten schritt vorbereiten
			}
		} else if(pAktors->m_stepper->init_steps_to_go==3){
			if(pAktors->m_stepper->get_pos()!=0){   						// motor noch nicht am anfang angekommen
				pAktors->m_stepper->go_to(0,FAST_ACCEL,FAST_SPEED);			// weiter dorthin scheuchen
			} else { 														// motor angekommen
				pAktors->m_stepper->init_steps_to_go=2; 					// fertig
			}
		} else if(pAktors->m_stepper->init_steps_to_go==2){
			if(pAktors->m_stepper->get_pos()!=80){   						// motor noch nicht am anfang angekommen
				pAktors->m_stepper->overwrite_pos(80);
			} else { 														// motor angekommen
				pAktors->m_stepper->init_steps_to_go=1; 					// fertig
			}
		} else if(pAktors->m_stepper->init_steps_to_go==1){
			if(pAktors->m_stepper->get_pos()!=0){   						// motor noch nicht am anfang angekommen
				pAktors->m_stepper->go_to(0,80,200);
			} else { 														// motor angekommen
				pAktors->m_stepper->init_steps_to_go=0; 					// fertig
			}
		}
	} else if(DEMO_MODE){
		if(differ>112){
			previous_time=now;

			int demo_mode=2;

			if(demo_mode==1){
				int temp=analogRead(OIL_TEMP_PIN)-180;
				if(temp<0) temp=0;
				if (temp>600) temp=600;
				rounded=15000-temp*25;
				exact=rounded;
				rounded=exact_disp;
			} else if(demo_mode==2){
				if(int(floor(millis()/1000))%10<=5){
				int dz=random(15000);
				exact=pSensors->flatIt(dz,&dz_faktor_counter,30,exact);
				exact_disp=exact;
				rounded=exact_disp;
				};
			} else if(demo_mode==3){
				int speed_me_up=50; // gut mit 50
				if(((millis()/speed_me_up)%210)<50){
					rounded=0;
				} else {
					rounded=((millis()/speed_me_up)%210)*70;
				};
			};
			pSensors->m_gear->calc();
			//pAktors->m_stepper->go_to(exact/11.73);
			pAktors->m_stepper->go_to(exact/11.5);
		}
	} else if(now_peaks>4 && differ>25){ 				// max mit 10Hz, bei niedriger drehzahl noch seltener, 1400 rpm => 680 ms
		// am 17.8. von 50 auf 25 geändert ... das sind jetzt 40 Hz, mal sehen ob das noch klappt
		// am 3.8. von 100 auf 50 geändert
		// bei 4.000 rpm => 66 pps => 15ms Pro Peak, 4 Peaks in 60 ms
		// somit kann man mit 50 statt 100 bis 4800 doppelt so schnell reagieren,
		// mit 100ms war das sehr träge, mal sehen ob das die COM schnittstelle hergibt
		//now_peaks=now_peaks>>anzahl_shift;								// könnte ja sein das man weniger umdrehungen als funken hat, hornet hat 2 Funken je Umdrehun
		// die maximale übertragungsrate zwischen ATm8 und ATm2560 sollte nicht überschritten werden
		// pro Übertragung werden benötigt: 19200 Baud, 2400 Byte/sek, 7 Byte, 2,916667 ms, machen wir mal 10 ms draus.
		// bei 15.000 U/min => 250 U/sec
		// da bei der Hornet 2 Zündungen pro Umdrehung vorkommen
		// 500 Pulse / sec => 2ms zwischen 2 Pulsen, 5 Pulse in 10 ms
		//
		// bei 1.300 U/min => 21,6 U/min
		// 43 Pulse / sec => 23ms zwischen 2 Pulsen, 5 Pulse in 116,27ms

		unsigned int dz=60000/differ*now_peaks/2; // Drehzahl berechnet (60.000 weil ms => min)
		if(dz>15000){ // wenn man über 15000 U/min => Abstand von 2 Zündungen = 60000[ms/min]/15000[U/min] = 4 [ms/U]
			dz=previous_dz; // alten Wert halten, kann nicht sein
		};

		/* Timing */
		peak_count=0;
		previous_time=now; // speichere wann ich zuletzt nachgesehen habe
		previous_dz=dz; // speichere die aktuelle dz
		/* Timing */

		/* values */
		//exact=dz;
		exact=pSensors->flatIt(dz,&dz_faktor_counter,4,exact);						// IIR mit Rückführungsfaktor 1 für DZmotor
		exact_disp=pSensors->flatIt(dz,&dz_disp_faktor_counter,10,exact_disp);		// IIR mit Rückführungsfaktor 9 für Anzeige, 20*4 Pulse, 1400U/min = 2,5 sec | 14000U/min = 0,25 sec
		rounded=50*round(exact_disp/50); 											// auf 250er Runden
		/* values */

		/* gear */
		pSensors->m_gear->calc();// blockt intern alle aufrufe die vor ablauf von 250 ms kommen
		/* gear */

		/*stepper*/
		pAktors->m_stepper->go_to(exact/11.73); // einfach mal probieren, sonst flatit
		/*stepper*/
	} else if(now-previous_time>500){
		rounded=0;
		exact=0;
		// zeiger
		pAktors->m_stepper->go_to(0);
		previous_time=now;
	};
};

ISR(INT4_vect){
	pSensors->m_dz->counter();
}


void speedo_dz::init() {
	EIMSK |= (1<<INT4); // Enable Interrupt
	EICRB |= (1<<ISC40) | (1<<ISC41); // rising edge on INT4


	Serial.println("DZ init done");
	blitz_en=false;
	Serial3.flush();
};

void speedo_dz::clear_vars(){
	previous_time=millis();
	rounded=0;                 // to show on display, rounded by 50
	exact=0;                 // real rotation speed
	peak_count=0;
	blitz_dz=0;
	blitz_en=false;
	dz_faktor_counter=0;
	dz_disp_faktor_counter=0;
}

bool speedo_dz::check_vars(){
	if(blitz_dz==0){
		pDebug->sprintp(PSTR("DZ failed"));
		blitz_dz=12500; // hornet maessig
		blitz_en=true; // gehen wir mal von "an" aus
		return true;
	}
	return false;
};

