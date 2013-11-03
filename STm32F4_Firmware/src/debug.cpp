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


void debugging::parse_float(int state,char buffer[],int i,int j,int k){
#ifdef PARSE_DEBUG
	if(state==0){
		Serial.puts(USART1,"parse float gestartet mit buffer: ");
		Serial.println(buffer);
	} else if(state==1){
		Serial.puts(USART1,"aktuelles zeichen:");
		Serial.println(buffer[i]);
	} else if(state==2){
		Serial.println("vorzeichen auf - gestellt");
	} else if(state==3){
		Serial.puts(USART1,"twert:");
		Serial.puts(USART1,i);
		Serial.puts(USART1," decade_counter:");
		Serial.puts(USART1,j);
		Serial.puts(USART1," vz:");
		Serial.puts(USART1,k);
	} else if(state==4){
		Serial.puts(USART1,"(float)");
		Serial.puts(USART1,i);
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
		Serial.puts(USART1,"config parse: "); Serial.println(buffer);
	}
#endif
}

void debugging::parse_a(int state,char buffer[]){
#ifdef PARSE_DEBUG
	if(state==0){
		Serial.puts(USART1,"(char[])"); Serial.println(buffer);
	}
#endif
}

void debugging::parse_ul(int state,unsigned long wert){
#ifdef PARSE_DEBUG
	if(state==0){
		Serial.puts(USART1,"(unsigned long)"); Serial.println(wert);
	}
#endif
}

void debugging::speedo_loop(int state,int intensive,unsigned long previousMillis,const char char_buffer[]){
#if defined(TACHO_DEBUG) or defined(TACHO_IDEBUG)
	if(intensive==
#endif
#ifdef TACHO_DEBUG
			0
#elif defined(TACHO_IDEBUG)
			1
#endif
#if defined(TACHO_DEBUG) or defined(TACHO_IDEBUG)
	){
		int differ_show;
		if(state==0){
			Serial.puts_ln(USART1,("Normaler Tacho Durchlauf"));
		} else if(state==1){
			Serial.puts_ln(USART1,("Schreibe Oeltemp"));
		} else if(state==2){
			Serial.puts_ln(USART1,("Schreibe Airtemp"));
		} else if(state==3){
			Serial.puts_ln(USART1,("Schreibe 'km/h'"));
		} else if(state==4){
			Serial.puts_ln(USART1,("Schreibe Speed"));
		} else if(state==5){
			Serial.puts_ln(USART1,("Schreibe drehzahl"));
		} else if(state==6){
			Serial.puts_ln(USART1,("Schreibe Drehzahl"));
		} else if(state==7){
			Serial.puts_ln(USART1,("Schreibe nen Titel wie Q3 alleine"));
		} else if(state==8){
			Serial.puts_ln(USART1,("Schreibe Average + Max"));
		} else if(state==9){
			Serial.puts_ln(USART1,("Schreibe zeiten"));
		} else if(state==10){
			Serial.puts_ln(USART1,("Schreibe Motorwarnung"));
		} else if(state==11){
			Serial.puts_ln(USART1,("Schreibe Blinkerwarnung"));
		} else if(state==12){
			Serial.puts_ln(USART1,("Schreibe Sicherungswarnung"));
		} else if(state==13){
			Serial.puts_ln(USART1,("Schreibe Navigationsanweisung"));
		} else if(state==14){
			Serial.puts_ln(USART1,("Schreibe SD warnung"));
		} else if(state==15){
			Serial.puts_ln(USART1,("Lösche Warnung"));
		} else if(state==16){
			Serial.puts_ln(USART1,("Schreibe trip. Sowas wie day 123.4 km"));
		} else if(state==17){
			Serial.puts_ln(USART1,("Schreibe Clock: "));
			Serial.println(char_buffer);
		} else if(state==18){
			Serial.puts_ln(USART1,("Schreibe Gang"));
		} else if(state==19){
			Serial.puts(USART1,("Schreibe Fuel:"));
			Serial.println(char_buffer);
		} else if(state==20){
			differ_show=Millis.get()-previousMillis;
			Serial.puts(USART1,("Normeler Tacho refresh dauerte "));
			Serial.puts(USART1,differ_show);
			Serial.puts(USART1,(" ms"));
		} else if(state==21){
			Serial.puts(USART1,("Loop Durchlauf, menustate ist "));
			Serial.puts(USART1,pMenu->state);
			Serial.puts(USART1,("\n"));
		} else if(state==22){
			Serial.puts_ln(USART1,("Oel fertig"));
		} else if(state==23){
			Serial.puts_ln(USART1,("Air fertig"));
		}
	}
#endif
}

void debugging::loop(){
#ifdef TACHO_SDEBUG
	Serial.puts(USART1,"State changed:");
	Serial.puts(USART1,pMenu->state);
	Serial.puts_ln(USART1,(": Calling menu_disp();"));
#endif
}


