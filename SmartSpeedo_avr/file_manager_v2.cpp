/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2011 Kolja Windeler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"


speedo_filemanager_v2::speedo_filemanager_v2(){
//	msgParseState=0;
};

speedo_filemanager_v2::~speedo_filemanager_v2(){};


// hier kommt man rein, wenn in der hauptschleife festgestellt wurde
// das sich im seriellen Puffer daten befinden ... das koennte
// a) ein Command zum "links" sein
// b) quatsch
// nach "links" geht er mit isLeave=1 raus

void speedo_filemanager_v2::check_input(){
	if(Serial.available()>100){ // wenns zuviele sind flushen
		Serial.flush();
	} else if(Serial.available()>0){ // an sonsten gern
		if(Serial.read()==MESSAGE_START){
			parse_command();
		};
	};
}

void speedo_filemanager_v2::parse_command(){
	// Direct start in, cause start was already fetched //msgParseState	=	ST_START;
	msgParseState	=	ST_GET_SEQ_NUM;
	seqNum			=	1;
	unsigned int	ii				=	0;
	unsigned char	checksum		=	MESSAGE_START;
	unsigned int	msgLength		=	0;
	unsigned int	msgStartCounter	=	0;
	unsigned int	timeout			=	0;
	unsigned char	msgBuffer[285];
	unsigned char	c;
	unsigned char   isLeave = 0;
	bool 			answere_transmitted=false;
	pSensors->m_reset->set_deactive(false,true);

	//*	main loop
	while (!isLeave){
		/*
		 * Collect received bytes to a complete message
		 */


		while ( (msgParseState != ST_PROCESS)  && (isLeave!=1) ){

			// solange keine Daten an der Schnittstelle anliegen
			// und wir auch noch keine WAIT_MS_FOR_DATA gewartet haben
			while(Serial.available()==0 && timeout<WAIT_MS_FOR_DATA){
				timeout++;
				if(timeout>=WAIT_MS_FOR_DATA){
					isLeave=1;					// exit the while loop on tops
					break;						// exit this while loop
				} else {
					_delay_ms(1);
				}
			};

			if(isLeave!=1){
				//////////////////
#ifdef DEBUG_TRANSFER
				pOLED->string(0,"-",0,1);
				_delay_ms(500);
#endif
				//////////////////
				c = Serial.read();
				timeout = 0;

				switch (msgParseState){
				case ST_START:
					//					pOLED->animation(3);
					//					pOLED->string(VISITOR_SMALL_1X_FONT,"LOADING...",5,0);

					if ( c == MESSAGE_START ){
						//////////////////
#ifdef DEBUG_TRANSFER
						pOLED->string(0,"1",0,1);
						_delay_ms(500);
#endif
#ifdef DEBUG_TRANSFER_INTENSIV
						Serial.println("MSG gehalten");
#endif
						//////////////////
						msgParseState	=	ST_GET_SEQ_NUM;
						checksum		=	MESSAGE_START;
						msgStartCounter	=	0;
					} else {
						msgStartCounter++;
						// wenn binnen 300 Bytes kein MESSAGE_START kommt Abbruch
						if(msgStartCounter>300){
							isLeave=1;
							break;
						}
					}
					break;

				case ST_GET_SEQ_NUM:
					if ((c == 1) || (c == seqNum)){
						//////////////////
#ifdef DEBUG_TRANSFER
						pOLED->string(0,"2",0,1);
						_delay_ms(500);
#endif
#ifdef DEBUG_TRANSFER_INTENSIV
						Serial.print("seq nr: ");
						Serial.print(c,DEC);
						Serial.println(" erhalten");
#endif
						//////////////////
						seqNum			=	c;
						msgParseState	=	ST_MSG_SIZE;
						checksum		^=	c;
					} else {
						msgParseState	=	ST_START;
					}
					break;

				case ST_MSG_SIZE:
					//////////////////
#ifdef DEBUG_TRANSFER
					pOLED->string(0,"3",0,1);
					_delay_ms(500);
#endif
#ifdef DEBUG_TRANSFER_INTENSIV
					Serial.print("MSG size:");
					Serial.println(c,DEC);
#endif
					//////////////////
					msgLength		=	c<<8;
					msgParseState	=	ST_MSG_SIZE_2;
					checksum		^=	c;
					break;

				case ST_MSG_SIZE_2:
					//////////////////
#ifdef DEBUG_TRANSFER
					pOLED->string(0,"4",0,1);
					_delay_ms(500);
#endif
#ifdef DEBUG_TRANSFER_INTENSIV
					Serial.print("MSG size:");
					Serial.println(c,DEC);
#endif
					//////////////////
					msgLength		|=	c;
					msgParseState	=	ST_GET_TOKEN;
					checksum		^=	c;
					break;

				case ST_GET_TOKEN:
					if ( c == TOKEN ){
						//////////////////
#ifdef DEBUG_TRANSFER
						pOLED->string(0,"5",0,1);
						_delay_ms(500);
#endif
#ifdef DEBUG_TRANSFER_INTENSIV
						Serial.println("Token erhalten");
#endif
						//////////////////
						msgParseState	=	ST_GET_DATA;
						checksum		^=	c;
						ii				=	0;
					} else {
						msgParseState	=	ST_START;
					}
					break;

				case ST_GET_DATA:
					//////////////////
#ifdef DEBUG_TRANSFER
					pOLED->string(0,"6",0,1);
					_delay_ms(500);
#endif
#ifdef DEBUG_TRANSFER_INTENSIV
					Serial.println("Daten erhalten");
#endif
					//////////////////
					msgBuffer[ii++]	=	c;
					checksum		^=	c;
					if (ii == msgLength ){
						msgParseState	=	ST_GET_CHECK;
					}
					break;

				case ST_GET_CHECK:
					//					char buffer[15];
					//					sprintf(buffer,"ist %i",c);
					//					pOLED->string(0,buffer,0,3);
					//					sprintf(buffer,"soll %i",checksum);
					//					pOLED->string(0,buffer,0,4);
					if ( c == checksum){
						//////////////////
#ifdef DEBUG_TRANSFER
						pOLED->string(0,"7",0,1);
						_delay_ms(500);
#endif
#ifdef DEBUG_TRANSFER_INTENSIV
						Serial.println("Checksum correct");
#endif
						//////////////////
						msgParseState	=	ST_PROCESS;
					} else {
						msgParseState	=	ST_START;
					}
					break;
				}	//	switch
			}; // if is leave != 1
		}	//	while(msgParseState!=ST_PROCESS && (isLeave!=1))
		// an dieser stelle endet die state machine des empfangens
		// hier kann sie entweder rauskommen wenn sie
		// timeout gegangen ist, dann ist ist isLeave=1
		// oder wenn sie was gueltiges empfangen hat, dann ist msgParseState ST_PROCESS


		if(isLeave!=1){
			/*
			 * Now process the STK500 commands, see Atmel Appnote AVR068
			 */
			//bool change_disp=false;
			answere_transmitted=false;

#ifdef DEBUG_TRANSFER
			char buffer[10];
			if(floor(msgBuffer[0]/16)>10){
				buffer[0]='a'+(floor(msgBuffer[0]/16)-10);
			} else {
				buffer[0]='0'+floor(msgBuffer[0]/16);
			};

			if((msgBuffer[0]%16)>10){
				buffer[1]='a'+(msgBuffer[0]%16)-10;
			} else {
				buffer[1]='0'+(msgBuffer[0]%16);
			}
			buffer[2]='\0';
			pOLED->string(0,buffer,0,1);
#endif

			/////////////////////////// SIGN ON ///////////////////////////////////////

			if(msgBuffer[0]==CMD_SIGN_ON || msgBuffer[0]==CMD_SIGN_ON_FIRMWARE){
				msgBuffer[1] 	=	STATUS_CMD_OK;
				// hier irgendwie GIT_REV reinbringen, nur wie ?! darauf kann man nicht mit GIT_REV[0] zugreifen
				char buffer[21];
				sprintf(buffer,GIT_REV);
				int i=0;
				while(buffer[i]!='\0'){
					msgBuffer[2+i]=buffer[i];
					i++;
				};
				msgLength=i+2;
				pSpeedo->reset_bak();

			} else if(msgBuffer[0]==CMD_LEAVE_FM){
				isLeave	=	1;

			} else if(msgBuffer[0]==CMD_PULSE_OILER){
				pAktors->m_oiler->send_impulse();
				/////////////////////////// SIGN ON ///////////////////////////////////////
				////////////////////////// UP DOWN LEFT RIGHT /////////////////////////////

				//			} else if(msgBuffer[0]==CMD_GO_LEFT){
				//				if(pMenu->button_links_valid){
				//					pMenu->go_left(true); // i wait on main loop, i won't update it myself
				//				}
				//				isLeave	=	1;
				//				msgLength		=	2;
				//				msgBuffer[0]	= 	CMD_GO_LEFT;
				//				msgBuffer[1] 	=	STATUS_CMD_OK;

				////////////////////////// UP DOWN LEFT RIGHT /////////////////////////////
				////////////////////////// SEND SMALL AVR TO BOOTLOADER /////////////////
			} else if(msgBuffer[0]==CMD_RESET_SMALL_AVR){
				msgLength=2; // cmd + status ok
				msgBuffer[0]=CMD_RESET_SMALL_AVR;
				msgBuffer[1]=STATUS_CMD_OK;
				send_answere(msgBuffer,msgLength,&seqNum,&msgParseState);
				answere_transmitted=true;

				pAktors->run_reset_on_ATm328(RESET_COMPLETE); //
//				pMenu->state=11; // speedo
//				pMenu->display();
				////////////////////////// SEND SMALL AVR TO BOOTLOADER /////////////////
				///////////////////////////// BOOTLOADER /////////////////////////////////
			} else if(msgBuffer[0]==CMD_GET_PARAMETER || msgBuffer[0]==CMD_SET_PARAMETER || msgBuffer[0]==CMD_SET_DEVICE_PARAMETERS){
				// jump to bootloader
				typedef void (*AppPtr_t)(void) __attribute__ ((noreturn));
				AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
				AppStartPtr();
				///////////////////////////// EMERGENCY /////////////////////////////////
			} else {
				msgLength		=	2;
				//msgBuffer[0]	=	msgBuffer[0]; // keep old command
				msgBuffer[1]	=	STATUS_CMD_UNKNOWN;
			}
			///////////////////////////// EMERGENCY /////////////////////////////////


			//////////////////////////// SEND BACK //////////////////////////////////
			if(!answere_transmitted){
				send_answere(msgBuffer,msgLength,&seqNum,&msgParseState);
			}
			//////////////////////////// SEND BACK //////////////////////////////////

		}; // if isleave!=1
	}; // while isLeave!=1
	//	pMenu->display();
	// t2a_file.close();
	pSensors->m_reset->set_active(false,true);
}; // fkt ende


// fkt to reply on incoming messages
int speedo_filemanager_v2::send_answere(unsigned char *msgBuffer,unsigned int msgLength){
	return send_answere(msgBuffer,msgLength,&seqNum,&msgParseState);
}

int speedo_filemanager_v2::send_answere(unsigned char *msgBuffer,unsigned int msgLength,unsigned char *seqNum, unsigned char *msgParseState){
	Serial.print((char)MESSAGE_START);
	unsigned char	checksum	=	MESSAGE_START^0;
	unsigned char	c, *p;

	Serial.print((char)*seqNum);
	checksum	^=	*seqNum;

	//c			=	msgLength&0x00FF;
	Serial.print((char)(msgLength>>8));
	checksum ^= (msgLength>>8);

	Serial.print((char)(msgLength&0xff));
	checksum ^= (msgLength&0xff);

	Serial.print((char)TOKEN);
	checksum ^= TOKEN;

	p	=	msgBuffer;
	while(msgLength){
		c	=	*p++;
		Serial.print((char)c);
		checksum ^=c;
		msgLength--;
	}
	Serial.print((char)checksum);

	*seqNum=*seqNum+1;
	*msgParseState = ST_START;
	return 0;
}

