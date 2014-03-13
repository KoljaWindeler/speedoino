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
/*
 * debug.cpp
 *
 *  Created on: 02.06.2011
 *      Author: kolja
 */

debugging::debugging(){

}

debugging::~debugging(){

}

void debugging::sprintp(const char *data){
	while(pgm_read_byte(data) != 0x00)
		Serial.print(pgm_read_byte(data++));
}

void debugging::sprintlnp(const char *data){
	while(pgm_read_byte(data) != 0x00)
		Serial.print(pgm_read_byte(data++));
	Serial.println();
}

void debugging::parse_float(int state,char buffer[],int i,int j,int k){
#ifdef PARSE_DEBUG
	if(state==0){
		Serial.print("parse float gestartet mit buffer: ");
		Serial.println(buffer);
	} else if(state==1){
		Serial.print("aktuelles zeichen:");
		Serial.println(buffer[i]);
	} else if(state==2){
		Serial.println("vorzeichen auf - gestellt");
	} else if(state==3){
		Serial.print("twert:");
		Serial.print(i);
		Serial.print(" decade_counter:");
		Serial.print(j);
		Serial.print(" vz:");
		Serial.print(k);
	} else if(state==4){
		Serial.print("(float)");
		Serial.print(i);
	}
#endif
}


void debugging::parse_int(){
#ifdef PARSE_DEBUG
	Serial.println("<--as int");
#endif
};

void debugging::parse(int state,char buffer[50]){
#ifdef PARSE_DEBUG
	if(state==0){
		Serial.print("config parse: "); Serial.println(buffer);
	}
#endif
}

void debugging::parse_a(int state,char buffer[]){
#ifdef PARSE_DEBUG
	if(state==0){
		Serial.print("(char[])"); Serial.println(buffer);
	}
#endif
}

void debugging::parse_ul(int state,unsigned long wert){
#ifdef PARSE_DEBUG
	if(state==0){
		Serial.print("(unsigned long)"); Serial.println(wert);
	}
#endif
}

void debugging::speedo_loop(int state,int intensive,unsigned long previousMillis,const char char_buffer[]){
#ifdef TACHO_DEBUG
	if(intensive==0){
#elseif TACHO_IDEBUG
	if(intensive==1){
#else
	if(false){
#endif
		int differ_show;
		if(state==0){
			sprintlnp(PSTR("Normaler Tacho Durchlauf"));
		} else if(state==1){
			sprintlnp(PSTR("Schreibe Oeltemp"));
		} else if(state==2){
			sprintlnp(PSTR("Schreibe Airtemp"));
		} else if(state==3){
			sprintlnp(PSTR("Schreibe 'km/h'"));
		} else if(state==4){
			sprintlnp(PSTR("Schreibe Speed"));
		} else if(state==5){
			sprintlnp(PSTR("Schreibe drehzahl"));
		} else if(state==6){
			sprintlnp(PSTR("Schreibe Drehzahl"));
		} else if(state==7){
			sprintlnp(PSTR("Schreibe nen Titel wie Q3 alleine"));
		} else if(state==8){
			sprintlnp(PSTR("Schreibe Average + Max"));
		} else if(state==9){
			sprintlnp(PSTR("Schreibe zeiten"));
		} else if(state==10){
			sprintlnp(PSTR("Schreibe Motorwarnung"));
		} else if(state==11){
			sprintlnp(PSTR("Schreibe Blinkerwarnung"));
		} else if(state==12){
			sprintlnp(PSTR("Schreibe Sicherungswarnung"));
		} else if(state==13){
			sprintlnp(PSTR("Schreibe Navigationsanweisung"));
		} else if(state==14){
			sprintlnp(PSTR("Schreibe SD warnung"));
		} else if(state==15){
			sprintlnp(PSTR("Lösche Warnung"));
		} else if(state==16){
			sprintlnp(PSTR("Schreibe trip. Sowas wie day 123.4 km"));
		} else if(state==17){
			sprintlnp(PSTR("Schreibe Clock: "));
			Serial.println(char_buffer);
		} else if(state==18){
			sprintlnp(PSTR("Schreibe Gang"));
		} else if(state==19){
			sprintp(PSTR("Schreibe Fuel:"));
			Serial.println(char_buffer);
		} else if(state==20){
			differ_show=millis()-previousMillis;
			sprintp(PSTR("Normeler Tacho refresh dauerte "));
			Serial.print(differ_show);
			sprintp(PSTR(" ms"));
		} else if(state==21){
			sprintp(PSTR("Loop Durchlauf, menustate ist "));
//			Serial.print(pMenu->state);
			sprintp(PSTR("\n"));
		} else if(state==22){
			sprintlnp(PSTR("Oel fertig"));
		} else if(state==23){
			sprintlnp(PSTR("Air fertig"));
		}
	}
}

void debugging::loop(){
#ifdef TACHO_SDEBUG
		Serial.print("State changed:");
		Serial.print(pMenu->state);
		sprintlnp(PSTR(": Calling menu_disp();"));
#endif
};
