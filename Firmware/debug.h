/*
 * debug.h
 *
 *  Created on: 02.06.2011
 *      Author: kolja
 */

#ifndef DEBUG_H_
#define DEBUG_H_

class debugging{
#define       PARSE_DEBUG     false	// ausgaben vom einlesen
#define       PARSE_SHORT     false	// ausgaben vom einlesen
#define       TACHO_DEBUG     false   // jeder schreibvorgang wird angezeigt, flackern finden
#define       TACHO_IDEBUG    false   // menustate hasn't change etc
#define       RESET_DEBUG     false   // status change ob nun reset enable oder nicht
#define       SPEED_DEBUG     false   // Die Kontraktschlüsse
#define       TACHO_SDEBUG    false   // only menustate
#define       CLOCK_DEBUG     false   // haha fehler gefunden -> nie wieder abs() nutzen
#define       STORAGE_DEBUG   false   // sehen ob das Datum richtig geladen wird
#define       BLINKER_DEBUG   false   // sehen wie der Blinkerstate ist
#define       DZ_DEBUG        false   // die peaks sehen
#define       TEMP_DEBUG      false   // i2c
#define       MENU_DEBUG      false   // sehen in welchem menu state man gerade ist etc
#define       SD_DEBUG        false   // sehen wann/wohin und wie lange die sd karte schreibt
#define       GPS_DEBUG       false   // ausgabe wenn sich was ergibt, also vollständige pakete empfangen werden
#define       IGPS_DEBUG      false   // alle NMEA zeilen ansehen
#define       NAVI_DEBUG      false   // berechnung der distanz checken


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
