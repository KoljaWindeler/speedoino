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
#include <ssd0323.h>

/////////////////////////////// Menus///////////////////////////////
///////////////////// Main Menu /////////////////////
const char main_m_0[]= "Speedoino";   // "String 0" etc are strings to store - change to suit.
const char main_m_1[]= "Sprint-Clock";
const char main_m_2[]= "Lap-Timer";
const char main_m_3[]= "Tour-Assistent";
const char main_m_4[]= "Extended Info";
const char main_m_5[]= "Customize";
const char main_m_6[]= "Basic Setup";
const char main_m_7[]= "Trip Avg Max";
const char main_m_8[]= "Refueling";
const char * const menu_main[9]= { main_m_0,main_m_1,main_m_2,main_m_3,main_m_4,main_m_5,main_m_6,main_m_7,main_m_8 }; 	   // change "string_table" name to suit

///////////////////// Laptimer /////////////////////
const char lap_t_m_0[]= "Race Mode";
const char lap_t_m_1[]= "Load Sectors";
const char lap_t_m_2[]= "Set new Sectors";
const char lap_t_m_3[]= "Reset besttimes";
const char lap_t_m_4[]= "Evaluation";
const char lap_t_m_5[]= "Best Lap Calc";
const char lap_t_m_6[]= "-";
const char lap_t_m_7[]= "-";
const char lap_t_m_8[]= "-";
const char  * const menu_lap_t[9]= { lap_t_m_0,lap_t_m_1,lap_t_m_2,lap_t_m_3,lap_t_m_4,lap_t_m_5,lap_t_m_6,lap_t_m_7,lap_t_m_8 };

///////////////////// Setup /////////////////////
const char setup_m_0[]= "Gear calib.";   // "String 0" etc are strings to store - change to suit.
const char setup_m_1[]= "Speed calib.";   // "String 0" etc are strings to store - change to suit.
const char setup_m_2[]= "-"; // display brightness
const char setup_m_3[]= "GPS Format";
const char setup_m_4[]= "Sensor source"; // can or no CAN
const char setup_m_5[]= "BT Reset state";
const char setup_m_6[]= "Water temp warn.";
const char setup_m_7[]= "Oil temp warn.";
const char setup_m_8[]= "Man. update 328";
const char  * const menu_setup[9]= { setup_m_0,setup_m_1,setup_m_2,setup_m_3,setup_m_4,setup_m_5,setup_m_6,setup_m_7,setup_m_8 };

///////////////////// Customize /////////////////////
const char custom_m_0[]= "Load Skin";
const char custom_m_1[]= "Add info area";
const char custom_m_2[]= "Shift-Light";
const char custom_m_3[]= "Speedo-Color";
const char custom_m_4[]= "RGB-Action";
const char custom_m_5[]= "-";
const char custom_m_6[]= "Set BT PIN";
const char custom_m_7[]= "-";
const char custom_m_8[]= "Dev: Stepper";
const char  * const menu_custom[9]= { custom_m_0,custom_m_1,custom_m_2,custom_m_3,custom_m_4,custom_m_5,custom_m_6,custom_m_7,custom_m_8 };

///////////////////// Trips /////////////////////
const char add_info_m_0[]= "Distance";
const char add_info_m_1[]= "Time";
const char add_info_m_2[]= "Avg Speed";
const char add_info_m_3[]= "Max Speed";
const char add_info_m_4[]= "Distance & Time";
const char add_info_m_5[]= "Distance & Avg";
const char add_info_m_6[]= "Distance & Max";
const char add_info_m_7[]= "Time & Avg";
const char add_info_m_8[]= "Time & Max";
const char  * const menu_add_info[9]= { add_info_m_0,add_info_m_1,add_info_m_2,add_info_m_3,add_info_m_4,add_info_m_5,add_info_m_6,add_info_m_7,add_info_m_8 };

///////////////////// Customize /////////////////////
const char fade_m_0[]= "Static color";
const char fade_m_1[]= "Speed based";
const char fade_m_2[]= "RPM based";
const char fade_m_3[]= "Oil temp based";
const char fade_m_4[]= "Water temp based";
const char fade_m_5[]= "-";
const char fade_m_6[]= "-";
const char fade_m_7[]= "-";
const char fade_m_8[]= "-";
const char  * const menu_fade[9]= { fade_m_0,fade_m_1,fade_m_2,fade_m_3,fade_m_4,fade_m_5,fade_m_6,fade_m_7,fade_m_8 };

///////////////////// Extend info /////////////////////
const char einfo_m_0[]= "GPS info";
const char einfo_m_1[]= "CAN-BUS DTCs";
const char einfo_m_2[]= "Sensors";
const char einfo_m_3[]= "Stepper";
const char einfo_m_4[]= "-";
const char einfo_m_5[]= "Test Watchdog";
const char einfo_m_6[]= "Show Animation";
const char einfo_m_7[]= "About";
const char einfo_m_8[]= "Tetris";
const char * const menu_einfo[9]= { einfo_m_0,einfo_m_1,einfo_m_2,einfo_m_3,einfo_m_4,einfo_m_5,einfo_m_6,einfo_m_7,einfo_m_8 };

///////////////////// Navigation /////////////////////
const char navi_m_0[]= "Navi on/off";   // "String 0" etc are strings to store - change to suit.
const char navi_m_1[]= "Set pointer";   // "String 0" etc are strings to store - change to suit.
const char navi_m_2[]= "Set file";   // "String 0" etc are strings to store - change to suit.
const char navi_m_3[]= "POI finder"; // hier vielleicht ein: way to ziel: koordinaten aktuell im vergleich zum ziel, aktuellen course
const char navi_m_4[]= "-";
const char navi_m_5[]= "-";
const char navi_m_6[]= "GPS writes";
const char navi_m_7[]= "SC_POI status";
const char navi_m_8[]= "SC_POI on/off";
const char  * const menu_navi[9]= {navi_m_0,navi_m_1,navi_m_2,navi_m_3,navi_m_4,navi_m_5,navi_m_6,navi_m_7,navi_m_8};

///////////////////// Menu title /////////////////////
const char titel_0[]= "= Main menu =";   // "String 0" etc are strings to store - change to suit.
const char titel_1[]= ""; // tacho hat keine caption
const char titel_2[]= ""; // sprint hat keine caption
const char titel_3[]= "= Lap-Timer =";
const char titel_4[]= "= Navigation =";
const char titel_5[]= "= Information =";
const char titel_6[]= "= Customize =";
const char titel_7[]= "= Setup =";
const char titel_8[]= "= Storage = ";
const char titel_9[]= "";
const char * const menu_titel[10]= { titel_0,titel_1,titel_2,titel_3,titel_4,titel_5,titel_6,titel_7,titel_8,titel_9};

/////
const char trip_setup_m_0[]= "Total";   // "String 0" etc are strings to store - change to suit.
const char trip_setup_m_1[]= "From start";
const char trip_setup_m_2[]= "Day";
const char trip_setup_m_3[]= "Tour";
const char trip_setup_m_4[]= "Chain/Tire";
const char trip_setup_m_5[]= "Fuel";
const char trip_setup_m_6[]= "Oiler";
const char trip_setup_m_7[]= "Saison";
const char trip_setup_m_8[]= "Board";
const char * const menu_trip_setup[9]= {trip_setup_m_0,trip_setup_m_1,trip_setup_m_2,trip_setup_m_3,trip_setup_m_4,trip_setup_m_5,trip_setup_m_6,trip_setup_m_7,trip_setup_m_8};
/////////////////////////////// Menus///////////////////////////////

menu::menu(void){
	button_time=Millis.get();
	button_first_push=Millis.get();
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


menu::~menu(){
};

///// vars ////////////
bool		button_rechts_valid=true;
bool       	button_links_valid=true;
bool       	button_oben_valid=true;
bool       	button_unten_valid=true;

#define menu_active 0 // low active menues
#define menu_lines 7 // wieviele lines k?nnen wir darstellen 64 / 8 = 8 | 8-1(f?r die Headline)=7
int			menu_max=8;   // 9 eintrr?ge aber da das array bei 0 anf?ngt isses 9-1=8
int			menu_marker=0;
int			menu_start=0;
int			menu_ende=menu_lines-1;

#define menu_second_wait 100  // spike l?nge
int			fuel_added=90; // predefined value "added fuel"
unsigned int addr;
///// vars ////////////

////// bei ver?nderung des state => einmaliges zeichen des men?s ///////

/************************************************************ display menu ************************************************************
 * Menu structure base! This function gets called as soon as the menu_state_machine var "state" gets changed.
 * As long as this state has an "if" clause, the menu will show / do something. Sometimes just show a symbol, or a text but sometimes
 * triggers other functions. So this is, were the beauty comes from :D
 ************************************************************ display menu ************************************************************/
void menu::display(){
#ifdef MENU_DEBUG
	Serial.puts(USART1,"menu.display() called; at ");
	Serial.puts_ln(USART1,Millis.get());
	Serial.puts(USART1,"state: ");
	Serial.puts_ln(USART1,state);
	Serial.puts(USART1,"menu_marker,menu_end,menu_start:");
	Serial.puts(USART1,menu_marker);
	Serial.puts(USART1,",");
	Serial.puts(USART1,menu_ende);
	Serial.puts(USART1,",");
	Serial.puts_ln(USART1,menu_start);
#endif

	// init hardware buttons
	bool button_state=true; // activate all buttons
	if(BUTTONS_OFF){ button_state=false;}  // but deactivate them if the define is active
	set_buttons(button_state,button_state,button_state,button_state); // set initially. if necessary the if structs will override us

	// init a "one line" storage
	char *char_buffer;
	char_buffer = (char*) malloc (22);
	if (char_buffer==NULL) Serial.puts_ln(USART1,("Malloc failed 3"));
	else memset(char_buffer,'\0',22);
	/******************************** "left" in main menu. Redirect to Speedoino ********************************************/
	if(state==BMP(0,0,0,0,0,0,0)){
#ifdef MENU_DEBUG
		Serial.puts_ln(USART1,"Menustate war 0, daher biege ich ihn auf SPEEDOINO um");
#endif
		state=BMP(0,0,0,0,0,1,1);
		update_display=true;
	}
	/********************************************* Show the Main menu *********************************************/
	else if(state<BMP(0,0,0,0,0,1,0)) {
#ifdef MENU_DEBUG
		Serial.puts_ln(USART1,"Menustate war kleiner als 10, Hauptmenue");
#endif
		// Menu vorbereiten
		draw(&menu_main[0],sizeof(menu_main)/sizeof(menu_main[0]));
	}
	/********************************************* generate GPS marker *********************************************/
	else if(state==BMP(0,0,0,0,1,1,1)) {
#ifdef MENU_DEBUG
		Serial.puts_ln(USART1,"Menustate war 111, daher biege ich ihn auf 11 um");
#endif
		Sensors.mGPS.set_gps_mark(SIMPLE_MARK);
		state=BMP(0,0,0,0,0,1,1);
	}
	/********************************************* Menu 1 - SPEEDOINO *********************************************
	 * Main Screen
	 * Initially setup
	 * Show all icons and prepare setup
	 ********************************************* Menu 1 - SPEEDOINO *********************************************/
	else if(floor(state/10)==1 || state==BMP(7,3,1,1,1,1,1)){
#ifdef MENU_DEBUG
		Serial.puts_ln(USART1,"Menustate 00001X, Bin jetzt im Tacho menu, zeichne icons");
#endif
		TFT.clear_screen();
		Speedo.initial_draw_screen(); // draw symbols

		// wenn wir nicht in der navigation sind, die tasten f?r hoch und runter deaktiveren
		if(!Sensors.mGPS.navi_active){
			set_buttons(button_state,!button_state,!button_state,button_state); // msg only
		};
		// preview of display settings, phase and Vref
		if(state==BMP(7,3,1,1,1,1,1)){
			set_buttons(button_state,!button_state,!button_state,!button_state);
		};
	}
	/********************************************* Menu 1 - SPEEDOINO *********************************************/

	/********************************************* Menu 2 - Sprint Mode *********************************************
	 * This section only prepares the Sprint mode.
	 * Based on the main loop, pSprint.loop(); will be called
	 * So here we just have to prepare the Vars from the Sprintmode
	 ********************************************* Menu 2 - Sprint Mode *********************************************/
	else if(floor(state/10)==BMP(0,0,0,0,0,0,MENU_SPRINT)){
		set_buttons(button_state,!button_state,!button_state,!button_state); // left only
#ifdef MENU_DEBUG
		Serial.puts_ln(USART1,"Menustate=000002X, Bin jetzt im Sprint Tacho menu");
#endif
		TFT.clear_screen();
		Sprint.prepare_startup();
		Speedo.reset_bak(); // alle disp_zeile_bak auf -99 setzen
	}
	/********************************************* Clock mode *********************************************/
	else if(state==BMP(0,0,0,0,MENU_SPRINT,9,1)){ // reused state!
		Sensors.mClock.loop();
	}
	/********************************************* Menu 2 - Sprint Mode *********************************************/

	/********************************************* Menu [M_LAP_T], Race mode  *********************************************
	 * Submenus:
	 * [M_LAP_T]1 Race Mode;   // "String 0" etc are strings to store - change to suit.
	 * [M_LAP_T]2 Select file
	 * [M_LAP_T]3 Set Sectors
	 * [M_LAP_T]4 Clear Besttimes
	 * [M_LAP_T]5 Evaluation
	 * [M_LAP_T]6 Setup (Realtime / theoretical time)
	 * [M_LAP_T]7 -
	 * [M_LAP_T]8 -
	 * [M_LAP_T]9 -
	 ********************************************* Menu [M_LAP_T], Race mode  *********************************************/
	else if(floor(state/10)==BMP(0,0,0,0,0,0,M_LAP_T)){ // (M_LAP_T)1/10 = M_LAP_T
		// Menu vorbereiten
		draw(&menu_lap_t[0],sizeof(menu_lap_t)/sizeof(menu_lap_t[0]));
	}
	///////////////////// set GPS, move to next state
	else if(state==BMP(0,0,0,0,M_LAP_T,1,1)){
		set_buttons(button_state,!button_state,!button_state,!button_state); // just back
		// check GPS handling
		if(floor(old_state/10)==state){ // comming from menu above
			Sensors.mGPS.update_rate_1Hz();
			back();
		} else {
			// comming from menu below .. direct by menu . switch on high speed gps, otherwise it is already active
			if(old_state*10+1==state){
				Sensors.mGPS.update_rate_10Hz();
			}
			// check moving, if we are moving race_loop will be shown, if not some nice text appears
			LapTimer.prepare_race_loop();
		}
	}
	///////////////////// is capture mode, active content is controlled in the LapTimer Class
	else if(state==BMP(0,0,0,0,M_LAP_T,2,1)){
		set_buttons(button_state,!button_state,!button_state,!button_state); // no up/down
		TFT.clear_screen();
		TFT.show_storry(("Right now, only one files is used. Here is some magic needed"),("TODO"));
	}
	///////////////////// ask if you really want to clear all current sectors
	else if(state==BMP(0,0,0,0,M_LAP_T,3,1)){
		set_buttons(button_state,!button_state,!button_state,button_state); // no up/down
		TFT.show_storry(("Do you really want to clear all marks and record new?"),("GPS Marker"),DIALOG_NO_YES);
	}
	///////////////////// state in the middle, switch gps update + clear file OR save finishline coordinates
	else if(state==BMP(0,0,0,M_LAP_T,3,1,1)){
		if(old_state*10+1==state){ // comming from menu below
			// delete that file and switch on high speed GPS
			LapTimer.clear_file(LapTimer.get_active_filename());
			Sensors.mGPS.update_rate_10Hz();
			// go to next state that will draw the screen
			state=state*10+1;
			update_display=true;
		} else if(old_state==state*10+1){ // comming from menu above, so obviously we have just reached the FINISH LINE
			if(Sensors.mGPS.get_info(6)<3){
				TFT.clear_screen();
				TFT.string_centered(("No GPS"),3,true);
				TFT.string_centered(("Nothing saved"),4,true);
				Sensors.mGPS.update_rate_1Hz();
				_delay_ms(1500); // show it for a certain time
				back();
				back(); // jump back to menu
			} else {
				// save point as regualr sector border
				LapTimer.add_sector(Sensors.mGPS.get_info(3),Sensors.mGPS.get_info(2),LapTimer.get_active_filename());

				// some fancy output
				TFT.clear_screen();
				TFT.string_centered(("Finish-Line"),3,true);
				_delay_ms(100); // we will loose one gps points !! wise?

				// switch the state & draw the screen
				state=BMP(0,0,0,0,M_LAP_T,1,1);
				LapTimer.prepare_race_loop();
			}
		}	else { // coming from elsewhere (431111)
			// point has been captured, now show capture screen again
			state=state*10+1;
			update_display=true;
		}
	}
	///////////////////// is capture mode, active content is controlled in the LapTimer Class
	else if(state==BMP(0,0,M_LAP_T,3,1,1,1)){
		set_buttons(button_state,!button_state,!button_state,button_state); // no up/down
		LapTimer.initial_draw_gps_capture_screen();
	}
	///////////////////// capture new GPS marker
	else if(state==BMP(0,M_LAP_T,3,1,1,1,1)){
		LapTimer.add_sector(Sensors.mGPS.get_info(3),Sensors.mGPS.get_info(2),LapTimer.get_active_filename());
		TFT.clear_screen();
		TFT.string_centered(("Saved"),3,true);
		Speedo.reset_bak();
		_delay_ms(200); // we will loose some gps points !! wise?
		old_state=state;
		state=BMP(0,0,0,M_LAP_T,3,1,1);
		update_display=true;
	}
	///////////// reset times
	else if(state==BMP(0,0,0,0,M_LAP_T,4,1)){
		set_buttons(button_state,!button_state,!button_state,button_state); // no up/down
		TFT.show_storry(("Do you really want to reset all sector times?"),("Reset record"),DIALOG_NO_YES);
	}
	else if(state==BMP(0,0,0,M_LAP_T,4,1,1)){
		LapTimer.reset_times(LapTimer.get_active_filename());
		TFT.clear_screen();
		TFT.string_centered(("Saved"),3,true);
		_delay_ms(1000);
		back();
		back();
	}
	////////// Eval
	else if(state==BMP(0,0,0,0,M_LAP_T,5,1)){
		set_buttons(button_state,!button_state,!button_state,!button_state); // no up/down
		TFT.clear_screen();
		TFT.show_storry(("Evaluation is not yet implemented. Here is some magic needed"),("TODO"));
	}
	////////// Setup of laptimer
	else if(floor(state/10)==BMP(0,0,0,0,0,M_LAP_T,6)) { // 00046X
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==BMP(0,0,0,0,0,M_LAP_T,6)) { // 00046XX
		set_value_dialog((int8_t*)&LapTimer.use_realtime_not_calculated,("= Best Time Mode ="),("Calculated time"),("Real time"));
	}
	/********************************************* Menu 3, Race mode  *********************************************/

	/********************************************* Menu 4 - Start of Navigation Menu *********************************************
	 * Submenus:
	 *	41 Switch on and off navigation
	 *	42 Set current pointer within file
	 *	43 Select current file of navigation
	 *	44 POI finder
	 *	45
	 *	46
	 *	47 Get number of written GPS points
	 *	48 SC_POI status
	 *	49 SC_POI on/off
	 ********************************************* Menu 4 -  of Navigation Menu *********************************************/
	else if(floor(state/10)==BMP(0,0,0,0,0,0,M_TOUR_ASSISTS)){ // x1/10 = x
		// Menu vorbereiten
		draw(&menu_navi[0],sizeof(menu_navi)/sizeof(menu_navi[0]));
	}
	/////////////////////////////////////////////////// Navigation an/aus schalten //////////////////////////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,1)) { // 00041X
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,1)) { // 00041XX
		set_value_dialog((int8_t*)&Sensors.mGPS.navi_active,("= Navigation ="));
		if(Sensors.mGPS.navi_active && Config.storage_outdated){
			Sensors.mGPS.generate_new_order();
		}
	}
	/////////////////////////////////////////////////// Pointer f?r die Navigation ///////////////////////////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_POINTER)){ // 32[X]
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	else if(floor(state/100)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_POINTER)){ // 321[X] change values here
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		if((state%10)==2){ // runter gedr?ckt
			Sensors.mGPS.navi_point++;
			Config.storage_outdated=true;
		} else if((state%10)==9){ // hoch gedr?ckt
			Sensors.mGPS.navi_point--;
			if(Sensors.mGPS.navi_point<1) { Sensors.mGPS.navi_point=0; };
			Config.storage_outdated=true;
		};
		state=floor(state/10)*10+1; // zur?ckschuppsen

		TFT.clear_screen();
		TFT.string(Speedo.default_font,("Navi Track Pointer"),0,0,0,DISP_BRIGHTNESS,0);
		Sensors.mGPS.generate_new_order(); // vor ausgabe von Sensors.mGPS.navi_point, wenn Sensors.mGPS.navi_point nach dem knopfdruck zu hoch war wirds in dieser fkt zur?ck gesetzt
		sprintf(char_buffer,"#%3i:",Sensors.mGPS.navi_point);
		TFT.string(Speedo.default_font,char_buffer,0,2,0,DISP_BRIGHTNESS,0);
		TFT.string(Speedo.default_font,(char*)Sensors.mGPS.navi_ziel_name,7,2,0,DISP_BRIGHTNESS,0); // stra?e
		sprintf(char_buffer,("Long: %05i%04i"),int(floor(Sensors.mGPS.navi_ziel_long/10000)),int(Sensors.mGPS.mod(Sensors.mGPS.navi_ziel_long,10000)));
		TFT.string(Speedo.default_font,char_buffer,2,6,0,DISP_BRIGHTNESS,0);
		sprintf(char_buffer,("Lati: %05i%04i"),int(floor(Sensors.mGPS.navi_ziel_lati/10000)),int(Sensors.mGPS.mod(Sensors.mGPS.navi_ziel_lati,10000))); // typisch 052033224 => 5203,3224 => kann pro feld bis zu 32767 => 3.276.732.767 => 3.276? was quasi 8 mal um die welt geht
		TFT.string(Speedo.default_font,char_buffer,2,7,0,DISP_BRIGHTNESS,0);
	}
	/////////////////////////////////////////////////// Dateien listen und highlighten... irgendwie ///////////////////////////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_FILE)){ // 33[X]
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	} else if(floor(state/100)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_FILE)){ // 331[X]
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		// Umschalten
		if(int(state%10)==9){ // schalter nach oben, abziehen
			if(Sensors.mGPS.active_file>0){
				Sensors.mGPS.active_file--;
				Config.storage_outdated=true;
			}
		} else  if(int(state%10)==2){
			if(Sensors.mGPS.active_file < 9){ // wir haben 0,1,2,3,4,5,6,7,8,9 ... max 10 Files
				Sensors.mGPS.active_file++;
				Config.storage_outdated=true;
			}
		};
		// immer wieder zur?ckschuppsen
		state=floor(state/10)*10+1; // (41+2)*10+1
		Config.storage_outdated=true;
		Config.write("BASE.TXT"); // save config
		// open SD
		char navi_filename[23];
		sprintf(navi_filename,("/%s/NAVI%i.SMF"),NAVI_FOLDER,Sensors.mGPS.active_file);
		char buffer_big[65];
		if(SD.get_line_n(0,(uint8_t*)navi_filename,(uint8_t*)buffer_big,sizeof(buffer_big))==0){
			char title[17];
			sprintf(title,("Navifile Nr: %i/9"),Sensors.mGPS.active_file);
			if(buffer_big[0]=='#' && buffer_big[1]=='d'){
				for(int i=2;i<=strlen(buffer_big);i++){ // remove "#d" from destriction string
					buffer_big[i-2]=buffer_big[i];
				};
				TFT.show_storry(buffer_big,strlen(buffer_big),title,sizeof(title)/sizeof(title[0]));
			} else {
				TFT.clear_screen();
				TFT.highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
				TFT.string(Speedo.default_font,title,2,0,DISP_BRIGHTNESS,0,0);
				TFT.string(Speedo.default_font,("No first line comment"),0,3,0,DISP_BRIGHTNESS,0);
			}
		} else { // if there is no file with that number
			TFT.clear_screen();
			char title[17];
			sprintf(title,("Navifile Nr: %i/9"),Sensors.mGPS.active_file);
			TFT.highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
			TFT.string(Speedo.default_font,title,2,0,DISP_BRIGHTNESS,0,0);
			TFT.string(Speedo.default_font,("File not found"),2,3,0,DISP_BRIGHTNESS,0);
		};
	}
	//////////////////////// POI  finder  //////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_POI_FINDER)){	// file selection
		//		bool build_complete_list=true; // TODO
		//		bool upper_one=true;	// assume we pressed the "down" button, so we have to refresh to one on top of us
		//		if(floor(old_state/10)==floor(state/10)){ // build just a short part of the list, not complee
		//			build_complete_list=false;
		//			if(old_state-1==state){ // we pressed the "up" button, so we have to refresh to one below
		//				upper_one=false;
		//			};
		//		} else { // first access of this menu . prepare
		//			TFT.clear_screen();
		//			TFT.string_centered(("= File selector ="),0,false);
		//			TFT.string(Speedo.default_font,("\x7E back      select \x7F"),0,7);
		//		}
		//
		//		if(state%10==1){ // avoid going up
		//			set_buttons(button_state,!button_state,button_state,button_state);
		//		} else if(state%10==6){	// avoid going down
		//			set_buttons(button_state,button_state,!button_state,button_state);
		//		}
		//
		//		SdFile dir_handle;
		//		SdFile file_handle;
		//		int item=0; // filecounter
		//		unsigned int file_count=0;
		//		int start=0;
		//		int stop=4; // 0..4 = 5 file + line on top + line on buttom = 7 lines of display
		//		char filename[22];
		//		strcpy((char*)filename,("/POI/"));
		//
		//
		//		if(Filemanager_v2.get_file_handle((unsigned char*)filename,(unsigned char*)filename,&file_handle,&dir_handle,O_READ|O_CREAT)<0){	// works :D
		//			TFT.show_storry(("Open POI dir failed"),("Error"),DIALOG_GO_LEFT_1000MS);
		//		} else {
		//			if(!build_complete_list){ // only "re-read" two items
		//				if(upper_one){ // e.G. state == 374, we pushed "down" 3 times (from 371.374), we want to refresh item 3 and the item above (2)
		//					start=(state%10)-2;
		//					stop=(state%10)-1;
		//				} else { // e.G. state == 374, we pushed "down" 4 times (from 371.375), and than "up" once (375.374) we want to refresh item 3 and the item below (4)
		//					start=(state%10)-1;
		//					stop=(state%10);
		//				}
		//			} else { // build complete list
		//				start=0;
		//				stop=5;
		//			}
		//
		//			item=start;
		//			while(item<=stop && item<=5){
		//				// status: 0=EOF, 1=FILE, 2=FOLDER
		//				unsigned long size;
		//				if(dir_handle.lsJKWNext((unsigned char*)filename,item,&size)){ // <- returns the filename of the file nr "item"
		//					file_count++;
		//					// check resulting filename
		//					if(strlen(filename)>=15){
		//						filename[15]='\0'; // short it
		//					};
		//					char temp[21];
		//					sprintf(temp,"%i.%s",item+1,filename); // <-- "gas.txt"."4.GAS.TXT"
		//					// fill up
		//
		//					strcpy(filename,temp);
		//					for(int i=strlen(filename);i<17;i++){
		//						filename[i]=' ';
		//					}
		//					filename[17]='\0';
		//				} else {
		//					sprintf(filename,"%i.-",item+1);
		//					if((state%10)>file_count){ // if we are explicit on this item
		//						set_buttons(button_links_valid,button_oben_valid,button_unten_valid,!button_state); // avoid go right
		//					}
		//				}
		//				// print it
		//				if((build_complete_list && (unsigned)item==(state%10)-1) || (!build_complete_list && upper_one && item==stop) || (!build_complete_list && !upper_one && item==start)){
		//					TFT.highlight_bar(0,(item+1)*8,128,8);
		//					TFT.string(Speedo.default_font,filename,2,item+1,0x0f,0x00,0);
		//				} else {
		//					TFT.filled_rect(0,(item+1)*8,128,8,0x00);
		//					TFT.string(Speedo.default_font,filename,2,item+1);
		//				}
		//
		//				item++;
		//			}
		//		}
	} else if(floor(state/100)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_POI_FINDER)){	// some output
		TFT.show_storry(("It will take some time to generate a navi file from this POI. Continue?"),("POI Navigation"),DIALOG_NO_YES);
	} else if(floor(state/1000)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_POI_FINDER)){	// file selection
		poi_finder* pPOI_Finder=new poi_finder();
		pPOI_Finder->calc(int(floor(state/100))%10);
	}
	//////////////////////// ausgeben wieviele points geschrieben wurden //////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_WRITEN_POINTS_CHECK)){ //37[X]
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		char buffer[10];
		sprintf(buffer,"%i",Sensors.mGPS.written_gps_points);
		TFT.clear_screen();
		TFT.string(Speedo.default_font,buffer,5,3,0,DISP_BRIGHTNESS,0);
		TFT.string(Speedo.default_font,("Points written"),3,4,0,DISP_BRIGHTNESS,0);
	}
	////////////////////////  prepare speedoCam status screen //////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_SPEEDCAM_STATUS)){ //38[X]
		set_buttons(true,false,false,false);
		Speedo.reset_bak();
		TFT.clear_screen();
	}
	////////// Setup of poi warner
	else if(floor(state/10)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_SPEEDCAM_ON_OFF)) { // 00039X
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==BMP(0,0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_SPEEDCAM_ON_OFF)) { // 000461X
		bool temp=SpeedCams.get_active();
		set_value_dialog((int8_t*)&temp,("= POI warning ="));
		if(temp!=SpeedCams.get_active()){
			SpeedCams.set_active(temp);
		}
	}

	/********************************************* Menu 4 - End of Navigation Menu *********************************************/


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
	else if(floor(state/10)==BMP(0,0,0,0,0,0,5)) { //5[X]
		// Menu vorbereiten
		draw(&menu_einfo[0],sizeof(menu_einfo)/sizeof(menu_einfo[0]));
	}
	//////////////////////// Einmaliges Setup des GPS Tacho ////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,5,1)) { // 51[X]
		set_buttons(button_state,!button_state,!button_state,!button_state); // left only
		TFT.clear_screen();
		TFT.string(Speedo.default_font,("GPS_time"),0,4);
		TFT.string(VISITOR_SMALL_1X_FONT,("(raw)"),17,5);

		TFT.string(Speedo.default_font,("GPS_date"),0,7);
		TFT.string(VISITOR_SMALL_1X_FONT,("(raw)"),17,8);

		TFT.string(Speedo.default_font,("GPS_long"),0,10);
		TFT.string(VISITOR_SMALL_1X_FONT,("(NMEA)"),17,11);

		TFT.string(Speedo.default_font,("GPS_lati"),0,13);
		TFT.string(VISITOR_SMALL_1X_FONT,("(NMEA)"),17,14);

		TFT.string(Speedo.default_font,("GPS_alti"),0,16);

		TFT.string(Speedo.default_font,("GPS_speed"),0,19);

		TFT.string(Speedo.default_font,("GPS_sats"),0,22);
		TFT.string(VISITOR_SMALL_1X_FONT,("(+Fix?)"),17,23);

		TFT.string(Speedo.default_font,("Counter"),0,25);
		TFT.string(VISITOR_SMALL_1X_FONT,("(SD writes)"),15,26);

		TFT.string(Speedo.default_font,("GPS_Course"),0,28);

		Speedo.reset_bak(); // alle disp_zeile_bak auf -99 setzen
	}
	//////////////////////// CAN info ////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,5,2)) {
		// show info that CAN is offline
		if(!Sensors.CAN_active){
			TFT.clear_screen();
			TFT.string(Speedo.default_font,("CAN is not active"),2,3,0,15,0);
		}
		// get CAN info and show them
		else {
			Speedo.reset_bak();
			TFT.clear_screen();
			TFT.highlight_bar(0,0,128,8);
			TFT.string(Speedo.default_font,("CAN DTS Codes"),2,0,15,0,0);

			int dtc_error_count=Sensors.mCAN.get_dtc_error_count();
			if(dtc_error_count==0){
				TFT.string(Speedo.default_font,("No errors :)"),6,4,0,15,0);
			} else {

#ifdef CAN_DEBUG
				Serial.puts(USART1,(dtc_error_count);
				Serial.puts_ln(USART1," Fehler gefunden");
#endif

				for(int i=0; i<dtc_error_count && i<3;i++){
					int error_code=Sensors.mCAN.get_dtc_error(i+3*state_helper);
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

						sprintf(char_buffer,("Error %i/%i:%c%04i"),i+3*state_helper+1,dtc_error_count,region,error_code&0x0fff);
						TFT.string(Speedo.default_font,char_buffer,0,2*i+1,0,15,0);
						sprintf(char_buffer,("%c%04i"),region,error_code&0x0fff);
						Sensors.mCAN.decode_dtc(char_buffer,SPEED_TRIPPLE);
						center_me(char_buffer,21);
						TFT.string(Speedo.default_font,char_buffer,0,2*i+2,0,7,0);
					} else {
						TFT.string(Speedo.default_font,("COMM FAILED"),2*i+2,0,15,0,0);
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
				TFT.string(Speedo.default_font,("Press down for more"),0,7,0,15,0);
			}
			if(old_state==BMP(0,0,0,0,0,5,2)){
				state_helper=0;
				set_buttons(button_state,!button_state,down,button_state); // no up
			} else if(state%10==2){ // ein runter gedr?ckt
				state--;
				set_buttons(button_state,button_state,down,button_state); // all
				state_helper++;
			} else if(state%10==9){ // ein hoch gedr?ckt (519)
				state+=2; // 519 + 2 = 521
				state_helper--;
				if(state_helper==0){
					set_buttons(button_state,!button_state,down,button_state); // no up
				}
			}
			// testen f?r mehr als 3 codes!! TODO
		};

	}
	//////////////////////// extend sensor info ////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,5,3)) {
		set_buttons(button_state,!button_state,!button_state,!button_state); // left only
		// Menu vorbereiten
		Speedo.reset_bak();
		TFT.clear_screen();
		TFT.highlight_bar(0,0,320,24);
		TFT.string(Speedo.default_font,("Additional info"),2,0,15,0,0);
		TFT.draw_water(10,16);
		TFT.draw_air(20,24);
		TFT.draw_oil(10,32);
		// values will be added by "main" loop to have a online-monitoring
	}
	//////////////////////// stepper info ////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,5,4)) {
		set_buttons(button_state,!button_state,!button_state,button_state); // left & right only
		// Menu vorbereiten
		Speedo.reset_bak();
		TFT.clear_screen();
		TFT.highlight_bar(0,0,320,24);
		TFT.string(Speedo.default_font,("Additional info"),2,0,15,0,0);
		// this is just the caption,
		// the loop will fill the screen
		//		 Aktors.m_stepper.go_to(MOTOR_OVERWRITE_END_POS);
	}
	//////////////////////// stepper test ////////////////////////////
	// this will kick to needle to 6k ... thats all
	else if(floor(state/100)==BMP(0,0,0,0,0,5,4)) {
		set_buttons(button_state,!button_state,!button_state,!button_state); // left & right only // TODO
		// Menu vorbereiten
		Speedo.reset_bak();
		TFT.clear_screen();
		TFT.highlight_bar(0,0,128,8);
		TFT.string(Speedo.default_font,("Additional info"),2,0,15,0,0);
		TFT.string(Speedo.default_font,("Kicking to 10k"),2,2,0,15,0);
		Aktors.mStepper.go_to(10000);
		_delay_ms(3000);
		back();
	}
	//////////////////////// TEST des watchdogs durch absitzen ////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,5,6)){ // 56[X]
		TFT.clear_screen();
		TFT.string(Speedo.default_font,("WAIT 30 sec"),3,3);
		TFT.string(Speedo.default_font, ("for reset"),4,4);
		_delay_ms(29999); // knapp 30 sec
	}

	//////////////////////// show animation ////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,5,7)){ //57[x]
		// 571 is the starting point
		// 572 means "pushed up"
		// 573 means static
		// 574 means "pushed down"
		if(old_state*10+1==state){ // comming from "upper" menu 57 and now state iss 571
			state_helper=0; // <-- that is the important step, reset the state helper
		} else {
			if((state%10)==2){
				if(state_helper>0){ // that means that we won't show anything new if a users starts with "up" after nr 0...
					state_helper--;
				}
			} else if((state%10)==4){
				state_helper++;
			}
		};

		state=floor(state/10)*10+3; // move to fixed state, to be sure
		int status=TFT.animation(state_helper); // <-- RUN ANIMATION
		if(status==-1){ //Post work -1: Dir problems, -2: EOF
			TFT.clear_screen();
			TFT.string(Speedo.default_font,"Open dir failed",3,2,0,DISP_BRIGHTNESS,0);
		} else if(status==-2){ // we reached end of file, restart with the first file
			if(state_helper==0){ // we were able to open the dir, but we found not one SGF file at all
				TFT.clear_screen();
				TFT.string(Speedo.default_font,"no SGF file found",3,2,0,DISP_BRIGHTNESS,0);
			} else {
				state_helper=0;
				update_display=true; // rerun this loop to show the first animation again
			}
		}
	}
	//////////////////////// helper for filemanager showing picture/animiation ////////////////////////
	// filemanager kicks menu_state to 57111, so [right] will be 571111, [up/down] leads to 5711X, [left] to 5711
	else if(state==BMP(0,5,7,1,1,1,1) || floor(state/10)==BMP(0,0,0,5,7,1,1) || state==BMP(0,0,0,5,7,1,1)){
		state=BMP(0,0,0,0,0,1,1); // kick us back to dispay
		update_display=true; // rerun this loop
	}
	////////////////////////////////// ABOUT //////////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,5,8)) { // 00058X
		set_buttons(button_state,!button_state,!button_state,!button_state); // msg only
		TFT.clear_screen();
		TFT.highlight_bar(0,0,320,24);
		TFT.string(Speedo.default_font,("Speedoino"),2,0,DISP_BRIGHTNESS,0,0);
		TFT.string(Speedo.default_font,("GNU - Licensed"),0,1,0,DISP_BRIGHTNESS,0);

		TFT.string(Speedo.default_font,("HW version:"),0,3,0,DISP_BRIGHTNESS,0);
		sprintf(char_buffer,"%03i",Config.get_hw_version());
		TFT.string(Speedo.default_font,char_buffer,11,3);
		TFT.string(Speedo.default_font,(GIT_REV),0,4,0,DISP_BRIGHTNESS,0);

		TFT.string(Speedo.default_font,("contact me at"),0,6,0,DISP_BRIGHTNESS,0);
		TFT.string(Speedo.default_font,("KKoolljjaa@gmail.com"),0,7,0,DISP_BRIGHTNESS,0);
	}
	/////////////////// TETRIS //////////////////77
	else if(state==BMP(0,0,0,0,5,9,1)) {
		TFT.clear_screen();
		TFT.highlight_bar(0,8,320,24);
		TFT.string(Speedo.default_font,("Tetris"),6,1,15,0,0);
		TFT.string(Speedo.default_font,("in tribut to"),2,3,0,15,0);
		TFT.string(Speedo.default_font,("Alexej Padschitnow"),2,4,0,15,0);
		TFT.string(Speedo.default_font,("\x7E Quit"),2,5,0,15,0);
		TFT.string(Speedo.default_font,("\x7F go for it"),2,6,0,15,0);
	} else if(state==BMP(0,0,0,5,9,1,1)){
		update_display=true;
		state++;
	} else if(state==BMP(0,0,0,5,9,1,2)){
		//		PCICR &=~(1<<PCIE1); // PCINT DEactivieren //TODO
		//		tetris* m_tetris=new tetris;
		//		m_tetris.run();
		//		delete m_tetris;
		//		PCICR |=(1<<PCIE1); // PCINT Activieren
		//		back();
	}
	/********************************************* End of Extend Info Menu *********************************************/



	/********************************************* Menu 6 - Start of Customize Menu *********************************************
	 * Submenus:
	 *	61 Choose Skin
	 *	62 Select the shown trip
	 *	63 Adjust the DZ flasher value and color
	 *	64 Adjust outer RGB LED's, Static, KMH related ...
	 *	65 RGB-Action Mode
	 *	66
	 *	67 Set Bluetooth pin
	 *	68 -
	 *	69 Development Menu: choose Stepper speed
	 ********************************************* Menu 6 - Start of Customize Menu *********************************************/
	else if(floor(state/10)==BMP(0,0,0,0,0,0,6)) { //6[X]
		// Menu vorbereiten
		draw(&menu_custom[0],sizeof(menu_custom)/sizeof(menu_custom[0]));
		Speedo.reset_bak();
	}
	///////////////////////// SKIN LADEN ///////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,6,1)){ //61[X]
		// generate filename
		uint8_t filename[18];
		sprintf((char*)filename,("/CONFIG/SKIN%i.SSF"),int((state%10)-1));

		// load first line from file
		uint8_t buffer[65];
		if(SD.get_line_n(0,filename,buffer,sizeof(buffer))==0){
			char title[15];
			sprintf(title,("Skinfile Nr: %i/8"),int(state%10)-1);
			if(buffer[0]=='#'){
				TFT.show_storry((const char*)buffer,strlen((const char*)buffer),title,sizeof(title)/sizeof(title[0]));
			} else {
				TFT.clear_screen();
				TFT.highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
				TFT.string(Speedo.default_font,title,2,0,DISP_BRIGHTNESS,0,0);
				TFT.string(Speedo.default_font,("No first line comment"),0,3,0,DISP_BRIGHTNESS,0);
			}
			// noch mal kurz den buffer missbrauchen
			sprintf((char*)buffer,("\x7F Preview - \x7F\x7F Save"));
			TFT.string(Speedo.default_font,(char*)buffer,0,7,0,DISP_BRIGHTNESS,0);
			// end of draw to display
		} else { // if there is no file with that number
			TFT.clear_screen();
			char title[17];
			sprintf(title,("Skinfile Nr: %i/8"),int(state%10)-1);
			TFT.highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
			TFT.string(Speedo.default_font,title,2,0,DISP_BRIGHTNESS,0,0);
			TFT.string(Speedo.default_font,("File not found"),2,3,0,DISP_BRIGHTNESS,0);
			set_buttons(button_state,button_state,button_state,!button_state); // no right
		};

		// keep loading current skinfile, because without changing
		sprintf((char*)filename,("SKIN%i.TXT"),(Config.skin_file+10)%10);
		Config.read(CONFIG_FOLDER,(char*)filename,READ_MODE_CONFIGFILE,"");
	}
	//////////////////////// skin laden ////////////////////////////
	else if(floor(state/100)==BMP(0,0,0,0,0,6,1)) { // 0061[X]1
		// load this skinfile
		char filename[10];
		sprintf(filename,("SKIN%i.SSF"),(int(floor(state/10))%10)-1);
		Config.read(CONFIG_FOLDER,filename,READ_MODE_CONFIGFILE,"");
		TFT.clear_screen();
		TFT.string(Speedo.default_font,("Preview"),6,3);
		_delay_ms(300);

		// set buttons
		set_buttons(button_state,!button_state,!button_state,button_state); // lr only

		// save,change,restore,and show one preview image
		unsigned long save_state=state;
		state=BMP(0,0,0,0,0,1,1);
		display(); // nicht sch?n, aber das muss sein :D
		Speedo.loop(0);
		state=save_state;
	}

	//////////////////////// skin speichern . zum men?punkt 11 gehen und alles ist gut ////////////////////////////
	else if(floor(state/1000)==BMP(0,0,0,0,0,6,1)) { // 0061[xyz]
		TFT.clear_screen();
		Config.skin_file=(int(floor(state/100))%10)-1;
		Config.storage_outdated=true;
		Config.write("BASE.TXT"); // save config

		TFT.string(Speedo.default_font,("Saved"),7,3);
		_delay_ms(300);
		TFT.clear_screen();

		state=BMP(0,0,0,0,0,1,1);
		update_display=true;
	}
	///////////////////// trip show setup //////////////////////////
	// first the sneaky storage helper by /10
	// first select to type of mode by /100
	// then select the storage by /1000
	else if(floor(state/10)==BMP(0,0,0,0,0,6,2)) {
		// sneaky, wir bauen ein "zwischen zustand" ein, um einen ?bergang zu erzeugen
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==BMP(0,0,0,0,0,6,2)) {
		if(floor(old_state)==BMP(0,0,0,0,0,6,2)){// || floor(old_state/1000)==BMP(0,0,0,0,0,6,2)){ // wenn wir von unten kommen, waren wir vorher auf 62. wenn wir von oben kommen waren wir auf 62[1][Y][Z] = y ist mode, z ist storage
			state+=(Speedo.m_trip_mode%10)-1; // state muss angepasst werden.
			state=state*10+1; // moving to one "fake"level high, to run back(). This should repositionate the menu around us
			back();

		}
		else { // speichern des neuen Werts
			if(floor(old_state/1000)!=BMP(0,0,0,0,0,6,2)){
				Config.storage_outdated=true;
				Speedo.m_trip_mode=state%10;
			}
			draw(&menu_add_info[0],sizeof(menu_add_info)/sizeof(menu_add_info[0]));
		};

	}
	// and now select the storage
	else if(floor(state/1000)==BMP(0,0,0,0,0,6,2)) {
		if(floor(old_state/100)==BMP(0,0,0,0,0,6,2)){ // wenn wir von unten kommen, waren wir vorher auf 62[1][y]. y ist mode
			state+=(Speedo.m_trip_storage%10)-1; // state muss angepasst werden.
			state=state*10+1;
			back();
		}
		else { // speichern des neuen Werts
			if(floor(old_state/10000)!=BMP(0,0,0,0,0,6,2)){
				Speedo.m_trip_storage=state%10;
				Config.storage_outdated=true;
			}
			draw(&menu_trip_setup[0],sizeof(menu_trip_setup)/sizeof(menu_trip_setup[0]));
		};
	}
	///////////////////////////// dz flasher /////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,6,3)) {
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	else if(floor(state/100)==BMP(0,0,0,0,0,6,3) || floor(state/1000)==BMP(0,0,0,0,0,6,3)){ // 631[X] or 6311[X]
		if(state!=old_state+1 && state!=old_state+8){ // 632 . 631, take this since its possible to come from 631 and from 631111
			TFT.clear_screen();
			TFT.highlight_bar(0,8*4-1,320,48); // mit hintergrundfarbe nen kasten malen. zeile 3 und 4
			TFT.string(Speedo.default_font,("DZ Flasher"),5,4,15,0,0);

			// bedienelemente anzeigen
			if(floor(state/1000)==BMP(0,0,0,0,0,0,6)){
				TFT.filled_rect(0,0,128,20,0x00);
				TFT.string(Speedo.default_font,("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
				TFT.string(Speedo.default_font,("Down = inactive"),4,1,0,DISP_BRIGHTNESS,0);

				TFT.filled_rect(0,56,128,8,0x00);
				if(Sensors.mRpm.blitz_en){
					TFT.string(Speedo.default_font,("\x7F to adjust level"),0,7);
				};
			} else if(floor(state/10000)==6){
				TFT.filled_rect(0,0,128,20,0x00);
				TFT.string(Speedo.default_font,("Select the RPM"),4,0,0,DISP_BRIGHTNESS,0);
				TFT.string(Speedo.default_font,("for Shiftlight"),4,1,0,DISP_BRIGHTNESS,0);

				TFT.filled_rect(0,56,128,8,0x00);
				TFT.string(Speedo.default_font,("\x7E to adjust color"),0,7);
			}
		};
		// AN AUS schaltung
		if(floor(state/100)==BMP(0,0,0,0,0,6,3)){ // 631[x]
			if(state%10==9){
				Sensors.mRpm.blitz_en=true;
				Config.storage_outdated=true;
				state-=8;
			} else if(state%10==2){
				Sensors.mRpm.blitz_en=false;
				Config.storage_outdated=true;
				state-=1;
			};
			// schaltdrehzahl einstellen
		} else if(floor(state/1000)==BMP(0,0,0,0,0,6,3)){
			// Wert anpassen
			if(state%10==9){
				Sensors.mRpm.blitz_dz+=100;
				if(Sensors.mRpm.blitz_dz>20000){
					Sensors.mRpm.blitz_dz=20000;
				}
				state-=8;
				Config.storage_outdated=true;
			} else if(state%10==2){
				Sensors.mRpm.blitz_dz-=100;
				if(Sensors.mRpm.blitz_dz<100){
					Sensors.mRpm.blitz_dz=100;
				}
				state-=1;
				Config.storage_outdated=true;
			};
		}


		// wenn der blitzer an ist dann drehzahl mit anzeigen
		if(Sensors.mRpm.blitz_en){
			set_buttons(button_state,button_state,button_state,button_state); // alles aktiv

			sprintf(char_buffer,("%3i00"),int(floor(Sensors.mRpm.blitz_dz/100)));// 12500
			TFT.highlight_bar(0,8*5-1,128,9); // mit hintergrundfarbe nen kasten malen. zeile 3 und 4
			if(floor(state/100)==BMP(0,0,0,0,0,6,3)){
				TFT.string(Speedo.default_font,("active"),2,5,0,15,0); // joa, unsch?n. Wird flackern, aber naja
				TFT.string(Speedo.default_font,char_buffer,11,5,15,0,0);
			} else {
				TFT.string(Speedo.default_font,("active"),2,5,15,0,0);  // joa, unsch?n. Wird flackern, aber naja
				TFT.string(Speedo.default_font,char_buffer,11,5,0,15,0);
			}

			// wenn inactiv mittig "inactive" anzeigen lassen
		} else {
			set_buttons(button_state,button_state,button_state,!button_state); // no right
			TFT.highlight_bar(0,8*5-1,128,9); // mit hintergrundfarbe nen kasten malen. zeile 3 und 4
			TFT.string(Speedo.default_font,("inactive"),5,5,0,15,0);
		};

		//////////////////////// adjust dz alert RGB LED ////////////////////////
	} else if(floor(state/100)==BMP(0,0,0,6,3,1,1) || floor(state/1000)==BMP(0,0,0,6,3,1,1) ||floor(state/10000)==BMP(0,0,0,6,3,1,1)){
		color_select_menu(BMP(0,0,6,3,1,1,1),&Aktors.dz_flasher,0,0,0,0,button_state, ("Shift-Light"),(""),99,true);
	}
	//////////////////////// adjust outer RGB LED ////////////////////////
	/* In this menu, we give the user the chance to choose his own color
	 * first of all we start with an selection model
	 * 1. Static color - just one fix color
	 * 2. Speed based color fade . setup the minimum speed and color for the speed below this border
	 * 								and the max, plus the color above the top speed. between these limits, the color will be faded
	 *
	 */
	else if(floor(state/10)==BMP(0,0,0,0,0,6,4)){
		// preset Menu marker
		if(old_state==floor(state/10)){
			state=(state+Aktors.led_mode)*10+1;
			back();
		}

		draw(&menu_fade[0],sizeof(menu_fade)/sizeof(menu_fade[0]));
		Speedo.reset_bak();

		////////////////////////// static /////////////////
	} else if(floor(state/10)==BMP(0,0,0,0,6,4,1) || floor(state/100)==BMP(0,0,0,0,6,4,1) ||floor(state/1000)==BMP(0,0,0,0,6,4,1)){
		color_select_menu(BMP(0,0,0,0,6,4,1),&Aktors.static_color,0,0,0,0,button_state, ("Static color"),(""),0,true);

		/////////////////////// speed based color fade ///////////////////
	} else if(floor(state/10)==BMP(0,0,0,0,6,4,2) || floor(state/100)==BMP(0,0,0,0,6,4,2) ||floor(state/1000)==BMP(0,0,0,0,6,4,2)){
		color_select_menu(BMP(0,0,0,0,6,4,2),&Aktors.kmh_start_color,&Aktors.kmh_end_color,&Aktors.kmh_min_value,&Aktors.kmh_max_value,300,button_state, ("Speed"),("km\\h"),1,false);

		/////////////////////// rpm based color fade ///////////////////
	} else if(floor(state/10)==BMP(0,0,0,0,6,4,3) || floor(state/100)==BMP(0,0,0,0,6,4,3) || floor(state/1000)==BMP(0,0,0,0,6,4,3)){
		color_select_menu(BMP(0,0,0,0,6,4,3),&Aktors.dz_start_color,&Aktors.dz_end_color,&Aktors.dz_min_value,&Aktors.dz_max_value,150,button_state,("RPM*100"), ("RPM"),2,false);

		/////////////////////// oil based color fade ///////////////////
	} else if(floor(state/10)==BMP(0,0,0,0,6,4,4) || floor(state/100)==BMP(0,0,0,0,6,4,4) || floor(state/1000)==BMP(0,0,0,0,6,4,4)){
		color_select_menu(BMP(0,0,0,0,6,4,4),&Aktors.oil_start_color,&Aktors.oil_end_color,&Aktors.oil_min_value,&Aktors.oil_max_value,150,button_state, ("Oil"),("TMP"),3,false);

		/////////////////////// water based color fade ///////////////////
	} else if(floor(state/10)==BMP(0,0,0,0,6,4,5) || floor(state/100)==BMP(0,0,0,0,6,4,5) || floor(state/1000)==BMP(0,0,0,0,6,4,5)){
		color_select_menu(BMP(0,0,0,0,6,4,5),&Aktors.water_start_color,&Aktors.water_end_color,&Aktors.water_min_value,&Aktors.water_max_value,150,button_state, ("Water"),("TMP"),4,false);

		/////////////////////// RGB-Action //////////////////
	} else if(floor(state/10)==BMP(0,0,0,0,0,6,5)){
		// storage guard
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!

	} else if(floor(state/10)==BMP(0,0,0,0,6,5,1)){
		set_value_dialog((int8_t*)&Aktors.pointer_highlight_mode,("= RGB-Action ="),("Static"),("Following"),("Stack"),("R-Stack"));
		if(Aktors.pointer_highlight_mode==RGB_ACTION_TYPE_STATIC){
			Aktors.set_rbg_active((int)0x0000,false); // activate all led's
		}
	}
	///////////////////////// set bt pin ///////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,6,7)){
		if(old_state>state && Config.storage_outdated){
			if(Aktors.set_bt_pin()!=0){
				Config.storage_outdated=false; // avoid saving if we could not set it
			}
		}
		// sneaky, wir bauen ein "zwischen zustand" ein, um einen ?bergang zu erzeugen
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
		Speedo.disp_zeile_bak[2]=999; // redraw everything
	}
	else if((floor(state/100)==BMP(0,0,0,0,0,6,7)) | (floor(state/1000)==BMP(0,0,0,0,0,6,7)) | (floor(state/10000)==BMP(0,0,0,0,0,6,7)) | (floor(state/100000)==BMP(0,0,0,0,0,6,7))){ //67[x][x][x][x][1]
		if(floor(state/100000)==BMP(0,0,0,0,0,6,7)){
			set_buttons(button_state,button_state,button_state,!button_state); // no right
		}
		// handle up down
		if(state%10==9){ // oben
			Config.storage_outdated=true;
			state-=8; // zur?ck
			if(floor(state/100)==BMP(0,0,0,0,0,6,7) && Aktors.bt_pin>999)						{	Aktors.bt_pin-=1000;	};
			if(floor(state/1000)==BMP(0,0,0,0,0,6,7) && (int(floor(Aktors.bt_pin/100))%10)>0)	{	Aktors.bt_pin-=100;	};
			if(floor(state/10000)==BMP(0,0,0,0,0,6,7) && (int(floor(Aktors.bt_pin/10))%10)>0)	{	Aktors.bt_pin-=10;	};
			if(floor(state/100000)==BMP(0,0,0,0,0,6,7) && (int(floor(Aktors.bt_pin/1))%10)>0)	{	Aktors.bt_pin-=1;		};
		} else if(state%10==2){ // unten
			Config.storage_outdated=true;
			state-=1; // zur?ck
			if(floor(state/100)==BMP(0,0,0,0,0,6,7) && Aktors.bt_pin<9000)					{	Aktors.bt_pin+=1000;	};
			if(floor(state/1000)==BMP(0,0,0,0,0,6,7) && (int(floor(Aktors.bt_pin/100))%10)<9)	{	Aktors.bt_pin+=100;	};
			if(floor(state/10000)==BMP(0,0,0,0,0,6,7) && (int(floor(Aktors.bt_pin/10))%10)<9)	{	Aktors.bt_pin+=10;	};
			if(floor(state/100000)==BMP(0,0,0,0,0,6,7) && (int(floor(Aktors.bt_pin/1))%10)<9)	{	Aktors.bt_pin+=1;		};
		}
		if(Speedo.disp_zeile_bak[2]==999){
			TFT.clear_screen();
			TFT.highlight_bar(0,0,128,8);
			TFT.string(Speedo.default_font,("Change BT PIN"),2,0,15,0,0);
			TFT.string(Speedo.default_font,("leave menu to save"),1,7);
			Speedo.disp_zeile_bak[2]=123;
		};

		sprintf(char_buffer,(" %01i "),(int)floor(Aktors.bt_pin/1000)%10);
		if(floor(state/100)==BMP(0,0,0,0,0,6,7)){
			TFT.string(Speedo.default_font,char_buffer,2,2,15,0,0);
		} else {
			TFT.string(Speedo.default_font,char_buffer,2,2);
		}

		sprintf(char_buffer,(" %01i "),(int)floor(Aktors.bt_pin/100)%10);
		if(floor(state/1000)==BMP(0,0,0,0,0,6,7)){
			TFT.string(Speedo.default_font,char_buffer,5,2,15,0,0);
		} else {
			TFT.string(Speedo.default_font,char_buffer,5,2);
		}

		sprintf(char_buffer,(" %01i "),(int)floor(Aktors.bt_pin/10)%10);
		if(floor(state/10000)==BMP(0,0,0,0,0,6,7)){
			TFT.string(Speedo.default_font,char_buffer,8,2,15,0,0);
		} else {
			TFT.string(Speedo.default_font,char_buffer,8,2);
		}

		sprintf(char_buffer,(" %01i "),Aktors.bt_pin%10);
		if(floor(state/100000)==BMP(0,0,0,0,0,6,7)){
			TFT.string(Speedo.default_font,char_buffer,11,2,15,0,0);
		} else {
			TFT.string(Speedo.default_font,char_buffer,11,2);
		}
	}

	/////// development set the shown mode
	else if(floor(state/10)==BMP(0,0,0,0,0,6,9)){
		set_value_dialog((int8_t*)&Aktors.mStepper.shown_mode,("= Stepper Update ="),("Direct"),("Flat"),("Flat Round"),("Rounded"));
	}
	/********************************************* End of Customize Menu *********************************************/



	/********************************************* Menu 7 - Start of Setup Menu *********************************************
	 * Submenus:
	 *	71 Gear Calibration
	 *	72 Speed Calibration
	 *	73 -
	 *	74 Reset memory
	 *	75 Sensor source
	 *	76 Bluetooth reset
	 *	77 Water Warning temperature
	 *	78 Oil Warning temperature
	 *	79 Menual Update of ATm328
	 ********************************************* Menu 7 - Start of Setup Menu *********************************************/
	//////////////////////// Setup Menu ////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,0,7)) { //00007[X]
		// Menu vorbereiten
		draw(&menu_setup[0],sizeof(menu_setup)/sizeof(menu_setup[0]));
	}
	//////////////F/////// Gear calibration //////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,7,1)) { // 00071[X]
		if(state%10==9){ // smaller than 1
			state=BMP(0,0,0,0,7,1,6);
			update_display=true;
		} else if(state%10>6){ // higher than 6
			state=BMP(0,0,0,0,7,1,1);
			update_display=true;
		} else {		// erstmal ne message
			char temp[20];
			sprintf(temp,"Calibrate Gear %i",int(state%10));
			TFT.show_storry("The following measurements will help you to calibrate the gear module.\x0D\x0DSelect the gear by pushing the up/down button.After selecting press right to start the calibration.\x0D\x0D Drive faster and slower within that gear but don't shift before you save!",temp,DIALOG_NO_YES);
			Menu.set_buttons(true,true,true,true); // reenable buttons
			Sensors.mGear.faktor_counter=0;
		};
	}
	//////////////////// calibration start . reset values, call in loop the gear_calc fkt ///////////////////
	else if(floor(state/100)==BMP(0,0,0,0,0,7,1)){
		set_buttons(button_state,!button_state,!button_state,button_state); // left,right only
		TFT.clear_screen();
		Speedo.reset_bak();
	}
	//////////////////// calibration done, save it now.  ///////////////////
	else if(floor(state/1000)==BMP(0,0,0,0,0,7,1)){
		set_buttons(!button_state,!button_state,!button_state,!button_state); // message only
		int current_gear=int(Sensors.mGPS.mod(state,1000))/100;

		Sensors.mGear.n_gang[current_gear]=Sensors.mGear.faktor_flat;
		Config.storage_outdated=true;
		Config.write("GANG.TXT");

		TFT.clear_screen();
		TFT.string(Speedo.default_font,("Saved"),7,3);
		_delay_ms(300);
		if(current_gear<6 && current_gear>0){
			state=BMP(0,0,0,7,1,0,1)+(current_gear+1)*10; // next gear
		} else {
			state=BMP(0,0,0,7,1,1,1); // first gear again
		}
		update_display=true;
	}
	///////////////////// speed calibration //////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,7,2)) {
		TFT.clear_screen();
		Speedo.reset_bak(); // dadurch wird auch der counter resetet
	}
	/////////////// save new value ////////////
	else if(state==BMP(0,0,0,7,2,1,1)){
		set_buttons(!button_state,!button_state,!button_state,!button_state); // message only
		TFT.clear_screen();
		Sensors.mSpeed.reifen_umfang=Sensors.mSpeed.flat_value_calibrate_umfang/100;
		Config.storage_outdated=true;
		Config.write("BASE.TXT");
		TFT.string(Speedo.default_font,("saved"),6,3);
		_delay_ms(300);
		state=BMP(0,0,0,0,7,2,1);
		update_display=true;
	}
	////////// Setup of GPS Log Format
	else if(floor(state/10)==BMP(0,0,0,0,0,7,4)) { // 00074X
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==BMP(0,0,0,0,0,7,4)) { // 00074XX
		set_value_dialog((int8_t*)&LapTimer.use_realtime_not_calculated,("= GPS Format ="),("Readable"),("Compressed"));
	}
	/////////// Sensor source  //////////
	// this is our sneaky state in the middle, see if we have to store
	else if(floor(state/10)==BMP(0,0,0,0,0,7,5)) { // 00075X
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
	}
	// now the "mode" selector
	else if(floor(state/100)==BMP(0,0,0,0,0,7,5)) { // 000751X
		set_value_dialog((int8_t*)&Sensors.sensor_source,("= Sensor source ="),("Analog Sensors"),("Auto detect"),("CAN Sensors"));
		if(Sensors.sensor_source==SENSOR_AUTO || Sensors.sensor_source==SENSOR_FORCE_CAN){ // add "submenu"
			TFT.string(Speedo.default_font,("\x7F to select type"),0,6);
			set_buttons(button_links_valid,button_oben_valid,button_unten_valid,true);
		}
	}
	// select type
	else if(floor(state/100)==BMP(0,0,0,0,7,5,1)) { // 0007511X
		int8_t temp=0;
		if(Sensors.mCAN.get_active_can_type()==CAN_TYPE_OBD2 || Sensors.mCAN.get_active_can_type()==CAN_TYPE_TRIUMPH){
			temp=Sensors.mCAN.get_active_can_type()-1; // 0=none, 1=triuph, 2=OBD2 . map it to 0=triumph, 1=OBD2
		}
		set_value_dialog(&temp,("= CAN Bus Type ="),("Triumph"),("OBD2"));
		if(temp+1!=Sensors.mCAN.get_active_can_type()){
			Serial.puts(USART1,"Set value to:");
			Serial.puts_ln(USART1,temp+1);
			Sensors.mCAN.set_active_can_type(temp+1);
		}
	}
	/////////// bt reset state  //////////
	else if(floor(state/10)==BMP(0,0,0,0,0,7,6)) { // 00089X
		set_buttons(button_state,button_state,button_state,!button_state); // sackgasse
		if(state%10==9){
			Sensors.mReset.set_active(true,true); // set eeprom,set var
		} else if(state%10==2){
			Sensors.mReset.set_deactive(true,true);  // set eeprom,set var
		};
		state=BMP(0,0,0,0,7,6,1);
		TFT.clear_screen();
		if(Sensors.mReset.reset_enabled){
			strcpy(char_buffer,(" active"));
		} else {
			strcpy(char_buffer,("inactive"));
		};
		TFT.string(Speedo.default_font,("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
		TFT.highlight_bar(0,8*3-1,128,17); // mit hintergrundfarbe nen kasten malen
		TFT.string(Speedo.default_font,("BT-Reset"),5,3,15,0,0);
		TFT.string(Speedo.default_font,char_buffer,5,4,15,0,0);
		TFT.string(Speedo.default_font,("Down = inactive"),4,7,0,DISP_BRIGHTNESS,0);

		// show reason why last reset happend
		TFT.string(Speedo.default_font,"Last was ",4,1,0,DISP_BRIGHTNESS,0);
		if(Sensors.mReset.last_reset==0 || Sensors.mReset.last_reset==-1){
			TFT.string(Speedo.default_font,("power"),13,1,0,DISP_BRIGHTNESS,0);
		} else if(Sensors.mReset.last_reset==1){
			TFT.string(Speedo.default_font,"avr",13,1,0,DISP_BRIGHTNESS,0);
		} else if(Sensors.mReset.last_reset==2){
			TFT.string(Speedo.default_font,"bt",13,1,0,DISP_BRIGHTNESS,0);
		}
		// show reason why last reset happend
	}
	////////////////////////////////// water temp premenu //////////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,7,7)) {
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
		////////////////////////////////// water temp setup //////////////////////////////////
	} else if(floor(state/100)==BMP(0,0,0,0,0,7,7)) {
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		if(state%10==9){
			Sensors.mTemperature.water_warning_temp+=10;
			state-=8;
			Config.storage_outdated=true;
		} else if(state%10==2){
			Sensors.mTemperature.water_warning_temp-=10;
			state--;
			Config.storage_outdated=true;
		}
		if(Speedo.disp_zeile_bak[2]!=88){
			Speedo.disp_zeile_bak[2]=88;
			TFT.clear_screen();
			TFT.string(Speedo.default_font,("Setup water"),5,1);
			TFT.string(Speedo.default_font,("temperature"),5,2);
			TFT.string(Speedo.default_font,("warning level"),4,3);
		}

		sprintf(char_buffer,("%3i,%i{C"),int(floor(Sensors.mTemperature.water_warning_temp/10)),int(Sensors.mTemperature.water_warning_temp%10));
		center_me(char_buffer,21);
		TFT.string(Speedo.default_font,char_buffer,0,5);
	}
	////////////////////////////////// water temp premenu //////////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,7,8)) {
		storage_update_guard(&state, old_state, Config.storage_outdated, &update_display); // remember to create a new value changing else if!
		////////////////////////////////// oil temp setup //////////////////////////////////
	} else if(floor(state/100)==BMP(0,0,0,0,0,7,8)) {
		set_buttons(button_state,button_state,button_state,!button_state); // no right
		if(state%10==9){
			Sensors.mTemperature.oil_warning_temp+=10;
			state-=8;
			Config.storage_outdated=true;
		} else if(state%10==2){
			Sensors.mTemperature.oil_warning_temp-=10;
			state--;
			Config.storage_outdated=true;
		}
		if(Speedo.disp_zeile_bak[2]!=87){
			Speedo.disp_zeile_bak[2]=87;
			TFT.clear_screen();
			TFT.string(Speedo.default_font,("Setup oil"),6,1);
			TFT.string(Speedo.default_font,("temperature"),5,2);
			TFT.string(Speedo.default_font,("warning level"),4,3);
		}

		sprintf(char_buffer,("%3i,%i{C"),int(floor(Sensors.mTemperature.oil_warning_temp/10)),int(Sensors.mTemperature.oil_warning_temp%10));
		center_me(char_buffer,21);
		TFT.string(Speedo.default_font,char_buffer,0,5);
	}
	//////////////////////////////// water temp premenu //////////////////////////////////
	else if(floor(state/10)==BMP(0,0,0,0,0,7,9)) {
		set_buttons(button_state,!button_state,!button_state,button_state); // no right
		Aktors.run_reset_on_ATm328(RESET_PREPARE);
	}
	/********************************************* End of Setup Menu *********************************************/



	/********************************************* Menu 8 - Start of Trip Menu *********************************************
	 * 81 lists all possible trips
	 * 81[X] shows the detailed info for trip [x]
	 * 81[X][Y] asks to delete info from trip [x]
	 * 81[X][Y][Z] delete info from trip [x]
	 ********************************************* Menu 8 - Start of Trip Menu *********************************************/
	else if(floor(state/10)==BMP(0,0,0,0,0,0,8)) { // 8[X]
		// Menu vorbereiten
		draw(&menu_trip_setup[0],sizeof(menu_trip_setup)/sizeof(menu_trip_setup[0]));
	}
	///////////////////// ansicht eines stands //////////////////////////
	else if(floor(state/100)==BMP(0,0,0,0,0,0,8) && state%10==1){ // 9[X]1
		TFT.clear_screen();

		int speicher=(int(floor(state/10))%10)-1;
		strcpy(char_buffer, menu_trip_setup[(int(floor(state/10))%10)-1]);
		center_me(char_buffer,13);
		TFT.highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
		TFT.string(Speedo.default_font,char_buffer,4,0,DISP_BRIGHTNESS,0,0);
		sprintf(char_buffer,("Avg:%10i km/h"),(int)round(Speedo.trip_dist[speicher]*3.6/Speedo.avg_timebase[speicher]));
		TFT.string(Speedo.default_font,char_buffer,0,2,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,("Time: %02i:%02i:%02i"),(int)round(Speedo.avg_timebase[speicher]/3600),(int)round((Speedo.avg_timebase[speicher]%3600)/60),(int)round(Speedo.avg_timebase[speicher]%60));
		TFT.string(Speedo.default_font,char_buffer,0,3,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,("Trip:%6lu,%02i km"),(unsigned long)floor(Speedo.trip_dist[speicher]/1000),int(floor((Speedo.trip_dist[speicher]%1000)/10)));
		TFT.string(Speedo.default_font,char_buffer,0,4,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,("Max:%10i km/h"),(int)Speedo.max_speed[speicher]);
		TFT.string(Speedo.default_font,char_buffer,0,5,0,DISP_BRIGHTNESS,0);
	}
	//////////////// quick jump //////////////
	else if(floor(state/100)==BMP(0,0,0,0,0,0,8) && state%10==2){ // 8[X]2
		state+=9; // macht aus 812 . 821
		// 892 . 901
		if(state>BMP(0,0,0,0,0,0,8)*100)
			state=BMP(0,0,0,0,8,1,1);
		update_display=true; 	// m?ssen wir hier nicht display aufrufen?
	}
	else if(floor(state/100)==BMP(0,0,0,0,0,0,8) && state%10==9){ // 8[X]9
		state-=18; // 949 . 931
		// 921 . 929 . 911
		// 911 . 919 . 901
		if(state==BMP(0,0,0,0,8,0,1))
			state=BMP(0,0,0,0,8,9,1);
		update_display=true; // m?ssen wir hier nicht display aufrufen?
	}
	/////////////////// l?schen abfrage ////////////////////
	else if(floor(state/1000)==BMP(0,0,0,0,0,0,8)){  // 9[X]11
		char temp[22];
		strcpy(temp, menu_trip_setup[(int(floor(state/100))%10)-1]);
		char storry[40];
		sprintf(storry,("Sure to reset \"%s\" storage"),temp);
		TFT.show_storry(storry,strlen(storry),"Reset",6,DIALOG_NO_YES);
	}
	// prevent deleting total && board
	else if(state==BMP(0,0,8,9,1,1,1) || state==BMP(0,0,8,1,1,1,1)){
		TFT.clear_screen();
		TFT.highlight_bar(0,0,128,8);
		TFT.string(Speedo.default_font,"Restriction",2,0,15,0,0);
		TFT.string(Speedo.default_font,"Delete storages",2,2);
		TFT.string(Speedo.default_font,"\"Total\" & \"Board\"",2,3);
		TFT.string(Speedo.default_font,"forbidden",2,4);
		set_buttons(button_state,!button_state,!button_state,!button_state);
	}
	////////////////// Tachostand leeren ////////////////////
	else if(floor(state/10000)==BMP(0,0,0,0,0,0,8)){ // 0008[X]111, jetzt alles l?schen
		set_buttons(!button_state,!button_state,!button_state,!button_state); // msg only
		Speedo.max_speed[(int(floor(state/1000))%10)-1]=0;
		Speedo.avg_timebase[(int(floor(state/1000))%10)-1]=0;
		Speedo.trip_dist[(int(floor(state/1000))%10)-1]=0;
		// prepare save
		Config.storage_outdated=true;
		Config.write("speedo.txt");

		TFT.clear_screen();
		// show the title
		char temp[30];
		strcpy(temp, menu_trip_setup[(int(floor(state/1000))%10)-1]);
		sprintf(char_buffer,"%s",temp); // hier noch die bezeichnung
		TFT.string(Speedo.default_font,char_buffer,2,3,0,DISP_BRIGHTNESS,0);
		TFT.string(Speedo.default_font,"cleared",2,4);
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
	else if(floor(state/10)==BMP(0,0,0,0,0,0,9)){ // 00009x
		// hoch runter schalten
		if(state%10==2 || state%10==9){ // 9[2/9]
			if(state%10==9) {
				fuel_added++;
				state-=8; // 99 . 91
			} else if(state%10==2) {
				fuel_added--;
				state-=1; // 92 . 91
			};
			if(fuel_added<1){
				fuel_added=1;
			}
			else if(fuel_added>800) { // 80L sollten doch wohl reichen
				fuel_added=800;
			};
		}

		TFT.clear_screen();
		TFT.string(Speedo.default_font,("Added fuel amount:"),1,2,0,DISP_BRIGHTNESS,4);
		sprintf(char_buffer,("%2i,%0i l"),(int)floor(fuel_added/10),(int)fuel_added%10);
		TFT.string(Speedo.default_font,char_buffer,7,4,0,DISP_BRIGHTNESS,0);
		if(Speedo.trip_dist[5]>0){
			sprintf(char_buffer,("%2i,%0i l/100km"),(int)floor(fuel_added*10/(Speedo.trip_dist[5]/1000)),(int)(fuel_added*100/(Speedo.trip_dist[5]/1000))%10);
			TFT.string(Speedo.default_font,char_buffer,5,5,0,DISP_BRIGHTNESS,0);
		}
	}

	///////////// Anzeigen wieviel verbraucht wurde ////////////////
	else if(floor(state/100)==BMP(0,0,0,0,0,0,9)){  // 8[XX]
		set_buttons(button_state,!button_state,!button_state,button_state); // msg only
		TFT.clear_screen();
		TFT.string(Speedo.default_font,("All Fuel"),7,6,0,DISP_BRIGHTNESS,0);
		TFT.string(Speedo.default_font,("storeages reseted"),3,7,0,DISP_BRIGHTNESS,0);
		Speedo.trip_dist[5]=0;
		Speedo.avg_timebase[5]=0;
		Speedo.max_speed[5]=0;
		Config.storage_outdated=true;
		Config.write("speedo.txt");
		_delay_ms(500);
		state=BMP(0,0,0,0,0,1,1); // damit kann man durch rechts/links dr?cken wieder zum tacho springen
		update_display=true;
	}
	/********************************************* End of Trip Menu *********************************************/


	/********************************************* Start of Backup *********************************************
	 * This if clause catches you, if there is no corresponding menu to your action
	 * it should never happen, but its better to have a guard than to end up in chaos
	 ********************************************* Start of Backup *********************************************/
	else if(floor(state/10)==BMP(0,9,9,9,9,9,9)) {
		state=BMP(0,0,0,0,0,1,1);
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
////// bei ver?nderung des state => einmaliges zeichen des men?s ///////

//// one step back in the menu ////////
void menu::back(){
	old_state=state;								// to remember where we have been
	state=floor(state/10);							// new state
	if((state%10)>menu_lines){						// menu positioning
		menu_marker=menu_lines-1;
		menu_start=(state%10)-menu_lines;
		menu_ende=(state%10)-1;
	}
	else {
		menu_marker=(state%10)-1;
		menu_start=0;
		menu_ende=menu_lines-1;
	};

	if(floor(state/100)==0 && ((int)floor(state/10))%10 == 0){ // no left in main menu 00X0
		button_links_valid=false;
	};
	just_marker_update=false;						// wasty, but to be sure that the whole screen is redrawn
	update_display=true;					// set flag for the display() routine to re-enter the statemachine . react on new state
};
//// one step back in the menu ////////


///// draw the menu ////////////
void menu::draw(const char* const* menu, int entries){
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
	Serial.puts(USART1,"Bin im menue, menu_marker:");
	Serial.puts(USART1,menu_marker);
	Serial.puts(USART1,", menu_ende:");
	Serial.puts(USART1,menu_ende);
	Serial.puts(USART1,", menu_start:");
	Serial.puts_ln(USART1,menu_start);
#endif
	////////// Menu Caption /////////////
	char char_buffer[22];
	if(!just_marker_update){
		TFT.clear_screen();
		TFT.SetTextColor(0,0,255);
		//		for(int i=0;i<15;i++){
		//			int16_t x_from=270+2*i;
		//			int16_t y_from=239;
		//
		//			int16_t x_to=319;
		//			int16_t y_to=150-6*i;
		//
		//			TFT.DrawUniLine(x_from,y_from,x_to,y_to);
		//		}
		//TFT.glow(319,150,319,229,0,30,0,1);
		//TFT.glow(230,30,230,30,255,0,0,15);
		TFT.filled_rect(0,0,320,30,0);
		strcpy(char_buffer, menu_titel[level_1]);
		TFT.string(Speedo.default_font+1,char_buffer,0,0,0,DISP_BRIGHTNESS,0);
	}
	sprintf(char_buffer,"%04lu",state);
	TFT.string(Speedo.default_font,char_buffer,44,1,0,DISP_BRIGHTNESS,0);
	////////// Menu Caption /////////////

	///////// Menu ausgeben ////////////
	if(menu_ende>entries-1){
		menu_ende=entries-1;
	};
	unsigned char fg;
	unsigned char hg;
	int   y=1;

	// prepare buffer
	char buffer[30];
	int line_scaler=4;
	int line_mover=1;

	// we got two modes, seperated by "just_marker_update" if its true,
	// that just update the marker bar, eg draw two line,
	// else update the whole screen
	if(just_marker_update){
		// we just have to repaint two lines, the actual "menu_marker" line plus the one above/below
		// draw maker line
		TFT.highlight_bar(14,28*(menu_marker+1)+3,240,24);
		strcpy(buffer, menu[menu_start+menu_marker]);
		// move to the back and insert number
		for(int i=21;i>1;i--){
			buffer[i]=buffer[i-3];
		}
		buffer[0]=menu_start+menu_marker+'1';
		buffer[1]='.';
		buffer[2]=' ';
		TFT.string(Speedo.default_font,buffer,5,(menu_marker+1)*line_scaler+line_mover,DISP_BRIGHTNESS,0,0);

		// lets see which one is the second: assuming that its the one below so the menu_marker_line + 1
		int second_line_switch=+1;
		if(old_state<state){
			// okay, failed, its the one above
			second_line_switch=-1;
		}
		//draw it
		TFT.filled_rect(14,28*(menu_marker+second_line_switch+1)+3,240,24,0); // mit hintergrundfarbe nen kasten malen
		strcpy(buffer, menu[int(menu_start+menu_marker+second_line_switch)]);
		// move to the back and insert number
		for(int i=21;i>1;i--){
			buffer[i]=buffer[i-3];
		}
		buffer[0]=int(menu_start+menu_marker+second_line_switch)+'1';
		buffer[1]='.';
		buffer[2]=' ';
		TFT.string(Speedo.default_font,buffer,5,int(menu_marker+second_line_switch+1)*line_scaler+line_mover,0,DISP_BRIGHTNESS,0);
		just_marker_update=false;

	} else {
		// draw it all
		for(int k=menu_start;k<=menu_ende;k++){
			if(y==(menu_marker+1)){
				fg=0;
				hg=DISP_BRIGHTNESS;
				TFT.highlight_bar(14,28*y+3,240,24); // mit hintergrundfarbe nen kasten malen
			} else {
				fg=DISP_BRIGHTNESS;
				hg=0;
				TFT.filled_rect(14,28*y+3,240,24,0); // mit hintergrundfarbe nen kasten malen
			};
			// copy string vom flash
			strcpy(buffer, menu[k]);
			// move to the back and insert number
			for(int i=21;i>1;i--){
				buffer[i]=buffer[i-3];
			}
			buffer[0]=k+'1';
			buffer[1]='.';
			buffer[2]=' ';
			TFT.string(Speedo.default_font,buffer,5,y*line_scaler+line_mover,hg,fg,0);
			// copy string vom flash
			y++; // abstand festlegen
		};

		while(y<8){ // die zeilen unter dem Men? ausmmalen
			TFT.filled_rect(14,28*y+3,240,24,0); // mit hintergrundfarbe nen kasten malen
			y++;
		}
	}
	///////// Menu ausgeben ////////////
};
///// ein text men? zeichnen ////////////
// go_left(update_twice)
// update_twice=true <- false, damit das update_display flag nicht gesetzt wird
// daher muss die funktion die go_left(false) aufruft, selbst display() aufrufen
// wenn man es mit go_left(true) aufruft, dann wird display autak ausgef?hrt
bool menu::go_left(bool update_twice){
	back();
	button_time=Millis.get();
	Debug.loop();
	return true;
};

bool menu::go_right(bool update_twice){
	// menu var umsetzen
	old_state=state;
	just_marker_update=false;
	menu_start=0; // rechts . leeres menu
	menu_ende=menu_lines-1;
	menu_marker=0;
	state=(state*10)+1;
	button_time=Millis.get();
	Debug.loop();
	update_display=update_twice;
	return true;
};

bool menu::go_up(bool update_twice){
	old_state=state;
	menu_marker--; // grunds?tzlich einfach nur den marker hochschieben
	if(menu_marker<0){ // wenn der allerdings oben verwinden w?rde
		menu_marker++; // dann wieder auf 0 schieben
		menu_start--; // daf?r das menu fr?her beginnen lassen
		menu_ende--; // und fr?her enden lassen
		if(menu_start<0){ // wenn wir jetzt allerdings nach oben den ?bertrag haben
			menu_start=menu_max-(menu_lines-1); // dann ist start ganz unten - wieviele lines wir haben 8-7=1 was menue 2. ist
			menu_ende=menu_max; // und wir sind ganz unten am ende 8=8
			menu_marker=menu_lines-1; // und der marker ist ganz unten
		};
	} else {
		just_marker_update=true;
	}

	//recalc level
	// z.b. 00061 . 00060 => 60%10=0 . soll 69
	state--;
	if(state%10==0){
		state+=menu_max+1; // macht aus 0 dann 9 => 8+1
	};
	button_time=Millis.get();
	Debug.loop();
	update_display=update_twice;
	return true;
};

bool menu::go_down(bool update_twice){
	old_state=state;
	menu_marker++; // ganz einfach hochsetzen
	if(menu_marker>(menu_lines-1)){ // wenn ?ber max sind
		menu_marker--; // zur?ck
		menu_start++; // menu hoch
		menu_ende++; // setzen
		if(menu_ende>menu_max){ //umsprung nach oben wenn 9 > 8 dann w?ren wir mit dem ende auf dem eintrag der 10 hei?en w?rde ..
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
	// xx10%10 == 0, . xx01
	if(state%10==0){
		state-=9; // macht aus [xx10] => 1
	};
	button_time=Millis.get();
	Debug.loop();
	update_display=update_twice;
	return true;
};

///// zyklisches abfragen der buttons ////////////
bool menu::button_test(bool bt_keys_en, bool hw_keys_en){
	unsigned char n=0; // count loop of "display()" max = 5
	while(update_display && n<5 && !hw_keys_en){ // check if its neccesary to draw anything
		update_display=false;
		n++;
		Serial.puts_ln(USART1,"run display()");
		display();
	}
	if(n>0){
		return true;
	}

	if(bt_keys_en){	// set to false if you need the serial interface
		if(Serial.available(USART1)>100){ // wenns zuviele sind flushen
			Serial.flush(USART1);
		} else if(Serial.available(USART1)>0){ // an sonsten gern
			if(Serial.read(USART1)==MESSAGE_START){
				Filemanager_v2.parse_command();
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
	 *  So rerun this loop SLOW after MENU_BUTTON_TIMEOUT by check (button_time+menu_button_timeout)<Millis.get()
	 *  OR
	 *  FASTER (after menu_button_fast_delay) if:
	 *  1. the first button push is longer than menu_button_fast_delay ( Millis.get()>(button_first_push+menu_button_fast_delay ) (1.2sec)
	 *  and
	 *  2. the shorter delay is passed (Millis.get()>(menu_button_fast_timeout+button_time)) (0.1sec)
	 */
	hw_keys_en=false;
	if((hw_keys_en || button_first_push!=0) && Speedo.startup_by_ignition){ // hier gehen wir nur rein wenn ein interrupt da war und einer der buttons noch gedr?ckt ist
		if((Millis.get()>(button_time+menu_button_timeout)) ||
				((button_first_push>0 && Millis.get()>(button_first_push+menu_button_fast_delay)) && Millis.get()>(menu_button_fast_timeout+button_time)) ){ // halbe sek timeout
			//////////////////////// rechts ist gedr?ckt ////////////////////////
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)==Bit_RESET && button_rechts_valid){
				if(button_first_push==0){
					button_first_push=Millis.get();
				};
#ifdef MENU_DEBUG
				Serial.puts_ln(USART1,"menu_button_rechts");
#endif
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				// wenn nach der Wartezeit der button immernoch gedr?ckt ist
				if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)==Bit_RESET){
					go_right(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}

			//////////////////////// links ist gedr?ckt ////////////////////////
			else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET && button_links_valid){
				if(button_first_push==0){
					button_first_push=Millis.get();
				};
#ifdef MENU_DEBUG
				Serial.puts_ln(USART1,"menu_button_links");
#endif
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				// wenn nach der Wartezeit der button immernoch gedr?ckt ist
				if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)==Bit_RESET){
					go_left(true); // update display() via menu
					return true;
					// wenn der pegel doch nicht mehr anliegt ( spike )
				} else {
					return false;
				};
			}

			//////////////////////// oben ist gedr?ckt ////////////////////////
			else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==Bit_RESET && button_oben_valid){
				if(button_first_push==0){
					button_first_push=Millis.get();
				};
#ifdef MENU_DEBUG
				Serial.puts_ln(USART1,"menu_button_oben");
#endif
				// move menu
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				// wenn nach der Wartezeit der button immernoch gedr?ckt ist
				if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==Bit_RESET){
					go_up(true); // update display() via menu
					return true;
				} else {
					return false;
				};
			}

			//////////////////////// unten ist gedr?ckt ////////////////////////
			else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)==Bit_RESET && button_unten_valid){
				if(button_first_push==0){
					button_first_push=Millis.get();
				};
#ifdef MENU_DEBUG
				Serial.puts_ln(USART1,"menu_button_unten");
#endif
				// move menu
				_delay_ms(menu_second_wait); // warte ein backup intervall um einen spike zu unterdr?cken
				// erst wenn nach dem _delay_ms noch der pegel anliegt
				// wenn nach der Wartezeit der button immernoch gedr?ckt ist
				if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)==Bit_RESET){
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

///// zyklisches abfragen der buttons ////////////

void menu::init(){
	Serial.puts(USART1,("Menu init ..."));
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* This sequence sets up the TX and RX pins
	 * so they work correctly with the USART1 peripheral
	 */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN; 			// the pins are configured as alternate function so the USART peripheral has access to them
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;		// this defines the IO speed and has nothing to do with the baudrate!
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;			// this defines the output type as push pull mode (as opposed to open drain)
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// this activates the pullup resistors on the IO pins
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	///////////////////// INPUT INTERRUPT //////////////////
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	// Clock enable (SYSCONFIG)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	// EXT_INT0 mit Pin verbinden
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);

	// EXT_INT0 config
	EXTI_InitStructure.EXTI_Line = EXTI_Line12|EXTI_Line13|EXTI_Line14|EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// NVIC config
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	// see if its a clock startup or a regular startup
	if(Speedo.startup_by_ignition){
		state=BMP(0,0,0,0,0,1,1);
	} else {
		state=BMP(0,0,0,0,2,9,1); // clock mode
	}
	old_state=state;
	button_time=Millis.get();
	button_first_push=Millis.get();
	update_display=false;
	just_marker_update=false;

	Serial.puts_ln(USART1,(" done"));
};

void menu::set_buttons(bool left,bool up,bool down,bool right){
	button_links_valid=left;
	button_oben_valid=up;
	button_unten_valid=down;
	button_rechts_valid=right;
};

int menu::center_me(char* input,int length){
	int word_length=0;
	while(input[word_length]!='\0') word_length++; // asdf\0  0,1,2,3,4
	if(length<=word_length){ // if the input is longer than the available space
		return -1;
	}

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

void menu::color_select_menu(unsigned long base_state,led_simple *led_from, led_simple *led_to, int16_t *min, int16_t *max, int upper_limit,bool button_state, const char *name, const char *unit, char set_led_mode, bool just_one_line_mode){
	if(floor(state/10)==base_state){
		// sneaky, wir bauen ein "zwischen zustand" ein, um einen ?bergang zu erzeugen
		// wenn wir starten bei 652 und gehen rechts, sind wir in 6521, 652 ist base, old_state auch, daher ist dann der neue state 65211
		if(	old_state==base_state){
			state=state*10+1;
			state_helper=0;
			if(just_one_line_mode) state_helper=1;
			// prevent dimmer from killing our displayed color
			Aktors.set_active_dimmer(false);
			// andernfalls wollen wir gerade vom Einstellungsmen? ins Hauptmen?
			// wir sind nach links, jetzt m?ssen wir checken: sind wir das weil wir zur?ck aus dem Men? wollten, oder
			// wollten wir nur ein Feld weiter nach links. Die Felder sind numeriert in "state_helper"
		} else {
			// wirklich zur?ck ins Hauptmen?
			if(state_helper==0 || (state_helper==1 && just_one_line_mode)){
				back(); // calc menu_state

				Speedo.disp_zeile_bak[0]=999;
				// save it, if you want it
				if(set_led_mode!=99){
					Aktors.led_mode=set_led_mode;
				}
				// refresh LED color
				Aktors.set_active_dimmer(true);
				Aktors.update_outer_leds(true,true);

				// store to SD
				Config.storage_outdated=true; //?
				if(Config.storage_outdated){
					TFT.clear_screen();
					if(Config.write("BASE.TXT")==0){
						TFT.string(Speedo.default_font,("Saved"),7,4);
						_delay_ms(500);
					} else {
						TFT.string(Speedo.default_font,("!SD ERROR!"),5,4);
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
		Aktors.stop_dimmer();
		Aktors.set_rgb_out(r,g,b);

		// neue werte
		if(state%10==2){ // unten
			Config.storage_outdated=true;
			state-=1; // zur?ck

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
			Config.storage_outdated=true;
			state-=8; // zur?ck

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

		if(Speedo.disp_zeile_bak[0]!=304){
			// clear screen
			TFT.clear_screen();
			// anzeige der werte
			TFT.highlight_bar(0,0,128,8);
			TFT.string((char*)name,2,0,15,0);


			if(!just_one_line_mode){
				TFT.string(Speedo.default_font,("Minimal values"),2,2,0,15,0);
				TFT.string(Speedo.default_font,("Maximal values"),2,5,0,15,0);


				// Red   Green   Blue
				// 255    255    255
				TFT.highlight_bar(0,24,128,16);
				TFT.string((char*)unit,1,3,15,0);
				TFT.string(Speedo.default_font,("Red Green Blue"),6,3,15,0,0);
				TFT.highlight_bar(0,48,128,16);
				TFT.string((char*)unit,1,6,15,0);
				TFT.string(Speedo.default_font,("Red Green Blue"),6,6,15,0,0);
			} else {
				// Red   Green   Blue
				// 255    255    255
				TFT.highlight_bar(0,24,128,16);
				TFT.string(Speedo.default_font,("Red   Green   Blue"),1,3,15,0,0);
			}
			Speedo.disp_zeile_bak[0]=304;
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
			TFT.string(Speedo.default_font,temp,1,4,back,front,0);
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
		if(!just_one_line_mode){	TFT.string(Speedo.default_font,temp,6,4,back,front,0); }
		else { 						TFT.string(Speedo.default_font,temp,1,4,back,front,0); }

		//// min g ///////
		sprintf(temp,"%3i",int((led_from->g%1000)/2.55)); // to scale from 0..255 to 0..100
		if(state_helper==2){
			back = 0;
			front = 15;
		} else {
			back = 15;
			front = 0;
		}
		if(!just_one_line_mode){	TFT.string(Speedo.default_font,temp,12,4,back,front,0); }
		else {	 					TFT.string(Speedo.default_font,temp,8,4,back,front,0);  }

		//// min b ///////
		sprintf(temp,"%3i",int((led_from->b%1000)/2.55)); // to scale from 0..255 to 0..100
		if(state_helper==3){
			back = 0;
			front = 15;
		} else {
			back = 15;
			front = 0;
		}
		if(!just_one_line_mode){	TFT.string(Speedo.default_font,temp,17,4,back,front,0); }
		else {						TFT.string(Speedo.default_font,temp,15,4,back,front,0); }

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
			TFT.string(Speedo.default_font,temp,1,7,back,front,0);

			//// max r ///////
			sprintf(temp,"%3i",int((led_to->r%1000)/2.55)); // to scale from 0..255 to 0..100
			if(state_helper==5){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			TFT.string(Speedo.default_font,temp,6,7,back,front,0);

			//// max g ///////
			sprintf(temp,"%3i",int((led_to->g%1000)/2.55)); // to scale from 0..255 to 0..100
			if(state_helper==6){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			TFT.string(Speedo.default_font,temp,12,7,back,front,0);

			//// max b ///////
			sprintf(temp,"%3i",int((led_to->b%1000)/2.55)); // to scale from 0..255 to 0..100
			if(state_helper==7){
				back = 0;
				front = 15;
			} else {
				back = 15;
				front = 0;
			}
			TFT.string(Speedo.default_font,temp,17,7,back,front,0);
		}
		////////////////// control //////////////////
	} else if(floor(state/1000)==base_state){
		state/=10;
		state_helper++;
		update_display=true;
	};
};

// just copy the name to a given array
void menu::copy_storagename_to_chararray(int id,char* array){
	strcpy(array, (char*)menu_trip_setup[id]);
}

/********************************** storage_update_guard **********************************
 * This is our sneaky "state in the middle"
 * If you on a Menu point e.g. 75 and go right you would expect to end up in state 751 but
 * storage_update_guard will redirect you to 7511. On your way back from 751X (by pushing
 * the "right"-Button) storage_update_guard will check if you have set the storeage_outdated
 * indicator to TRUE. If so storage_update_guard will call the save routine of the config
 * class and show the user that his values are now saved on the SD card (or show an error)
 ********************************** storage_update_guard **********************************/
void menu::storage_update_guard(unsigned long* state, unsigned long old_state,bool storage_outdated, bool* updated_display){
	if(old_state*10+1==*state){ // coming from menu . shift us
		*state=(*state)*10+1;
	} else { // coming from selection, store value?
		back();
		// store to SD
		if(storage_outdated){
			TFT.clear_screen();
			char char_buffer[22];
			strcpy(char_buffer,("Save to SD card"));
			center_me(char_buffer,21);
			TFT.string(Speedo.default_font,char_buffer,0,5);
			bool remember_CAN_state=Sensors.CAN_active;
			Sensors.CAN_active=false;
			if(Config.write("BASE.TXT")==0){
				strcpy(char_buffer,("OK"));
			} else {
				strcpy(char_buffer,("FAILED !!"));
			}
			Sensors.CAN_active=remember_CAN_state;
			center_me(char_buffer,21);
			TFT.string(Speedo.default_font,char_buffer,0,6);

			_delay_ms(1200);
		};
	};
	update_display=true;
};

// overloader, see func below
void menu::set_value_dialog(int8_t* value,const char* title){
	set_value_dialog(value,title,("inactive"),("active"),(""), (""));
}

// overloader, see func below
void menu::set_value_dialog(int8_t* value,const char* title,const char* opt0,const char* opt1){
	set_value_dialog(value,title,opt0,opt1,(""), (""));
}

// overloader, see func below
void menu::set_value_dialog(int8_t* value,const char* title,const char* opt0,const char* opt1,const char* opt2){
	set_value_dialog(value,title,opt0,opt1,opt2, (""));
}

/*************** set_value_dialog ***********************
 * This function will generate a "selection" menu.
 * It shows the opts in lines and enables the user to
 * choose by "up" and "down" button. The value will be
 * stored in the "value" var. This could be an bool as well?!
 * Line0: False / 0
 * Line1: True / 1
 * Line2: True / 2
 * Line3: True / 3
 *************** set_value_dialog ***********************/
void menu::set_value_dialog(int8_t* value,const char* title,const char* opt0,const char* opt1,const char* opt2,const char* opt3){
	// first check how many options we have
	uint8_t opt_count=2;
	uint8_t line=3; // line 3+4
	if(strlen(opt2)>0){
		if(strlen(opt3)>0){
			opt_count=4; // line 2+3+4+5
			line=2;
		} else {
			opt_count=3; // line 2+3+4
			line=2;
		}
	}

	// handle button press
	if(state%10==9){ // "up" key
		*value=*value-1; // from true to false, or simply --
		Config.storage_outdated=true;
	} else if(state%10==2){ // "down" key
		*value=*value+1; // from false to true, or simply ++
		Config.storage_outdated=true;
	};
	if(*value<0){
		*value=0;
	} else if(*value>=opt_count){
		*value=opt_count-1;
	}

	state=floor(state/10)*10+1;

	// displaying values
	TFT.clear_screen();
	TFT.string((char*)title,2,0);

	unsigned char fg;
	unsigned char bg;
	const char* options[]={opt0,opt1,opt2,opt3};

	/// options //////////////////////////////
	for(int i=0;i<opt_count; i++){
		if(*value==i){
			fg=0x00;
			bg=DISP_BRIGHTNESS;
			TFT.highlight_bar(0,8*line,118,8); // mit hintergrundfarbe nen kasten malen
		} else {
			fg=DISP_BRIGHTNESS;
			bg=0x00;
		}
		TFT.string((char*)options[i],2,line,bg,fg);
		line++;
	}
	/// options //////////////////////////////


	TFT.string(Speedo.default_font,("\x7E save"),0,7);

	// key settings and corresponding var state changing
	bool up=true;
	bool down=true;
	if(*value==0){
		up=false;
	} else if(*value>=opt_count-1){
		down=false;
	}
	set_buttons(true,up,down,false); // button directions
}
