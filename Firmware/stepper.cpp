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
	init_steps_to_go=5;
	overwrite_pos(200);
	go_to(0);
};

void speedo_stepper::overwrite_pos(int new_pos){
	Serial3.flush();
	Serial3.print("$o");
	Serial3.print(new_pos);
	Serial3.print("*");
}

bool speedo_stepper::go_to(int winkel,int accel,int speed){
	Serial3.print("$m");
	Serial3.print(winkel);
	Serial3.print(",");
	Serial3.print(accel);
	Serial3.print(",");
	Serial3.print(speed);
	Serial3.print("*");

	// debug
//	Serial.print("$m");
//	Serial.print(winkel);
//	Serial.print(",");
//	Serial.print(accel);
//	Serial.print(",");
//	Serial.print(speed);
//	Serial.print("*");

	return true;
};

bool speedo_stepper::go_to(int winkel){
	Serial3.print("$m");
	Serial3.print(winkel);
	Serial3.print("*");

	//
//	Serial.print(millis());
//	Serial.print(",");
//	Serial.println(winkel);

	return true;
};

int speedo_stepper::get_pos(){
	Serial3.flush();
	Serial3.print("$p*"); // send request

	unsigned long start_time=millis();
	unsigned int recv=0;
	int pos=0;

	while(start_time+200>millis() && recv<8){ // max 200ms und $p20000* = 8 chars
		while(Serial3.available()>0){
			unsigned char buffer = Serial3.read();
			if(buffer=='$'){
				recv++;
			} else if(buffer=='p'){
				recv++;
			} else if(buffer=='*'){
				recv=99;
				Serial3.flush();
			} else {
				recv++;
				pos=pos*10+(buffer-'0');
			}
		}
	}

	// if illegal
	if(pos <0 || pos>=99999){
		pos=-1;
	}

	return pos;
};
