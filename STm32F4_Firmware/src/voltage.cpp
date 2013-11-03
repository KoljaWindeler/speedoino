/*
 * voltage.cpp
 *
 *  Created on: 19.11.2011
 *      Author: jkw
 */

#include "global.h"

voltage::voltage(){
	start_time=0;
	value_counter=0;
	value=0;
	bat_empty=false;
};

voltage::~voltage(){
};

void voltage::calc(bool first_start){
//	bool was_regular_startup_befor_mesurement=Speedo.startup_by_ignition;
//	/* bei 12V => durch den Spannungsteiler ~1/3 = 4V */
//	/* value/1023*5*(3.2k)/1k  */
//
//	//int aktueller_wert=round(analogRead(VOLTAGE_PIN)/6.4);
//	int aktueller_wert=round(analogRead(VOLTAGE_PIN)*1.5625); // cV centiVolt
//
//	// hier können wir ruhig heftig tiefpassen, beim startup gibts ohnehin nur einen wert und der ist der aktuelle
//	value=Sensors.flatIt_shift(aktueller_wert,&value_counter,6,value); // TP with 64
//
//	/////////////////////// clock mode stuff ///////////////////////////
//	//check mode
//	// first start => Value >45, regular start, keep_alive!
//	// first start => value <45, clock start, keep_alive AND set regular start to false
//	// loop => was NOT regular start, but value is now >45 -> init
//	// loop => was NOT regular start, value is still <45 -> keep_alive
//	// loop => was regular start, value is still >45 -> keep_alive
//	// loop => was regular start, value is now <45 -> shut down
//	if(first_start){
//		if(value<350){ // 3,5V
//			Speedo.startup_by_ignition=false;
//			keep_me_alive(true); // shutdown is done in clock class
//		} else {
//			Speedo.startup_by_ignition=true;
//		}
//	} else {
//		if(Speedo.startup_by_ignition){
//			if(Sensors.get_RPM(RPM_TYPE_DIRECT)>0){
//				keep_me_alive(true); // balance load
//			} else {
//				keep_me_alive(false); // fast switch off
//			}
//		} else { // i am in clock mode
//			// check if we still hold the "left" button, to extend the time
//			if(!(PINJ & (1<<menu_button_links))){ // button still down
//				start_time=Millis.get()/1000;
//			};
//
//			// check if we have to deactivate us self
//			if((start_time+CLOCK_UP_TIME)<(Millis.get()/1000)){ // clock was visible for N sec
//				keep_me_alive(false); // shut down
//				_delay_ms(9999);
//			} else if((start_time+CLOCK_UP_TIME-1)<(Millis.get()/1000)){ // 1sek previous
//				TFT.string(Speedo.default_font,("ByeBye "),7,7);
//			};
//
//			// main power turn up while in clock mode
//			if(!was_regular_startup_befor_mesurement && aktueller_wert>45){
//				value_counter=0; 					// reset counter to avoid "voltage below 11V" - warning, cause we are at >45 but not at >110
//				Speedo.startup_by_ignition=true; 	// since we have power on the power switch, it should be a regular startup
//				Serial.puts_ln(USART1,("Power up during Clock Mode"));
//				TFT.init_speedo();
//				Menu.init(); 						// restart init process
//				Menu.display(); 					// execute this AFTER TFT.init_speedo!! this will show the menu and, if state==11, draws speedosymbols
//				Speedo.reset_bak(); 				// reset all storages, to force the redraw of the speedo
//			}
//		};
//	}
	Speedo.startup_by_ignition=true; // TODO remove this lines
	value=120;
	/////////////////////// clock mode stuff ///////////////////////////
}

int voltage::get(){
	if(value<=1500 && value>=0){
		return value;
	} else {
		return -1;
	};
	return 0;
}

int voltage::check_vars(){
	return 0;
};


void voltage::init(){
	//	pinMode(VOLTAGE_PIN,INPUT);
	//	bat_empty=false;
	//
	//	 if hardware version is above 6, there is an emergency V_BACKUP_driver for the GPS
	//	if(pConfig->get_hw_version()>6){
	//		// 1. Set analog measurment pin connected to the backup bat to input
	//		// 2. Measure value, 2V min => if < 1.9V -> activate emergency voltage, 1.9 / 5 * 1024 < 389
	//
	//		// 1
	//		pinMode(8,INPUT); // measure pin
	//		pinMode(24,INPUT); // driver pin, but keep it as input until measurent is completed
	//
	//		// 2
	//		if(analogRead(8)<389){
	//			pinMode(24,OUTPUT); // activate driver pin
	//			digitalWrite(24,HIGH);
	//			bat_empty=true;
	//		}
	//	}

	// do a initial read, to estimate if its a regular startup or a "show clock"-startup
	Speedo.startup_by_ignition=true;
	calc(true); // submit true as "this is the first start"
	if(!Speedo.startup_by_ignition){
		start_time=Millis.get()/1000; // sec of start
		keep_me_alive(true);
	} else {
		start_time=9999;
	}
	Serial.puts_ln(USART1,("Voltage init done"));
};


// handle transistor to dauerplus
void voltage::keep_me_alive(bool active){
//	if(active){
//		DDRL |= (1<<PINL0); // define as output
//		PORTL |= (1<<PINL0); // set high
//	} else {
//		PORTL &= ~(1<<PINL0); // shut down
//	}
}
