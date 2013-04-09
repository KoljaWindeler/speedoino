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
#include <avr/pgmspace.h>
#include <ssd0323.h>

speedo_menu::speedo_menu(void){
	button_time=millis();
	button_first_push=millis();
	update_display=false;
	just_marker_update=false;

	button_links_valid=true;
	button_rechts_valid=true;
	button_oben_valid=true;
	button_unten_valid=true;

	old_state=0; // will be overridden in init()
	state=0;	 // will be overridden in init()
	state_helper=0; // only  used in color_select_menu

}


speedo_menu::~speedo_menu(){
};

/////////////////////////////// Menus PROGMEM ///////////////////////////////
///////////////////// Main Menu /////////////////////
const char main_m_0[] PROGMEM = "1. Speedoino";   // "String 0" etc are strings to store - change to suit.
const char main_m_1[] PROGMEM = "2. Sprint clock";
const char main_m_2[] PROGMEM = "3. Tour-Assistent";
const char main_m_3[] PROGMEM = "4. -";
const char main_m_4[] PROGMEM = "5. Extended info";
const char main_m_5[] PROGMEM = "6. Customize";
const char main_m_6[] PROGMEM = "7. Basic Setup";
const char main_m_7[] PROGMEM = "8. Trip Avg Max";
const char main_m_8[] PROGMEM = "9. Refueling";
const char * const menu_main[9] PROGMEM= { main_m_0,main_m_1,main_m_2,main_m_3,main_m_4,main_m_5,main_m_6,main_m_7,main_m_8 }; 	   // change "string_table" name to suit

///////////////////// Setup /////////////////////
const char setup_m_0[] PROGMEM = "1. Gear calib.";   // "String 0" etc are strings to store - change to suit.
const char setup_m_1[] PROGMEM = "2. Speed calib.";   // "String 0" etc are strings to store - change to suit.
const char setup_m_2[] PROGMEM = "3. Display setup";
const char setup_m_3[] PROGMEM = "4. Reset memory";
const char setup_m_4[] PROGMEM = "5. Sensor source"; // can or no CAN
const char setup_m_5[] PROGMEM = "6. BT Reset state";
const char setup_m_6[] PROGMEM = "7. Water temp warn.";
const char setup_m_7[] PROGMEM = "8. Oil temp warn.";
const char setup_m_8[] PROGMEM = "9. -";
const char  * const menu_setup[9] PROGMEM= { setup_m_0,setup_m_1,setup_m_2,setup_m_3,setup_m_4,setup_m_5,setup_m_6,setup_m_7,setup_m_8 };

///////////////////// Customize /////////////////////
const char custom_m_0[] PROGMEM = "1. Load Skin";
const char custom_m_1[] PROGMEM = "2. Add info area";
const char custom_m_2[] PROGMEM = "3. Shift-Light";
const char custom_m_3[] PROGMEM = "4. Speedo-Color";
const char custom_m_4[] PROGMEM = "5. -";
const char custom_m_5[] PROGMEM = "6. -";
const char custom_m_6[] PROGMEM = "7. Set BT PIN";
const char custom_m_7[] PROGMEM = "8. -";
const char custom_m_8[] PROGMEM = "9. -";
const char  * const menu_custom[9] PROGMEM= { custom_m_0,custom_m_1,custom_m_2,custom_m_3,custom_m_4,custom_m_5,custom_m_6,custom_m_7,custom_m_8 };

///////////////////// Trips /////////////////////
const char add_info_m_0[] PROGMEM = "1. Distance";
const char add_info_m_1[] PROGMEM = "2. Time";
const char add_info_m_2[] PROGMEM = "3. Avg Speed";
const char add_info_m_3[] PROGMEM = "4. Max Speed";
const char add_info_m_4[] PROGMEM = "5. Distance & Time";
const char add_info_m_5[] PROGMEM = "6. Distance & Avg";
const char add_info_m_6[] PROGMEM = "7. Distance & Max";
const char add_info_m_7[] PROGMEM = "8. Time & Avg";
const char add_info_m_8[] PROGMEM = "9. Time & Max";
const char  * const menu_add_info[9] PROGMEM= { add_info_m_0,add_info_m_1,add_info_m_2,add_info_m_3,add_info_m_4,add_info_m_5,add_info_m_6,add_info_m_7,add_info_m_8 };

///////////////////// Customize /////////////////////
const char fade_m_0[] PROGMEM = "1. Static color";
const char fade_m_1[] PROGMEM = "2. Speed based";
const char fade_m_2[] PROGMEM = "3. RPM based";
const char fade_m_3[] PROGMEM = "4. Oil temp based";
const char fade_m_4[] PROGMEM = "5. Water temp based";
const char fade_m_5[] PROGMEM = "6. -";
const char fade_m_6[] PROGMEM = "7. -";
const char fade_m_7[] PROGMEM = "8. -";
const char fade_m_8[] PROGMEM = "9. -";
const char  * const menu_fade[9] PROGMEM= { fade_m_0,fade_m_1,fade_m_2,fade_m_3,fade_m_4,fade_m_5,fade_m_6,fade_m_7,fade_m_8 };

///////////////////// Extend info /////////////////////
const char einfo_m_0[] PROGMEM = "1. GPS info";
const char einfo_m_1[] PROGMEM = "2. CAN-BUS DTCs";
const char einfo_m_2[] PROGMEM = "3. Sensors";
const char einfo_m_3[] PROGMEM = "4. Stepper";
const char einfo_m_4[] PROGMEM = "5. -";
const char einfo_m_5[] PROGMEM = "6. Test Watchdog";
const char einfo_m_6[] PROGMEM = "7. Show Animation";
const char einfo_m_7[] PROGMEM = "8. About";
const char einfo_m_8[] PROGMEM = "9. Tetris";
const char * const menu_einfo[9] PROGMEM= { einfo_m_0,einfo_m_1,einfo_m_2,einfo_m_3,einfo_m_4,einfo_m_5,einfo_m_6,einfo_m_7,einfo_m_8 };

///////////////////// Navigation /////////////////////
const char navi_m_0[] PROGMEM = "1. Show in Speedo";   // "String 0" etc are strings to store - change to suit.
const char navi_m_1[] PROGMEM = "2. Set pointer";   // "String 0" etc are strings to store - change to suit.
const char navi_m_2[] PROGMEM = "3. Set file";   // "String 0" etc are strings to store - change to suit.
const char navi_m_3[] PROGMEM = "4. Check writes"; // hier vielleicht ein: way to ziel: koordinaten aktuell im vergleich zum ziel, aktuellen course
const char navi_m_4[] PROGMEM = "5. -";
const char navi_m_5[] PROGMEM = "6. -";
const char navi_m_6[] PROGMEM = "7. -";
const char navi_m_7[] PROGMEM = "8. -";
const char navi_m_8[] PROGMEM = "9. -";
const char  * const menu_navi[9] PROGMEM= {navi_m_0,navi_m_1,navi_m_2,navi_m_3,navi_m_4,navi_m_5,navi_m_6,navi_m_7,navi_m_8};

///////////////////// Menu title /////////////////////
const char titel_0[] PROGMEM = "= Main menu =";   // "String 0" etc are strings to store - change to suit.
const char titel_1[] PROGMEM = ""; // tacho hat keine caption
const char titel_2[] PROGMEM = ""; // sprint hat keine caption
const char titel_3[] PROGMEM = "= Navigation ="; // GPS hat keine caption
const char titel_4[] PROGMEM = "";
const char titel_5[] PROGMEM = "= Information =";
const char titel_6[] PROGMEM = "= Customize =";
const char titel_7[] PROGMEM = "= Setup =";
const char titel_8[] PROGMEM = "= Storage = ";
const char titel_9[] PROGMEM = "";
const char * const menu_titel[10] PROGMEM = { titel_0,titel_1,titel_2,titel_3,titel_4,titel_5,titel_6,titel_7,titel_8,titel_9};

/////
const char trip_setup_m_0[] PROGMEM = "Total";   // "String 0" etc are strings to store - change to suit.
const char trip_setup_m_1[] PROGMEM = "NonPermanent";
const char trip_setup_m_2[] PROGMEM = "Day";
const char trip_setup_m_3[] PROGMEM = "Tour";
const char trip_setup_m_4[] PROGMEM = "Chain/Tire";
const char trip_setup_m_5[] PROGMEM = "Fuel";
const char trip_setup_m_6[] PROGMEM = "Oiler";
const char trip_setup_m_7[] PROGMEM = "Saison";
const char trip_setup_m_8[] PROGMEM = "Board";
const char * const menu_trip_setup[9] PROGMEM = {trip_setup_m_0,trip_setup_m_1,trip_setup_m_2,trip_setup_m_3,trip_setup_m_4,trip_setup_m_5,trip_setup_m_6,trip_setup_m_7,trip_setup_m_8};


/////////////////////////////// Menus PROGMEM ///////////////////////////////
///// vars ////////////
bool		button_rechts_valid=true;
bool       	button_links_valid=true;
bool       	button_oben_valid=true;
bool       	button_unten_valid=true;

#define menu_active 0 // low active menues
#define menu_lines 7 // wieviele lines können wir darstellen 64 / 8 = 8 | 8-1(für die Headline)=7
int			menu_max=8;   // 9 eintrräge aber da das array bei 0 anfängt isses 9-1=8
int			menu_marker=0;
int			menu_start=0;
int			menu_ende=menu_lines-1;

#define menu_second_wait 100  // spike länge
int			fuel_added=90; // predefined value "added fuel"
bool		menu_preload;
unsigned int addr;
///// vars ////////////

////// bei veränderung des state => einmaliges zeichen des menüs ///////

/************************************************************ display menu ************************************************************
 * Menu structure base! This function gets called as soon as the menu_state_machine var "state" gets changed.
 * As long as this state has an "if" clause, the menu will show / do something. Sometimes just show a symbol, or a text but sometimes
 * triggers other functions. So this is, were the beauty comes from :D
 ************************************************************ display menu ************************************************************/
void speedo_menu::display(){
#ifdef MENU_DEBUG
	Serial.print("menu.display() called; at ");
	Serial.println(millis());
	Serial.print("state: ");
	Serial.println(state);
	Serial.print("menu_marker,menu_end,menu_start:");
	Serial.print(menu_marker);
	Serial.print(",");
	Serial.print(menu_ende);
	Serial.print(",");
	Serial.println(menu_start);
#endif

	// init hardware buttons
	bool button_state=true; // activate all buttons
	if(BUTTONS_OFF){ button_state=false;}  // but deactivate them if the define is active
	set_buttons(button_state,button_state,button_state,button_state); // set initially. if necessary the if structs will override us

	// init a "one line" storage
	char *char_buffer;
	char_buffer = (char*) malloc (22);
	if (char_buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed 3"));
	else memset(char_buffer,'\0',22);
	/******************************** "left" in main menu. Redirect to Speedoino ********************************************/
	if(state==0){
#ifdef MENU_DEBUG
		Serial.println("Menustate war 0, daher biege ich ihn auf SPEEDOINO um");
#endif
		state=11;
		update_display=true;
	}
	/********************************************* Show the Main menu *********************************************/
	else if(state<10) {
#ifdef MENU_DEBUG
		Serial.println("Menustate war kleiner als 10, Hauptmenue");
#endif
		// Menu vorbereiten
		draw(&menu_main[0],sizeof(menu_main)/sizeof(menu_main[0]));
	}
	/********************************************* generate GPS marker *********************************************/
	else if(state==111) {
#ifdef MENU_DEBUG
		Serial.println("Menustate war 111, daher biege ich ihn auf 11 um");
#endif
		pSensors->m_gps->note_this_place=SIMPLE_MARK;
		state=11;
	}
	/********************************************* Menu 1 - SPEEDOINO *********************************************
	 * Main Screen
	 * Initially setup
	 * Show all icons and prepare setup
	 ********************************************* Menu 1 - SPEEDOINO *********************************************/
	else if(floor(state/10)==1 || state==7311111){
		pOLED->clear_screen();
#ifdef MENU_DEBUG
		Serial.println("Menustate 00001X, Bin jetzt im Tacho menu, zeichne icons");
#endif

		pSpeedo->initial_draw_screen(); // draw symbols

		// wenn wir nicht in der navigation sind, die tasten für hoch und runter deaktiveren
		if(!pSensors->m_gps->navi_active){
			set_buttons(button_state,!button_state,!button_state,button_state); // msg only
		};
		// preview of display settings, phase and Vref
		if(state==7311111){
			set_buttons(button_state,!button_state,!button_state,!button_state);
		};
	}
	/********************************************* Menu 2 - Sprint Mode *********************************************
	 * This section only prepares the Sprint mode.
	 * Based on the main loop, pSprint->loop(); will be called
	 * So here we just have to prepare the Vars from the Sprintmode
	 ********************************************* Menu 2 - Sprint Mode *********************************************/
	else if(floor(state/10)==2){
		set_buttons(button_state,!button_state,!button_state,!button_state); // left only
#ifdef MENU_DEBUG
		Serial.println("Menustate=000002X, Bin jetzt im Sprint Tacho menu");
#endif
		pOLED->clear_screen();
		pSprint->prepare_startup();
		pSpeedo->reset_bak(); // alle disp_zeile_bak auf -99 setzen
	}
	/********************************************* Clock mode *********************************************/
	else if(state==291){ // reused state!
		pSensors->m_clock->loop();
	}


	/********************************************* Menu 3 - Start of Navigation Menu *********************************************
	 * Submenus:
	 *	31 Switch on and off navigation
	 *	32 Set current pointer within file
	 *	33 Select current file of navigation
	 *	34 Get number of written GPS points
	 ********************************************* Menu 3 -  of Navigation Menu *********************************************/
	else if(floor(state/10)==3){ // 31/10 = 3
		// Menu vorbereiten
		draw(&menu_navi[0],sizeof(menu_navi)/sizeof(menu_navi[0]));
	}
	/////////////////////////////////////////////////// Navigation an/aus schalten //////////////////////////////////////////////////
	else if(floor(state/10)==31){ // 311
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		if((state%10)==9){
			pSensors->m_gps->navi_active=true;
			pSensors->m_gps->generate_new_order();
			byte tempByte = (1 & 0xFF);
			eeprom_write_byte((uint8_t *)146,tempByte); // store navistate to eeprom ... warum auch immer wir das noch tun ... TODO!!!!
		} else if((state%10)==2){
			pSensors->m_gps->navi_active=false;
			byte tempByte = (0 & 0xFF);
			eeprom_write_byte((uint8_t *)146,tempByte); // store navistate to eeprom
		};
		state=311;
		pOLED->clear_screen();
		if(pSensors->m_gps->navi_active){
			sprintf(char_buffer," active");
		} else {
			sprintf(char_buffer,"inactive");
		};
		pOLED->string_P(pSpeedo->default_font,PSTR("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->highlight_bar(0,8*3-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(pSpeedo->default_font,PSTR("Navigation"),4,3,15,0,0);
		pOLED->string(pSpeedo->default_font,char_buffer,5,4,15,0,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("Down = inactive"),4,7,0,DISP_BRIGHTNESS,0);
	}
	/////////////////////////////////////////////////// Pointer für die Navigation ///////////////////////////////////////////////////
	else if(floor(state/10)==32){ // 32[X]
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	else if(floor(state/100)==32){ // 321[X] change values here
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		if((state%10)==2){ // runter gedrückt
			pSensors->m_gps->navi_point++;
			pConfig->storage_outdated=true;
		} else if((state%10)==9){ // hoch gedrückt
			pSensors->m_gps->navi_point--;
			if(pSensors->m_gps->navi_point<1) { pSensors->m_gps->navi_point=0; };
			pConfig->storage_outdated=true;
		};
		state=floor(state/10)*10+1; // zurückschuppsen

		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("Navi Track Pointer"),0,0,0,DISP_BRIGHTNESS,0);
		pSensors->m_gps->generate_new_order(); // vor ausgabe von pSensors->m_gps->navi_point, wenn pSensors->m_gps->navi_point nach dem knopfdruck zu hoch war wirds in dieser fkt zurück gesetzt
		sprintf(char_buffer,"#%3i:",pSensors->m_gps->navi_point);
		pOLED->string(pSpeedo->default_font,char_buffer,0,2,0,DISP_BRIGHTNESS,0);
		pOLED->string(pSpeedo->default_font,pSensors->m_gps->navi_ziel_name,7,2,0,DISP_BRIGHTNESS,0); // straße
		sprintf(char_buffer,"Long: %05i%04i",int(floor(pSensors->m_gps->navi_ziel_long/10000)),int(pSensors->m_gps->mod(pSensors->m_gps->navi_ziel_lati,10000)));
		pOLED->string(pSpeedo->default_font,char_buffer,2,6,0,DISP_BRIGHTNESS,0);
		sprintf(char_buffer,"Lati: %05i%04i",int(floor(pSensors->m_gps->navi_ziel_lati/10000)),int(pSensors->m_gps->mod(pSensors->m_gps->navi_ziel_lati,10000))); // typisch 052033224 => 5203,3224 => kann pro feld bis zu 32767 => 3.276.732.767 => 3.276° was quasi 8 mal um die welt geht
		pOLED->string(pSpeedo->default_font,char_buffer,2,7,0,DISP_BRIGHTNESS,0);
	}
	/////////////////////////////////////////////////// Dateien listen und highlighten... irgendwie ///////////////////////////////////////////////////
	else if(floor(state/10)==33){ // 33[X]
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
	} else if(floor(state/100)==33){ // 331[X]
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		// Umschalten
		if(int(state%10)==9){ // schalter nach oben, abziehen
			if(pSensors->m_gps->active_file>0){
				pSensors->m_gps->active_file--;
				pConfig->storage_outdated=true;
			}
		} else  if(int(state%10)==2){
			if(pSensors->m_gps->active_file < 9){ // wir haben 0,1,2,3,4,5,6,7,8,9 ... max 10 Files
				pSensors->m_gps->active_file++;
				pConfig->storage_outdated=true;
			}
		};
		// immer wieder zurückschuppsen
		state=floor(state/10)*10+1; // (41+2)*10+1
		pConfig->storage_outdated=true;
		pConfig->write("BASE.TXT"); // save config
		// open SD
		SdFile root;
		root.openRoot(&pSD->volume);
		SdFile subdir;
		if(!subdir.open(&root, NAVI_FOLDER, O_READ)) {  Serial.println("open subdir /config failed"); };
		// generate filename
		char navi_filename[13];
		sprintf(navi_filename,"NAVI%i.SMF",pSensors->m_gps->active_file);
		char buffer[22]; // 21 zeichen + \0
		sprintf(buffer,"Navifile Nr: %i",pSensors->m_gps->active_file);
		// file exists
		SdFile file;
		if (file.open(&subdir, navi_filename, O_READ)){ //kann ich offentsichtlich ändern, datei auslesen
			int n=1;
			int byte_read=0;
			// reserve buffer space
			char buffer_big[65];

			while (n > 0 && byte_read<63) { // n=wieviele byte gelesen wurden
				char buf[2];
				n = file.read(buf, 1);

				if(buf[0]!='\n'){
					buffer_big[byte_read]=buf[0];
					byte_read=byte_read+n;
				} else { // wenn '\n' break machen
					n=0;
					buffer_big[byte_read]='\0';
					break;
				}
			};
			// close file
			file.close();
			char title[17];
			sprintf(title,"Navifile Nr: %i/9",pSensors->m_gps->active_file);
			if(buffer_big[0]=='#'){
				pOLED->show_storry(buffer_big,byte_read,title,sizeof(title)/sizeof(title[0]));
			} else {
				pOLED->clear_screen();
				pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string(pSpeedo->default_font,title,2,0,DISP_BRIGHTNESS,0,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("No first line comment"),0,3,0,DISP_BRIGHTNESS,0);
			}
		} else { // if there is no file with that number
			pOLED->clear_screen();
			char title[17];
			sprintf(title,"Navifile Nr: %i/8",pSensors->m_gps->active_file);
			pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
			pOLED->string(pSpeedo->default_font,title,2,0,DISP_BRIGHTNESS,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("File not found"),2,3,0,DISP_BRIGHTNESS,0);
		};
		subdir.close();
		root.close();
	}
	//////////////////////// ausgeben wieviele points geschrieben wurden //////////////
	else if(floor(state/10)==34){ //34[X]
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		char buffer[10];
		sprintf(buffer,"%i",pSensors->m_gps->written_gps_points);
		pOLED->clear_screen();
		pOLED->string(pSpeedo->default_font,buffer,5,3,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("Points written"),3,4,0,DISP_BRIGHTNESS,0);
	}
	/********************************************* Menu 3 - End of Navigation Menu *********************************************/


	/********************************************* Menu 4, completly empty by now *********************************************
	 ********************************************* Menu 4, completly empty by now *********************************************/



	/********************************************* Menu 5 - Start of Extend Info Menu *********************************************
	 * Submenus:
	 *	51 Show GPS info
	 *	52 Show CAN-BUS info
	 *	53 Show extendend analog Sensor info
	 *	54 Show current Stepper position and mode info
	 *	55 -
	 *	56 Test the Watchdog, simply by waiting 30 sec without toggling
	 *	57 Show some old Animation ... obsolete?
	 *	58 Show only my eMail Adress. Some kind of About
	 *	59 Tetris :D
	 ********************************************* Menu 5 - Start of Extend Info Menu *********************************************/
	else if(floor(state/10)==5) { //5[X]
		// Menu vorbereiten
		draw(&menu_einfo[0],sizeof(menu_einfo)/sizeof(menu_einfo[0]));
	}
	//////////////////////// Einmaliges Setup des GPS Tacho ////////////////////////////
	else if(floor(state/10)==51) { // 51[X]
		set_buttons(button_state,!button_state,!button_state,!button_state); // left only
		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("GPS_time"),0,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("GPS_date"),0,1);
		pOLED->string_P(pSpeedo->default_font,PSTR("GPS_long"),0,2);
		pOLED->string_P(pSpeedo->default_font,PSTR("GPS_lati"),0,3);
		pOLED->string_P(pSpeedo->default_font,PSTR("GPS_alti"),0,4);
		pOLED->string_P(pSpeedo->default_font,PSTR("GPS_speed"),0,5);
		pOLED->string_P(pSpeedo->default_font,PSTR("GPS_sats"),0,6);
		pOLED->string_P(pSpeedo->default_font,PSTR("Counter"),0,7);

		pSpeedo->reset_bak(); // alle disp_zeile_bak auf -99 setzen
	}
	//////////////////////// CAN info ////////////////////////////
	else if(floor(state/10)==52) {
		// show info that CAN is offline
		if(!pSensors->CAN_active){
			pOLED->clear_screen();
			pOLED->string_P(pSpeedo->default_font,PSTR("CAN is not active"),2,3,0,15,0);
		}
		// get CAN info and show them
		else {
			pSpeedo->reset_bak();
			pOLED->clear_screen();
			pOLED->highlight_bar(0,0,128,8);
			pOLED->string_P(pSpeedo->default_font,PSTR("CAN DTS Codes"),2,0,15,0,0);

			int dtc_error_count=pSensors->m_CAN->get_dtc_error_count();
			if(dtc_error_count==0){
				pOLED->string_P(pSpeedo->default_font,PSTR("No errors :)"),6,4,0,15,0);
			} else {

#ifdef CAN_DEBUG
				Serial.print(dtc_error_count);
				Serial.println(" Fehler gefunden");
#endif

				for(int i=0; i<dtc_error_count && i<3;i++){
					int error_code=pSensors->m_CAN->get_dtc_error(i+3*state_helper);
					if(error_code!=-1){
						// char P=00, C=01, 10=B, 11=U
						char region=(error_code&0x3000)>>12;
						if(region==0){
							region='P';
						} else if(region==1){
							region='C';
						} else if(region==2){
							region='B';
						} else if(region==3){
							region='U';
						}

						sprintf(char_buffer,"Error %i/%i:%c%04i",i+3*state_helper+1,dtc_error_count,region,error_code&0x0fff);
						pOLED->string(pSpeedo->default_font,char_buffer,0,2*i+1,0,15,0);
						sprintf(char_buffer,"%c%04i",region,error_code&0x0fff);
						pSensors->m_CAN->decode_dtc(char_buffer,SPEED_TRIPPLE);
						center_me(char_buffer,21);
						pOLED->string(pSpeedo->default_font,char_buffer,0,2*i+2,0,7,0);
					} else {
						pOLED->string_P(pSpeedo->default_font,PSTR("COMM FAILED"),2*i+2,0,15,0,0);
					}
				} // for
			} // error count >0

			/* idea: as soon as we enter this menu, we reset the state_helper
			 * after that: every "down" push will just increase the state_helper
			 * the state should return to 521
			 * "up" is viceversa until the helper reaches 0 again
			 * Within processing we'll use the state_helper as a multiplier
			 *
			 * In addition, only activate "down" if there are more errors
			 */
			bool down=!button_state;
			if((2+3*state_helper)<dtc_error_count){
				down=button_state;
				pOLED->string_P(pSpeedo->default_font,PSTR("Press down for more"),0,7,0,15,0);
			}
			if(old_state==52){
				state_helper=0;
				set_buttons(button_state,!button_state,down,button_state); // no up
			} else if(state%10==2){ // ein runter gedrückt
				state--;
				set_buttons(button_state,button_state,down,button_state); // all
				state_helper++;
			} else if(state%10==9){ // ein hoch gedrückt (519)
				state+=2; // 519 + 2 = 521
				state_helper--;
				if(state_helper==0){
					set_buttons(button_state,!button_state,down,button_state); // no up
				}
			}
			// testen für mehr als 3 codes!! TODO
		};

	}
	//////////////////////// extend sensor info ////////////////////////////
	else if(floor(state/10)==53) {
		set_buttons(button_state,!button_state,!button_state,!button_state); // left only
		// Menu vorbereiten
		pSpeedo->reset_bak();
		pOLED->clear_screen();
		pOLED->highlight_bar(0,0,128,8);
		pOLED->string_P(pSpeedo->default_font,PSTR("Additional info"),2,0,15,0,0);
		pOLED->draw_water(10,16);
		pOLED->draw_air(20,24);
		pOLED->draw_oil(10,32);
		// values will be added by "main" loop to have a online-monitoring
	}
	//////////////////////// stepper info ////////////////////////////
	else if(floor(state/10)==54) {
		set_buttons(button_state,!button_state,!button_state,!button_state); // left only
		// Menu vorbereiten
		pSpeedo->reset_bak();
		pOLED->clear_screen();
		pOLED->highlight_bar(0,0,128,8);
		pOLED->string_P(pSpeedo->default_font,PSTR("Additional info"),2,0,15,0,0);
		// this is just the caption,
		// the loop will fill the screen
		//		 pAktors->m_stepper->go_to(MOTOR_OVERWRITE_END_POS);
	}
	//////////////////////// TEST des watchdogs durch absitzen ////////////////////////
	else if(floor(state/10)==56){ // 56[X]
		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("WAIT 30 sec"),3,3);
		pOLED->string_P(pSpeedo->default_font, PSTR("for reset"),4,4);
		_delay_ms(29999); // knapp 30 sec
	}

	//////////////////////// show animation ////////////////////////
	else if(floor(state/10)==57){ //57[x]
		pOLED->animation(state%10);
	}
	else if(floor(state/100)==57){
		state/=10;
		update_display=true;
	}

	////////////////////////////////// ABOUT //////////////////////////////////
	else if(floor(state/10)==58) { // 00058X
		set_buttons(button_state,!button_state,!button_state,!button_state); // msg only
		pOLED->clear_screen();
		pOLED->highlight_bar(0,0,128,8);
		pOLED->string_P(pSpeedo->default_font,PSTR("Speedoino"),2,0,DISP_BRIGHTNESS,0,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("GNU - Licensed"),0,1,0,DISP_BRIGHTNESS,0);

		pOLED->string_P(pSpeedo->default_font,PSTR("HW version:"),0,3,0,DISP_BRIGHTNESS,0);
		sprintf(char_buffer,"%03i",pConfig->get_hw_version());
		pOLED->string(pSpeedo->default_font,char_buffer,11,3);
		pOLED->string_P(pSpeedo->default_font,PSTR(GIT_REV),0,4,0,DISP_BRIGHTNESS,0);

		pOLED->string_P(pSpeedo->default_font,PSTR("contact me at"),0,6,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("KKoolljjaa@gmail.com"),0,7,0,DISP_BRIGHTNESS,0);
	}
	/////////////////// TETRIS //////////////////77
	else if(state==591) {
		pOLED->clear_screen();
		pOLED->highlight_bar(0,8,128,8);
		pOLED->string_P(pSpeedo->default_font,PSTR("Tetris"),6,1,15,0,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("in tribut to"),2,3,0,15,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("Alexej Padschitnow"),2,4,0,15,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("L Quit"),2,5,0,15,0);
		char temp[2];
		sprintf(temp,"%c",126);
		pOLED->string(pSpeedo->default_font,temp,2,5,0,15,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("R go for it"),2,6,0,15,0);
		sprintf(temp,"%c",127);
		pOLED->string(pSpeedo->default_font,temp,2,6,0,15,0);
	} else if(state==5911){
		update_display=true;
		state++;
	} else if(state==5912){
		PCICR &=~(1<<PCIE1); // PCINT DEactivieren
		tetris* m_tetris=new tetris;
		m_tetris->run();
		delete m_tetris;
		PCICR |=(1<<PCIE1); // PCINT Activieren
		back();
		update_display=true;
	}
	/********************************************* End of Extend Info Menu *********************************************/



	/********************************************* Menu 6 - Start of Customize Menu *********************************************
	 * Submenus:
	 *	61 Choose Skin
	 *	62 Select the shown trip
	 *	63 Adjust the DZ flasher value and color
	 *	64 Adjust outer RGB LED's, Static, KMH related ...
	 *	65 -
	 *	66
	 *	67 Set Bluetooth pin
	 *	68 -
	 *	69 -
	 ********************************************* Menu 6 - Start of Customize Menu *********************************************/
	else if(floor(state/10)==6) { //6[X]
		// Menu vorbereiten
		draw(&menu_custom[0],sizeof(menu_custom)/sizeof(menu_custom[0]));
		menu_preload=true;
		pSpeedo->reset_bak();
	}
	///////////////////////// SKIN LADEN ///////////////////////
	else if(floor(state/10)==61){ //61[X]
		// open SD
		SdFile root;
		root.openRoot(&pSD->volume);
		SdFile subdir;
		if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  Serial.println("open subdir /config failed"); };

		// generate filename
		char filename[10];
		sprintf(filename,"SKIN%i.SSF",int((state%10)-1));

		// check if file exists
		SdFile file;
		if (file.open(&subdir, filename, O_READ)){ //kann ich offentsichtlich ändern, datei auslesen
			int n=1;
			int byte_read=0;
			// reserve buffer space
			char *buffer;
			buffer = (char*) malloc (65);
			if (buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed 1"));
			else memset(buffer,'\0',65);

			while (n > 0 && byte_read<63) { // n=wieviele byte gelesen wurden
				char buf[2];
				n = file.read(buf, 1);

				if(buf[0]!='\n'){
					buffer[byte_read]=buf[0];
					byte_read=byte_read+n;
				} else { // wenn '\n' break machen
					break;
				}
			};
			// close file
			file.close();

			char title[15];
			sprintf(title,"Skinfile Nr: %i/8",int(state%10)-1);
			if(buffer[0]=='#'){
				pOLED->show_storry(buffer,byte_read,title,sizeof(title)/sizeof(title[0]));
			} else {
				pOLED->clear_screen();
				pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string(pSpeedo->default_font,title,2,0,DISP_BRIGHTNESS,0,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("No first line comment"),0,3,0,DISP_BRIGHTNESS,0);
			}
			// noch mal kurz den buffer missbrauchen
			sprintf(buffer,"  Preview -    Save");
			buffer[0]=127;
			buffer[12]=127;
			buffer[13]=127;
			pOLED->string(pSpeedo->default_font,buffer,0,7,0,DISP_BRIGHTNESS,0);
			// free buffer
			free(buffer);
			// end of draw to display
		} else { // if there is no file with that number
			pOLED->clear_screen();
			char title[17];
			sprintf(title,"Skinfile Nr: %i/8",int(state%10)-1);
			pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
			pOLED->string(pSpeedo->default_font,title,2,0,DISP_BRIGHTNESS,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("File not found"),2,3,0,DISP_BRIGHTNESS,0);
			set_buttons(button_state,button_state,button_state,!button_state); // no right
		};
		subdir.close();
		root.close();

		// keep loading current skinfile, because without changing
		sprintf(filename,"SKIN%i.TXT",(pConfig->skin_file+10)%10);
		pConfig->read(CONFIG_FOLDER,filename,READ_MODE_CONFIGFILE,"");
	}
	//////////////////////// skin laden ////////////////////////////
	else if(floor(state/100)==61) { // 0061[X]1
		// load this skinfile
		char filename[10];
		sprintf(filename,"SKIN%i.SSF",(int(floor(state/10))%10)-1);
		pConfig->read(CONFIG_FOLDER,filename,READ_MODE_CONFIGFILE,"");
		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("Preview"),6,3);
		_delay_ms(300);

		// set buttons
		set_buttons(button_state,!button_state,!button_state,button_state); // lr only

		// save,change,restore,and show one preview image
		unsigned long save_state=state;
		state=11;
		display(); // nicht schön, aber das muss sein :D
		pSpeedo->loop(0);
		state=save_state;
	}

	//////////////////////// skin speichern -> zum menüpunkt 11 gehen und alles ist gut ////////////////////////////
	else if(floor(state/1000)==61) { // 0061[XXX]
		pOLED->clear_screen();
		pConfig->skin_file=(int(floor(state/100))%10)-1;
		pConfig->storage_outdated=true;
		pConfig->write("BASE.TXT"); // save config

		pOLED->string_P(pSpeedo->default_font,PSTR("Saved"),7,3);
		_delay_ms(300);
		pOLED->clear_screen();

		state=11;
		update_display=true;
	}
	///////////////////// trip show setup //////////////////////////
	// first the sneaky storage helper by /10
	// first select to type of mode by /100
	// then select the storage by /1000
	else if(floor(state/10)==62) {
		// sneaky, wir bauen ein "zwischen zustand" ein, um einen übergang zu erzeugen
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==62) {
		if(floor(old_state)==62 || floor(old_state/1000)==62){ // wenn wir von unten kommen, waren wir vorher auf 62. wenn wir von oben kommen waren wir auf 62[1][Y][Z] = y ist mode, z ist storage
			state=10*floor(state/10)+(pSpeedo->m_trip_mode%10); // state muss angepasst werden.
			if(pSpeedo->m_trip_mode>menu_lines){  // positionierung des Markers
				menu_marker=menu_lines-1;
				menu_start=pSpeedo->m_trip_mode-menu_lines;
				menu_ende=pSpeedo->m_trip_mode-1;
			}
			else {
				menu_marker=pSpeedo->m_trip_mode-1;
				menu_start=0;
				menu_ende=menu_lines-1;
			};
		}
		else { // speichern des neuen Werts
			pConfig->storage_outdated=true;
			pSpeedo->m_trip_mode=state%10;
		};
		draw(&menu_add_info[0],sizeof(menu_add_info)/sizeof(menu_add_info[0]));
	}
	// and now select the storage
	else if(floor(state/1000)==62) {
		if(floor(old_state/100)==62){ // wenn wir von unten kommen, waren wir vorher auf 62[1][y]. y ist mode
			state=10*floor(state/10)+(pSpeedo->m_trip_storage%10); // state muss angepasst werden.
			if(pSpeedo->m_trip_storage>menu_lines){  // positionierung des Markers
				menu_marker=menu_lines-1;
				menu_start=pSpeedo->m_trip_storage-menu_lines;
				menu_ende=pSpeedo->m_trip_storage-1;
			}
			else {
				menu_marker=pSpeedo->m_trip_storage-1;
				menu_start=0;
				menu_ende=menu_lines-1;
			};
		}
		else { // speichern des neuen Werts
			pSpeedo->m_trip_storage=state%10;
			pConfig->storage_outdated=true;
		};
		draw(&menu_trip_setup[0],sizeof(menu_trip_setup)/sizeof(menu_trip_setup[0]));
	}
	///////////////////////////// dz flasher /////////////////////////////
	else if(floor(state/10)==63) {
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	else if(floor(state/100)==63 || floor(state/1000)==63){ // 631[X] or 6311[X]
		if(state!=old_state+1 && state!=old_state+8){ // 632 -> 631, take this since its possible to come from 631 and from 631111
			pOLED->clear_screen();
			pOLED->highlight_bar(0,8*4-1,128,17); // mit hintergrundfarbe nen kasten malen. zeile 3 und 4
			pOLED->string_P(pSpeedo->default_font,PSTR("DZ Flasher"),5,4,15,0,0);

			// bedienelemente anzeigen
			if(floor(state/1000)==6){
				pOLED->filled_rect(0,0,128,20,0x00);
				pOLED->string_P(pSpeedo->default_font,PSTR("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("Down = inactive"),4,1,0,DISP_BRIGHTNESS,0);

				pOLED->filled_rect(0,56,128,8,0x00);
				if(pSensors->m_dz->blitz_en){
					char temp[2];
					sprintf(temp,"%c",127);
					pOLED->string(pSpeedo->default_font,temp,0,7);
					pOLED->string_P(pSpeedo->default_font,PSTR("to adjust level"),2,7);
				};
			} else if(floor(state/10000)==6){
				pOLED->filled_rect(0,0,128,20,0x00);
				pOLED->string_P(pSpeedo->default_font,PSTR("Select the RPM"),4,0,0,DISP_BRIGHTNESS,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("for Shiftlight"),4,1,0,DISP_BRIGHTNESS,0);

				char temp[2];
				sprintf(temp,"%c",127);
				pOLED->filled_rect(0,56,128,8,0x00);
				pOLED->string(pSpeedo->default_font,temp,0,7);
				pOLED->string_P(pSpeedo->default_font,PSTR("to adjust color"),2,7);
			}
		};
		// AN AUS schaltung
		if(floor(state/100)==63){ // 631[x]
			if(state%10==9){
				pSensors->m_dz->blitz_en=true;
				pConfig->storage_outdated=true;
				state-=8;
			} else if(state%10==2){
				pSensors->m_dz->blitz_en=false;
				pConfig->storage_outdated=true;
				state-=1;
			};
			// schaltdrehzahl einstellen
		} else if(floor(state/1000)==63){
			// Wert anpassen
			if(state%10==9){
				pSensors->m_dz->blitz_dz+=100;
				if(pSensors->m_dz->blitz_dz>20000){
					pSensors->m_dz->blitz_dz=20000;
				}
				state-=8;
				pConfig->storage_outdated=true;
			} else if(state%10==2){
				pSensors->m_dz->blitz_dz-=100;
				if(pSensors->m_dz->blitz_dz<100){
					pSensors->m_dz->blitz_dz=100;
				}
				state-=1;
				pConfig->storage_outdated=true;
			};
		}


		// wenn der blitzer an ist dann drehzahl mit anzeigen
		if(pSensors->m_dz->blitz_en){
			set_buttons(button_state,button_state,button_state,button_state); // alles aktiv

			sprintf(char_buffer,"%3i00",int(floor(pSensors->m_dz->blitz_dz/100)));// 12500
			pOLED->highlight_bar(0,8*5-1,128,9); // mit hintergrundfarbe nen kasten malen. zeile 3 und 4
			if(floor(state/100)==63){
				pOLED->string_P(pSpeedo->default_font,PSTR("active"),2,5,0,15,0); // joa, unschön. Wird flackern, aber naja
				pOLED->string(pSpeedo->default_font,char_buffer,11,5,15,0,0);
			} else {
				pOLED->string_P(pSpeedo->default_font,PSTR("active"),2,5,15,0,0);  // joa, unschön. Wird flackern, aber naja
				pOLED->string(pSpeedo->default_font,char_buffer,11,5,0,15,0);
			}

			// wenn inactiv mittig "inactive" anzeigen lassen
		} else {
			set_buttons(button_state,button_state,button_state,!button_state); // no right
			pOLED->highlight_bar(0,8*5-1,128,9); // mit hintergrundfarbe nen kasten malen. zeile 3 und 4
			pOLED->string_P(pSpeedo->default_font,PSTR("inactive"),5,5,0,15,0);

		};

		//////////////////////// adjust dz alert RGB LED ////////////////////////
	} else if(floor(state/100)==6311 || floor(state/1000)==6311 ||floor(state/10000)==6311){
		color_select_menu(63111L,&pAktors->dz_flasher,0,0,0,0,button_state, PSTR("Shift-Light"),PSTR(""),99,true);
	}
	//////////////////////// adjust outer RGB LED ////////////////////////
	/* In this menu, we give the user the chance to choose his own color
	 * first of all we start with an selection model
	 * 1. Static color - just one fix color
	 * 2. Speed based color fade -> setup the minimum speed and color for the speed below this border
	 * 								and the max, plus the color above the top speed. between these limits, the color will be faded
	 *
	 */
	else if(floor(state/10)==64){
		// Menu vorbereiten
		draw(&menu_fade[0],sizeof(menu_fade)/sizeof(menu_fade[0]));
		menu_preload=true;
		pSpeedo->reset_bak();

		////////////////////////// static /////////////////
	} else if(floor(state/10)==641 || floor(state/100)==641 ||floor(state/1000)==641){
		color_select_menu(641,&pAktors->static_color,0,0,0,0,button_state, PSTR("Static color"),PSTR(""),0,true);

		/////////////////////// speed based color fade ///////////////////
	} else if(floor(state/10)==642 || floor(state/100)==642 ||floor(state/1000)==642){
		color_select_menu(642,&pAktors->kmh_start_color,&pAktors->kmh_end_color,&pAktors->kmh_min_value,&pAktors->kmh_max_value,300,button_state, PSTR("Speed"),PSTR("km\h"),1,false);

		/////////////////////// rpm based color fade ///////////////////
	} else if(floor(state/10)==643 || floor(state/100)==643 || floor(state/1000)==643){
		color_select_menu(643,&pAktors->dz_start_color,&pAktors->dz_end_color,&pAktors->dz_min_value,&pAktors->dz_max_value,150,button_state,PSTR("RPM*100"), PSTR("RPM"),2,false);

		/////////////////////// oil based color fade ///////////////////
	} else if(floor(state/10)==644 || floor(state/100)==644 || floor(state/1000)==644){
		color_select_menu(644,&pAktors->oil_start_color,&pAktors->oil_end_color,&pAktors->oil_min_value,&pAktors->oil_max_value,150,button_state, PSTR("Oil"),PSTR("TMP"),3,false);

		/////////////////////// water based color fade ///////////////////
	} else if(floor(state/10)==645 || floor(state/100)==645 || floor(state/1000)==645){
		color_select_menu(645,&pAktors->water_start_color,&pAktors->water_end_color,&pAktors->water_min_value,&pAktors->water_max_value,150,button_state, PSTR("Water"),PSTR("TMP"),4,false);

		///////////////////////// set bt pin ///////////////////////////
	} else if(floor(state/10)==67){
		// sneaky, wir bauen ein "zwischen zustand" ein, um einen übergang zu erzeugen
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
		pSpeedo->disp_zeile_bak[2]=999; // redraw everything
	}
	else if((floor(state/100)==67) | (floor(state/1000)==67) | (floor(state/10000)==67) | (floor(state/100000)==67)){ //67[x][x][x][x][1]
		if(floor(state/100000)==67){
			set_buttons(button_state,button_state,button_state,!button_state); // no right
		}
		// handle up down
		if(state%10==9){ // oben
			pConfig->storage_outdated=true;
			state-=8; // zurück
			if(floor(state/100)==67 && pAktors->bt_pin>999)						{	pAktors->bt_pin-=1000;	};
			if(floor(state/1000)==67 && (int(floor(pAktors->bt_pin/100))%10)>0)	{	pAktors->bt_pin-=100;	};
			if(floor(state/10000)==67 && (int(floor(pAktors->bt_pin/10))%10)>0)	{	pAktors->bt_pin-=10;	};
			if(floor(state/100000)==67 && (int(floor(pAktors->bt_pin/1))%10)>0)	{	pAktors->bt_pin-=1;		};
		} else if(state%10==2){ // unten
			pConfig->storage_outdated=true;
			state-=1; // zurück
			if(floor(state/100)==67 && pAktors->bt_pin<9000)					{	pAktors->bt_pin+=1000;	};
			if(floor(state/1000)==67 && (int(floor(pAktors->bt_pin/100))%10)<9)	{	pAktors->bt_pin+=100;	};
			if(floor(state/10000)==67 && (int(floor(pAktors->bt_pin/10))%10)<9)	{	pAktors->bt_pin+=10;	};
			if(floor(state/100000)==67 && (int(floor(pAktors->bt_pin/1))%10)<9)	{	pAktors->bt_pin+=1;		};
		}
		if(pSpeedo->disp_zeile_bak[2]==999){
			pOLED->clear_screen();
			pOLED->highlight_bar(0,0,128,8);
			pOLED->string_P(pSpeedo->default_font,PSTR("Change BT PIN"),2,0,15,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("leave menu to save"),1,7);
			pSpeedo->disp_zeile_bak[2]=123;
		};

		sprintf(char_buffer," %01i ",(int)floor(pAktors->bt_pin/1000)%10);
		if(floor(state/100)==67){
			pOLED->string(pSpeedo->default_font,char_buffer,2,2,15,0,0);
		} else {
			pOLED->string(pSpeedo->default_font,char_buffer,2,2);
		}

		sprintf(char_buffer," %01i ",(int)floor(pAktors->bt_pin/100)%10);
		if(floor(state/1000)==67){
			pOLED->string(pSpeedo->default_font,char_buffer,5,2,15,0,0);
		} else {
			pOLED->string(pSpeedo->default_font,char_buffer,5,2);
		}

		sprintf(char_buffer," %01i ",(int)floor(pAktors->bt_pin/10)%10);
		if(floor(state/10000)==67){
			pOLED->string(pSpeedo->default_font,char_buffer,8,2,15,0,0);
		} else {
			pOLED->string(pSpeedo->default_font,char_buffer,8,2);
		}

		sprintf(char_buffer," %01i ",pAktors->bt_pin%10);
		if(floor(state/100000)==67){
			pOLED->string(pSpeedo->default_font,char_buffer,11,2,15,0,0);
		} else {
			pOLED->string(pSpeedo->default_font,char_buffer,11,2);
		}
	}
	/********************************************* End of Customize Menu *********************************************/



	/********************************************* Menu 7 - Start of Setup Menu *********************************************
	 * Submenus:
	 *	71 Gear Calibration
	 *	72 Speed Calibration
	 *	73 Display Settings
	 *	74 Reset memory
	 *	75 Sensor source
	 *	76 Bluetooth reset
	 *	77 Water Warning temperature
	 *	78 Oil Warning temperature
	 *	79 -
	 ********************************************* Menu 7 - Start of Setup Menu *********************************************/
	//////////////////////// Setup Menu ////////////////////////////
	else if(floor(state/10)==7) { //00007[X]
		// Menu vorbereiten
		draw(&menu_setup[0],sizeof(menu_setup)/sizeof(menu_setup[0]));
		menu_preload=true;
	}
	///////////////////// Gear calibration //////////////////////////
	else if(floor(state/10)==71) { // 00071[X]
		if(state%10>6){	// gänge höher als 6
			state=716;
			update_display=true;
		} else {		// erstmal ne message
			pOLED->clear_screen();
			pOLED->highlight_bar(0,0,128,8);
			pOLED->string_P(pSpeedo->default_font,PSTR("Calibrate Gear"),2,0,15,0,0);
			char temp[2];
			sprintf(temp,"%i",int(state%10));
			pOLED->string(pSpeedo->default_font,temp,17,0,15,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("Change Gear up/down"),0,3);
			pOLED->string_P(pSpeedo->default_font,PSTR("!Keep driving!"),3,4);
			pOLED->string_P(pSpeedo->default_font,PSTR("  continue"),0,7);
			sprintf(temp,"%c",127);
			pOLED->string(pSpeedo->default_font,temp,0,7);
			pSensors->m_gear->faktor_counter=0;
		};
	}
	//////////////////// calibration start -> reset values, call in loop the gear_calc fkt ///////////////////
	else if(floor(state/100)==71){
		set_buttons(button_state,!button_state,!button_state,button_state); // left,right only
		pOLED->clear_screen();
		pSpeedo->reset_bak();
	}
	//////////////////// calibration done, save it now.  ///////////////////
	else if(floor(state/1000)==71){
		set_buttons(!button_state,!button_state,!button_state,!button_state); // message only

		pSensors->m_gear->n_gang[int(pSensors->m_gps->mod(pMenu->state,1000))/100]=pSensors->m_gear->faktor_flat;
		pConfig->storage_outdated=true;
		pConfig->write("GANG.TXT");

		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("Saved"),7,3);
		_delay_ms(300);
		state=711;
		update_display=true;
	}
	///////////////////// speed calibration //////////////////////////
	else if(floor(state/10)==72) {
		pOLED->clear_screen();
		pSpeedo->reset_bak(); // dadurch wird auch der counter resetet
	}
	/////////////// save new value ////////////
	else if(pMenu->state==7211){
		set_buttons(!button_state,!button_state,!button_state,!button_state); // message only
		pOLED->clear_screen();
		pSensors->m_speed->reifen_umfang=pSensors->m_speed->flat_value_calibrate_umfang/100;
		pConfig->storage_outdated=true;
		pConfig->write("BASE.TXT");
		pOLED->string_P(pSpeedo->default_font,PSTR("saved"),6,3);
		_delay_ms(300);
		state=721;
		update_display=true;
	}
	////////// display phase ////////////////
	else if(floor(state/10)==73 || floor(state/10)==731|| floor(state/10)==7311 || floor(state/10)==73111) {
		if(state%10==9){
			if(floor(state/10)==73) { pOLED->phase=pOLED->phase+16; }
			else if(floor(state/10)==731) {pOLED->phase++;}
			else if(floor(state/10)==7311) {pOLED->ref=pOLED->ref+16;}
			else if(floor(state/10)==73111) {pOLED->ref++;}

			pOLED->init(pOLED->phase,pOLED->ref);
			byte tempByte = (pOLED->phase & 0xFF);
			eeprom_write_byte((uint8_t *)144,tempByte);
			tempByte = (pOLED->ref & 0xFF);
			eeprom_write_byte((uint8_t *)145,tempByte);

		} else if(state%10==2){
			if(floor(state/10)==73) { pOLED->phase=pOLED->phase-16; } // warum auch immer wie das machen im eeprom, warum nicht auf der SD ?? TODO
			else if(floor(state/10)==731) {pOLED->phase--;}
			else if(floor(state/10)==7311) {pOLED->ref=pOLED->ref-16;}
			else if(floor(state/10)==73111) {pOLED->ref--;}

			pOLED->init(pOLED->phase,pOLED->ref);
			byte tempByte = (pOLED->phase & 0xFF);
			eeprom_write_byte((uint8_t *)144,tempByte);
			tempByte = (pOLED->ref & 0xFF);
			eeprom_write_byte((uint8_t *)145,tempByte);
		};

		int olc=0,orc=0;
		int ulc=0,urc=0;
		if(floor(state/10)==73)       { state=731; olc=1; orc=0; ulc=0; urc=0;}
		else if(floor(state/10)==731) {state=7311; olc=0; orc=1; ulc=0; urc=0;}
		else if(floor(state/10)==7311) {state=73111; olc=0; orc=0; ulc=1; urc=0;}
		else if(floor(state/10)==73111) {state=731111; olc=0; orc=0; ulc=0; urc=1;};

		pOLED->clear_screen();
		pOLED->highlight_bar(0,8*1-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(pSpeedo->default_font,PSTR("Choose phase"),4,1,15,0,0);
		sprintf(char_buffer," %02i ",int(floor(int(pOLED->phase)/16)));
		pOLED->string(pSpeedo->default_font,char_buffer,4,2,abs(olc-1)*15,olc*15,0);
		pOLED->string(pSpeedo->default_font," // ",8,2,15,0,0);
		sprintf(char_buffer," %02i ",int(int(pOLED->phase)%16));
		pOLED->string(pSpeedo->default_font,char_buffer,12,2,abs(orc-1)*15,orc*15,0);

		pOLED->highlight_bar(0,8*5-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(pSpeedo->default_font,PSTR("Choose ref V"),4,5,15,0,0);
		sprintf(char_buffer," %02i ",int(floor(int(pOLED->ref)/16)));
		pOLED->string(pSpeedo->default_font,char_buffer,4,6,abs(ulc-1)*15,ulc*15,0);
		pOLED->string(pSpeedo->default_font," // ",8,6,15,0,0);
		sprintf(char_buffer," %02i ",int(int(pOLED->ref)%16));
		pOLED->string(pSpeedo->default_font,char_buffer,12,6,abs(urc-1)*15,urc*15,0);

	}
	///////////////////// reset setup //////////////////////////
	else if(state==741) {
		yesno("Really reset","total EEPROM","");
		set_buttons(button_state,!button_state,!button_state,button_state); // left-right only
	}
	else if(state==7411) {
		set_buttons(!button_state,!button_state,!button_state,!button_state); // message only
		byte tempByte;
		eeprom_write_byte((uint8_t *)2, 1& 0xFF);   // tripmode=1


		eeprom_write_byte((uint8_t *)144, 0xA8); // display
		eeprom_write_byte((uint8_t *)145, 0x38); // display

		tempByte = ((int)15 & 0xFF);
		eeprom_write_byte((uint8_t *)147, tempByte); // Navi_pos=0
		eeprom_write_byte((uint8_t *)149, (int)1 & 0xFF); // reset_enable=>1
		tempByte = ((int)92 & 0xFF);
		eeprom_write_byte((uint8_t *)150, tempByte); // fuel_max=9.2 l
		tempByte = ((int)0 & 0xFF);
		eeprom_write_byte((uint8_t *)151, tempByte); // fuel_step=0
		eeprom_write_byte((uint8_t *)152, tempByte); // fuel_counter=0


		pSD->sd_failed=false;
		for(unsigned int i=0;i<sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]);i++){
			pSpeedo->max_speed[i]=0;
			pSpeedo->avg_timebase[i]=0;
			pSpeedo->trip_dist[i]=0;
		}
		pConfig->write("SPEEDO.TXT");
		pConfig->write("BASE.TXT");
		pConfig->write("GANG.TXT");


		popup(PSTR("The memory"),PSTR("has been reset"));
		state=11;
	}
	/////////// Sensor source  //////////
	// this is our sneaky state in the middle, see if we have to store
	else if(floor(state/10)==75) { // 00075X
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==75) { // 000751X
		//sensor_source 0=analog, 1=auto_detect, 2=CAN
		if(state%10==9){ // "up" key
			if(pSensors->sensor_source>0){
				pSensors->sensor_source--;
				pConfig->storage_outdated=true;
			}
		} else if(state%10==2){ // "down" key
			if(pSensors->sensor_source<2){
				pSensors->sensor_source++;
				pConfig->storage_outdated=true;
			}
		};

		state=7511;

		// displaying values
		pOLED->clear_screen();

		pOLED->highlight_bar(0,0,128,8); // title
		pOLED->string_P(pSpeedo->default_font,PSTR("Sensor source mode"),2,0,DISP_BRIGHTNESS,0,0);

		unsigned char fg;
		unsigned char bg;

		/// Analog
		if(pSensors->sensor_source!=SENSOR_AUTO && pSensors->sensor_source!=SENSOR_FORCE_CAN){
			fg=0x00;
			bg=DISP_BRIGHTNESS;
			pOLED->highlight_bar(8,8*2,110,8); // mit hintergrundfarbe nen kasten malen
		} else {
			fg=DISP_BRIGHTNESS;
			bg=0x00;
		}
		pOLED->string_P(pSpeedo->default_font,PSTR("Analog Sensors"),3,2,bg,fg,0);

		/// Auto detect
		if(pSensors->sensor_source==SENSOR_AUTO){
			fg=0x00;
			bg=DISP_BRIGHTNESS;
			pOLED->highlight_bar(8,8*3,110,8); // mit hintergrundfarbe nen kasten malen
		} else {
			fg=DISP_BRIGHTNESS;
			bg=0x00;
		}
		pOLED->string_P(pSpeedo->default_font,PSTR("Auto detect"),3,3,bg,fg,0);

		/// Force Can
		if(pSensors->sensor_source==SENSOR_FORCE_CAN){
			fg=0x00;
			bg=DISP_BRIGHTNESS;
			pOLED->highlight_bar(8,8*4,110,8); // mit hintergrundfarbe nen kasten malen
		} else {
			fg=DISP_BRIGHTNESS;
			bg=0x00;
		}
		pOLED->string_P(pSpeedo->default_font,PSTR("CAN Sensors"),3,4,bg,fg,0);

		// key settings and corresponding var state changing
		bool up=button_state;
		bool down=button_state;
		if(pSensors->sensor_source<=0){
			up=!button_state;
		} else if(pSensors->sensor_source>=2){
			down=!button_state;
		}

		// go on to select can type
		if(pSensors->sensor_source!=SENSOR_AUTO && pSensors->sensor_source!=SENSOR_FORCE_CAN){ //analog
			set_buttons(button_state,up,down,!button_state); // button directions
		} else {
			set_buttons(button_state,up,down,button_state); // button directions
			char temp[2];
			sprintf(temp,"%c",127);
			pOLED->string(pSpeedo->default_font,temp,0,7);
			pOLED->string_P(pSpeedo->default_font,PSTR("to select type"),2,7);
		}
	}
	// select type
	else if(floor(state/100)==751) { // 0007511X
		if(state%10==9){ // "up" key
			if(pSensors->m_CAN->get_active_can_type()>1){
				pSensors->m_CAN->set_active_can_type(pSensors->m_CAN->get_active_can_type()-1);
				pConfig->storage_outdated=true;
			}
		} else if(state%10==2){ // "down" key
			if(pSensors->m_CAN->get_active_can_type()<2){
				pSensors->m_CAN->set_active_can_type(pSensors->m_CAN->get_active_can_type()+1);
				pConfig->storage_outdated=true;
			}
		};

		state=75111;

		// displaying values
		pOLED->clear_screen();

		pOLED->highlight_bar(0,0,128,8); // title
		pOLED->string_P(pSpeedo->default_font,PSTR("CAN Bus Type"),2,0,DISP_BRIGHTNESS,0,0);

		unsigned char fg;
		unsigned char bg;

		/// CAN_TYPE_TRIUMPH
		if(pSensors->m_CAN->get_active_can_type()==CAN_TYPE_TRIUMPH){
			fg=0x00;
			bg=DISP_BRIGHTNESS;
			pOLED->highlight_bar(8,8*2,110,8); // mit hintergrundfarbe nen kasten malen
		} else {
			fg=DISP_BRIGHTNESS;
			bg=0x00;
		}
		pOLED->string_P(pSpeedo->default_font,PSTR("Triumph"),3,2,bg,fg,0);

		/// CAN_TYPE_OBD2
		if(pSensors->m_CAN->get_active_can_type()==CAN_TYPE_OBD2){
			fg=0x00;
			bg=DISP_BRIGHTNESS;
			pOLED->highlight_bar(8,8*3,110,8); // mit hintergrundfarbe nen kasten malen
		} else {
			fg=DISP_BRIGHTNESS;
			bg=0x00;
		}
		pOLED->string_P(pSpeedo->default_font,PSTR("OBD2"),3,3,bg,fg,0);


		// key settings and corresponding var state changing
		bool up=button_state;
		bool down=button_state;
		if(pSensors->m_CAN->get_active_can_type()<=1){
			up=!button_state;
		} else if(pSensors->m_CAN->get_active_can_type()>=2){
			down=!button_state;
		}

		set_buttons(button_state,up,down,!button_state); // button directions
		char temp[3];
		sprintf(temp,"%c%c",126,126);
		pOLED->string(pSpeedo->default_font,temp,0,7);
		pOLED->string_P(pSpeedo->default_font,PSTR("to save"),2,7);

	}
	/////////// bt reset state  //////////
	else if(floor(state/10)==76) { // 00089X
		set_buttons(button_state,button_state,button_state,!button_state); // sackgasse
		if(state%10==9){
			pSensors->m_reset->set_active(true,true); // set eeprom,set var
		} else if(state%10==2){
			pSensors->m_reset->set_deactive(true,true);  // set eeprom,set var
		};
		state=761;
		pOLED->clear_screen();
		if(pSensors->m_reset->reset_enabled){
			sprintf(char_buffer," active");
		} else {
			sprintf(char_buffer,"inactive");
		};
		pOLED->string_P(pSpeedo->default_font,PSTR("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->highlight_bar(0,8*3-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(pSpeedo->default_font,PSTR("BT-Reset"),5,3,15,0,0);
		pOLED->string(pSpeedo->default_font,char_buffer,5,4,15,0,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("Down = inactive"),4,7,0,DISP_BRIGHTNESS,0);

		// show reason why last reset happend
		pOLED->string(pSpeedo->default_font,"Last was ",4,1,0,DISP_BRIGHTNESS,0);
		if(pSensors->m_reset->last_reset==0 || pSensors->m_reset->last_reset==-1){
			pOLED->string_P(pSpeedo->default_font,PSTR("power"),13,1,0,DISP_BRIGHTNESS,0);
		} else if(pSensors->m_reset->last_reset==1){
			pOLED->string(pSpeedo->default_font,"avr",13,1,0,DISP_BRIGHTNESS,0);
		} else if(pSensors->m_reset->last_reset==2){
			pOLED->string(pSpeedo->default_font,"bt",13,1,0,DISP_BRIGHTNESS,0);
		}
		// show reason why last reset happend
	}
	////////////////////////////////// water temp premenu //////////////////////////////////
	else if(floor(state/10)==77) {
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
		////////////////////////////////// water temp setup //////////////////////////////////
	} else if(floor(state/100)==77) {
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		if(state%10==9){
			pSensors->m_temperature->water_warning_temp+=10;
			state-=8;
			pConfig->storage_outdated=true;
		} else if(state%10==2){
			pSensors->m_temperature->water_warning_temp-=10;
			state--;
			pConfig->storage_outdated=true;
		}
		if(pSpeedo->disp_zeile_bak[2]!=88){
			pSpeedo->disp_zeile_bak[2]=88;
			pOLED->clear_screen();
			pOLED->string_P(pSpeedo->default_font,PSTR("Setup water"),5,1);
			pOLED->string_P(pSpeedo->default_font,PSTR("temperature"),5,2);
			pOLED->string_P(pSpeedo->default_font,PSTR("warning level"),4,3);
		}

		sprintf(char_buffer,"%3i,%i{C",int(floor(pSensors->m_temperature->water_warning_temp/10)),int(pSensors->m_temperature->water_warning_temp%10));
		center_me(char_buffer,21);
		pOLED->string(pSpeedo->default_font,char_buffer,0,5);
	}
	////////////////////////////////// water temp premenu //////////////////////////////////
	else if(floor(state/10)==78) {
		storage_update_guard(&state, old_state, pConfig->storage_outdated, &update_display); // remember to create a new value changing else if!
		////////////////////////////////// oil temp setup //////////////////////////////////
	} else if(floor(state/100)==78) {
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		if(state%10==9){
			pSensors->m_temperature->oil_warning_temp+=10;
			state-=8;
			pConfig->storage_outdated=true;
		} else if(state%10==2){
			pSensors->m_temperature->oil_warning_temp-=10;
			state--;
			pConfig->storage_outdated=true;
		}
		if(pSpeedo->disp_zeile_bak[2]!=87){
			pSpeedo->disp_zeile_bak[2]=87;
			pOLED->clear_screen();
			pOLED->string_P(pSpeedo->default_font,PSTR("Setup oil"),6,1);
			pOLED->string_P(pSpeedo->default_font,PSTR("temperature"),5,2);
			pOLED->string_P(pSpeedo->default_font,PSTR("warning level"),4,3);
		}

		sprintf(char_buffer,"%3i,%i{C",int(floor(pSensors->m_temperature->oil_warning_temp/10)),int(pSensors->m_temperature->oil_warning_temp%10));
		center_me(char_buffer,21);
		pOLED->string(pSpeedo->default_font,char_buffer,0,5);
	}
	/********************************************* End of Setup Menu *********************************************/



	/********************************************* Menu 8 - Start of Trip Menu *********************************************
	 * 81 lists all possible trips
	 * 81[X] shows the detailed info for trip [x]
	 * 81[X][Y] asks to delete info from trip [x]
	 * 81[X][Y][Z] delete info from trip [x]
	 ********************************************* Menu 8 - Start of Trip Menu *********************************************/
	else if(floor(state/10)==8) { // 8[X]
		// Menu vorbereiten
		draw(&menu_trip_setup[0],sizeof(menu_trip_setup)/sizeof(menu_trip_setup[0]));
	}
	///////////////////// ansicht eines stands //////////////////////////
	else if(floor(state/100)==8 && state%10==1){ // 9[X]1
		pOLED->clear_screen();

		int speicher=(int(floor(state/10))%10)-1;
		strcpy_P(char_buffer, (char*)pgm_read_word(&(menu_trip_setup[(int(floor(state/10))%10)-1])));
		center_me(char_buffer,13);
		pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
		pOLED->string(pSpeedo->default_font,char_buffer,4,0,DISP_BRIGHTNESS,0,0);
		sprintf(char_buffer,"Avg:       %3i km/h",(int)round(pSpeedo->trip_dist[speicher]*3.6/pSpeedo->avg_timebase[speicher]));
		pOLED->string(pSpeedo->default_font,char_buffer,0,2,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,"Time: %02i:%02i:%02i",(int)round(pSpeedo->avg_timebase[speicher]/3600),(int)round((pSpeedo->avg_timebase[speicher]%3600)/60),(int)round(pSpeedo->avg_timebase[speicher]%60));
		pOLED->string(pSpeedo->default_font,char_buffer,0,3,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,"Trip: %5lu,%02i km",(unsigned long)floor(pSpeedo->trip_dist[speicher]/1000),int(floor((pSpeedo->trip_dist[speicher]%1000)/10)));
		pOLED->string(pSpeedo->default_font,char_buffer,0,4,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,"Max:       %3i km/h",(int)pSpeedo->max_speed[speicher]);
		pOLED->string(pSpeedo->default_font,char_buffer,0,5,0,DISP_BRIGHTNESS,0);
	}
	//////////////// quick jump //////////////
	else if(floor(state/100)==8 && state%10==2){ // 8[X]2
		state+=9; // macht aus 812 -> 821
		// 892 -> 901
		if(state>900)
			state=811;
		update_display=true; 	// müssen wir hier nicht display aufrufen?
	}
	else if(floor(state/100)==8 && state%10==9){ // 8[X]9
		state-=18; // 949 -> 931
		// 921 -> 929 -> 911
		// 911 -> 919 -> 901
		if(state==801)
			state=891;
		update_display=true; // müssen wir hier nicht display aufrufen?
	}
	/////////////////// löschen abfrage ////////////////////
	else if(floor(state/1000)==8){  // 9[X]11
		set_buttons(!button_state,button_state,button_state,!button_state); // msg only
		char temp[22];
		strcpy_P(temp, (char*)pgm_read_word(&(menu_trip_setup[(int(floor(state/100))%10)-1])));

		int bereich=pMenu->center_me(temp,sizeof(temp)/sizeof(temp[0]));
		temp[int(floor(bereich/100)-1)]='"';
		temp[bereich%100+1]='"';
		yesno("Sure to reset",temp,"       storage");
	}
	// prevent deleting total && board
	else if(state==89111 || state==81111){
		pOLED->clear_screen();
		pOLED->highlight_bar(0,0,128,8);
		pOLED->string(pSpeedo->default_font,"Restriction",2,0,15,0,0);
		pOLED->string(pSpeedo->default_font,"Delete storages",2,2);
		pOLED->string(pSpeedo->default_font,"\"Total\" & \"Board\"",2,3);
		pOLED->string(pSpeedo->default_font,"forbidden",2,4);
		set_buttons(button_state,!button_state,!button_state,!button_state);
	}
	////////////////// Tachostand leeren ////////////////////
	else if(floor(state/10000)==8){ // 0008[X]111, jetzt alles löschen
		set_buttons(!button_state,!button_state,!button_state,!button_state); // msg only
		pSpeedo->max_speed[(int(floor(state/1000))%10)-1]=0;
		pSpeedo->avg_timebase[(int(floor(state/1000))%10)-1]=0;
		pSpeedo->trip_dist[(int(floor(state/1000))%10)-1]=0;
		// prepare save
		pConfig->storage_outdated=true;
		pConfig->write("speedo.txt");

		pOLED->clear_screen();
		// show the title
		char temp[30];
		strcpy_P(temp, (char*)pgm_read_word(&(menu_trip_setup[(int(floor(state/1000))%10)-1])));
		sprintf(char_buffer,"%s.",temp); // hier noch die bezeichnung
		pOLED->string(pSpeedo->default_font,char_buffer,2,3,0,DISP_BRIGHTNESS,0);
		pOLED->string(pSpeedo->default_font,"cleared",2,4);
		_delay_ms(800);
		state=floor(state/100);
		update_display=true;
	}
	/********************************************* End of Trip Menu *********************************************/



	/********************************************* Menu 9 - Start of fuel Menu *********************************************
	 * Submenus:
	 * 91 Shows "how many fuel have you added"
	 * 911 Resets your fuel memory
	 ********************************************* Menu 9 - Start of fuel Menu *********************************************/
	else if(floor(state/10)==9){ // 00009x
		// hoch runter schalten
		if(state%10==2 || state%10==9){ // 9[2/9]
			if(state%10==9) {
				fuel_added++;
				state-=8; // 99 -> 91
			} else if(state%10==2) {
				fuel_added--;
				state-=1; // 92 -> 91
			};
			if(fuel_added<1){
				fuel_added=1;
			}
			else if(fuel_added>800) { // 80L sollten doch wohl reichen
				fuel_added=800;
			};
		}

		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("Added fuel amount:"),1,2,0,DISP_BRIGHTNESS,4);
		sprintf(char_buffer,"%2i,%0i l",(int)floor(fuel_added/10),(int)fuel_added%10);
		pOLED->string(pSpeedo->default_font,char_buffer,7,4,0,DISP_BRIGHTNESS,0);
		if(pSpeedo->trip_dist[5]>0){
			sprintf(char_buffer,"%2i,%0i l/100km",(int)floor(fuel_added*10/(pSpeedo->trip_dist[5]/1000)),(int)(fuel_added*100/(pSpeedo->trip_dist[5]/1000))%10);
			pOLED->string(pSpeedo->default_font,char_buffer,5,5,0,DISP_BRIGHTNESS,0);
		}
	}

	///////////// Anzeigen wieviel verbraucht wurde ////////////////
	else if(floor(state/100)==9){  // 8[XX]
		set_buttons(button_state,!button_state,!button_state,button_state); // msg only
		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("All Fuel"),7,6,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("storeages reseted"),3,7,0,DISP_BRIGHTNESS,0);
		pSpeedo->trip_dist[5]=0;
		pSpeedo->avg_timebase[5]=0;
		pSpeedo->max_speed[5]=0;
		pConfig->storage_outdated=true;
		pConfig->write("speedo.txt");
		_delay_ms(500);
		state=11; // damit kann man durch rechts/links drücken wieder zum tacho springen
		update_display=true;
	}
	/********************************************* End of Trip Menu *********************************************/


	/********************************************* Start of Backup *********************************************
	 * This if clause catches you, if there is no corresponding menu to your action
	 * it should never happen, but its better to have a guard than to end up in chaos
	 ********************************************* Start of Backup *********************************************/
	else if(floor(state/10)==999999) {
		state=11;
		update_display=true;
	}
	///////////////////// "notfall" //////////////////////////
	else {
		back();
	};
	free(char_buffer);
	/*********************************************     End of Backup     *********************************************/
	/********************************************* End of Menu_display() *********************************************/
};
////// bei veränderung des state => einmaliges zeichen des menüs ///////

//// ein rückschritt im menü machen ////////
void speedo_menu::back(){
	unsigned long return_value=floor(state/10);
	if((return_value%10)>menu_lines){
		menu_marker=menu_lines-1;
		menu_start=(return_value%10)-menu_lines;
		menu_ende=(return_value%10)-1;
	}
	else {
		menu_marker=(return_value%10)-1;
		menu_start=0;
		menu_ende=menu_lines-1;
	};
	state=return_value;
	if(floor(state/100)==0 && ((int)floor(state/10))%10 == 0){ // 00X0
		button_links_valid=false;
	};
};
//// ein rückschritt im menü machen ////////

/// eine rückfrage: soll ich wirklich löschen ////
void speedo_menu::yesno(const char first[30],const char second[30],const char third[30]){
	// Menu vorbereiten
	pOLED->clear_screen();
	pOLED->string(pSpeedo->default_font,first,4,1,0,DISP_BRIGHTNESS,0);
	pOLED->string(pSpeedo->default_font,second,0,2,0,DISP_BRIGHTNESS,0);
	pOLED->string(pSpeedo->default_font,third,0,3,0,DISP_BRIGHTNESS,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("X no            yes X"),0,7,0,DISP_BRIGHTNESS,0);
	char zeiger[2];
	zeiger[1]=0x00;
	zeiger[0]=127; // pfeil rechts
	pOLED->string(pSpeedo->default_font,zeiger,20,7);
	zeiger[0]=126; // pfeil links
	pOLED->string(pSpeedo->default_font,zeiger,0,7);
	button_links_valid=true;
	button_oben_valid=false;
	button_rechts_valid=true;
	button_unten_valid=false;

};
/// eine rückfrage: soll ich wirklich löschen ////

/// bestätigen eines löschens ///
void speedo_menu::popup(const char *first,const char *second){
	pOLED->clear_screen();
	pOLED->string_P(pSpeedo->default_font,first,6,2,0,DISP_BRIGHTNESS,0);
	pOLED->string_P(pSpeedo->default_font,second,2,4,0,DISP_BRIGHTNESS,0);

	_delay_ms(1300);
	back();
	display();
};
/// bestätigen eines löschens ///

///// ein text menü zeichnen ////////////
void speedo_menu::draw(const char* const* menu, int entries){
	//show "main menu" in mainmenu (state < 10 )
	//otherwise, show caption of the particular menu
	unsigned long level_1=state;
	if(level_1>10){
		while(level_1>=10){
			level_1/=10;
		};
	} else {
		level_1=0; // main
	}


#ifdef MENU_DEBUG
	Serial.print("Bin im menue, menu_marker:");
	Serial.print(menu_marker);
	Serial.print(", menu_ende:");
	Serial.print(menu_ende);
	Serial.print(", menu_start:");
	Serial.println(menu_start);
#endif
	////////// Menu Caption /////////////
	pOLED->filled_rect(0,0,128,8,0);
	char char_buffer[22];

	strcpy_P(char_buffer, (char*)pgm_read_word(&(menu_titel[level_1])));
	pOLED->string(pSpeedo->default_font,char_buffer,0,0,0,DISP_BRIGHTNESS,0);

	sprintf(char_buffer,"%04lu",state);
	pOLED->string(pSpeedo->default_font,char_buffer,16,0,0,DISP_BRIGHTNESS,0);
	////////// Menu Caption /////////////

	///////// Menu ausgeben ////////////
	if(menu_ende>entries-1){
		menu_ende=entries-1;
	};
	unsigned char fg;
	unsigned char hg;
	int   y=1;

	// prepare buffer
	char *buffer;
	buffer = (char*) malloc (30);
	if (buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed 2"));
	else memset(buffer,'\0',30);

	// we got two modes, seperated by "just_marker_update" if its true,
	// that just update the marker bar, eg draw two line,
	// else update the whole screen
	if(just_marker_update){
		// we just have to repaint two lines, the actual "menu_marker" line plus the one above/below
		// draw maker line
		pOLED->highlight_bar(0,8*(menu_marker+1),128,8);
		strcpy_P(buffer, (char*)pgm_read_word(&(menu[menu_start+menu_marker])));
		pOLED->string(pSpeedo->default_font,buffer,2,(menu_marker+1),DISP_BRIGHTNESS,0,0);

		// lets see which one is the second: assuming that its the one below so the menu_marker_line + 1
		int second_line_switch=+1;
		if(old_state<state){
			// okay, failed, its the one above
			second_line_switch=-1;
		}
		//draw it
		pOLED->filled_rect(0,8*(menu_marker+second_line_switch+1),128,8,0); // mit hintergrundfarbe nen kasten malen
		strcpy_P(buffer, (char*)pgm_read_word(&(menu[int(menu_start+menu_marker+second_line_switch)])));
		pOLED->string(pSpeedo->default_font,buffer,2,int(menu_marker+second_line_switch+1),0,DISP_BRIGHTNESS,0);
		just_marker_update=false;

	} else {

		// draw it all
		for(int k=menu_start;k<=menu_ende;k++){
			if(y==(menu_marker+1)){
				fg=0;
				hg=DISP_BRIGHTNESS;
				pOLED->highlight_bar(0,8*y,128,8); // mit hintergrundfarbe nen kasten malen
			} else {
				fg=DISP_BRIGHTNESS;
				hg=0;
				pOLED->filled_rect(0,8*y,128,8,0); // mit hintergrundfarbe nen kasten malen
			};
			// copy string vom flash
			strcpy_P(buffer, (char*)pgm_read_word(&(menu[k])));
			pOLED->string(pSpeedo->default_font,buffer,2,y,hg,fg,0);
			// copy string vom flash
			y++; // abstand festlegen
		};

		while(y<8){ // die zeilen unter dem Menü ausmmalen
			pOLED->filled_rect(0,8*y,128,8,0); // mit hintergrundfarbe nen kasten malen
			y++;
		}
	}
	// free text buffer
	free(buffer);
	///////// Menu ausgeben ////////////
};
///// ein text menü zeichnen ////////////
// go_left(update_twice)
// update_twice=true <- false, damit das update_display flag nicht gesetzt wird
// daher muss die funktion die go_left(false) aufruft, selbst display() aufrufen
// wenn man es mit go_left(true) aufruft, dann wird display autak ausgeführt
bool speedo_menu::go_left(bool update_twice){
	old_state=state;
	just_marker_update=false;
	state=floor(state/10);

	if((state%10)>menu_lines){ // bei 8,9
		menu_marker=menu_lines-1;// ganz unten
		menu_start=(state%10)-menu_lines; // 8 - 7 => 1 bis 8
		menu_ende=(state%10)-1;
	}
	else {
		menu_marker=(state%10)-1;
		menu_start=0;
		menu_ende=menu_lines-1;
	};
	button_time=millis();
	pDebug->loop();
	update_display=update_twice;
	return true;
};

bool speedo_menu::go_right(bool update_twice){
	// menu var umsetzen
	old_state=state;
	just_marker_update=false;
	menu_start=0; // rechts -> leeres menu
	menu_ende=menu_lines-1;
	menu_marker=0;
	state=(state*10)+1;
	button_time=millis();
	pDebug->loop();
	update_display=update_twice;
	return true;
};

bool speedo_menu::go_up(bool update_twice){
	old_state=state;
	menu_marker--; // grundsätzlich einfach nur den marker hochschieben
	if(menu_marker<0){ // wenn der allerdings oben verwinden würde
		menu_marker++; // dann wieder auf 0 schieben
		menu_start--; // dafür das menu früher beginnen lassen
		menu_ende--; // und früher enden lassen
		if(menu_start<0){ // wenn wir jetzt allerdings nach oben den übertrag haben
			menu_start=menu_max-(menu_lines-1); // dann ist start ganz unten - wieviele lines wir haben 8-7=1 was menue 2. ist
			menu_ende=menu_max; // und wir sind ganz unten am ende 8=8
			menu_marker=menu_lines-1; // und der marker ist ganz unten
		};
	} else {
		just_marker_update=true;
	}

	//recalc level
	// z.b. 00061 -> 00060 => 60%10=0 -> soll 69
	state--;
	if(state%10==0){
		state+=menu_max+1; // macht aus 0 dann 9 => 8+1
	};
	button_time=millis();
	pDebug->loop();
	update_display=update_twice;
	return true;
};

bool speedo_menu::go_down(bool update_twice){
	old_state=state;
	menu_marker++; // ganz einfach hochsetzen
	if(menu_marker>(menu_lines-1)){ // wenn über max sind
		menu_marker--; // zurück
		menu_start++; // menu hoch
		menu_ende++; // setzen
		if(menu_ende>menu_max){ //umsprung nach oben wenn 9 > 8 dann wären wir mit dem ende auf dem eintrag der 10 heißen würde ..
			menu_marker=0;
			menu_start=0;
			menu_ende=menu_lines-1;
		};
	} else {
		just_marker_update=true;
	}

	//recalc level
	state++;
	// wenn wir bei xxx9 sind und ++ machen landen wir bei xx10
	// xx10%10 == 0, -> xx01
	if(state%10==0){
		state-=9; // macht aus [xx10] => 1
	};
	button_time=millis();
	pDebug->loop();
	update_display=update_twice;
	return true;
};

///// zyklisches abfragen der buttons ////////////
bool speedo_menu::button_test(bool bt_keys_en, bool hw_keys_en){
	unsigned char n=0; // count loop of "display()" max = 5
	while(update_display && n<5 && !hw_keys_en){
		update_display=false;
		n++;
		display();
	}
	if(n>0){
		return true;
	}

	if(bt_keys_en){
		if(Serial.available()>100){ // wenns zuviele sind flushen
			Serial.flush();
		} else if(Serial.available()>0){ // an sonsten gern
			if(Serial.read()==MESSAGE_START){
				pFilemanager_v2->parse_command();
			};
		};
	};

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

	if((hw_keys_en || button_first_push!=0) && pSpeedo->regular_startup){		// hier gehen wir nur rein wenn ein interrupt da war und einer der buttons noch gedrückt ist
		if((millis()>(button_time+menu_button_timeout)) ||
				((button_first_push>0 && millis()>(button_first_push+menu_button_fast_delay)) && millis()>(menu_button_fast_timeout+button_time)) ){ // halbe sek timeout
			//////////////////////// rechts ist gedrückt ////////////////////////
			if(((PINJ & (1<<menu_button_rechts))==menu_active) && button_rechts_valid){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_rechts");
#endif
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_rechts))==menu_active){ // wenn nach der Wartezeit der button immernoch gedrückt ist
					go_right(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}

			//////////////////////// links ist gedrückt ////////////////////////
			else if(((PINJ & (1<<menu_button_links))==menu_active)  && button_links_valid){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_links");
#endif
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_links))==menu_active){ // wenn nach der Wartezeit der button immernoch gedrückt ist
					go_left(true); // update display() via menu
					return true;
					// wenn der pegel doch nicht mehr anliegt ( spike )
				} else {
					return false;
				};
			}

			//////////////////////// oben ist gedrückt ////////////////////////
			else if(((PINJ & (1<<menu_button_oben))==menu_active) && button_oben_valid){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_oben");
#endif
				// move menu
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_oben))==menu_active){ // wenn nach der Wartezeit der button immernoch gedrückt ist
					go_up(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}

			//////////////////////// unten ist gedrückt ////////////////////////
			else if(((PINJ & (1<<menu_button_unten))==menu_active) && button_unten_valid){
				if(button_first_push==0){
					button_first_push=millis();
				};
#ifdef MENU_DEBUG
				Serial.println("menu_button_unten");
#endif
				// move menu
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				if((PINJ & (1<<menu_button_unten))==menu_active){ // wenn nach der Wartezeit der button immernoch gedrückt ist
					go_down(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}
			//////////////////////// nix ist gedrückt ////////////////////////
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

// hässlich hier den interrupt eingefügt ..
ISR(PCINT1_vect ){
	//	Serial.print("interrupt @");
	//	Serial.println(millis());
	pMenu->button_test(false,true);
};


///// zyklisches abfragen der buttons ////////////

void speedo_menu::init(){
	DDRJ &= ~((1<<menu_button_links)|(1<<menu_button_unten)|(1<<menu_button_rechts)|(1<<menu_button_oben)); //alles eingänge
	PORTJ |= ((1<<menu_button_links)|(1<<menu_button_unten)|(1<<menu_button_rechts)|(1<<menu_button_oben)); // pullup
	PCMSK1 |= ((1<<PCINT12)|(1<<PCINT13)|(1<<PCINT14)|(1<<PCINT15));										// PCINT freischalten
	PCICR |=(1<<PCIE1);																						// PCINT Activieren

	// see if its a clock startup or a regular startup
	if(pSpeedo->regular_startup){
		state=11;
	} else {
		state=291; // clock mode
	}
	old_state=state;
	button_time=millis();
	button_first_push=millis();
	update_display=false;
	just_marker_update=false;

	Serial.println("Menu init done");
};

void speedo_menu::set_buttons(bool left,bool up,bool down,bool right){
	button_links_valid=left;
	button_oben_valid=up;
	button_unten_valid=down;
	button_rechts_valid=right;
};

int speedo_menu::center_me(char* input,int length){
	int word_length=0;
	while(input[word_length]!='\0') word_length++; // asdf\0  0,1,2,3,4

	char temp[length];
	int free_space=length-word_length; // 21-4=17
	int start_at=floor(free_space/2); // 17/2= 6 .. [0..5] // 6 zeichen


	for(int i=0;i<=start_at;i++){ // 0..5
		temp[i]=' ';
	};
	for(int i=0;i<word_length;i++){ // 8 .. 8+3=11
		temp[i+start_at]=input[i];
	};
	for(int i=word_length+start_at;i<length-1;i++){
		temp[i]=' ';
	};
	temp[length-1]='\0';

	for(int i=0;i<length;i++){
		input[i]=temp[i];
	};
	return (start_at)*100+(start_at+word_length-1);
}

/*	Color_select_menu
 *
 */

void speedo_menu::color_select_menu(unsigned long base_state,led_simple *led_from, led_simple *led_to, int *min, int *max, int upper_limit,bool button_state, const char *name, const char *unit, char set_led_mode, bool just_one_line_mode){
	if(floor(state/10)==base_state){
		// sneaky, wir bauen ein "zwischen zustand" ein, um einen übergang zu erzeugen
		// wenn wir starten bei 652 und gehen rechts, sind wir in 6521, 652 ist base, old_state auch, daher ist dann der neue state 65211
		if(	old_state==base_state){
			state=state*10+1;
			state_helper=0;
			if(just_one_line_mode) state_helper=1;
			// prevent dimmer from killing our displayed color
			pAktors->set_active_dimmer(false);
			// andernfalls wollen wir gerade vom Einstellungsmenü ins Hauptmenü
			// wir sind nach links, jetzt müssen wir checken: sind wir das weil wir zurück aus dem Menü wollten, oder
			// wollten wir nur ein Feld weiter nach links. Die Felder sind numeriert in "state_helper"
		} else {
			// wirklich zurück ins Hauptmenü
			if(state_helper==0 || (state_helper==1 && just_one_line_mode)){
				back(); // calc menu_state

				pSpeedo->disp_zeile_bak[0]=999;
				// save it, if you want it
				if(set_led_mode!=99){
					pAktors->led_mode=set_led_mode;
				}
				// refresh LED color
				pAktors->set_active_dimmer(true);
				pAktors->update_outer_leds(true,true);

				// store to SD
				pConfig->storage_outdated=true; //?
				if(pConfig->storage_outdated){
					pOLED->clear_screen();
					if(pConfig->write("BASE.TXT")==0){
						pOLED->string_P(pSpeedo->default_font,PSTR("Saved"),7,4);
						_delay_ms(500);
					} else {
						pOLED->string_P(pSpeedo->default_font,PSTR("!SD ERROR!"),5,4);
						_delay_ms(5000);
					}
				};
				// doch nur ein Feld weiter nach links
			} else {
				state_helper--;
				state=state*10+1;
			}
		};
		// in each case, redraw this menu, cause this is just a "state in the middle"
		update_display=true;
		/////////////////// COLOR setup ////////////////
	} else if(floor(state/100)==base_state){
		if((state_helper==7 && !just_one_line_mode) || (state_helper==3 && just_one_line_mode)){
			set_buttons(button_state,button_state,button_state,!button_state); // no right
		};

		// show actual selected color "on screen"
		int r,g,b;
		if(state_helper<4){
			r=led_from->r;
			g=led_from->g;
			b=led_from->b;
		} else {
			r=led_to->r;
			g=led_to->g;
			b=led_to->b;
		}

		// show selected color immidiently on screen, stop any dimming process
		pAktors->stop_dimmer();
		pAktors->set_rgb_out(r,g,b);

		// neue werte
		if(state%10==2){ // unten
			pConfig->storage_outdated=true;
			state-=1; // zurück

			if(state_helper==0){			*min-=5;		if(*min<0){*min=0;};
			} else if(state_helper==1){		r-=5;			if(r<0){r=0;};
			} else if(state_helper==2){		g-=5;			if(g<0){g=0;};
			} else if(state_helper==3){		b-=5;			if(b<0){b=0;};
			} else if(state_helper==4){		*max-=5;		if(*max<*min){*max=*min;};
			} else if(state_helper==5){		r-=5;			if(r<0){r=0;};
			} else if(state_helper==6){		g-=5;			if(g<0){g=0;};
			} else if(state_helper==7){		b-=5;			if(b<0){b=0;};
			}

			// save the new value to var
			if(state_helper<4){
				led_from->r=r;
				led_from->g=g;
				led_from->b=b;
			} else {
				led_to->r=r;
				led_to->g=g;
				led_to->b=b;
			}
		} else if(state%10==9){ // oben
			pConfig->storage_outdated=true;
			state-=8; // zurück

			if(state_helper==0){			*min+=5;	if(*min>upper_limit){*min=upper_limit;};
			} else if(state_helper==1){		r+=5;		if(r>255){r=255;};
			} else if(state_helper==2){		g+=5;		if(g>255){g=255;};
			} else if(state_helper==3){		b+=5;		if(b>255){b=255;};
			} else if(state_helper==4){		*max+=5;	if(*max>upper_limit){*max=upper_limit;};
			} else if(state_helper==5){		r+=5;		if(r>255){r=255;};
			} else if(state_helper==6){		g+=5;		if(g>255){g=255;};
			} else if(state_helper==7){		b+=5;		if(b>255){b=255;};
			}

			// save the new value to var
			if(state_helper<4){
				led_from->r=r;
				led_from->g=g;
				led_from->b=b;
			} else {
				led_to->r=r;
				led_to->g=g;
				led_to->b=b;
			}
		}

		if(pSpeedo->disp_zeile_bak[0]!=304){
			// clear screen
			pOLED->clear_screen();
			// anzeige der werte
			pOLED->highlight_bar(0,0,128,8);
			pOLED->string_P(pSpeedo->default_font,name,2,0,15,0,0);


			if(!just_one_line_mode){
				pOLED->string_P(pSpeedo->default_font,PSTR("Minimal values"),2,2,0,15,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("Maximal values"),2,5,0,15,0);


				// Red   Green   Blue
				// 255    255    255
				pOLED->highlight_bar(0,24,128,16);
				pOLED->string_P(pSpeedo->default_font,unit,1,3,15,0,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("Red Green Blue"),6,3,15,0,0);
				pOLED->highlight_bar(0,48,128,16);
				pOLED->string_P(pSpeedo->default_font,unit,1,6,15,0,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("Red Green Blue"),6,6,15,0,0);
			} else {
				// Red   Green   Blue
				// 255    255    255
				pOLED->highlight_bar(0,24,128,16);
				pOLED->string_P(pSpeedo->default_font,PSTR("Red   Green   Blue"),1,3,15,0,0);
			}
			pSpeedo->disp_zeile_bak[0]=304;
		};

		char temp[4];
		unsigned char front,back;

		///////// node 1 //////////
		if(!just_one_line_mode){
			sprintf(temp,"%3i",int(*min%1000)); // to scale from 0..255 to 0..100
			if(state_helper==0){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			pOLED->string(pSpeedo->default_font,temp,1,4,back,front,0);
		}

		//// min r ///////
		sprintf(temp,"%3i",int((led_from->r%1000)/2.55)); // to scale from 0..255 to 0..100
		if(state_helper==1){
			back = 0;
			front = 15;
		} else {
			back = 15;
			front = 0;
		}
		if(!just_one_line_mode){	pOLED->string(pSpeedo->default_font,temp,6,4,back,front,0); }
		else { 						pOLED->string(pSpeedo->default_font,temp,1,4,back,front,0); }

		//// min g ///////
		sprintf(temp,"%3i",int((led_from->g%1000)/2.55)); // to scale from 0..255 to 0..100
		if(state_helper==2){
			back = 0;
			front = 15;
		} else {
			back = 15;
			front = 0;
		}
		if(!just_one_line_mode){	pOLED->string(pSpeedo->default_font,temp,12,4,back,front,0); }
		else {	 					pOLED->string(pSpeedo->default_font,temp,8,4,back,front,0);  }

		//// min b ///////
		sprintf(temp,"%3i",int((led_from->b%1000)/2.55)); // to scale from 0..255 to 0..100
		if(state_helper==3){
			back = 0;
			front = 15;
		} else {
			back = 15;
			front = 0;
		}
		if(!just_one_line_mode){	pOLED->string(pSpeedo->default_font,temp,17,4,back,front,0); }
		else {						pOLED->string(pSpeedo->default_font,temp,15,4,back,front,0); }

		if(!just_one_line_mode){
			//// max ///////
			sprintf(temp,"%3i",int(*max%1000)); // to scale from 0..255 to 0..100
			if(state_helper==4){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			pOLED->string(pSpeedo->default_font,temp,1,7,back,front,0);

			//// max r ///////
			sprintf(temp,"%3i",int((led_to->r%1000)/2.55)); // to scale from 0..255 to 0..100
			if(state_helper==5){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			pOLED->string(pSpeedo->default_font,temp,6,7,back,front,0);

			//// max g ///////
			sprintf(temp,"%3i",int((led_to->g%1000)/2.55)); // to scale from 0..255 to 0..100
			if(state_helper==6){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			pOLED->string(pSpeedo->default_font,temp,12,7,back,front,0);

			//// max b ///////
			sprintf(temp,"%3i",int((led_to->b%1000)/2.55)); // to scale from 0..255 to 0..100
			if(state_helper==7){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			pOLED->string(pSpeedo->default_font,temp,17,7,back,front,0);
		}
		////////////////// control //////////////////
	} else if(floor(state/1000)==base_state){
		state/=10;
		state_helper++;
		update_display=true;
	};
};

// just copy the name to a given array
void speedo_menu::copy_storagename_to_chararray(int id,char* array){
	strcpy_P(array, (char*)pgm_read_word(&(menu_trip_setup[id])));
}

/********************************** storage_update_guard **********************************
 * This is our sneaky "state in the middle"
 * If you on a Menu point e.g. 75 and go right you would expect to end up in state 751 but
 * storage_update_guard will redirect you to 7511. On your way back from 751X (by pushing
 * the "right"-Button) storage_update_guard will check if you have set the storeage_outdated
 * indicator to TRUE. If so storage_update_guard will call the save routine of the config
 * class and show the user that his values are now saved on the SD card (or show an error)
 ********************************** storage_update_guard **********************************/
void speedo_menu::storage_update_guard(unsigned long* state, unsigned long old_state,bool storage_outdated, bool* updated_display){
	if(old_state*10+1==*state){ // coming from menu -> shift us
		*state=(*state)*10+1;
	} else { // coming from selection, store value?
		back();
		// store to SD
		if(storage_outdated){
			pOLED->clear_screen();
			char char_buffer[22];
			strcpy_P(char_buffer,PSTR("Save to SD card"));
			center_me(char_buffer,21);
			pOLED->string(pSpeedo->default_font,char_buffer,0,5);
			bool remember_CAN_state=pSensors->CAN_active;
			pSensors->CAN_active=false;
			if(pConfig->write("BASE.TXT")==0){
				strcpy_P(char_buffer,PSTR("OK"));
			} else {
				strcpy_P(char_buffer,PSTR("FAILED !!"));
			}
			pSensors->CAN_active=remember_CAN_state;
			center_me(char_buffer,21);
			pOLED->string(pSpeedo->default_font,char_buffer,0,6);

			_delay_ms(2000);
		};
	};
	update_display=true;
};
