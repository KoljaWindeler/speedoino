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

reset::reset(){
	toggle_high=true; // beliebig, einfach mal als startwert
	last_time=Millis.get();
	reset_enabled=true;
	// get reset reason
	last_reset=-1; // denn wenn wir wegen strom neustarten ist der avr noch nicht soweit weil er 3 sek wartet damit wir soweit sind ;)
	reboots_caused_by_sd_problems=0;
};

reset::~reset(){};

int reset::check_vars(){
	return 0;
};


void reset::init(){
	Serial.init(USART3,19200);
//	pinMode(reset_toogle_pin,OUTPUT);		// TODO
//
//	if(eeprom_read_byte((const uint8_t *)149)==1){ // glatte 1 heisst ist an, alle andere heisst ist aus
//		set_active(false,true); // eeprom is set,set var
//	} else {
//		set_deactive(false,true); // eeprom is set,set var
//	}
	Serial.puts(USART1,("Reset init done. Status: "));
	ask_reset();
	if(last_reset==0){
		Serial.puts_ln(USART1,("Power down"));
	} else if(last_reset==1){
		Serial.puts_ln(USART1,("AVR watchdog"));
	} else if(last_reset==2){
		Serial.puts_ln(USART1,("BT connect"));
	} else {
		Serial.puts_ln(USART1,("ATm328 down!"));
	}

	// watchdog einschalten um ihn dann zu deaktivieren ... strange aber muss wohl so
//	wdt_enable(WDTO_8S);	TODO
//	MCUSR &= ~(1<<WDRF);
//	WDTCSR |= (1<<WDCE) | (1<<WDE);
//	WDTCSR = 0x00;
};

void reset::set_active(bool save_to_eeprom,bool save_to_var){
	Serial.puts(USART3,"$r1*");
	if(save_to_var){
		reset_enabled=true;
	}
//	if(save_to_eeprom){						TODO
//		byte tempByte = (1 & 0xFF);
//		eeprom_write_byte((uint8_t *)149,tempByte);
//	}
#ifdef RESET_DEBUG
	Serial.puts_ln(USART1,("Reset enabled"));
	if(save_to_eeprom){
		Serial.puts_ln(USART1,("written to storage"));
	};
#endif
};

void reset::set_deactive(bool save_to_eeprom,bool save_to_var){
	Serial.puts(USART3,"$r0*");
	if(save_to_var){
		reset_enabled=false;
	}
//	if(save_to_eeprom){						TODO
//		byte tempByte = (0 & 0xFF);
//		eeprom_write_byte((uint8_t *)149,tempByte);
//	};

#ifdef RESET_DEBUG
	Serial.puts_ln(USART1,("Reset disabled"));
	if(save_to_eeprom){
		Serial.puts_ln(USART1,("written to storage"));
	};
#endif
};

void reset::restore(){
	if(reset_enabled){
		set_active(false,false);
	} else {
		set_deactive(false,false); // ist wiederherstellen, var && eeprom sind ohnehin gesetzt
	}
}

void reset::toggle(){
//	wdt_reset();								TODO
//	if(reset_enabled){
//		if(Millis.get()-last_time>rst_blink_freq){
//			if(toggle_high) {
//				digitalWrite(reset_toogle_pin,LOW);
//			} else {
//				digitalWrite(reset_toogle_pin,HIGH);
//			}
//			toggle_high=!toggle_high;
//			last_time=Millis.get();
//		}
//	}
}

void reset::ask_reset(){
	if(last_reset==-1){
		Serial.puts(USART3,"$y*");
		unsigned long time=Millis.get();
		char recv[5];
		unsigned int recv_counter=0;

		while(recv_counter<4 && (Millis.get()-time)<1000){ // max 1 sec auf ein zeichen warten
			if(Serial.available(USART3)>0){
				recv[recv_counter]=Serial.read(USART3);
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

