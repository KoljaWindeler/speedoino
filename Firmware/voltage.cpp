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
};

void speedo_voltage::loop(){
	if(pSpeedo->disp_zeile_bak[0]!=1){
		pOLED->string_P(pSpeedo->default_font,PSTR("Voltage "),1,2);
		pSpeedo->disp_zeile_bak[0]=1;
	}
	int voltage=pSensors->m_voltage->get();
	if(voltage!=pSpeedo->disp_zeile_bak[1]){
		pSpeedo->disp_zeile_bak[1]=voltage;
		char temp[6];
		sprintf(temp,"%2i,%iV",int(floor(voltage/10)),int(voltage%10));
		pOLED->string(pSpeedo->default_font,temp,9,2);
	};
}
