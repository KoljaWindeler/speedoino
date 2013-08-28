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

speedo_reset::speedo_reset(){
	toggle_high=true; // beliebig, einfach mal als startwert
	last_time=millis();
	reset_enabled=true;
	// get reset reason
	last_reset=-1; // denn wenn wir wegen strom neustarten ist der avr noch nicht soweit weil er 3 sek wartet damit wir soweit sind ;)
	reboots_caused_by_sd_problems=0;
};

speedo_reset::~speedo_reset(){};

int speedo_reset::check_vars(){
	return 0;
};


void speedo_reset::init(){
	Serial3.begin(19200);
	pinMode(reset_toogle_pin,OUTPUT);

	if(eeprom_read_byte((const uint8_t *)149)==1){ // glatte 1 heisst ist an, alle andere heisst ist aus
		set_active(false,true); // eeprom is set,set var
	} else {
		set_deactive(false,true); // eeprom is set,set var
	}
	pDebug->sprintp(PSTR("Reset init done. Status: "));
	ask_reset();
	if(last_reset==0){
		pDebug->sprintlnp(PSTR("Power down"));
	} else if(last_reset==1){
		pDebug->sprintlnp(PSTR("AVR watchdog"));
	} else if(last_reset==2){
		pDebug->sprintlnp(PSTR("BT connect"));
	} else {
		pDebug->sprintlnp(PSTR("ATm328 down!"));
	}

	// watchdog einschalten um ihn dann zu deaktivieren ... strange aber muss wohl so
	wdt_enable(WDTO_8S);
	MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = 0x00;
};

void speedo_reset::set_active(bool save_to_eeprom,bool save_to_var){
	Serial3.print("$r1*");
	if(save_to_var){
		reset_enabled=true;
	}
	if(save_to_eeprom){
		byte tempByte = (1 & 0xFF);
		eeprom_write_byte((uint8_t *)149,tempByte);
	}
#ifdef RESET_DEBUG
	pDebug->sprintlnp(PSTR("Reset enabled"));
	if(save_to_eeprom){
		pDebug->sprintlnp(PSTR("written to storage"));
	};
#endif
};

void speedo_reset::set_deactive(bool save_to_eeprom,bool save_to_var){
	Serial3.print("$r0*");
	if(save_to_var){
		reset_enabled=false;
	}
	if(save_to_eeprom){
		byte tempByte = (0 & 0xFF);
		eeprom_write_byte((uint8_t *)149,tempByte);
	};

#ifdef RESET_DEBUG
	pDebug->sprintlnp(PSTR("Reset disabled"));
	if(save_to_eeprom){
		pDebug->sprintlnp(PSTR("written to storage"));
	};
#endif
};

void speedo_reset::restore(){
	if(reset_enabled){
		set_active(false,false);
	} else {
		set_deactive(false,false); // ist wiederherstellen, var && eeprom sind ohnehin gesetzt
	}
}

void speedo_reset::toggle(){
	wdt_reset();
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
		Serial3.flush();
		Serial3.print("$y*");
		unsigned long time=millis();
		char recv[5];
		unsigned int recv_counter=0;

		while(recv_counter<4 && (millis()-time)<1000){ // max 1 sec auf ein zeichen warten
			if(Serial3.available()>0){
				recv[recv_counter]=Serial3.read();
				recv_counter++;
			}
		};


		if(recv_counter>0 && recv[0]=='$' && recv[1]=='y' && recv[3]=='*'){
			//char to int
			if(recv[2]=='0') last_reset=0;
			else if(recv[2]=='1') last_reset=1; // avr
			else if(recv[2]=='2') last_reset=2; // bluetooth
		}
	}
}

