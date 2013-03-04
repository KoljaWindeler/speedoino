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

speedo_sprint::speedo_sprint(void){
	init();
}

speedo_sprint::~speedo_sprint(){
}

void speedo_sprint::init(){
	done=false;                              // der sprint ist noch nicht fertig
	lock=false;                              // ist der sprint am laufen
	blink_show=false;                        // anzeigen oder nicht
	previousMillis=millis();
}

void speedo_sprint::loop(){
	int speed=pSensors->get_speed(true);
	if(!lock){ // ich bin der Meinung das es noch gar nicht losging
		start=millis();
		done=false;
		if(speed>0){ // aber jetzt sollte es losgehen
			lock=true;
		};
	}
	else if(!done){ // ich bin unterwegs, aber noch nicht fertig
		if(speed==0){ // reset
			lock=false;
		}
		else if(speed>=100){ // geschafft
			done=true;
			end=millis();
		};
	};
	boolean refresh=false;
	if(!done && lock && (millis() - previousMillis > (refresh_interval/3)) ){ // wenn ich noch voll dabei bin alle 200 ms
		refresh=true;
	} else if((millis() - previousMillis > refresh_interval)){ // sonst alle 600 ms
		refresh=true;
	};
	if(refresh){
		char *char_buffer;
		char_buffer = (char*) malloc (22);
		if (char_buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));

		// geschwindigkeit
		if(pSpeedo->disp_zeile_bak[0]==-99){ // wenn es gerade reseted wurde -> alles hinmalen
			sprintf(char_buffer,"%3i km/h",speed);
			pOLED->string(pSpeedo->default_font+1,char_buffer,2,3,0,DISP_BRIGHTNESS,0);
		}   else if(pSpeedo->disp_zeile_bak[0]!=speed+1){ // wenn er anders ist als beim letzen refresh -> nur Zahl schreiben
			sprintf(char_buffer,"%3i",speed);
			pOLED->string(pSpeedo->default_font+1,char_buffer,2,3,0,DISP_BRIGHTNESS,0);
		};
		pSpeedo->disp_zeile_bak[0]=int(speed+1); // zustand sichern


		// drehzahl
		pSensors->m_dz->calc();
		if(pSpeedo->disp_zeile_bak[2]==-99){ // wenn es gerade reseted wurde -> alles hinmalen
			sprintf(char_buffer,"%5u U/min",pSensors->get_RPM(false));
			pOLED->string(pSpeedo->default_font,char_buffer,5,0,0,DISP_BRIGHTNESS,0);
		} else if (pSpeedo->disp_zeile_bak[2]!=signed(pSensors->get_RPM(false)+1)){ // wenn er anders ist als beim letzen refresh -> nur Zahl schreiben
			sprintf(char_buffer,"%5u",pSensors->get_RPM(false));
			pOLED->string(pSpeedo->default_font,char_buffer,5,0,0,DISP_BRIGHTNESS,0);
		};
		pSpeedo->disp_zeile_bak[2]=int(pSensors->get_RPM(false)+1); // zustand sichern


		// zeit
		unsigned long sprint_time;
		if(done){ // bin fertig mit dem sprint
			blink_show=!blink_show;
			sprintf(char_buffer," ");
			sprint_time=end - start;
		}
		else if(lock){ // ich bin noch dabei
			sprint_time=millis()-start;
			blink_show=true;
		}
		else { // ich steh noch sinnfrei rum
			sprint_time = 0;
			blink_show=true;
		};

		if(pSpeedo->disp_zeile_bak[1]==-99){ // alles hinschreiben
			pSpeedo->disp_zeile_bak[1]=1; // 1 ist nicht 0 und nicht sprint_time+1 => 2. schleife wird gestartet ...
			sprintf(char_buffer,"%02i,%03i sec",(int)floor(sprint_time/1000),(int)floor((sprint_time%1000)));
			pOLED->string(pSpeedo->default_font,char_buffer,6,6,0,DISP_BRIGHTNESS,0);
		} else if(pSpeedo->disp_zeile_bak[1]!=signed(sprint_time+1)){
			pSpeedo->disp_zeile_bak[1]=int(sprint_time+1);
			sprintf(char_buffer,"%02i,%03i",(int)floor(sprint_time/1000),(int)floor((sprint_time%1000)));
			if(blink_show){
				pOLED->string(pSpeedo->default_font,char_buffer,6,6,0,DISP_BRIGHTNESS,0);
			} else {
				pOLED->string(pSpeedo->default_font,"          ",6,6,0,DISP_BRIGHTNESS,0);
				pSpeedo->disp_zeile_bak[1]=-99;
			};
		};
		// sichere zeitstempel

		free(char_buffer);
		previousMillis=millis();
	};
};
