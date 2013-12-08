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

sprint::sprint(void){
	status=SPRINT_WAIT_ON_STOP;
	blink_show=false;                        // anzeigen oder nicht
	previousMillis=Millis.get();
}

sprint::~sprint(){
}


void sprint::prepare_startup(){
	if(Sensors.get_speed(true)>0){ //mag speed if possible
		status=SPRINT_WAIT_ON_STOP;
	} else {
		status=SPRINT_READY_TO_GO;
	}
}

void sprint::loop(){
	int speed=Sensors.get_speed(true);

	// Change status
	if(status==SPRINT_READY_TO_GO){
		if(speed>0){ // aber jetzt sollte es losgehen
			status=SPRINT_ACCEL;
			start=Millis.get();
		};
	} else if(status==SPRINT_WAIT_ON_STOP){ // ich bin der Meinung das es noch gar nicht losging
		start=Millis.get();
		if(speed==0){ // aber jetzt sollte es losgehen
			status=SPRINT_READY_TO_GO;
		};
	} else if(status==SPRINT_ACCEL){ // ich bin unterwegs, aber noch nicht fertig
		if(speed==0){ // reset
			status=SPRINT_READY_TO_GO;
		}
		else if(speed>=100){ // geschafft
			status=SPRINT_DONE;
			end=Millis.get();
		};
	};

	// calculate display refresh
	boolean refresh=false;
	if(status==SPRINT_ACCEL && (Millis.get() - previousMillis > (REFRESH_INTERVAL/3)) ){ // wenn ich noch voll dabei bin alle 200 ms
		refresh=true;
	} else if((Millis.get() - previousMillis > REFRESH_INTERVAL)){ // sonst alle 600 ms
		refresh=true;
	};

	// refresh display if needed
	if(refresh){
		char *char_buffer;
		char_buffer = (char*) malloc (22);
		if (char_buffer==NULL) Serial.puts_ln(USART1,("Malloc failed"));

		// geschwindigkeit
		if(Speedo.disp_zeile_bak[0]==-99){ // wenn es gerade reseted wurde -> alles hinmalen
			sprintf(char_buffer,"%3i km/h",speed);
			TFT.string(Speedo.default_font+1,char_buffer,2,3,0,DISP_BRIGHTNESS,0);

			for(int i=0;i<6;i+=3){
				bool override=true;
				int color=120;
				int j=i;
				int h=i;
				if(i>0){
					override=false;
					color=220;
					j++;
					h--;
				}

				TFT.draw_filled_rect( 10-i,100-j,45,45,color-80,0,0);
				TFT.draw_filled_rect( 40-i,155-j,40,40,color-75,0,0);
				TFT.draw_filled_rect( 90-i,180-j,30,30,color-70,0,0);
				TFT.draw_filled_rect(130-i,190-j,20,20,color-60,0,0);
				TFT.draw_filled_rect(160-i,200-j,16,16,color-40,0,0);
				TFT.draw_filled_rect(185-i,205-j,16,16,color-20,0,0);

				TFT.draw_arrow(90,205-h,200-j,color,0,0,override);
			}


		}   else if(Speedo.disp_zeile_bak[0]!=speed+1){ // wenn er anders ist als beim letzen refresh -> nur Zahl schreiben
			sprintf(char_buffer,"%3i",speed);
			TFT.string(Speedo.default_font+1,char_buffer,2,3,0,DISP_BRIGHTNESS,0);
		};
		Speedo.disp_zeile_bak[0]=int(speed+1); // zustand sichern


		// drehzahl
		if(Speedo.disp_zeile_bak[2]==-99){ // wenn es gerade reseted wurde -> alles hinmalen
			sprintf(char_buffer,"%5u U/min",Sensors.get_RPM(RPM_TYPE_DIRECT));
			TFT.string(Speedo.default_font,char_buffer,5,0,0,DISP_BRIGHTNESS,0);
		} else if (Speedo.disp_zeile_bak[2]!=signed(Sensors.get_RPM(RPM_TYPE_DIRECT)+1)){ // wenn er anders ist als beim letzen refresh -> nur Zahl schreiben
			sprintf(char_buffer,"%5u",Sensors.get_RPM(RPM_TYPE_DIRECT));
			TFT.string(Speedo.default_font,char_buffer,5,0,0,DISP_BRIGHTNESS,0);
		};
		Speedo.disp_zeile_bak[2]=int(Sensors.get_RPM(RPM_TYPE_DIRECT)+1); // zustand sichern


		// zeit
		unsigned long sprint_time;
		if(status==SPRINT_DONE){ // bin fertig mit dem sprint
			blink_show=!blink_show; //toggle
			sprint_time=end - start;
		}
		else if(status==SPRINT_ACCEL){ // ich bin noch dabei
			sprint_time=Millis.get()-start;
			blink_show=true;
		}
		else { // ich steh noch sinnfrei rum
			blink_show=true;
			sprint_time=0;
		};

		if(Speedo.disp_zeile_bak[1]!=signed(sprint_time+1+status+int(blink_show))){
			Speedo.disp_zeile_bak[1]=int(sprint_time+1+status+int(blink_show));
			if(blink_show){
				if(status==SPRINT_WAIT_ON_STOP){
					strcpy(char_buffer, ("Wait on stop"));
				} else if(status==SPRINT_READY_TO_GO){
					strcpy(char_buffer, ("READY"));
				} else if(status==SPRINT_ACCEL || status==SPRINT_DONE){
					sprintf(char_buffer,"%02i,%03i sec",(int)floor(sprint_time/1000),(int)floor((sprint_time%1000)));
				}
			} else {
				strcpy(char_buffer, ("          "));
			}
			Menu.center_me(char_buffer,22);
			Serial.puts_ln(USART1,char_buffer);
			TFT.string(Speedo.default_font,char_buffer,0,6,0,DISP_BRIGHTNESS,0);

		};
		// sichere zeitstempel

		free(char_buffer);
		previousMillis=Millis.get();
	};
};
