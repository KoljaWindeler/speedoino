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

speedo_menu::speedo_menu(void){
	button_time=millis();
	button_first_push=millis();
}


speedo_menu::~speedo_menu(){
};

///// vars ////////////
#define menu_second_wait 100  // spike l?nge
unsigned int addr;
///// vars ////////////


///// ein text men? zeichnen ////////////
// go_left(update_twice)
// update_twice=true <- false, damit das update_display flag nicht gesetzt wird
// daher muss die funktion die go_left(false) aufruft, selbst display() aufrufen
// wenn man es mit go_left(true) aufruft, dann wird display autak ausgef?hrt
bool speedo_menu::go_left(bool update_twice){
	button_time=millis();
	pDebug->loop();
	unsigned char data[2];
	data[0]=CMD_GO_LEFT;
	pFilemanager_v2->send_answere(data,1);
	return true;
};

bool speedo_menu::go_right(bool update_twice){
	// menu var umsetzen
	button_time=millis();
	pDebug->loop();
	unsigned char data[2];
	data[0]=CMD_GO_RIGHT;
	pFilemanager_v2->send_answere(data,1);
	return true;
};

bool speedo_menu::go_up(bool update_twice){
	button_time=millis();
	pDebug->loop();
	unsigned char data[2];
	data[0]=CMD_GO_UP;
	pFilemanager_v2->send_answere(data,1);
	return true;
};

bool speedo_menu::go_down(bool update_twice){
	button_time=millis();
	pDebug->loop();
	unsigned char data[2];
	data[0]=CMD_GO_DOWN;
	pFilemanager_v2->send_answere(data,1);
	return true;
};

///// zyklisches abfragen der buttons ////////////
bool speedo_menu::button_test(){
	/* get hardware keys IF :
	 *  OVERALL: Only if we have a reagular startup!!
	 *   ====================================================
	 *  AND
	 *  1.) HW_KEYS_EN = true
	 *  OR
	 *  2.) HW_KEYS were true in the past and at that time any key was pressed
	 *  Because:
	 *  If a key is pressed AND at the last check no button was pushed: Button_first_push is set to the actual timestamp
	 *  by using this way, we always know since how long the button is pushed
	 *
	 *  In addition on every push the var button_time is set to the actual time stamp, so we know when we "did" something (like go_left)
	 *
	 *  So rerun this loop SLOW after MENU_BUTTON_TIMEOUT by check (button_time+menu_button_timeout)<millis()
	 *  OR
	 *  FASTER (after menu_button_fast_delay) if:
	 *  1. the first button push is longer than menu_button_fast_delay ( millis()>(button_first_push+menu_button_fast_delay ) (1.2sec)
	 *  and
	 *  2. the shorter delay is passed (millis()>(menu_button_fast_timeout+button_time)) (0.1sec)
	 */

	if((button_first_push!=0) && pSpeedo->startup_by_ignition){		// hier gehen wir nur rein wenn ein interrupt da war und einer der buttons noch gedr?ckt ist
		if((millis()>(button_time+menu_button_timeout)) ||
				((button_first_push>0 && millis()>(button_first_push+menu_button_fast_delay)) && millis()>(menu_button_fast_timeout+button_time)) ){ // halbe sek timeout
			//////////////////////// rechts ist gedr?ckt ////////////////////////
			if((PINJ & (1<<menu_button_rechts))==LOW){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_rechts");
#endif
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_rechts))==LOW){ // wenn nach der Wartezeit der button immernoch gedr?ckt ist
					go_right(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}

			//////////////////////// links ist gedr?ckt ////////////////////////
			else if(((PINJ & (1<<menu_button_links))==LOW)){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_links");
#endif
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_links))==LOW){ // wenn nach der Wartezeit der button immernoch gedr?ckt ist
					go_left(true); // update display() via menu
					return true;
					// wenn der pegel doch nicht mehr anliegt ( spike )
				} else {
					return false;
				};
			}

			//////////////////////// oben ist gedr?ckt ////////////////////////
			else if(((PINJ & (1<<menu_button_oben))==LOW)){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_oben");
#endif
				// move menu
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_oben))==LOW){ // wenn nach der Wartezeit der button immernoch gedr?ckt ist
					go_up(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}

			//////////////////////// unten ist gedr?ckt ////////////////////////
			else if(((PINJ & (1<<menu_button_unten))==LOW)){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_unten");
#endif
				// move menu
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_unten))==LOW){ // wenn nach der Wartezeit der button immernoch gedr?ckt ist
					go_down(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}
			//////////////////////// nix ist gedr?ckt ////////////////////////
			else {
				button_first_push=0;
				return false;
			};
		}; // hardware keys pressed
		// menu_button_unten zumenu_ende
	};
	// return false wenn die zeit nicht um war
	return false;
};

// h?sslich hier den interrupt eingef?gt ..
ISR(PCINT1_vect ){
	//	Serial.print("interrupt @");
	//	Serial.println(millis());
	pMenu->button_test();
};


///// zyklisches abfragen der buttons ////////////

void speedo_menu::init(){
	DDRJ &= ~((1<<menu_button_links)|(1<<menu_button_unten)|(1<<menu_button_rechts)|(1<<menu_button_oben)); //alles eing?nge
	PORTJ |= ((1<<menu_button_links)|(1<<menu_button_unten)|(1<<menu_button_rechts)|(1<<menu_button_oben)); // pullup
	PCMSK1 |= ((1<<PCINT12)|(1<<PCINT13)|(1<<PCINT14)|(1<<PCINT15));										// PCINT freischalten
	PCICR |=(1<<PCIE1);																						// PCINT Activieren

	// see if its a clock startup or a regular startup
	button_time=millis();
	button_first_push=millis();

	pDebug->sprintlnp(PSTR("Menu init done"));
};

