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


speedo_filemanager_v2::speedo_filemanager_v2(){};

speedo_filemanager_v2::~speedo_filemanager_v2(){};


// hier kommt man rein, wenn in der hauptschleife festgestellt wurde
// das sich im seriellen Puffer daten befinden ... das koennte
// a) ein Command zum "links" sein
// b) quatsch
// nach "links" geht er mit isLeave=1 raus

void speedo_filemanager_v2::parse_command(){
	// Direct start in, cause start was already fetched //msgParseState	=	ST_START;
	unsigned char	msgParseState	=	ST_GET_SEQ_NUM;
	unsigned int	ii				=	0;
	unsigned char	checksum		=	MESSAGE_START;
	unsigned char	seqNum			=	1;
	unsigned int	msgLength		=	0;
	unsigned int	msgStartCounter	=	0;
	unsigned int	timeout			=	0;
	unsigned char	msgBuffer[285];
	unsigned char	c;
	unsigned char   isLeave = 0;
	unsigned char 	last_file[22];
	unsigned long last_file_seek=-1; // max
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

			} else if(msgBuffer[0]==CMD_LEAVE_FM){
				isLeave	=	1;

				/////////////////////////// SIGN ON ///////////////////////////////////////
				////////////////////////// UP DOWN LEFT RIGHT /////////////////////////////

			} else if(msgBuffer[0]==CMD_GO_LEFT){
				pMenu->go_left(true); // i wait on main loop, i won't update it myself
				isLeave	=	1;
				msgLength		=	2;
				msgBuffer[0]	= 	CMD_GO_LEFT;
				msgBuffer[1] 	=	STATUS_CMD_OK;

			} else if(msgBuffer[0]==CMD_GO_RIGHT){
				pMenu->go_right(true); // i wait on main loop, i won't update it myself
				isLeave	=	1;
				msgLength		=	2;
				msgBuffer[0]	= 	CMD_GO_RIGHT;
				msgBuffer[1] 	=	STATUS_CMD_OK;

			} else if(msgBuffer[0]==CMD_GO_UP){
				pMenu->go_up(true); // i wait on main loop, i won't update it myself
				isLeave	=	1;
				msgLength		=	2;
				msgBuffer[0]	= 	CMD_GO_UP;
				msgBuffer[1] 	=	STATUS_CMD_OK;

			} else if(msgBuffer[0]==CMD_GO_DOWN){
				pMenu->go_down(true); // i wait on main loop, i won't update it myself
				isLeave	=	1;
				msgLength		=	2;
				msgBuffer[0]	= 	CMD_GO_DOWN;
				msgBuffer[1] 	=	STATUS_CMD_OK;

				////////////////////////// UP DOWN LEFT RIGHT /////////////////////////////
				///////////////////////////// COMMAND DIR ////////////////////////////////

			} else if(msgBuffer[0]==CMD_DIR) {
				/* msgBuffer[0]==CMD_DIR
				 * msgBuffer[1]==NR_HIGH
				 * msgBuffer[2]==NR_LOW
				 * msgBuffer[3]==G
				 * msgBuffer[4]==P
				 * msgBuffer[5]==S
				 *
				 * messageLength=6
				 *
				 * rueckweg:
				 * msgBuffer[0]=CMD_DIR
				 * msgBuffer[1]=COMMAND_OK
				 * msgBuffer[2]=STATUS (File 1, DIR 2, Ende 0)
				 * msgBuffer[3]=filesize high uint_32
				 * msgBuffer[4]=filesize 2nd uint_32
				 * msgBuffer[5]=filesize 3rd uint_32
				 * msgBuffer[6]=filesize 4th uint_32
				 * msgBuffer[7..x]=DATA
				 */

				// buffer for dir name
				unsigned char dir[msgLength];
				// copy reqested dir
				for(unsigned int i=0;i<msgLength-3;i++){
					dir[i]=msgBuffer[i+3];
				}
				// add end of string
				dir[msgLength-3]='\0';

				// get item id
				int item=(msgBuffer[1]<<8)|msgBuffer[2];

				// open root and maybe go on
				SdFile fm_handle;
				int status=0;
				unsigned long size=0;
				// get filename and type of item
				unsigned char name[13];
				// write the returner
				msgBuffer[0]=CMD_DIR;
				msgBuffer[1]=STATUS_CMD_OK;

				if(get_file_handle(dir,last_file,&fm_file,&fm_handle,O_READ)<0){
					status=-1;
				} else {
					last_file[0]='\0';// remove from cache to avoid reuse

					status=fm_handle.lsJKWNext(name,item,&size);
					fm_handle.close();
				}

				if(status>0) {
					msgBuffer[2]=status;
					msgBuffer[3]=(size&0xFF000000)>>24; // filesize high nibble
					msgBuffer[4]=(size&0x00FF0000)>>16; // filesize 2nd nibble
					msgBuffer[5]=(size&0x0000FF00)>>8; // filesize 3rd nibble
					msgBuffer[6]=(size&0x000000FF); // filesize 4th nibble
					int i=0;
					while(name[i]!='\0'){
						msgBuffer[i+7]=name[i];
						i++;
					};
					msgLength		=	i+2+1+4; // i==anzahl an zeichen fuer name + 1 fuer type {1/2} + 2 fuer cmd/ok + 4 fuer filesize
				} else {
					msgLength		= 	3;
					msgBuffer[2]	= 	STATUS_EOF;
				};
				///////////////////////////// COMMAND DIR ////////////////////////////////
				////////////////////////////// GET FILE /////////////////////////////////

				/* hinweg:
				 * msgBuffer[0]=CMD_GET_FILE
				 * msgBuffer[1]=length of filename
				 * msgBuffer[2..]=filename  ... datei.txt oder folder/datei.txt
				 * msgBuffer[x]=high_nibble of cluster nr
				 * msgBuffer[x+1]=low_nibble of cluster nr

				 *
				 * rueckweg:
				 * msgBuffer[0]=CMD_GET_FILE
				 * msgBuffer[1]=COMMAND_OK
				 * msgBuffer[2..]=DATA
				 */

			} else if(msgBuffer[0]==CMD_GET_FILE){
				/* hinweg:
				 * msgBuffer[0]=CMD_GET_FILE
				 * msgBUffer[1]=length of filename
				 * msgBuffer[2..1+length_of_filename]=filename  ... datei.txt oder folder/datei.txt
				 * msgBuffer[length_of_filename+2]=high_nibble of cluster nr
				 * msgBuffer[length_of_filename+3]=low_nibble of cluster nr
				 *
				 * e.G.
				 * msgBuffer[0]=CMD_GET_FILE
				 * msgBuffer[1]=3
				 * msgBuffer[2]='A'
				 * msgBuffer[3]='S'
				 * msgBuffer[4]='D'
				 * msgBuffer[5]= high
				 * msgBuffer[6]= low
				 *
				 * rueckweg:
				 * msgBuffer[0]=CMD_GET_FILE
				 * msgBuffer[1]=COMMAND_OK
				 * msgBuffer[2..]=DATA
				 */

				// 1. checken: haben wir im t2a_file noch was offen
				// 2. checken: ist der dateiname noch der gleiche wie der, den wir bekommen haben?
				// 3. wenn nicht: t2a_dir soll erst root oeffnen und dann eventuell unterverzeichnisse
				// 4. dann setseek()
				// 5. while() bla bla copy inhalt zur msg

				bool file_already_open=true;
				bool file_open_failed=false;
				bool file_seek_failed=false;
				unsigned int length_of_filename=msgBuffer[1];

				// move filename
				for(unsigned int i=0;i<length_of_filename;i++){
					msgBuffer[i]=msgBuffer[i+2]; // make it openable for get_file_handle
				}
				msgBuffer[length_of_filename]='\0';

				if(fm_file.isFile()){
					for(unsigned int i=0;i<length_of_filename;i++){
						if(last_file[i]!=msgBuffer[i]){
							file_already_open=false;
						}
					}
				} else {
					file_already_open=false;
				};

				// wenn die datei noch nicht geoeffnet ist,
				// muessen wir
				// 1. checken ob sie in einem unterverzeichniss liegt
				// 1.1. wenn ja verzeichniss namen auslesen
				// 1.2. subverzeichniss oeffnen
				// 2. Dateiname auslesen
				// 3. Dateihandle oeffnen
				// jetzt noch verschieben an die richtige stelle im buffer


				if(!file_already_open){
					if(get_file_handle(msgBuffer,last_file,&fm_file,&fm_handle,O_CREAT| O_READ)<0){
						file_open_failed=true;
					}
				} // file already open

				// setze pointer
				if(!file_open_failed){
					unsigned long pos;
					pos=msgBuffer[length_of_filename+2]<<8;
					pos|=msgBuffer[length_of_filename+3];
					pos*=250;

					if(last_file_seek!=pos){
						if(!fm_file.seekSet(pos)){
							file_seek_failed=true;
							last_file_seek=pos;
						}
					};
				};

				// wenn immer noch alles gut, dann konnten wir die Datei oeffnen und auch den Filepointer dahin setzten wo er hin soll
				if(!file_open_failed && !file_seek_failed){
					//Serial.println("file_seek OK");
					int n=fm_file.read(msgBuffer, sizeof(byte)*250); // 250

					if(n > 0) { // 250 Byte happen
						// jetzt noch verschieben an die richtige stelle im buffer
						for(int i=n-1; i>=0; i--){
							msgBuffer[i+2]=msgBuffer[i];
						};
						msgLength=n+2; // n buchstaben + cmd + status ok = 252 im besten Fall
						msgBuffer[0]=CMD_GET_FILE;
						msgBuffer[1]=STATUS_CMD_OK;
						//Serial.println("gelesen: kommt noch");

					} else {
						msgLength=2; // n buchstaben + cmd + status eof
						msgBuffer[0]=CMD_GET_FILE;
						msgBuffer[1]=STATUS_EOF;
					}
				} else if(file_seek_failed){
					msgLength=2; // n buchstaben + cmd + status failed
					msgBuffer[0]=CMD_GET_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
					last_file[0]='\0'; // damit er nicht denkt das haette geklappt
				} else {
					msgLength=2; // n buchstaben + cmd + status failed
					msgBuffer[0]=CMD_GET_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
					last_file[0]='\0'; // damit er nicht denkt das haette geklappt
				}

				////////////////////////////// GET FILE /////////////////////////////////
				////////////////////////////// PUT FILE /////////////////////////////////

				// TRANSFER VOM HANDY ZUM TACHO
				/* hinweg:
				 * msgBuffer[0]=CMD_PUT_FILE
				 * msgBuffer[1]=length of filename = X
				 * msgBuffer[2..1+X]=filename  ... datei.txt oder folder/datei.txt
				 * msgBuffer[x+2]=high_nibble of cluster nr
				 * msgBuffer[x+3]=low_nibble of cluster nr
				 * msgBuffer[X+4..250]=Content
				 *
				 * rueckweg:
				 * msgBuffer[0]=CMD_PUT_FILE
				 * msgBuffer[1]=COMMAND_OK
				 */
			} else if(msgBuffer[0]==CMD_PUT_FILE && !((msgLength==2) && (msgBuffer[1]==STATUS_EOF))){
				bool file_already_open=true;
				bool file_open_failed=false;
				bool file_seek_failed=false;
				uint16_t filename_length=msgBuffer[1];
				int start_of_payload=filename_length+4;
				for(unsigned int i=0;i<filename_length;i++){
					msgBuffer[i]=msgBuffer[i+2]; // make it suiteable for get_file_handle
					if(last_file[i]!=msgBuffer[i]){
						file_already_open=false;
					}
				}
				msgBuffer[filename_length]='\0';

				if(fm_file.isFile()){
					for(unsigned int i=0;i<filename_length;i++){
						if(last_file[i]!=msgBuffer[i]){
							file_already_open=false;
						}
					}
				} else {
					file_already_open=false;
				};


				// wenn die datei noch nicht geoeffnet ist,
				// muessen wir
				// 1. checken ob sie in einem unterverzeichniss liegt
				// 1.1. wenn ja verzeichniss namen auslesen
				// 1.2. subverzeichniss oeffnen
				// 2. Dateiname auslesen
				// 3. Dateihandle oeffnen

				if(!file_already_open){
					if(get_file_handle(&msgBuffer[0],&last_file[0],&fm_file,&fm_handle, O_CREAT| O_RDWR | O_SYNC | O_APPEND)<0){
						file_open_failed=true;
					}
				}

				// setze pointer
				if(!file_open_failed){
					int pos;
					pos=msgBuffer[filename_length+2]<<8;
					pos|=msgBuffer[filename_length+3];
					//					if(!fm_file.seekSet(pos*(msgLength-offset))){ // das ist noch totaler mist, das hier kein Seeken moeglich ist
					//						file_seek_failed=true;
					//					} else {
					//						//////////
					//						sprintf(buf,"fs:%i,pos:%i ",(int)fm_file.fileSize(),pos*(msgLength-offset));
					//						pOLED->string(0,buf,0,6);
					//						_delay_ms(1000);
					//					}
					//					////////
				};

				// wenn immer noch alles gut, dann konnten wir die Datei oeffnen und auch den Filepointer dahin setzten wo er hin soll
				if(!file_open_failed && !file_seek_failed){
					//Serial.println("file_seek OK");
					for(unsigned int i=0;i<msgLength-start_of_payload;i++){ // start_of_payload wird -2 weil noch 2 byte seek info
						msgBuffer[i]=msgBuffer[i+start_of_payload]; // msgBuffer[233]=msgBuffer[253]
					}

					int n=fm_file.write(msgBuffer, msgLength-start_of_payload); // 254 - 20

					if(n > 0) { // 250 Byte happen
						msgLength=2; // cmd + status ok
						msgBuffer[0]=CMD_PUT_FILE;
						msgBuffer[1]=STATUS_CMD_OK;
					} else {
						msgLength=2; // n buchstaben + cmd + status eof
						msgBuffer[0]=CMD_PUT_FILE;
						msgBuffer[1]=STATUS_EOF;
						last_file[0]='\0'; // damit er nicht denkt das haette geklappt
					}
				} else if(file_seek_failed){
					msgLength=3; // n buchstaben + cmd + status failed
					msgBuffer[0]=CMD_PUT_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
					msgBuffer[2]='1';
					last_file[0]='\0'; // damit er nicht denkt das haette geklappt
				} else {
					msgLength=3; // n buchstaben + cmd + status failed
					msgBuffer[0]=CMD_PUT_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
					msgBuffer[2]='2';
					last_file[0]='\0'; // damit er nicht denkt das haette geklappt
				}
			} else if(msgBuffer[0]==CMD_PUT_FILE && ((msgLength==2) && (msgBuffer[1]==STATUS_EOF))){
				fm_file.sync();
				fm_file.close();
				fm_handle.sync();
				fm_handle.close();
				msgLength=2; // n buchstaben + cmd + status eof
				msgBuffer[0]=CMD_PUT_FILE;
				msgBuffer[1]=STATUS_EOF;
				last_file[0]='\0'; // damit er nicht denkt das haette geklappt
				////////////////////////////// PUT FILE /////////////////////////////////
				////////////////////////////// DEL FILE /////////////////////////////////
			} else if(msgBuffer[0]==CMD_DEL_FILE){
				/* hinweg:
				 * msgBuffer[0]=CMD_DEL_FILE
				 * msgBuffer[1]=length of filename
				 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
				 *
				 * rueckweg:
				 * msgBuffer[0]=CMD_PUT_FILE
				 * msgBuffer[1]=COMMAND_OK
				 */
				unsigned int length_of_filename=msgBuffer[1];
				for(unsigned int i=0; i<length_of_filename; i++){ // move it for get_file_handle
					msgBuffer[i]=msgBuffer[i+2];
				}
				msgBuffer[length_of_filename]='\0';
				get_file_handle(msgBuffer,&last_file[0],&fm_file,&fm_handle, O_CREAT| O_WRITE);
				if(fm_file.remove()){
					fm_file.close();
					fm_handle.close();
					msgLength=2; // cmd + status ok
					msgBuffer[0]=CMD_DEL_FILE;
					msgBuffer[1]=STATUS_CMD_OK;
				} else {
					msgLength=2; // cmd + status ok
					msgBuffer[0]=CMD_DEL_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
				}
				////////////////////////////// DEL FILE /////////////////////////////////
				////////////////////////////// SHOW GFX /////////////////////////////////
				// TRANSFER VOM HANDY ZUM TACHO
				// incoming:
				// [0]CMD_SHOW_GFX
				// [1..(msgLength-1)]CMD_SHOW_GFX
				// outgoing
				// [0]CMD_SHOW_GFX
				// [1]STATUS
			} else if(msgBuffer[0]==CMD_SHOW_GFX){
				for(unsigned int i=0;i<msgLength-1;i++){
					msgBuffer[i]=msgBuffer[i+1]; // shift it from 1..(msgLength-1) to 0..(msgLength-2)
				}
				msgBuffer[msgLength-1]=0x00; // set stopper

				//assume that its alright
				pMenu->state=57111; // 11 will refresh the speedo every second
				unsigned char fast_reply_buffer[3];
				msgLength=2; // cmd + status ok
				fast_reply_buffer[0]=CMD_SHOW_GFX;
				fast_reply_buffer[1]=STATUS_CMD_OK;
				send_answere(fast_reply_buffer,msgLength,&seqNum,&msgParseState);
				answere_transmitted=true;

				// show gif/jpg
				pOLED->show_animation(msgBuffer);
				////////////////////////////// SHOW GFX /////////////////////////////////
				////////////////////////// SEND SMALL AVR TO BOOTLOADER /////////////////
			} else if(msgBuffer[0]==CMD_RESET_SMALL_AVR){
				msgLength=2; // cmd + status ok
				msgBuffer[0]=CMD_RESET_SMALL_AVR;
				msgBuffer[1]=STATUS_CMD_OK;
				send_answere(msgBuffer,msgLength,&seqNum,&msgParseState);
				answere_transmitted=true;

				pAktors->run_reset_on_ATm328(RESET_COMPLETE); //
				pMenu->state=11; // speedo
				pMenu->display();
				////////////////////////// SEND SMALL AVR TO BOOTLOADER /////////////////
				////////////////////////// USE THIS AS STARTUP ANIMATION /////////////////
			} else if(msgBuffer[0]==CMD_SET_STARTUP){
				// incoming:
				// [0]CMD_SET_STARTUP
				// [1..(msgLength-1)]CMD_SET_STARTUP
				// outgoing
				// [0]CMD_SET_STARTUP
				// [1]STATUS
				for(unsigned int i=1;i<msgLength && i<200;i++){
					pOLED->startup[i-1]=msgBuffer[i];
				}
				pOLED->startup[msgLength]=0x00; // set stopper

				msgLength=2;
				msgBuffer[1]=STATUS_CMD_FAILED;
				pConfig->storage_outdated=true;
				if(pConfig->write("BASE.TXT")==0){
					msgBuffer[1]=STATUS_CMD_OK;
				}
				////////////////////////// USE THIS AS STARTUP ANIMATION /////////////////
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
	fm_file.close();
	fm_handle.close();
	pSensors->m_reset->set_active(false,true);
}; // fkt ende


// fkt to reply on incoming messages
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


int speedo_filemanager_v2::get_file_handle(unsigned char *msgBuffer,unsigned char *last_file,SdFile *fm_file,SdFile *fm_handle,uint8_t flags){
	/* msgBuffer[0..X] =Filename
	 */
	// gibt den pointer auf die datei -> FM_FILE
	// und das zugehörigen Verzeichniss -> FM_HANDLE
	// zurück

	unsigned int start_of_real_filename=0;		// pos of the "FILE"name
	unsigned int length_of_filename=strlen((char*)msgBuffer);
	char filename[13];
	char subdir[13];
	subdir[0]='\0';
	unsigned char subdir_pointer=0;

	fm_handle->close();
	fm_file->close();
	fm_handle->openRoot(&pSD->volume);

	// check filename for subdirs and open them one by one
	for(unsigned int i=0;i<length_of_filename;i++){
		// we have to destinguish between normal chars and the '/'
		if(msgBuffer[i]=='/'){ // open actual subdir
			//Serial.print("Opening subdir:");
			//Serial.println(subdir);
			start_of_real_filename=i+1;
			if(subdir[0]!='\0'){ // check if we have something .. just error prevention
				SdFile returner;
				//				Serial.print("open dir:");
				//				Serial.println(subdir);
				if(!returner.open(fm_handle, subdir, O_READ)){ // create it, if it is not existing
					//Serial.println("Failed");
					if((flags && O_CREAT)==0){
						return -2;
					}
					//Serial.println("creating");
					if(!returner.mkdir(fm_handle,subdir,true)){
						//Serial.println("Failed!");
						return -1; // could not create it
					}

				}
				//Serial.println("passing on");
				*fm_handle=returner; // pass on handle
				subdir[0]='\0';
				subdir_pointer=0;
			}
		} else { // regular char
			subdir[subdir_pointer]=msgBuffer[i];
			if(subdir_pointer<10) subdir_pointer++; // dir should be max 8 chars ..
			subdir[subdir_pointer]='\0';
		}
	}

	if(msgBuffer[length_of_filename-1]!='/'){ // if the very last char in the filename NOT equals "/" --> then its a file
		// dateinamen auslesen
		for(unsigned int i=start_of_real_filename;i<length_of_filename;i++){
			filename[i-start_of_real_filename]=msgBuffer[i];
			last_file[i-start_of_real_filename]=msgBuffer[i];
			if(i==(length_of_filename-1)){
				filename[i-start_of_real_filename+1]='\0';
				last_file[i-start_of_real_filename+1]='\0';
			};
		};

		SdFile temp_f;
		// datei oeffnen
		//		Serial.print("open file:");
		//		Serial.println(filename);
		if (!temp_f.open(fm_handle, filename,flags)){
			//			Serial.println("failed");
			return -1;
		} else {
			//			Serial.println("passed");
			*fm_file=temp_f;
		}
	}
	return length_of_filename;
}
