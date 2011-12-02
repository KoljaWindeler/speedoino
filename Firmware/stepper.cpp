/*
 * stepper.cpp
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#include "global.h"

speedo_stepper::speedo_stepper(void){
}

speedo_stepper::~speedo_stepper(){
};


void speedo_stepper::init(){
	Serial3.begin(19200); // macht eigentlich schon der reset, aber zur sicherheit
	Serial3.flush();

	while(!go_to(1600,1)){ delay(1); }; // warten bis voll ausgeschlagen
//	while(!go_to(0,0)){delay(1); };	// motor vollausschlag
//	while(!go_to(1001,0)){delay(1); };	// motor vollausschlag
//	go_to(0,0);

};

bool speedo_stepper::go_to(int winkel,int overwrite){
	if(Serial3.available()>=3){ // 3 buchstaben $k*
		char serial_buffer[3];
		serial_buffer[0]=Serial3.read();
		serial_buffer[1]=Serial3.read();
		serial_buffer[2]=Serial3.read();
		Serial3.flush();
		delay(50);

		if(serial_buffer[0]=='$' && serial_buffer[1]=='k' && serial_buffer[2]=='*'){
			Serial3.print("$m");
			Serial3.print(winkel);
			Serial3.print("*");
			Serial3.flush();
			return true;
		};

	} else if(overwrite==1){
		Serial3.flush();
		Serial3.print("$m");
		Serial3.print(winkel);
		Serial3.print("*");
	};
	return false;
};
