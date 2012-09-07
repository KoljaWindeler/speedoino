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
	// select startup mode
	if(pSpeedo->regular_startup){
		init_steps_to_go=5;//5
	} else {
		init_steps_to_go=0;//5
	}
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


void speedo_stepper::get_motor_status(int* ist_pos, int* delay, int* status){
	Serial3.flush();
	Serial3.print("$g*");

	unsigned long start_time=millis();
	unsigned char recv=0;
	unsigned char field=0;
	*ist_pos=0;
	*delay=0;
	*status=0;


	while(start_time+200>millis() && recv<40){ // max 200ms und $p20000* = 8 chars
		while(Serial3.available()>0){
			unsigned char buffer = Serial3.read();
			Serial.print(buffer);
			if(buffer=='$'){
				recv++;
			} else if(buffer=='g'){
				recv++;
			} else if(buffer=='*'){
				recv=99;
				Serial3.flush();
			} else if(buffer==','){
				recv++;
				field++;
			} else {
				recv++;
				if(field==0){ // ist_pos
					*ist_pos=*ist_pos*10+(buffer-'0');
				} else if(field==1){ //soll
					*delay=*delay*10+(buffer-'0');
				} else {
					*status=(buffer-'0');
				}
			}
		}
	}
}

void speedo_stepper::loop(){
	if(pSpeedo->disp_zeile_bak[0]!=99){
		pSpeedo->disp_zeile_bak[0]=99;
		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("POS:"),0,1);
		pOLED->string_P(pSpeedo->default_font,PSTR("DELAY:"),0,2);
		pOLED->string_P(pSpeedo->default_font,PSTR("STATE:"),0,3);
	};

	if(pSpeedo->disp_zeile_bak[1]!=pSensors->m_clock->get_ss()){
		pSpeedo->disp_zeile_bak[1]=pSensors->m_clock->get_ss();
		int ist_pos,delay,status;
		get_motor_status(&ist_pos,&delay,&status);
		char buffer[20];
		sprintf(buffer,"%06i",ist_pos);
		pOLED->string(pSpeedo->default_font,buffer,7,1);
		sprintf(buffer,"%06i",delay);
		pOLED->string(pSpeedo->default_font,buffer,7,2);
		sprintf(buffer,"%06i",status);
		pOLED->string(pSpeedo->default_font,buffer,7,3);
	};
};

