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

speedo_reset::speedo_reset(){};

speedo_reset::~speedo_reset(){};

void speedo_reset::init(){
	Serial3.begin(4800);
	pinMode(reset_enabled_pin,OUTPUT);
	pinMode(reset_toogle_pin,OUTPUT);
	if(EEPROM.read(149)==1){ // glatte 1 heisst ist an, alle andere heisst ist aus
		set_active(false,true); // eeprom is set,set var
	} else {
		set_deactive(false,true); // eeprom is set,set var
	}
	toggle_high=true; // beliebig, einfach mal als startwert
	last_time=millis();

	// get reset reason
	last_reset=-1; // denn wenn wir wegen strom neustarten ist der avr noch nicht soweit weil er 3 sek wartet damit wir soweit sind ;)
	ask_reset();

	pDebug->sprintp(PSTR("Reset init done. Status "));
	Serial.println(last_reset);
};

void speedo_reset::set_active(bool save_to_eeprom,bool save_to_var){
	digitalWrite(reset_enabled_pin,HIGH);
	if(save_to_var){
		reset_enabled=true;
	}
	if(save_to_eeprom){
		byte tempByte = (1 & 0xFF);
		EEPROM.write(149,tempByte);
	}
	if(RESET_DEBUG){
		pDebug->sprintlnp(PSTR("Reset enabled"));
		if(save_to_eeprom){
			pDebug->sprintlnp(PSTR("written to storage"));
		};
	}
};

void speedo_reset::set_deactive(bool save_to_eeprom,bool save_to_var){
	digitalWrite(reset_enabled_pin,LOW);
	if(save_to_var){
		reset_enabled=false;
	}
	if(save_to_eeprom){
		byte tempByte = (0 & 0xFF);
		EEPROM.write(149,tempByte);
	};

	if(RESET_DEBUG){
		pDebug->sprintlnp(PSTR("Reset disabled"));
		if(save_to_eeprom){
			pDebug->sprintlnp(PSTR("written to storage"));
		};
	};
};

void speedo_reset::restore(){
	if(reset_enabled){
		set_active(false,false);
	} else {
		set_deactive(false,false); // ist wiederherstellen, var && eeprom sind ohnehin gesetzt
	}
}

void speedo_reset::toggle(){
	if(reset_enabled){
		if(millis()-last_time>rst_blink_freq){
			if(toggle_high) {
				digitalWrite(reset_toogle_pin,LOW);
			} else {
				digitalWrite(reset_toogle_pin,HIGH);
			}
			toggle_high=!toggle_high;
			last_time=millis();
		}
	}
}

void speedo_reset::ask_reset(){
	if(last_reset==-1){
		Serial3.print("$y*");
		unsigned long time=millis();
		char recv[5];
		unsigned int recv_counter=0;

		while(recv_counter<2 && (millis()-time)<1000){ // max 1 sec auf ein zeichen warten
			if(Serial3.available()>0){
				recv[recv_counter]=Serial3.read();
				recv_counter++;
			}
		};

		if(recv_counter>0 && recv[0]=='$' && recv[1]=='y' && recv[3]=='*'){
			//char to int
			if(recv[2]=='0') last_reset=0;
			else if(recv[2]=='1') last_reset=1;
			else if(recv[2]=='2') last_reset=2;
		}
	}
}
