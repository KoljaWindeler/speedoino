/*
 * stepper.cpp
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#include "global.h"

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
			init_steps_to_go=5;//5
		} else {
			init_steps_to_go=0;//5
		}
	} else {
		TTMC222Status TMC222Status;
		TTMC222Parameters TMC222Parameter;
		TMC222Parameter.IRun=0;			// 59mA
		TMC222Parameter.IHold=0;		// 59mA
		TMC222Parameter.VMax=15;		// 15 = 973 FullSteps/sec
		TMC222Parameter.VMin=3;			// 1/32= 973/32 FullSteps/sec
		TMC222Parameter.SecPosHi=0b000; // TRY
		TMC222Parameter.Shaft=0;		// Clockwise
		TMC222Parameter.Acc=7;			// 473 FS/sec² 7 war super
		TMC222Parameter.SecPosLo=0x0f;	// TRY
		TMC222Parameter.AccShape=0;		// regular accel
		TMC222Parameter.StepMode=0b00;	// 1/2 µStepps
		// select startup mode
		if(pSpeedo->startup_by_ignition){
			//InitTWI();
			GetFullStatus1(&TMC222Status);
			ResetToDefault();
			SetMotorParameters(&TMC222Parameter);
			ResetPosition();
			//int max_pos=13500;
			init_steps_to_go=3;//5
		} else {
			init_steps_to_go=0;//5
		}

	}
};

void speedo_stepper::startup(){
	if(init_steps_to_go!=0){
		if(pConfig->get_hw_version()!=7){ // TMC 222
			if(init_steps_to_go>=3){
				Serial.print("Schritt 3 -> ");
				Serial.println("run init");
				go_to(13500,0,0);
				init_steps_to_go=2;
			} else if(init_steps_to_go==2){
				Serial.println("Schritt 2");
				if(get_pos()==13500){		// wait on init done
					Serial.println("ziel erreicht -> gehe zu schritt 1");
					init_steps_to_go=1;
				}
			} else if(init_steps_to_go==1){
				Serial.println("Schritt 1");
				if(get_pos()!=0){ 	// set go back to zero
					Serial.println("goto -13500");
					go_to(0,0,0);
				} else {				// zero reached
					Serial.println("pos==-13500, done");
					init_steps_to_go=0;	// ready
				}
			}
		} else {
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
		}
	}
}

void speedo_stepper::overwrite_pos(int new_pos){
	Serial3.flush();
	Serial3.print("$o");
	Serial3.print(new_pos);
	Serial3.print("*");
}

bool speedo_stepper::go_to(int winkel,int accel,int speed){
	if(pConfig->get_hw_version()==7){
		Serial3.print("$m");
		Serial3.print(winkel);
		Serial3.print(",");
		Serial3.print(accel);
		Serial3.print(",");
		Serial3.print(speed);
		Serial3.print("*");
	} else {
		pAktors->m_stepper->SetPosition(winkel); // max 13500 -> 1600
		Serial.print("goto:");
		Serial.print(winkel);
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
		Serial3.print("$m");
		Serial3.print(winkel);
		Serial3.print("*");
	} else {
		pAktors->m_stepper->SetPosition(winkel); // max 13500 -> 1600
		Serial.print("goto:");
		Serial.print(winkel);
	}

#ifdef STEPPER_DEBUG /// DEBUG
	Serial.print(millis());
	Serial.print(",");
	Serial.println(winkel);
#endif

	return true;
};

int speedo_stepper::get_pos(){
	int pos=0;

	if(pConfig->get_hw_version()==7){
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
	} else {
		int temp,temp2;
		pos=pAktors->m_stepper->GetFullStatus2(&temp,&temp2);
	}
	// if illegal
	if(pos <0 || pos>=99999){
		pos=-1;
	}
	Serial.print("get pos:");
	Serial.println(pos);

	return pos;
};




void speedo_stepper::get_motor_status(int* ist_pos, int* delay, int* status,TTMC222Status *TMC222Status){
	if(pConfig->get_hw_version()==6){
		Serial.println("get motor status via i2c");
		int sensorAddress = 0x60;  // From datasheet sensor address is 0x91 shift the address 1 bit right
		uint8_t i2c_buffer[10];

		/////////////////////// READ //////////////
		I2c.write(sensorAddress,0x81);
		Serial.println("read");
		I2c.read(sensorAddress,8,i2c_buffer);
		Serial.println("received:");

		for(int i=0; i<8; i++){
			Serial.print("#");
			Serial.print(i);
			Serial.print(":");
			Serial.println(int(i2c_buffer[i]));
		}

		for(int i=0; i<5; i++){
			*(((unsigned char *) TMC222Status)+i)=i2c_buffer[i+1];
		}

		Serial.print("SlaveAddr:");
		Serial.println(int(i2c_buffer[0]));

		Serial.print("Addr:");
		Serial.println(int(i2c_buffer[1]));

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

		Serial.println("end");
		_delay_ms(10000);
	} else {
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

		TTMC222Status TMC222Status;
		get_motor_status(&ist_pos,&delay,&status,&TMC222Status);
		char buffer[20];
		sprintf(buffer,"%06i",ist_pos);
		pOLED->string(pSpeedo->default_font,buffer,7,1);
		sprintf(buffer,"%06i",delay);
		pOLED->string(pSpeedo->default_font,buffer,7,2);
		sprintf(buffer,"%06i",status);
		pOLED->string(pSpeedo->default_font,buffer,7,3);
	};
};



/*******************************************************************************
  Example library for using a TMC222 together with AVR processors that have a
  hardware TWI interface (e.g. ATmega8, ATmega48, ATmega88, ATmega168,
  ATmega16, ATmega32, ATmega64, ATmega128, AT90CAN128 and many more).

  It implements and interrupt driven TWI master communication over the TWI
  of the AVR MCU and utilizes this for communication with the TMC222.
  Routines for all TMC222 commands are provided to make it easy to use the
  TMC222.

	It has been developed using the AVR GCC (WinARM distribution).

   Copyright (C) 2008 TRINAMIC Motion Control GmbH & Co KG
                      Sternstraße 67
                      D - 20357 Hamburg, Germany
                      http://www.trinamic.com/

   This program is free software; you can redistribute it and/or modify it
   freely.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.

 ******************************************************************************/


#define FALSE 0
#define TRUE 1

//Useful macros for accessing single bytes of int and long variables
#define BYTE3(var) *((unsigned char *) &var+3)
#define BYTE2(var) *((unsigned char *) &var+2)
#define BYTE1(var) *((unsigned char *) &var+1)
#define BYTE0(var) *((unsigned char *) &var)

//TWI address of the TMC222
//(when only one TMC222 is connected zero can be used which
// always works (it is the broadcast address)).
#define TWI_ADDR 0x00


//Variables needed by this library to handle the TWI communication
static volatile unsigned char TWIMasterSendBuffer[9];
static volatile unsigned char TWIMasterReceiveBuffer[9];
static volatile unsigned char TWIMasterSendPtr;
static volatile unsigned char TWIMasterReceivePtr;
static volatile unsigned char TWIMasterSendCount;
static volatile unsigned char TWIMasterReceiveCount;
static volatile unsigned char TWIInUse;
static volatile unsigned char TWIErrorFlag;


//TWI interrupt handler
//Handles all states of the TWI of the AVR that can occur in master mode
ISR(TWI_vect)
{
	if(TWSR==0x08 || TWSR==0x10)  //Start condition or repeated start condition transmitted by us => we are becoming bus master!
	{
		TWDR=TWIMasterSendBuffer[0];   //Load the address byte ("SLA+W" or "SLA+R")
		TWCR=0x85;                     //Generate start condition and send the address byte
		TWIMasterSendPtr=1;
	}
	else if(TWSR==0x18 || TWSR==0x20)  //Address byte ("SLA+W") has been transmitted => send our first data byte.
	{
		TWDR=TWIMasterSendBuffer[TWIMasterSendPtr++];
		TWCR=0x85;
	}
	else if(TWSR==0x28 || TWSR==0x30)  //We can send all the other data bytes.
	{
		if(TWIMasterSendPtr<TWIMasterSendCount)     //more data bytes to be sent
		{
			TWDR=TWIMasterSendBuffer[TWIMasterSendPtr++];
			TWCR=0xC5;   //send next data byte
		}
		else
		{
			TWCR=0xD5;   //No more data bytes to be sent => Generate stop condition to release the bus
			TWIInUse=FALSE;
		}
	}
	else if(TWSR==0x38) //Bus arbitration lost => release bus immediately
	{
		TWIErrorFlag=TRUE;
		TWIInUse=FALSE;
		TWCR=0xC5;
	}
	else if(TWSR==0x40)  //Address byte ("SLA+R") has been transmitted and ACK has been received => we can now start to receive data bytes
	{
		TWIMasterReceivePtr=0;
		TWCR=0xC5;  //request first data byte
	}
	else if(TWSR==0x48)  //Address byte ("SLA+R") has been transmitted and NOT ACK has been received => abort the read process
	{
		TWIErrorFlag=TRUE;
		TWCR=0xd5;  //generate stop condition and release the bus  0x95
		TWIInUse=FALSE;
	}
	else if(TWSR==0x50)  //a data byte has been read from the slave and acknowledged
	{
		TWIMasterReceiveBuffer[TWIMasterReceivePtr++]=TWDR;
		if(TWIMasterReceivePtr<TWIMasterReceiveCount)
			TWCR=0xC5;  //at least two more data bytes to get => send ACK
		else
			TWCR=0x85;  //only one more data byte to get => send NOT ACK
	}
	else if(TWSR==0x58)  //the last data byte has been read from the slave
	{
		TWIMasterReceiveBuffer[TWIMasterReceivePtr++]=TWDR;
		TWCR=0xD5;   //generate a stop condition and release the bus  0x95
		TWIInUse=FALSE;
	}
	else  //Unknown state, just reset the interrupt flag.
	{
		TWIErrorFlag=TRUE;
		TWIInUse=FALSE;
		TWCR = 0xC5;  //Acknowledge and reset the interrupt flag
	}
}



/*****************************************************************************
   Function: InitTWI()
   Parameters: ---
   Return value: ---

   Purpose: Initialization of the TWI. This function must be called prior to
            using any other function of this library.
            Furthermore, interrupts must be activated before using any other
            function of this library (with GCC this is can be done using
            sei() ).
 ******************************************************************************/
void speedo_stepper::InitTWI(void)
{
	TWBR=64;    //TWI bit rate; do not set below 64 for use with TMC222 and 16MHz clock frequency
	TWCR=0xC5;  //Switch on TWI using interrupts

	TWIErrorFlag=FALSE;
	TWIInUse=FALSE;
}


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

	I2c.write(0x60,0x81);
	I2c.read(0x60,8,buffer);

	//Copy the data to the TMC222Status structure (if pointer not NULL)
	if(TMC222Status!=NULL){
		for(int i=0; i<5; i++){
			*(((unsigned char *) TMC222Status)+i)=buffer[i+1];
		}
	}

}


/*****************************************************************************
   Function: GetFullStatus2()
   Parameters: *TargetPosition: Pointer to variable for the target position
                                or NULL.
 *SecurePosition: Pointer to variable for the secure position
                               or NULL.

   Return value: actual position

   Purpose: Read all TMC222 position registers (using the GetFullStatus2 command).
            NULL can be used for values that are not needed.
 ******************************************************************************/
int speedo_stepper::GetFullStatus2(int *TargetPosition, int *SecurePosition)
{
//	while(TWIInUse);              //Wait until TWI is free
//
//	//Send GetFullStatus2 command (0xfc) to the TMC222
//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
//	TWIMasterSendBuffer[1]=0xfc;  //Command byte for GetFullStatus2
//	TWIMasterSendCount=2;
//	TWIInUse=TRUE;
//	TWCR=0xA5;                    //start TWI communication
//	while(TWIInUse);              //and wait until done.
//
//	//Read back 8 bytes from the TMC222
//	TWIMasterSendBuffer[0]=TWI_ADDR|0x01;  //Address byte (with read bit set)
//	TWIMasterReceiveCount=8;      //8 bytes are to be read
//	TWIInUse=TRUE;
//	TWCR=0xA5;                    //Start TWI communication
//	while(TWIInUse);              //and wait until done.
//
//	//Return the position values
//	if(TargetPosition!=NULL) *TargetPosition=(TWIMasterReceiveBuffer[3]<<8) | TWIMasterReceiveBuffer[4];
//	if(SecurePosition!=NULL) *SecurePosition=((TWIMasterReceiveBuffer[6] & 0x06)<<8) | TWIMasterReceiveBuffer[5];
//	return (TWIMasterReceiveBuffer[1]<<8) | TWIMasterReceiveBuffer[2];
	uint8_t buffer[20];

	I2c.write(0x60,0xfc);
	I2c.read(0x60,8,buffer);
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
void speedo_stepper::SetMotorParameters(TTMC222Parameters *TMC222Parameters)
{
	//	unsigned char i;
	//
	//	while(TWIInUse);              //Wait until TWI is free
	//
	//	//Send command 0x89 with the data
	//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
	//	TWIMasterSendBuffer[1]=0x89;  //Command byte for SetMotorParameters (0x89)
	//	TWIMasterSendBuffer[2]=0xff;
	//	TWIMasterSendBuffer[3]=0xff;
	//
	//	for(i=0; i<5; i++)
	//		TWIMasterSendBuffer[i+4]=*(((unsigned char *) TMC222Parameters)+i);
	//
	//	TWIMasterSendCount=9;
	//	TWIInUse=TRUE;
	//	TWCR=0xA5;                    //Start TWI communication

	TWCR &= ~(1<<TWINT);

	uint8_t Buffer[7];
	Buffer[0]=0xff;
	Buffer[1]=0xff;
	for(int i=0; i<5; i++){
		Buffer[i+2]=*(((unsigned char *) TMC222Parameters)+i);
	}
	I2c.write(0x60,0x89,Buffer,7);
}


/*****************************************************************************
   Function: SetPosition()
   Parameters: Position: Target position

   Return value: ---

   Purpose: Set the target position. This makes the motor move to the new
            target position (if the motor parameters have been set properly).
 ******************************************************************************/
void speedo_stepper::SetPosition(int Position)
{
	//	while(TWIInUse);              //Wait until TWI is free
	//	Send command 0x8b with the data
	//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
	//	TWIMasterSendBuffer[1]=0x8b;  //Command byte for SetPosition (0x8b)
	//	TWIMasterSendBuffer[2]=0xff;
	//	TWIMasterSendBuffer[3]=0xff;
	//	TWIMasterSendBuffer[4]=Position >> 8;
	//	TWIMasterSendBuffer[5]=Position & 0xff;
	//	TWIMasterSendCount=6;
	//	TWIInUse=TRUE;
	//	TWCR=0xA5;                    //Start TWI communication
	TWCR &= ~(1<<TWINT);

	uint8_t Buffer[6];
	Buffer[0]=0xff;
	Buffer[1]=0xff;
	Buffer[2]=Position >> 8;
	Buffer[3]=Position & 0xff;
	I2c.write(0x60,0x8b,Buffer,4);
	//TWCR |= (1<<TWINT);
}


/*****************************************************************************
   Function: ResetPosition()
   Parameters: ---

   Return value: ---

   Purpose: Set actual position and target position to zero (without moving the
            motor).
 ******************************************************************************/
void speedo_stepper::ResetPosition(void)
{
	//	while(TWIInUse);              //Wait until TWI is free
	//
	//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
	//	TWIMasterSendBuffer[1]=0x86;  //Command byte for ResetPosition (0x86)
	//	TWIMasterSendCount=2;
	//	TWIInUse=TRUE;
	//	TWCR=0xA5;                    //Start TWI communication
	I2c.write(0x60,0x86);
}


/*****************************************************************************
   Function: GotoSecurePosition()
   Parameters: ---

   Return value: ---

   Purpose: Drive the motor to the secure position (that has been set using
   the SetMotorParameters command).
 ******************************************************************************/
void speedo_stepper::GotoSecurePosition(void)
{
	//	while(TWIInUse);              //Wait until TWI is free
	//
	//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
	//	TWIMasterSendBuffer[1]=0x84;  //Command byte for GotoSecurePosition (0x84)
	//	TWIMasterSendCount=2;
	//	TWIInUse=TRUE;
	//	TWCR=0xA5;                    //Start TWI communication
	I2c.write(0x60,0x84);

}


/*****************************************************************************
   Function: ResetToDefault()
   Parameters: ---

   Return value: ---

   Purpose: Reset all values to the default values (from the OTP).
 ******************************************************************************/
void speedo_stepper::ResetToDefault(void)
{
	//	while(TWIInUse);              //Wait until TWI is free
	//
	//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
	//	TWIMasterSendBuffer[1]=0x87;  //Command byte for ResetToDefault (0x87)
	//	TWIMasterSendCount=2;
	//	TWIInUse=TRUE;
	//	TWCR=0xA5;                    //Start TWI communication
	I2c.write(0x60,0x87);
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
void speedo_stepper::RunInit(unsigned char VMin, unsigned char VMax, int Position1, int Position2)
{
//	while(TWIInUse);              //Wait until TWI is free
//
//	//Send RunInit command and the data
//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
//	TWIMasterSendBuffer[1]=0x88;  //Command byte for RunInit (0x88)
//	TWIMasterSendBuffer[2]=0xff;
//	TWIMasterSendBuffer[3]=0xff;
//	TWIMasterSendBuffer[4]=(VMax << 4) | (VMin & 0x0f);
//	TWIMasterSendBuffer[5]=BYTE1(Position1);
//	TWIMasterSendBuffer[6]=BYTE0(Position1);
//	TWIMasterSendBuffer[7]=BYTE1(Position2);
//	TWIMasterSendBuffer[8]=BYTE0(Position2);
//	TWIMasterSendCount=9;
//	TWIInUse=TRUE;
//	TWCR=0xA5;                    //Start TWI communication

	uint8_t Buffer[7];
	Buffer[0]=0xff;
	Buffer[1]=0xff;
	Buffer[2]=(VMax << 4) | (VMin & 0x0f);
	Buffer[3]=BYTE1(Position1);
	Buffer[4]=BYTE0(Position1);
	Buffer[5]=BYTE1(Position2);
	Buffer[6]=BYTE0(Position2);
	I2c.write(0x60,0x88,Buffer,7);
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
void speedo_stepper::HardStop(void)
{
//	while(TWIInUse);              //Wait until TWI is free
//
//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
//	TWIMasterSendBuffer[1]=0x85;  //Command byte for HardStop (0x85)
//	TWIMasterSendCount=2;
//	TWIInUse=TRUE;
//	TWCR=0xA5;                    //Start TWI communication
	I2c.write(0x60,0x85);
}


/*****************************************************************************
   Function: SoftStop()
   Parameters: ---

   Return value: ---

   Purpose: Stop the motor using the normal decelaration.
 ******************************************************************************/
void speedo_stepper::SoftStop(void)
{
//	while(TWIInUse);              //Wait until TWI is free
//
//	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
//	TWIMasterSendBuffer[1]=0x8f;  //Command byte for SoftStop (0x8f)
//	TWIMasterSendCount=2;
//	TWIInUse=TRUE;
//	TWCR=0xA5;                    //Start TWI communication
	I2c.write(0x60,0x8f);
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
