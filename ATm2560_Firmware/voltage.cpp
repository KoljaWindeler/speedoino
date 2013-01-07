/*
 * voltage.cpp
 *
 *  Created on: 19.11.2011
 *      Author: jkw
 */

#include "global.h"

speedo_voltage::speedo_voltage(){
};

speedo_voltage::~speedo_voltage(){
};

void speedo_voltage::calc(){
	bool was_regular_startup_befor_mesurement=pSpeedo->regular_startup;
	/* bei 12V => durch den Spannungsteiler 1/3 = 4V */
	/* value/1023*5*(3.2k)/1k  */

	int aktueller_wert=round(analogRead(VOLTAGE_PIN)/6.4);
	value=pSensors->flatIt(aktueller_wert,&value_counter,3,value);

	/////////////////////// clock mode stuff ///////////////////////////
	//check mode
	if(value<45){ // 4,5V
		pSpeedo->regular_startup=false;
	} else {
		pSpeedo->regular_startup=true;
	}

	if(!pSpeedo->regular_startup){ // i am in clock mode
		// check if we still hold the "left" button, to extend the time
		if(!(PINJ & (1<<menu_button_links))){ // button still down
			start_time=millis()/1000;
		};

		// check if we have to deactivate us self
		if((start_time+CLOCK_UP_TIME)<(millis()/1000)){ // clock was visible for N sec
			PORTL &= ~(1<<PINL0); // shut down
		} else if((start_time+CLOCK_UP_TIME-1)<(millis()/1000)){ // 1sek previous
			pOLED->string_P(pSpeedo->default_font,PSTR("ByeBye "),7,7);
		};
	};

	// main power turn up while in clock mode
	if(!was_regular_startup_befor_mesurement && pSpeedo->regular_startup){
		PORTL &= ~(1<<PINL0); // shut down
		pOLED->init_speedo();
		pMenu->init(); // restart init process
		pMenu->display(); 			// execute this AFTER pOLED->init_speedo!! this will show the menu and, if state==11, draws speedosymbols
		pSpeedo->reset_bak(); 		// reset all storages, to force the redraw of the speedo
	}
	/////////////////////// clock mode stuff ///////////////////////////
}

int speedo_voltage::get(){
	if(value<=150 && value>=0){
		return value;
	} else {
		return -1;
	};
	return 0;
}

bool speedo_voltage::check_vars(){
	return false;
};

void speedo_voltage::clear_vars(){
	value_counter=0;
	value=0;
}

void speedo_voltage::init(){
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
	pSpeedo->regular_startup=true;
	calc();
	if(!pSpeedo->regular_startup){
		start_time=millis()/1000; // sec of start
		DDRL |= (1<<PINL0); // define as output
		PORTL |= (1<<PINL0); // set high
	} else {
		start_time=9999;
	}
	pDebug->sprintlnp(PSTR("Voltage init done"));
};

