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
#define	DEBUG_TRANSFER 0
#define	DEBUG_TRANSFER_INTENSIV 0

speedo_filemanager_v2::speedo_filemanager_v2(){};

speedo_filemanager_v2::~speedo_filemanager_v2(){};

//////////////////////////////////////// CHANGE DIRECTORY /////////////////////
//bool speedo_filemanager_v2::cd(char dir[20]){
//	SdFile sub1;
//	//Serial.print("Soll verzeichniss wechseln:"); Serial.print(dir[0]); Serial.print(dir[1]);  Serial.print("<-");
//	if(char(dir[0])=='.' && char(dir[1])=='.'){
//		if(!sub1.openRoot(&pSD->volume)){
//			Serial.println("ERR0");
//			return false;
//		} else {
//			//	fm_handle=sub1;
//			return true;
//		};
//	} else {
//		//if(!sub1.open(&fm_handle, dir, O_READ)) {
//		if(1){
//			Serial.println("ERR1");
//			return false;
//		} else {
//			//fm_handle=sub1;
//			return true;
//		};
//	};
//};
//////////////////////////////////////// CHANGE DIRECTORY /////////////////////
//
//
/////////////////////////////////////// DATIEI EMPFANGEN /////////////////////
//bool speedo_filemanager_v2::recv_file(char filename[13]){
//	int buf;//serieller empfangs buffer
//	int close_cou=0;
//	char char_buffer[23];
//	unsigned long num_byte=0; // anzahl zählen aahhh hier natürlich kein int!
//	unsigned long last=millis();
//	bool stop_it=false; // abbruch
//	if (!fm_file.open(&fm_handle, filename, O_CREAT| O_TRUNC | O_WRITE)){
//		pSD->error("ERR2");
//		stop_it=true;
//		return false;
//	};
//	while(!stop_it){
//		// check buttons
//		if(pMenu->button_test(false)){  stop_it=true; fm_file.close(); };
//		// check buttons
//		if(millis()-last>3000){
//			Serial.println("timeout");
//		};
//
//
//		while(Serial.available()){
//			buf=Serial.read();
//			last=millis();
//			num_byte++;
//			//Serial.print("Habe gelesen: "); Serial.println(buf,BYTE);
//			if(num_byte%100==0) { // wenns 10 sind dann
//				sprintf(char_buffer,"%04lu byte received",num_byte); // ch_buffer[23]-20-1 // max 999.999 Byte
//				pOLED->string(pSpeedo->default_font,char_buffer,0,0,0,DISP_BRIGHTNESS,0);
//				Serial.print("ACK"); // sende ACK
//			};
//
//			if(buf==0){ // eine 0 müssen wir seperat schreiben
//				if(!fm_file.write((uint8_t*)"\0", 1)){
//					Serial.print("FaiW");
//				}
//			} else if(buf!='*'){
//				// falsches closes schreiben
//				while(close_cou>0){
//					sprintf(char_buffer,"*");
//					pSD->writeString(fm_file,char_buffer);
//					close_cou--;
//				};
//
//				sprintf(char_buffer,"%c",buf);
//				if(pSD->writeString(fm_file,char_buffer)<=0){
//					digitalWrite(13,HIGH);
//				};
//				/*if(!fm_file.write((uint8_t*)buf,1)){
//          Serial.print("FaiW");
//        };*/
//				//Serial1.print(char_buffer);
//			} else {
//				//Serial.println("ein * bekommen");
//				close_cou=(close_cou+1)%3; // 0,1,2 // schiebe einen weiter -> nach *, close_cou=1, nach *, close_cou=2,  nach *, close_cou=0,
//				// das ding ist nur 0 wenn es vorher auf -> 1->2->0 lief
//				if(close_cou==0){
//					//Serial.println("Eingabe war *,schließe Datei");
//					fm_file.close();
//					//Serial.println("Datei geschlossen");
//					stop_it=true; // außen
//					return true;
//					break; // innen
//				};
//			}; // buf=?
//		};  // serial.available
//	}; // !stop_it
//	return false;
//};
/////////////////////////////////////// DATIEI EMPFANGEN /////////////////////
/////////////////////////////////////// DATIEI SENDEN /////////////////////
//bool speedo_filemanager_v2::send_file(char filename[13]){
//	if(!fm_file.open(&fm_handle, filename, O_READ)) {
//		//Serial.println("open file failed");
//		return false;
//	};
//	char *buf;
//	buf = (char*) malloc (65);
//	if (buf==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
//	else memset(buf,'\0',65);
//
//	int n;
//	while ((n = fm_file.read(buf, sizeof(byte)*64)) > 0) {
//		for(int j=0;j<n;j++){
//			Serial.print(buf[j]);
//		};
//		//delay(100); // pro 64 byte 100 ms => 64/56700 + 0,1 = 0,0011 + 0,1 = 0,1011 => 633 Byte/sec
//	};
//	Serial.println("***");
//	free(buf);
//	fm_file.close();
//	return true;
//};
/////////////////////////////////////// DATIEI SENDEN /////////////////////
/////////////////////////////////////// VERZEICHNISS DURCHSUCHEN /////////////////////
//bool speedo_filemanager_v2::ls(){
//	fm_handle.ls(LS_DATE | LS_SIZE);
//	return true;
//};
/////////////////////////////////////// VERZEICHNISS DURCHSUCHEN /////////////////////
/////////////////////////////////////// MKDIR /////////////////////
//bool speedo_filemanager_v2::mkdir(char dir[20]){
//	if (!fm_file.makeDir(&fm_handle, dir)){
//		pSD->error("failed");
//		return false;
//	} else {
//		fm_file.close();
//		return true;
//	};
//};
/////////////////////////////////////// MKDIR /////////////////////
/////////////////////////////////////// DELETE FILE /////////////////////
//bool speedo_filemanager_v2::rm(char filename[13]){
//	if (!fm_file.open(&fm_handle, filename, O_WRITE)) {
//		PgmPrint("Can't open ");
//		Serial.println(filename);
//		pSD->error("file.open failed");
//		return false;
//	} else {
//		if (!fm_file.remove()){
//			pSD->error("file.remove failed");
//			return false;
//		} else {
//			fm_file.close();
//			return true;
//
//		};
//	};
//};
/////////////////////////////////////// DELETE FILE /////////////////////
/////////////////////////////////////// DELETE DIR /////////////////////
//bool speedo_filemanager_v2::rmdir(){
//	if (!fm_handle.rmDir()){
//		pSD->error("sub1.rmDir failed");
//		return false;
//	} else {
//		//Serial.println("dir removed");
//		return true;
//	};
//};
/////////////////////////////////////// DELETE DIR /////////////////////
/////////////////////////////////////// read filename /////////////////////
//void speedo_filemanager_v2::get_filename(char* buffer){
//	int a=0;
//	int fn_input;
//	// setze buffer für dateinamen zurück
//	for(int b=0; b<29; b++) buffer[b]=0;
//	// gib der Seriellen Schnittstelle zeit zum bufferm
//	_delay_ms(100);
//	// name einlesen
//	while(Serial.available()>0 && a<29){
//		fn_input=Serial.read();
//		if(fn_input!=10){ buffer[a]=fn_input; }
//		else { a=99; };
//		a++;
//	};
//};
/////////////////////////////////////// read filename /////////////////////
/////////////////////////////////////// MAIN /////////////////////
///* mögliche Commands:
// * wTEST.TXTdatadatadatadatadatadatadata***  <- legt Datei TEST.TXT an
// * rTEST.TXT|<- sendet daten ->|***          <- überträgt Datei
// * eTEST.TXT                                 <- löscht Datei
// * l                                         <- listet Verzeichniss
// * cGFX                                      <- wechselt in Verzeichniss GFX
// * mGFX                                      <- erstellt Verzeichniss GFX
// * dGFX                                      <- löscht Verzeichniss GFX wenn leer
// * sBILD.TXT                                 <- zeigt das bild an
// * p                                         <- sendet "pilot" zurück
// * atest%i.txt,0,20,80!                      <- Zeige datei test1 bis test19 im abstand von 80 ms
// */
//void speedo_filemanager_v2::run(){
//	pDebug->sprintlnp(PSTR("file_manager activated"));
//	pSensors->m_reset->set_deactive(false,false); // deaktiviere reset aber speichere das nicht
//	pOLED->animation(3);
//	pOLED->string(pSpeedo->default_font,"started",5,4,0,DISP_BRIGHTNESS,0);
//	// open the root filesystem
//	// var //
//	int tries;
//	int input;
//
//	char *buffer;
//	buffer = (char*) malloc (30);
//	if (buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
//	else memset(buffer,'\0',30);
//
//	bool stop_it=false;
//	// schleife, warten auf input
//	if (!fm_handle.openRoot(&pSD->volume)) pSD->error("openRoot failed");
//	while(!stop_it){
//		if(Serial.available()>0){
//			input = Serial.read();
//			//////////////////// ab hier Struktur passend zur Eingabe ////////////////////
//			if(char(input)=='r'){  // || Eingabe: "rDATEI.TXT"     || Ausgabe: "<Dateiinhalt>***"
//				// Dateinamen einlesen
//				get_filename(&buffer[0]);
//				//Serial.print("Lese Datei: "); Serial.print(buffer); Serial.println(".");
//				///////////////// Ausführung /////////////////
//				tries=0;
//				while(tries<3){
//					if(send_file(buffer)) tries=99;
//					tries++;
//				};
//				if(tries==100){  pDebug->sprintlnp(PSTR("Done"));  }
//				else {        pDebug->sprintlnp(PSTR("Failed"));  };
//				///////////////// Ausführung /////////////////
//			} else if(char(input)=='w'){ // || Eingabe: "wDATEI.TXT<Dateiinhalt>***"     || Ausgabe: je 10 Zeichen ein "ACK"
//				_delay_ms(100); // 100ms => 0,1*57600=5760 Zeichen -> für 8.3 = 12 Zeichen, sollte reichen
//				// Dateinamen einlesen
//				get_filename(&buffer[0]);
//				//Serial.print("Schreibe Datei: "); Serial.print(buffer); Serial.println(".");
//				///////////////// Ausführung /////////////////
//				tries=0;
//				while(tries<3){
//					if(recv_file(buffer)) tries=99;
//					tries++;
//				};
//				if(tries==100){  pDebug->sprintlnp(PSTR("Done"));  }
//				else {        pDebug->sprintlnp(PSTR("Failed"));  };
//				///////////////// Ausführung /////////////////
//			} else if(char(input)=='l'){   // || Eingabe: "l"     || Ausgabe: <LS Ausgabe>
//				//Serial.println("ls");
//				///////////////// Ausführung /////////////////
//				tries=0;
//				while(tries<3){
//					if(ls()) tries=99;
//					tries++;
//				};
//				if(tries==100){  pDebug->sprintlnp(PSTR("Done"));  }
//				else {        pDebug->sprintlnp(PSTR("Failed"));  };
//				///////////////// Ausführung /////////////////
//				//			} else if(char(input)=='s'){   // || Eingabe: "s"     || Ausgabe: Bild auf schirm
//				//				// Dateinamen einlesen
//				//				get_filename(&buffer[0]);
//				//				///////////////// Ausführung ////////////////a
//				//				pOLED->sd2ssd(buffer);
//				//				///////////////// Ausführung /////////////////
//				//			} else if(char(input)=='a'){   // Eingabe: "asimp%i.txt,0,20,80!" für zeige datei simp1 bis simp19 im abstand von 80 ms"     || Ausgabe: Bild auf schirm
//				//				// Dateinamen einlesen
//				//				get_filename(&buffer[0]); // missbraucht zum einlesen des ganzen commandos
//				//				///////////////// Ausführung /////////////////
//				//				pOLED->show_animation(buffer);
//				//				///////////////// Ausführung /////////////////
//			} else if(char(input)=='p'){   // || Eingabe: "p"     || Ausgabe: pilot
//				Serial.print("pilot");
//			} else if(char(input)=='c'){   // || Eingabe: "cDIR"     || Ausgabe: ""
//				_delay_ms(100);
//				// Dateinamen einlesen
//				get_filename(&buffer[0]);
//				//Serial.print("cd to:"); Serial.print(buffer); Serial.println(".");
//				///////////////// Ausführung /////////////////
//				tries=0;
//				while(tries<3){
//					if(cd(buffer)) tries=99;
//					tries++;
//				};
//				if(tries==100){  pDebug->sprintlnp(PSTR("Done"));  }
//				else {        pDebug->sprintlnp(PSTR("Failed"));  };
//				///////////////// Ausführung /////////////////
//			} else if(char(input)=='m'){   // || Eingabe: "mDIR2"     || Ausgabe: ""
//				_delay_ms(100);
//				// Dateinamen einlesen
//				get_filename(&buffer[0]);
//				///////////////// Ausführung /////////////////
//				tries=0;
//				while(tries<3){
//					if(mkdir(buffer)) tries=99;
//					tries++;
//				};
//				if(tries==100){  pDebug->sprintlnp(PSTR("Done"));  }
//				else {        pDebug->sprintlnp(PSTR("Failed"));  };
//				///////////////// Ausführung /////////////////
//			} else if(char(input)=='e'){   // || Eingabe: "eDATEI.TXT"     || Ausgabe: ""
//				_delay_ms(100);
//				// Dateinamen einlesen
//				get_filename(&buffer[0]);
//				///////////////// Ausführung /////////////////
//				tries=0;
//				while(tries<3){
//					if(rm(buffer)) tries=99;
//					tries++;
//				};
//				if(tries==100){  pDebug->sprintlnp(PSTR("Done"));  }
//				else {        pDebug->sprintlnp(PSTR("Failed"));  };
//				///////////////// Ausführung /////////////////
//			} else if(char(input)=='d'){   // || Eingabe: "dDIR"     || Ausgabe: ""
//				_delay_ms(100);
//				// Dateinamen einlesen
//				get_filename(&buffer[0]);
//				///////////////// Ausführung /////////////////
//				tries=0;
//				while(tries<3){
//					if(cd(buffer)) tries=99;
//					tries++;
//				};
//				if(tries==100){ pDebug->sprintlnp(PSTR("Done"));   }
//				else {          pDebug->sprintlnp(PSTR("Failed")); };
//
//				tries=0;
//				while(tries<3){
//					if(rmdir()) tries=99;
//					tries++;
//				};
//				if(tries==100){  pDebug->sprintlnp(PSTR("Done"));  }
//				else {        pDebug->sprintlnp(PSTR("Failed"));  };
//				///////////////// Ausführung /////////////////
//				if (!fm_handle.openRoot(&pSD->volume)) pSD->error("openRoot failed");
//				// setze buffer für dateinamen zurück
//			} else if(char(input)=='?'){   // || Eingabe: "?"     || Ausgabe: "Done"  keep alive
//				Serial.print("Done");
//			} else { // wenn die Eingabe keine gültige Kommandoeingabe ist, brich den file_manager ab
//				stop_it=true;
//				fm_handle.close();
//				pDebug->sprintlnp(PSTR("file_manager deactivated"));
//			}; // else if
//		}; // if serial_available
//	}; //stop_it
//	pSensors->m_reset->restore();// stelle den status wieder her
//	free(buffer);
//}; // filemanager
/////////////////////////////////////// MAIN /////////////////////

// hier kommt man rein, wenn in der hauptschleife festgestellt wurde
// das sich im seriellen Puffer daten befinden ... das könnte
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
	unsigned char	c, *p;
	unsigned char   isLeave = 0;
	unsigned char 	last_file[22];
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
					delay(1);
				}
			};

			if(isLeave!=1){
				//////////////////
				if(DEBUG_TRANSFER){
					pOLED->string(0,"-",0,1);
					delay(500);
				}
				//////////////////
				c = Serial.read();
				timeout = 0;

				switch (msgParseState){
				case ST_START:
					//					pOLED->animation(3);
					//					pOLED->string(VISITOR_SMALL_1X_FONT,"LOADING...",5,0);

					if ( c == MESSAGE_START ){
						//////////////////
						if(DEBUG_TRANSFER){
							pOLED->string(0,"1",0,1);
							delay(500);
						}
						if(DEBUG_TRANSFER_INTENSIV){
							Serial.println("MSG gehalten");
						}
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
						if(DEBUG_TRANSFER){
							pOLED->string(0,"2",0,1);
							delay(500);
						}
						if(DEBUG_TRANSFER_INTENSIV){
							Serial.print("seq nr: ");
							Serial.print(c,DEC);
							Serial.println(" erhalten");
						}
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
					if(DEBUG_TRANSFER){
						pOLED->string(0,"3",0,1);
						delay(500);
					}
					if(DEBUG_TRANSFER_INTENSIV){
						Serial.print("MSG size:");
						Serial.println(c,DEC);
					}
					//////////////////
					msgLength		=	c;
					msgParseState	=	ST_GET_TOKEN;
					checksum		^=	c;
					break;

				case ST_GET_TOKEN:
					if ( c == TOKEN ){
						//////////////////
						if(DEBUG_TRANSFER){
							pOLED->string(0,"4",0,1);
							delay(500);
						}
						if(DEBUG_TRANSFER_INTENSIV){
							Serial.println("Token erhalten");
						}
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
					if(DEBUG_TRANSFER){
						pOLED->string(0,"5",0,1);
						delay(500);
					}
					if(DEBUG_TRANSFER_INTENSIV){
						Serial.println("Daten erhalten");
					}
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
						if(DEBUG_TRANSFER){
							pOLED->string(0,"6",0,1);
							delay(500);
						}
						if(DEBUG_TRANSFER_INTENSIV){
							Serial.println("Checksum correct");
						}
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
		// oder wenn sie was gültiges empfangen hat, dann ist msgParseState ST_PROCESS


		if(isLeave!=1){
			/*
			 * Now process the STK500 commands, see Atmel Appnote AVR068
			 */
			if(DEBUG_TRANSFER){
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
			};

			/////////////////////////// SIGN ON ///////////////////////////////////////

			if(msgBuffer[0]==CMD_SIGN_ON){
				msgBuffer[0]	= 	CMD_SIGN_ON;
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
				pMenu->go_left();
				pMenu->display();
				isLeave	=	1;
				msgLength		=	2;
				msgBuffer[0]	= 	CMD_GO_LEFT;
				msgBuffer[1] 	=	STATUS_CMD_OK;

			} else if(msgBuffer[0]==CMD_GO_RIGHT){
				pMenu->go_right();
				pMenu->display();
				isLeave	=	1;
				msgLength		=	2;
				msgBuffer[0]	= 	CMD_GO_RIGHT;
				msgBuffer[1] 	=	STATUS_CMD_OK;

			} else if(msgBuffer[0]==CMD_GO_UP){
				pMenu->go_up();
				pMenu->display();
				isLeave	=	1;
				msgLength		=	2;
				msgBuffer[0]	= 	CMD_GO_UP;
				msgBuffer[1] 	=	STATUS_CMD_OK;

			} else if(msgBuffer[0]==CMD_GO_DOWN){
				pMenu->go_down();
				pMenu->display();
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
				 */

				// buffer for dir name
				char dir[8];
				// copy reqested dir
				for(unsigned int i=3;i<msgLength;i++){
					dir[i-3]=msgBuffer[i];
				}
				// add end of string
				dir[msgLength-3]='\0';
				// get item id
				unsigned int item=(msgBuffer[1]<<8)|msgBuffer[2];

				// open root and maybe go on
				SdFile fm_handle,returner;
				fm_handle.openRoot(&pSD->volume);
				if(!(dir[0]=='/')){	// wenn es NICHT ".." ist
					returner.open(&fm_handle, dir, O_READ);
					fm_handle=returner;
				}
				// get filename and type of item
				char name[13];
				int status=fm_handle.lsJKWNext(name,item);
				returner.close();
				fm_handle.close();

				// write the returner
				msgBuffer[0]=CMD_DIR;
				msgBuffer[1]=STATUS_CMD_OK;

				if(status>0) {
					msgBuffer[2]=status;
					int i=0;
					while(name[i]!='\0'){
						msgBuffer[i+3]=name[i];
						i++;
					};
					msgLength		=	i+2+1; // i==anzahl an zeichen für name + 1 für type {1/2} + 2 für cmd/ok
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
				 * rückweg:
				 * msgBuffer[0]=CMD_GET_FILE
				 * msgBuffer[1]=COMMAND_OK
				 * msgBuffer[2..]=DATA
				 */

			} else if(msgBuffer[0]==CMD_GET_FILE){

				// 1. checken: haben wir im t2a_file noch was offen
				// 2. checken: ist der dateiname noch der gleiche wie der, den wir bekommen haben?
				// 3. wenn nicht: t2a_dir soll erst root öffnen und dann eventuell unterverzeichnisse
				// 4. dann setseek()
				// 5. while() bla bla copy inhalt zur msg

				bool file_already_open=true;
				bool file_open_failed=false;
				bool file_seek_failed=false;
				int payload_start=0;

				if(fm_file.isFile()){
					for(unsigned int i=0;i<msgLength-2;i++){
						if(last_file[i]!=msgBuffer[i+3]){
							file_already_open=false;
						}
					}
				} else {
					file_already_open=false;
				};

				// wenn die datei noch nicht geöffnet ist,
				// müssen wir
				// 1. checken ob sie in einem unterverzeichniss liegt
				// 1.1. wenn ja verzeichniss namen auslesen
				// 1.2. subverzeichniss öffnen
				// 2. Dateiname auslesen
				// 3. Dateihandle öffnen

				if(!file_already_open){
					payload_start=get_file_handle(&msgBuffer[0],&last_file[0],&fm_file,&fm_handle,O_CREAT| O_READ);
					if(payload_start<0)
						file_open_failed=true;
				} // filealready open

				// setze pointer
				if(!file_open_failed){
					int pos;
					pos=msgBuffer[payload_start]<<8;
					pos|=msgBuffer[payload_start+1];

					if(!fm_file.seekSet(pos*250)){
						file_seek_failed=true;

					}
				};

				// wenn immer noch alles gut, dann konnten wir die Datei öffnen und auch den Filepointer dahin setzten wo er hin soll
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
					msgBuffer[1]=STATUS_EOF;
					last_file[0]='\0'; // damit er nicht denkt das hätte geklappt
				} else {
					msgLength=2; // n buchstaben + cmd + status failed
					msgBuffer[0]=CMD_GET_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
					last_file[0]='\0'; // damit er nicht denkt das hätte geklappt
				}

				////////////////////////////// GET FILE /////////////////////////////////
				////////////////////////////// PUT FILE /////////////////////////////////

				// TRANSFER VOM HANDY ZUM TACHO
				/* hinweg:
				 * msgBuffer[0]=CMD_PUT_FILE
				 * msgBuffer[1]=length of filename
				 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
				 * msgBuffer[x+1]=high_nibble of cluster nr
				 * msgBuffer[x+2]=low_nibble of cluster nr
				 * msgBuffer[X+3..250]=Content
				 *
				 * rückweg:
				 * msgBuffer[0]=CMD_PUT_FILE
				 * msgBuffer[1]=COMMAND_OK
				 */
			} else if(msgBuffer[0]==CMD_PUT_FILE && !((msgLength==2) && (msgBuffer[1]==STATUS_EOF))){
				bool file_already_open=true;
				bool file_open_failed=false;
				bool file_seek_failed=false;
				int start_of_payload=0;


				if(fm_file.isFile()){
					for(unsigned int i=0;i<msgLength-2;i++){
						if(last_file[i]!=msgBuffer[i+3]){
							file_already_open=false;
						}
					}
				} else {
					file_already_open=false;
				};


				// wenn die datei noch nicht geöffnet ist,
				// müssen wir
				// 1. checken ob sie in einem unterverzeichniss liegt
				// 1.1. wenn ja verzeichniss namen auslesen
				// 1.2. subverzeichniss öffnen
				// 2. Dateiname auslesen
				// 3. Dateihandle öffnen

				if(!file_already_open){
					start_of_payload=get_file_handle(&msgBuffer[0],&last_file[0],&fm_file,&fm_handle, O_CREAT| O_RDWR | O_SYNC | O_APPEND);
					if(start_of_payload<0)
						file_open_failed=true;
				}

				// setze pointer
				if(!file_open_failed){
					int pos;
					pos=msgBuffer[start_of_payload]<<8;
					pos|=msgBuffer[start_of_payload+1];
					//					if(!fm_file.seekSet(pos*(msgLength-offset))){ // das ist noch totaler mist, das hier kein Seeken möglich ist
					//						file_seek_failed=true;
					//					} else {
					//						//////////
					//						sprintf(buf,"fs:%i,pos:%i ",(int)fm_file.fileSize(),pos*(msgLength-offset));
					//						pOLED->string(0,buf,0,6);
					//						delay(1000);
					//					}
					//					////////
				};

				// wenn immer noch alles gut, dann konnten wir die Datei öffnen und auch den Filepointer dahin setzten wo er hin soll
				if(!file_open_failed && !file_seek_failed){
					//Serial.println("file_seek OK");
					for(unsigned int i=0;i<msgLength-start_of_payload-2;i++){ // start_of_payload wird -2 weil noch 2 byte seek info
						msgBuffer[i]=msgBuffer[i+start_of_payload+2]; // msgBuffer[233]=msgBuffer[253]
					}

					int n=fm_file.write(msgBuffer, msgLength-start_of_payload-2); // 254 - 20

					if(n > 0) { // 250 Byte happen
						msgLength=2; // cmd + status ok
						msgBuffer[0]=CMD_PUT_FILE;
						msgBuffer[1]=STATUS_CMD_OK;
					} else {
						msgLength=2; // n buchstaben + cmd + status eof
						msgBuffer[0]=CMD_PUT_FILE;
						msgBuffer[1]=STATUS_EOF;
						last_file[0]='\0'; // damit er nicht denkt das hätte geklappt
					}
				} else if(file_seek_failed){
					msgLength=3; // n buchstaben + cmd + status failed
					msgBuffer[0]=CMD_PUT_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
					msgBuffer[2]='1';
					last_file[0]='\0'; // damit er nicht denkt das hätte geklappt
				} else {
					msgLength=3; // n buchstaben + cmd + status failed
					msgBuffer[0]=CMD_PUT_FILE;
					msgBuffer[1]=STATUS_CMD_FAILED;
					msgBuffer[2]='2';
					last_file[0]='\0'; // damit er nicht denkt das hätte geklappt
				}
			} else if(msgBuffer[0]==CMD_PUT_FILE && ((msgLength==2) && (msgBuffer[1]==STATUS_EOF))){
				fm_file.sync();
				fm_file.close();
				fm_handle.sync();
				fm_handle.close();
				msgLength=2; // n buchstaben + cmd + status eof
				msgBuffer[0]=CMD_PUT_FILE;
				msgBuffer[1]=STATUS_EOF;
				last_file[0]='\0'; // damit er nicht denkt das hätte geklappt
				////////////////////////////// PUT FILE /////////////////////////////////
				////////////////////////////// DEL FILE /////////////////////////////////
			} else if(msgBuffer[0]==CMD_DEL_FILE){
				get_file_handle(&msgBuffer[0],&last_file[0],&fm_file,&fm_handle, O_CREAT| O_WRITE);
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
				/* hinweg:
				 * msgBuffer[0]=CMD_SHOW_GFX
				 * msgBuffer[1]=length of filename
				 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
				 *
				 * rückweg:
				 * msgBuffer[0]=CMD_SHOW_GFX
				 * msgBuffer[1]=COMMAND_OK
				 */
			} else if(msgBuffer[0]==CMD_SHOW_GFX){
				unsigned int length_of_filename=(unsigned int)msgBuffer[1];
				char filename[13];
				for(unsigned int i=0;i<length_of_filename;i++){
					filename[i]=msgBuffer[i+2];
				}
				filename[length_of_filename]='\0';
				int return_value=pOLED->sd2ssd(filename,0);
				if(return_value==0){
					pMenu->state=9999999;
					msgLength=2; // cmd + status ok
					msgBuffer[0]=CMD_SHOW_GFX;
					msgBuffer[1]=STATUS_CMD_OK;
				} else {
					msgLength=3; // cmd + status ok
					msgBuffer[0]=CMD_SHOW_GFX;
					msgBuffer[1]=STATUS_CMD_FAILED;
					msgBuffer[2]=return_value & 0xFF;
				}
				////////////////////////////// SHOW GFX /////////////////////////////////
				///////////////////////////// EMERGENCY /////////////////////////////////

			} else {
				msgLength		=	2;
				//msgBuffer[0]	=	msgBuffer[0]; // keep old command
				msgBuffer[1]	=	STATUS_CMD_UNKNOWN;
			}
			///////////////////////////// EMERGENCY /////////////////////////////////


			//////////////////////////// SEND BACK //////////////////////////////////
			Serial.print((char)MESSAGE_START);
			checksum	=	MESSAGE_START^0;

			Serial.print((char)seqNum);
			checksum	^=	seqNum;

			//c			=	msgLength&0x00FF;
			Serial.print((char)msgLength);
			checksum ^= msgLength;

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

			seqNum++;
			msgParseState = ST_START;
			//////////////////////////// SEND BACK //////////////////////////////////

		}; // if isleave!=1
	}; // while isLeave!=1
	//	pMenu->display();
	// t2a_file.close();
	fm_file.close();
	fm_handle.close();
	pSensors->m_reset->set_active(false,true);
}; // fkt ende



int speedo_filemanager_v2::get_file_handle(unsigned char *msgBuffer,unsigned char *last_file,SdFile *fm_file,SdFile *fm_handle,uint8_t flags){
	/* msgBuffer[0] =CMD
	 * msgBuffer[1] =FN_length
	 * msgBuffer[2..2+FN_length] =Filename
	 * msgBuffer[3+FN_length..]=Data
	 */

	unsigned int start_of_filename=2;
	unsigned int start_of_real_filename=0;
	unsigned int length_of_filename=(unsigned int)msgBuffer[1];
	char filename[13];
	char subdir[13];
	fm_handle->close();
	fm_file->close();
	fm_handle->openRoot(&pSD->volume);

	// checken ob ein "/" drin ist, und die datei somit in einem verzeichniss liegt
	bool is_subdir_file=false;
	for(unsigned int i=start_of_filename;i<start_of_filename+length_of_filename;i++){
		if(msgBuffer[i]=='/'){
			is_subdir_file=true;
			subdir[i-start_of_filename]='\0';
			start_of_real_filename=i+1;
			break;
		} else {
			subdir[i-start_of_filename]=msgBuffer[i];
		}
	};


	// verzeichnissnamen auslesen und öffnen
	if(is_subdir_file){
		SdFile temp;
		if(!temp.open(fm_handle, subdir, O_READ)){
			return -2;
		}
		*fm_handle=temp; // das hier ist quatsch
	};

	// dateinamen auslesen
	for(unsigned int i=start_of_real_filename;i<start_of_filename+length_of_filename;i++){
		filename[i-start_of_real_filename]=msgBuffer[i];
		last_file[i-start_of_real_filename]=msgBuffer[i];
		if(i==(start_of_filename+length_of_filename-1)){
			filename[i-start_of_real_filename+1]='\0';
			last_file[i-start_of_real_filename+1]='\0';
		};
	};

	SdFile temp_f;
	// datei öffnen
	if (!temp_f.open(fm_handle, filename,flags)){
		return -1;
	} else {
		*fm_file=temp_f;
	}

	return start_of_filename+length_of_filename;
}
