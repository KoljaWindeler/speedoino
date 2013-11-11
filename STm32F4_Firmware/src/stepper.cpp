/*
 * stepper.cpp
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#include "global.h"

#define FALSE 0
#define TRUE 1



stepper::stepper(void){
	init_steps_to_go=0;
}

stepper::~stepper(){
};


void stepper::init(){
	//	if(pConfig->get_hw_version()==7){
	Serial.init(USART3,19200); // macht eigentlich schon der reset, aber zur sicherheit
//	//Serial3.flush();
	overwrite_pos(200);
	go_to(0);
	shown_mode=RPM_TYPE_DIRECT;
	// select startup mode
	if(Speedo.startup_by_ignition){
		//			init_steps_to_go=5; //5 steps to go
		run_calibration();
		init_steps_to_go=0;
	} else {
		init_steps_to_go=0;
	}
};

void stepper::startup(){
	if(init_steps_to_go!=0){
		///////////// ATM /////////////
		if(init_steps_to_go>=5){
			if(get_pos()!=0){		//
				go_to(0,SLOW_ACCEL,FAST_SPEED);			// 240*8 = 1920
			} else {
				init_steps_to_go=4; 					// nächsten schritt vorbereiten
			}
		} else if(init_steps_to_go==4){

			if(get_pos()!=MOTOR_OVERWRITE_END_POS){		// motor noch nicht am ende angekommen
				go_to(MOTOR_OVERWRITE_END_POS,SLOW_ACCEL,FAST_SPEED);// weiter dorthin scheuchen
			} else { 														// motor angekommen
				init_steps_to_go=3; 					// nächsten schritt vorbereiten
			}
		} else if(init_steps_to_go==3){
			if(get_pos()!=0){   						// motor noch nicht am anfang angekommen
				go_to(0,SLOW_ACCEL,FAST_SPEED);			// weiter dorthin scheuchen
			} else { 														// motor angekommen
				init_steps_to_go=0; 					// fertig
			}
		} else if(init_steps_to_go==2){
			if(get_pos()!=80){   						// motor noch nicht am anfang angekommen
				overwrite_pos(80);
			} else { 														// motor angekommen
				init_steps_to_go=1; 					// fertig
			}
		} else if(init_steps_to_go==1){
			if(get_pos()!=0){   						// motor noch nicht am anfang angekommen
				go_to(0,80,200);
			} else { 														// motor angekommen
				init_steps_to_go=0; 					// fertig
			}
		}
		///////////// ATM /////////////
	} // init steps!=0
}

// common functions //

/*****************************************************************************
   Function: go_to()
   Parameters: winkel: 	angle of the stepper motor, this is tricky: the stepper has 325° rotation freedom,
						that equals 13500 stepps in TMC222 mode and 1600 in ATmega mode (could 13500 be = 12800 ?)
   Return value: ---
   Zweck: Read out the status information of the TMC222.
          It is also possible to set the parameter to NULL, when only the
          error flags are to be reset without the need to know the status.
 ******************************************************************************/
bool stepper::go_to(int winkel,int accel,int speed){
	///////////// ATM /////////////
	Serial.puts(USART3,"$m");
	Serial.puts(USART3,int(round(winkel/ATM_DIV_FACTOR)));
	//		Serial.puts(USART3,","); // leave this factor to watchdog firmware (from 28.10.2013)
	//		Serial.puts(USART3,accel);
	//		Serial.puts(USART3,",");
	//		Serial.puts(USART3,speed);
	Serial.puts(USART3,"*");
	///////////// ATM /////////////
	// debug
#ifdef STEPPER_DEBUG /// DEBUG
	Serial.puts(USART3,"$m");
	Serial.puts(USART3,winkel);
	Serial.puts(USART3,",");
	Serial.puts(USART3,accel);
	Serial.puts(USART3,",");
	Serial.puts(USART3,speed);
	Serial.puts(USART3,"*");
#endif

	return true;
};

void stepper::run_calibration(void){
	///////////// ATM /////////////
	Serial.puts(USART3,"$c*");
	///////////// ATM /////////////
}

bool stepper::go_to(int winkel){
	Serial.puts(USART3,"$m");
	Serial.puts(USART3,int(round(winkel/ATM_DIV_FACTOR)));
	Serial.puts(USART3,"*");
#ifdef STEPPER_DEBUG /// DEBUG
	Serial.puts(USART3,Millis.get());
	Serial.puts(USART3,",");
	Serial.println(winkel);
#endif
	return true;
};


// get Pos
int stepper::get_pos(){
	int pos=0;
	///////////// ATM /////////////
	//Serial3.flush();
	Serial.puts(USART3,"$p*"); // send request

//	if(Sensors.mReset.last_reset==-1) return -1; // ATm328 down TODO TODO

	unsigned long start_time=Millis.get();
	unsigned int recv=0;


	while(start_time+200>Millis.get() && recv<8){ // max 200ms und $p20000* = 8 chars
		while(Serial.available(USART3)>0){
			unsigned char buffer = Serial.read(USART3);
			if(buffer=='$'){
				recv++;
			} else if(buffer=='p'){
				recv++;
			} else if(buffer=='*'){
				recv=99;
				//Serial3.flush();
			} else {
				recv++;
				pos=pos*10+(buffer-'0');
			}
		}
	}
	pos*=ATM_DIV_FACTOR;
	///////////// ATM /////////////

	// if illegal
	if(pos <0 || pos>=99999){
		pos=-1;
	}
	//	Serial.puts(USART3,"get pos:");
	//	Serial.println(pos);

	return pos;
};



void stepper::get_motor_status(int* ist_pos, int* delay, int* status){
	get_atm_motor_status(ist_pos,delay,status);
}

void stepper::loop(){

	if(Speedo.disp_zeile_bak[0]!=99){
		Speedo.disp_zeile_bak[0]=99;
		TFT.clear_screen();
		TFT.string(Speedo.default_font,("POS:"),0,1);
		TFT.string(Speedo.default_font,("DELAY:"),0,2);
		TFT.string(Speedo.default_font,("STATE:"),0,3);
	};

	if(Speedo.disp_zeile_bak[1]!=Sensors.mClock.get_ss()){
		Speedo.disp_zeile_bak[1]=Sensors.mClock.get_ss();
		int ist_pos,delay,status;

		get_motor_status(&ist_pos,&delay,&status);
		char buffer[20];
		sprintf(buffer,"%06i",ist_pos);
		TFT.string(Speedo.default_font,buffer,7,1);
		sprintf(buffer,"%06i",delay);
		TFT.string(Speedo.default_font,buffer,7,2);
		sprintf(buffer,"%06i",status);
		TFT.string(Speedo.default_font,buffer,7,3);
	};
};

///////////////////////////////////////// ATM /////////////////////////////////////////
void stepper::overwrite_pos(int new_pos){
	//Serial3.flush();
	Serial.puts(USART3,"$o");
	Serial.puts(USART3,new_pos);
	Serial.puts(USART3,"*");
}


void stepper::get_atm_motor_status(int* ist_pos, int* delay, int* status){
	//Serial3.flush();
	Serial.puts(USART3,"$g*");

	unsigned long start_time=Millis.get();
	unsigned char recv=0;
	unsigned char field=0;
	*ist_pos=0;
	*delay=0;
	*status=0;


	while(start_time+200>Millis.get() && recv<40){ // max 200ms und $p20000* = 8 chars
		while(Serial.available(USART3)>0){
			unsigned char buffer = Serial.read(USART3);
			Serial.puts(USART3,buffer);
			if(buffer=='$'){
				recv++;
			} else if(buffer=='g'){
				recv++;
			} else if(buffer=='*'){
				recv=99;
				//Serial3.flush();
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
///////////////////////////////////////// ATM /////////////////////////////////////////
