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
	/* bei 12V => durch den Spannungsteiler 1/3 = 4V */
	/* value/1023*5*(3.2k)/1k  */
	//	ADCSRB &= ~(1 << MUX5); // mux5 ausschalten
	//	ADMUX =  (1<<REFS0)|(1<<MUX1); // 01 für avcc,kein left adjust,00010 für kanal 2
	//	ADCSRA |= (1<<ADSC);
	//	while (bit_is_set(ADCSRA, ADSC));
	//	uint8_t low = ADCL;
	//	uint8_t high = ADCH;
	//
	//	// combine the two bytes
	//	int analog=(high << 8) | low;

	int aktueller_wert=round(analogRead(VOLTAGE_PIN)/6.4);
	value=pSensors->flatIt(aktueller_wert,&value_counter,3,value);
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
	pinMode(VOLTAGE_PIN,INPUT);
	bat_empty=false;

	// if hardware version is above 6, there is an emergency V_BACKUP_driver for the GPS
	if(pConfig->get_hw_version()>6){
		// 1. Set analog measurment pin connected to the backup bat to input
		// 2. Measure value, 2V min => if < 1.9V -> activate emergency voltage, 1.9 / 5 * 1024 < 389

		// 1
		pinMode(8,INPUT); // measure pin
		pinMode(24,INPUT); // driver pin, but keep it as input until measurent is completed

		// 2
		if(analogRead(8)<389){
			pinMode(24,OUTPUT); // activate driver pin
			digitalWrite(24,HIGH);
			bat_empty=true;
		}
	}

};

