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
	if(PARSE_DEBUG){
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
	}
}


void debugging::parse_int(){
	if(PARSE_DEBUG){
		Serial.println("<--as int");
	};
};

void debugging::parse(int state,char buffer[50]){
	if(PARSE_DEBUG){
		if(state==0){
			Serial.print("config parse: "); Serial.println(buffer);
		}
	}
}

void debugging::parse_a(int state,char buffer[]){
	if(PARSE_DEBUG){
		if(state==0){
			Serial.print("(char[])"); Serial.println(buffer);
		}
	}
}

void debugging::parse_ul(int state,unsigned long wert){
	if(PARSE_DEBUG){
		if(state==0){
			Serial.print("(unsigned long)"); Serial.println(wert);
		}
	}
}

void debugging::speedo_loop(int state,int intensive,unsigned long previousMillis,const char char_buffer[]){
	if((intensive==0 && TACHO_DEBUG) || (intensive==1 && TACHO_IDEBUG)){
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
			Serial.print("Schreibe Fuel:");
			Serial.println(char_buffer);
		} else if(state==20){
			differ_show=millis()-previousMillis;
			Serial.print("Normeler Tacho refresh dauerte ");
			Serial.print(differ_show);
			Serial.print(" ms");
		} else if(state==21){
			Serial.print("Loop Durchlauf, menustate ist ");
			Serial.print(pMenu->state);
			Serial.print("\n");
		} else if(state==22){
			sprintlnp(PSTR("Oel fertig"));
		} else if(state==23){
			sprintlnp(PSTR("Air fertig"));
		}
	}
}

void debugging::loop(){
	if(TACHO_SDEBUG){
		Serial.print("State changed:");
		Serial.print(pMenu->state);
		sprintlnp(PSTR(": Calling menu_disp();"));
	};
};
