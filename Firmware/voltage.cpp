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
	int aktueller_wert=round(analogRead(VOLTAGE_PIN)/6.4);
	value=pSensors->flatIt(aktueller_wert,&value_counter,3,value);
}

int speedo_voltage::get(){
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

void speedo_voltage::loop(){
	if(pSpeedo->disp_zeile_bak[0]!=1){
		pOLED->string(0,"Voltage ",1,2);
		pSpeedo->disp_zeile_bak[0]=1;
	}
	int voltage=pSensors->m_voltage->get();
	if(voltage!=pSpeedo->disp_zeile_bak[1]){
		pSpeedo->disp_zeile_bak[1]=voltage;
		char temp[6];
		sprintf(temp,"%2i,%iV",int(floor(voltage/10)),int(voltage%10));
		pOLED->string(0,temp,9,2);
	};
}
