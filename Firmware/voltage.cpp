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
	int aktueller_wert=round(analogRead(3)/6.8);
	value=pSensors->flatIt(aktueller_wert,&value_counter,20,value);
}

int speedo_voltage::get(){
	// 1023
	if(value<=150 && value>=0){
		return value;
	} else {
		return -1;
	};
}

void speedo_voltage::init(){
	pinMode(VOLTAGE_PIN,INPUT);
	value_counter=0;
	value=0;
};
