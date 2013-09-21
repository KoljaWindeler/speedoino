/*
 * stepper.cpp
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#include "global.h"



#define FALSE 0
#define TRUE 1



speedo_stepper::speedo_stepper(void){
	init_steps_to_go=0;
}

speedo_stepper::~speedo_stepper(){
};


void speedo_stepper::init(){
	if(pConfig->get_hw_version()==7){
		Serial3.begin(19200); // macht eigentlich schon der reset, aber zur sicherheit
		Serial3.flush();
		overwrite_pos(200);
		go_to(0);
		// select startup mode
		if(pSpeedo->startup_by_ignition){
			init_steps_to_go=5; //5 steps to go
		} else {
			init_steps_to_go=0;
		}
	} else {
		TTMC222Status TMC222Status;
		TTMC222Parameters TMC222Parameter;
		TMC222Parameter.IRun=4;			// 84mA, 270||270=135R, 12V/135R=88mA, Motor max 35mA<42.0mA, ok :D
		TMC222Parameter.IHold=0;		// 59mA, 270||270=135R, 12V/135R=88mA, Motor max 20mA<29.5mA, ok :D
		TMC222Parameter.VMax=15;		// 15 = 973 FullSteps/sec
		TMC222Parameter.VMin=3;			// 1/32= 973/32 FullSteps/sec
		TMC222Parameter.SecPosHi=0b000; // TRY
		TMC222Parameter.Shaft=0;		// Clockwise
		TMC222Parameter.Acc=5;			// 473 FS/sec² 7 war super
		TMC222Parameter.SecPosLo=0x0f;	// TRY
		TMC222Parameter.AccShape=0;		// regular accel
		TMC222Parameter.StepMode=0b00;	// 1/2 µStepps .. 1930*n1/n2
		// select startup mode
		if(pSpeedo->startup_by_ignition || true){ // TODO
			//InitTWI();
			GetFullStatus1(&TMC222Status);
			ResetToDefault();
			SetMotorParameters(&TMC222Parameter);
			ResetPosition();
			init_steps_to_go=4;//5
		} else {
			init_steps_to_go=0;//5
		}
	}
};

void speedo_stepper::startup(){
	if(init_steps_to_go!=0){
		if(pConfig->get_hw_version()!=7){
			///////////// TMC 222 /////////////
			if(init_steps_to_go>=4){
				Serial.print("Schritt 4 -> ");
				Serial.println("run init");
				go_to(18550);
				init_steps_to_go=3;
			} else if(init_steps_to_go==3){
				Serial.println("Schritt 3");
				if(abs(get_pos()-18550)<10){		// wait on init done
					Serial.println("ziel erreicht -> gehe zu schritt 1");
					init_steps_to_go=2;
					go_to(0);
				}
			} else if(init_steps_to_go==2){
				Serial.println("Schritt 2");
				if(abs(get_pos())<10){ 	// set go back to zero

					// reduce full speed to 4/15
					TTMC222Parameters TMC222Parameters;
					GetMotorParameters(&TMC222Parameters);
					TMC222Parameters.VMax=3;
					TMC222Parameters.Shaft=1; // ccw
					SetMotorParameters(&TMC222Parameters);

					go_to(3000); // against mech stop:ccw
					init_steps_to_go=1;	// ready
				}
			} else if(init_steps_to_go==1){
				Serial.println("Schritt 1");
				if(abs(get_pos()-3000)<10){
					init_steps_to_go=0;

					TTMC222Parameters TMC222Parameters;
					GetMotorParameters(&TMC222Parameters);
					TMC222Parameters.VMax=8;
					TMC222Parameters.Shaft=0; // cw
					TMC222Parameters.IRun=1;
					TMC222Parameters.Acc=2;
					SetMotorParameters(&TMC222Parameters);
					TTMC222Status TMC222Status;
					GetFullStatus1(&TMC222Status);
					ResetPosition();
				}
			}
			///////////// TMC 222 /////////////
		} else {
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
					ResetPosition();
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
		}
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
bool speedo_stepper::go_to(int winkel,int accel,int speed){
	if(pConfig->get_hw_version()==7){
		///////////// ATM /////////////
		Serial3.print("$m");
		Serial3.print(winkel/ATM_DIV_FACTOR);
		Serial3.print(",");
		Serial3.print(accel);
		Serial3.print(",");
		Serial3.print(speed);
		Serial3.print("*");
		///////////// ATM /////////////
	} else {
		///////////// TCM222 /////////////
		pAktors->m_stepper->SetPosition(winkel/TMC222_DIV_FACTOR); // max 13500 -> 1600
		//		Serial.print("goto:");
		//		Serial.print(int(winkel/TMC222_DIV_FACTOR));
		///////////// TCM222 /////////////
	}

	// debug
#ifdef STEPPER_DEBUG /// DEBUG
	Serial.print("$m");
	Serial.print(winkel);
	Serial.print(",");
	Serial.print(accel);
	Serial.print(",");
	Serial.print(speed);
	Serial.print("*");
#endif

	return true;
};

bool speedo_stepper::go_to(int winkel){
	if(pConfig->get_hw_version()==7){
		///////////// ATM /////////////
		Serial3.print("$m");
		Serial3.print(winkel/ATM_DIV_FACTOR);
		Serial3.print("*");
		///////////// ATM /////////////
	} else {
		///////////// TCM222 /////////////
		pAktors->m_stepper->SetPosition(winkel/TMC222_DIV_FACTOR); // max 13500 -> 1600
		//		Serial.print("goto:");
		//		Serial.println(int(winkel/TMC222_DIV_FACTOR)); // 1.488
		///////////// TCM222 /////////////
	}

#ifdef STEPPER_DEBUG /// DEBUG
	Serial.print(millis());
	Serial.print(",");
	Serial.println(winkel);
#endif
	return true;
};


// get Pos
int speedo_stepper::get_pos(){
	int pos=0;

	if(pConfig->get_hw_version()==7){
		///////////// ATM /////////////
		Serial3.flush();
		Serial3.print("$p*"); // send request

		if(pSensors->m_reset->last_reset==-1) return -1; // ATm328 down

		unsigned long start_time=millis();
		unsigned int recv=0;


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
		pos*=ATM_DIV_FACTOR;
		///////////// ATM /////////////
	} else {
		///////////// TCM222 /////////////
		pos=pAktors->m_stepper->GetFullStatus2(NULL,NULL) * TMC222_DIV_FACTOR;
		///////////// TCM222 /////////////
	}
	// if illegal
	if(pos <0 || pos>=99999){
		pos=-1;
	}
	Serial.print("get pos:");
	Serial.println(pos);

	return pos;
};



void speedo_stepper::get_motor_status(int* ist_pos, int* delay, int* status){
	if(pConfig->get_hw_version()==7){
		get_atm_motor_status(ist_pos,delay,status);
	} else {
		TTMC222Status TMC222Status;
		GetFullStatus1(&TMC222Status);
	}
}

void speedo_stepper::loop(){
	if(pConfig->get_hw_version()==7){
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
	} else {
		if(pSpeedo->disp_zeile_bak[0]!=99){
			pSpeedo->disp_zeile_bak[0]=99;
			pOLED->clear_screen();
			pOLED->string_P(pSpeedo->default_font,PSTR("Vmax:"),0,1);
			pOLED->string_P(pSpeedo->default_font,PSTR("Pos:"),0,2);
			pOLED->string_P(pSpeedo->default_font,PSTR("Target:"),0,3);
			pOLED->string_P(pSpeedo->default_font,PSTR("Eldef:"),0,4);
		};

		if(pSpeedo->disp_zeile_bak[1]!=pSensors->m_clock->get_ss()){
			pSpeedo->disp_zeile_bak[1]=pSensors->m_clock->get_ss();


			TTMC222Status TMC222Status;
			GetFullStatus1(&TMC222Status);
			int target_pos;
			int pos=GetFullStatus2(&target_pos,NULL);

			char buffer[20];
			sprintf(buffer,"%06i",TMC222Status.VMax);
			pOLED->string(pSpeedo->default_font,buffer,7,1);
			sprintf(buffer,"%06i",pos);
			pOLED->string(pSpeedo->default_font,buffer,7,2);
			sprintf(buffer,"%06i",target_pos);
			pOLED->string(pSpeedo->default_font,buffer,7,3);
			sprintf(buffer,"%06i",TMC222Status.ElDef);
			pOLED->string(pSpeedo->default_font,buffer,7,4);
		};
	}
};

///////////////////////////////////////// ATM /////////////////////////////////////////
void speedo_stepper::overwrite_pos(int new_pos){
	Serial3.flush();
	Serial3.print("$o");
	Serial3.print(new_pos);
	Serial3.print("*");
}


void speedo_stepper::get_atm_motor_status(int* ist_pos, int* delay, int* status){
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
///////////////////////////////////////// ATM /////////////////////////////////////////

///////////////////////////////////////// TMC 222 /////////////////////////////////////////

/*****************************************************************************
   Function: GetFullStatus1()
   Parameters: *TMC222Status: Pointer to variable that gets the TMC222 status
                              information.

   Return value: ---

   Zweck: Read out the status information of the TMC222.
          It is also possible to set the parameter to NULL, when only the
          error flags are to be reset without the need to know the status.
 ******************************************************************************/
void speedo_stepper::GetFullStatus1(TTMC222Status *TMC222Status){
	TWCR &= ~(1<<TWINT);
	uint8_t buffer[20];

	I2c.write(TMC_ADDR,TMC222_COMMAND_GET_STATUS1);
	I2c.read(TMC_ADDR,8,buffer);

	//Copy the data to the TMC222Status structure (if pointer not NULL)
	if(TMC222Status!=NULL){
		for(int i=0; i<5; i++){
			*(((unsigned char *) TMC222Status)+i)=buffer[i+1];
		}
	}

	Serial.println("----------\r\n Start");

	Serial.print("IRun:");
	Serial.println(int(TMC222Status->IRun));

	Serial.print("IHold:");
	Serial.println(int(TMC222Status->IHold));

	Serial.print("VMax:");
	Serial.println(int(TMC222Status->VMax));

	Serial.print("VMin:");
	Serial.println(int(TMC222Status->VMin));

	Serial.print("Shaft:");
	Serial.println(int(TMC222Status->Shaft));

	Serial.print("Acc:");
	Serial.println(int(TMC222Status->Acc));

	Serial.print("StepMode:");
	Serial.println(int(TMC222Status->StepMode));

	Serial.print("ElDef:");
	Serial.println(int(TMC222Status->ElDef));

	Serial.print("CPFail:");
	Serial.println(int(TMC222Status->CPFail));

	Serial.print("ESW:");
	Serial.println(int(TMC222Status->ESW));

	Serial.print("OVC1:");
	Serial.println(int(TMC222Status->OVC1));

	Serial.print("OVC2:");
	Serial.println(int(TMC222Status->OVC2));


	Serial.println("end\r\n----------");

}


/*****************************************************************************
   Function: GetFullStatus2()
   Parameters: 	*TargetPosition: Pointer to variable for the target position or NULL.
 *SecurePosition: Pointer to variable for the secure position or NULL.

   Return value: actual position

   Purpose: Read all TMC222 position registers (using the GetFullStatus2 command). NULL can be used for values that are not needed.
 ******************************************************************************/
int speedo_stepper::GetFullStatus2(int *TargetPosition, int *SecurePosition){
	uint8_t buffer[20];

	I2c.write(TMC_ADDR,TMC222_COMMAND_GET_STATUS2);
	I2c.read(TMC_ADDR,8,buffer);
	if(TargetPosition!=NULL) *TargetPosition=(buffer[3]<<8) | buffer[4];
	if(SecurePosition!=NULL) *SecurePosition=((buffer[6] & 0x06)<<8) | buffer[5];
	return (buffer[1]<<8) | buffer[2];
}


/*****************************************************************************
   Function: SetMotorParameters()
   Parameters: *TMC222Parameters: Pointer to TTMC222Parameters structure
                                  containing all parameters.

   Return value: ---

   Purpose: Set the TMC222 motor parameters.
 ******************************************************************************/
void speedo_stepper::SetMotorParameters(TTMC222Parameters *TMC222Parameters){
	uint8_t Buffer[7];
	Buffer[0]=0xff;
	Buffer[1]=0xff;
	for(int i=0; i<5; i++){
		Buffer[i+2]=*(((unsigned char *) TMC222Parameters)+i);
	}
	I2c.write(TMC_ADDR,0x89,Buffer,7);
}


/*****************************************************************************
   Function: SetPosition()
   Parameters: Position: Target position

   Return value: ---
   Note:
	   Input 320 = 1x 360° Rot of magnet.
	   N1: 10/60 = 6
	   N2: 10/72 = 7.2
	   Nges 43.2
	   Move Pointer 360°   = 43.2*320 = 13440
	   Max  Pointer 330,2° = pos:12680 = rpm:18512
	   rpm->pos :: 20000/13440=1.488

   Purpose: Set the target position. This makes the motor move to the new
            target position (if the motor parameters have been set properly).
 ******************************************************************************/
void speedo_stepper::SetPosition(int Position){
	uint8_t Buffer[6];
	Buffer[0]=0xff;
	Buffer[1]=0xff;
	Buffer[2]=Position >> 8;
	Buffer[3]=Position & 0xff;
	I2c.write(TMC_ADDR,0x8b,Buffer,4);
}


/*****************************************************************************
   Function: ResetPosition()
   Parameters: ---

   Return value: ---

   Purpose: Set actual position and target position to zero (without moving the
            motor).
 ******************************************************************************/
void speedo_stepper::ResetPosition(void){
	I2c.write(TMC_ADDR,TMC222_COMMAND_RESET_POS);
}


/*****************************************************************************
   Function: GotoSecurePosition()
   Parameters: ---

   Return value: ---

   Purpose: Drive the motor to the secure position (that has been set using
   the SetMotorParameters command).
 ******************************************************************************/
void speedo_stepper::GotoSecurePosition(void){
	I2c.write(TMC_ADDR,TMC222_COMMAND_GOTO_SECURE);
}


/*****************************************************************************
   Function: ResetToDefault()
   Parameters: ---

   Return value: ---

   Purpose: Reset all values to the default values (from the OTP).
 ******************************************************************************/
void speedo_stepper::ResetToDefault(void){
	I2c.write(TMC_ADDR,TMC222_COMMAND_RESET_FULL);
}


/*****************************************************************************
   Function: RunInit()
   Parameters: VMin: Velocity 1 (0..15)
               VMax: Velocity 2 (0..15)
               Position1: Position 1
               Position2: Position 2

   Return value: ---

   Purpose: RunInit command (reference search) of the TMC222 (please see the
            TMC222 data sheet for details).
 ******************************************************************************/
void speedo_stepper::RunInit(unsigned char VMin, unsigned char VMax, int Position1, int Position2){
	uint8_t Buffer[7];
	Buffer[0]=0xff;
	Buffer[1]=0xff;
	Buffer[2]=(VMax << 4) | (VMin & 0x0f);
	Buffer[3]=BYTE1(Position1);
	Buffer[4]=BYTE0(Position1);
	Buffer[5]=BYTE1(Position2);
	Buffer[6]=BYTE0(Position2);
	I2c.write(TMC_ADDR,0x88,Buffer,7);
}


/*****************************************************************************
   Function: HardStop()
   Parameterss: ---

   Return value: ---

   Purpose: Hard stop of the motor.
            Afterwards the error flags have to be reset using a GetFullStatus1
            command, as otherwise the motor will not run any more.
            (HardStop sets some error flags in the TMC222 that prevent running
             until they have been reset).
 ******************************************************************************/
void speedo_stepper::HardStop(void){
	I2c.write(TMC_ADDR,TMC222_COMMAND_HARDSTOP);
}


/*****************************************************************************
   Function: SoftStop()
   Parameters: ---

   Return value: ---

   Purpose: Stop the motor using the normal decelaration.
 ******************************************************************************/
void speedo_stepper::SoftStop(void){
	I2c.write(TMC_ADDR,TMC222_COMMAND_SOFTSTOP);
}


/*****************************************************************************
   Function: GetMotorParameters()
   Parameters TMC222Parameters: Pointer to data structure that will contain
              all parameters read from the TMC222.

   Return value: ---

   Purpose: This function uses a GetFullStatus1 ccommand and a GetFullStatus2
            command to read out motor parameters into a TTTMC222Parmaters
            data structure.
            After that, the desired values can be modified and the same data
            structure can then be used wit SetMotorParameters again.
 ******************************************************************************/
void speedo_stepper::GetMotorParameters(TTMC222Parameters *TMC222Parameters)
{
	TTMC222Status TMC222Status;
	int SecurePosition;

	GetFullStatus1(&TMC222Status);

	TMC222Parameters->IRun=TMC222Status.IRun;
	TMC222Parameters->IHold=TMC222Status.IHold;
	TMC222Parameters->VMin=TMC222Status.VMin;
	TMC222Parameters->VMax=TMC222Status.VMax;
	TMC222Parameters->Acc=TMC222Status.Acc;
	TMC222Parameters->AccShape=TMC222Status.AccShape;
	TMC222Parameters->Shaft=TMC222Status.Shaft;
	TMC222Parameters->StepMode=TMC222Status.StepMode;

	GetFullStatus2(NULL, &SecurePosition);
	TMC222Parameters->SecPosHi=BYTE1(SecurePosition);
	TMC222Parameters->SecPosLo=BYTE0(SecurePosition);
}
///////////////////////////////////////// TMC 222 /////////////////////////////////////////
