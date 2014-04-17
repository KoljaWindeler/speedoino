/*
 * debug.h
 *
 *  Created on: 02.06.2011
 *      Author: kolja
 */

#ifndef DEBUG_H_
#define DEBUG_H_

class debugging{
#undef       PARSE_DEBUG     			// ausgaben vom einlesen
#undef       PARSE_SHORT     			// ausgaben vom einlesen
#undef       TACHO_DEBUG       			// jeder schreibvorgang wird angezeigt, flackern finden
#undef       TACHO_IDEBUG      			// menustate hasn't change etc
#undef       TACHO_SMALLDEBUG 		 	// tell BT receiver what we are doing
#undef       RESET_DEBUG      		 	// status change ob nun reset enable oder nicht
#undef       SPEED_DEBUG    		   	// Die Kontraktschlüsse
#undef       TACHO_SDEBUG   		  	// only menustate
#undef       CLOCK_DEBUG     		 	// haha fehler gefunden -> nie wieder abs() nutzen
#undef       STORAGE_DEBUG   		 	// sehen ob das Datum richtig geladen wird
#undef       BLINKER_DEBUG    		 	// sehen wie der Blinkerstate ist
#undef       DZ_DEBUG         		 	// die peaks sehen
#undef       TEMP_DEBUG       		 	// i2c
#undef       MENU_DEBUG       		 	// sehen in welchem menu state man gerade ist etc
#undef       GPS_DEBUG        		 	// ausgabe wenn sich was ergibt, also vollständige pakete empfangen werden
#undef       IGPS_DEBUG       		 	// alle NMEA zeilen ansehen
#undef       NAVI_DEBUG       		 	// berechnung der distanz checken
#undef       CAN_DEBUG					// Show incoming and outgoing frames
#undef       DEBUG_TRANSFER           	// show traffic on filemanager
#undef       DEBUG_TRANSFER_INTENSIV  	// show every traffic on filemanager
#undef		 STEPPER_DEBUG				// Show stepper info
#undef		 DEMO_MODE					// show fancy demo values
#undef		 LOAD_CALC					// show calc load
#undef		 GPS_FAKE_MODE				// simply fake some data
#undef		 CONTROLLIGHTS_DEBUG		// blinker state etc
#undef		 VOLTAGE_DEBUG
#undef		 AIR_DEBUG
#undef 	 	 WATER_DEBUG
#undef		 OIL_DEBUG
#undef		 SPEED_DEBUG

public:
	debugging(void);
	~debugging(void);
	void parse_float(int state,char buffer[],int i,int j,int k);
	void parse_int();
	void parse(int state,char buffer[]);
	void parse_a(int state,char buffer[]);
	void parse_ul(int state,unsigned long wert);
	void speedo_loop(int state,int intensive,unsigned long previousMillis,const char char_buffer[]);
	void loop();
	void sprintp(const char *data);
	void sprintlnp(const char *data);
};
extern debugging* pDebug;
#endif /* DEBUG_H_ */
