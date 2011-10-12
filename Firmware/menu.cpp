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
}

//prog_char asd_0[30000] PROGMEM ="a";
//prog_char asd_1[30000] PROGMEM ="a";


speedo_menu::~speedo_menu(){
};

/////////////////////////////// Menus PROGMEM ///////////////////////////////
/////
prog_char titel_0[] PROGMEM = "= Main menu =";   // "String 0" etc are strings to store - change to suit.
prog_char titel_1[] PROGMEM = ""; // tacho hat keine caption
prog_char titel_2[] PROGMEM = ""; // sprint hat keine caption
prog_char titel_3[] PROGMEM = "= Shown trips ="; // GPS hat keine caption
prog_char titel_4[] PROGMEM = "= Trips = ";
prog_char titel_5[] PROGMEM = "= Max speed =";
prog_char titel_6[] PROGMEM = "= AvG speed =";
prog_char titel_7[] PROGMEM = "";
prog_char titel_8[] PROGMEM = "= Setup =";
prog_char titel_9[] PROGMEM = "= Quick setup = ";

PROGMEM const char *menu_titel[10] = 	   // change "string_table" name to suit
{
		titel_0,titel_1,titel_2,titel_3,titel_4,titel_5,titel_6,titel_7,titel_8,titel_9};
/////
/////
prog_char main_m_0[] PROGMEM = "1. Speedoino";   // "String 0" etc are strings to store - change to suit.
prog_char main_m_1[] PROGMEM = "2. Sprint mode";
prog_char main_m_2[] PROGMEM = "3. Load Skin";
prog_char main_m_3[] PROGMEM = "4. Navigation";
prog_char main_m_4[] PROGMEM = "5. GPS infos";
prog_char main_m_5[] PROGMEM = "6. Trip Avg Max";
prog_char main_m_6[] PROGMEM = "7. Calc amount";
prog_char main_m_7[] PROGMEM = "8. Setup";
prog_char main_m_8[] PROGMEM = "9. Delete slots";

/////
/////
prog_char s_titel_0[] PROGMEM = "";
prog_char s_titel_1[] PROGMEM = "";
prog_char s_titel_2[] PROGMEM = "";
prog_char s_titel_3[] PROGMEM = "= Shown trips ="; // GPS hat keine caption
prog_char s_titel_4[] PROGMEM = "= Outline = ";
prog_char s_titel_5[] PROGMEM = "= xxxxx =";

PROGMEM const char *setup_titel[6] = 	   // change "string_table" name to suit
{
		s_titel_0,s_titel_1,s_titel_2,s_titel_3,s_titel_4,s_titel_5};

PROGMEM const char *menu_main[9] = 	   // change "string_table" name to suit
{
		main_m_0,main_m_1,main_m_2,main_m_3,main_m_4,main_m_5,main_m_6,main_m_7,main_m_8};
////
/////
prog_char setup_m_0[] PROGMEM = "1. Filemanager";   // "String 0" etc are strings to store - change to suit.
prog_char setup_m_1[] PROGMEM = "2. Gear values";   // "String 0" etc are strings to store - change to suit.
prog_char setup_m_2[] PROGMEM = "3. Shown trip";
prog_char setup_m_3[] PROGMEM = "4. Tire outline";
prog_char setup_m_4[] PROGMEM = "5. Display";
prog_char setup_m_5[] PROGMEM = "6. Reset mem";
prog_char setup_m_6[] PROGMEM = "7. DZ Flasher";
prog_char setup_m_7[] PROGMEM = "8. Daylight saving";
prog_char setup_m_8[] PROGMEM = "9. BT Reset state";

PROGMEM const char *menu_setup[9] = 	   // change "string_table" name to suit
{
		setup_m_0,setup_m_1,setup_m_2,setup_m_3,setup_m_4,setup_m_5,setup_m_6,setup_m_7,setup_m_8};
/////
/////
prog_char navi_m_0[] PROGMEM = "Navi state";   // "String 0" etc are strings to store - change to suit.
prog_char navi_m_1[] PROGMEM = "Set pointer";   // "String 0" etc are strings to store - change to suit.
prog_char navi_m_2[] PROGMEM = "Set file";   // "String 0" etc are strings to store - change to suit.
prog_char navi_m_3[] PROGMEM = "Check writes"; // hier vielleicht ein: way to ziel: koordinaten aktuell im vergleich zum ziel, aktuellen course
prog_char navi_m_4[] PROGMEM = "-";
prog_char navi_m_5[] PROGMEM = "-";
prog_char navi_m_6[] PROGMEM = "Show Animation";
prog_char navi_m_7[] PROGMEM = "Test areal";
prog_char navi_m_8[] PROGMEM = "Watchdog test";


PROGMEM const char *menu_navi[9] = 	   // change "string_table" name to suit
{
		navi_m_0,navi_m_1,navi_m_2,navi_m_3,navi_m_4,navi_m_5,navi_m_6,navi_m_7,navi_m_8};


/////
/////
prog_char trip_setup_m_0[] PROGMEM = "Total";   // "String 0" etc are strings to store - change to suit.
prog_char trip_setup_m_1[] PROGMEM = "NonPermanent";
prog_char trip_setup_m_2[] PROGMEM = "Day";
prog_char trip_setup_m_3[] PROGMEM = "Tour";
prog_char trip_setup_m_4[] PROGMEM = "Quick";
prog_char trip_setup_m_5[] PROGMEM = "Fuel";
prog_char trip_setup_m_6[] PROGMEM = "Oiler";
prog_char trip_setup_m_7[] PROGMEM = "Saison";
prog_char trip_setup_m_8[] PROGMEM = "Board";


PROGMEM const char *menu_trip_setup[9] = 	   // change "string_table" name to suit
{
		trip_setup_m_0,trip_setup_m_1,trip_setup_m_2,trip_setup_m_3,trip_setup_m_4,trip_setup_m_5,trip_setup_m_6,trip_setup_m_7,trip_setup_m_8};
/////
/////
prog_char flasher_m_0[] PROGMEM = "Off";
prog_char flasher_m_1[] PROGMEM = "Shifting";
prog_char flasher_m_2[] PROGMEM = "Warning";


PROGMEM const char *menu_flasher[3] = 	   // change "string_table" name to suit
{
		flasher_m_0,flasher_m_1,flasher_m_2
};

/////////////////////////////// Menus PROGMEM ///////////////////////////////
///// vars ////////////
bool		button_rechts_valid=true;
bool       	button_links_valid=true;
bool       	button_oben_valid=true;
bool       	button_unten_valid=true;
#define menu_button_links 24 // 26
#define menu_button_unten 28 // 22
#define menu_button_rechts 22 // 28
#define menu_button_oben 26 // 24
#define menu_active 0 // low active menues
#define menu_lines 7 // wieviele lines können wir darstellen 64 / 8 = 8 | 8-1(für die Headline)=7
int           menu_max=8;   // 9 eintrräge aber da das array bei 0 anfängt isses 9-1=8
int           menu_marker=0;
int           menu_start=0;
int           menu_ende=menu_lines-1;

#define menu_second_wait 100  // spike länge
int           fuel_added=90; // predefined value "added fuel"
bool       menu_preload;
///// vars ////////////

////// bei veränderung des state => einmaliges zeichen des menüs ///////
void speedo_menu::display(){ // z.B. state = 26
	if(MENU_DEBUG){
		Serial.println("menu.display() called;");
		Serial.print("state: ");
		Serial.println(state);
	};
	// ??? for(unsigned int i=0;i<sizeof(pSpeedo->disp_zeile_bak)/sizeof(pSpeedo->disp_zeile_bak[0]);i++){
	// [5][4][3][2][1][0]
	// 000001 => im hauptmenü, tacho standart ausgewählt
	// 000002 => im hautpmenü, sprint ausgewählt
	// 000031 => im tank reset menü,erste position
	// 000032 => im tank reset menü,zweite postion
	// 000041 => display setup position 1
	// 000011 => standart anzeige
	// 000340 => im Tacho reset, Trip4, [menu_button_rechts] -> also aktion nötig

	// init buttons
	bool button_state=true;
	if(BUTTONS_OFF){ button_state=false;}  // gundsätzlich sind alle an
	button_rechts_valid=button_state;
	button_links_valid=button_state;
	button_oben_valid=button_state;
	button_unten_valid=button_state;

	char *char_buffer;
	char_buffer = (char*) malloc (22);
	if (char_buffer==NULL) Serial.println("Malloc failed");
	else memset(char_buffer,'\0',22);
	///////////////////// im hauptmenü nach links => tacho //////////
	if(state==0){
		if(MENU_DEBUG){Serial.println("Menustate war 0, daher biege ich ihn auf 11 um");};
		state=11;
		display();
	}
	///////////////////// im hauptmenü nach links => tacho //////////
	///////////////////// Hauptmenu //////////////////////////
	else if(state<10) {
		if(MENU_DEBUG){Serial.println("Menustate war kleiner als 10, Hauptmenue");};
		// Menu vorbereiten
		draw(&menu_main[0],sizeof(menu_main)/sizeof(menu_main[0]));
	}
	//////////////////////// hier eine gps markierungs motivieren /////////////////////////7
	else if(state==111) {
		if(MENU_DEBUG){Serial.println("Menustate war 111, daher biege ich ihn auf 11 um");};
		pSensors->m_gps->note_this_place=SIMPLE_MARK;
		state=11;
	}
	//////////////////////// Einmaliges Setup des Standart Tacho ////////////////////////////
	else if(floor(state/10)==1 || state==8511111){
		pOLED->clear_screen();
		if(MENU_DEBUG){Serial.println("Menustate 00001X, Bin jetzt im Tacho menu, zeichne icons");};

		if(pSpeedo->oil_widget.symbol && !(pSpeedo->oil_widget.x==-1 && pSpeedo->oil_widget.y==-1))
			pOLED->draw_oil(pSpeedo->oil_widget.x*3,pSpeedo->oil_widget.y*8); //3=6/2 weil doppelpixxel
		if(pSpeedo->fuel_widget.symbol && !(pSpeedo->fuel_widget.x==-1 && pSpeedo->fuel_widget.y==-1))
			pOLED->draw_fuel(pSpeedo->fuel_widget.x*3,pSpeedo->fuel_widget.y*8); // 7 => 56
		if(pSpeedo->air_widget.symbol && !(pSpeedo->air_widget.x==-1 && pSpeedo->air_widget.y==-1))
			pOLED->draw_air(pSpeedo->air_widget.x*3,pSpeedo->air_widget.y*8);
		if(pSpeedo->clock_widget.symbol && !(pSpeedo->clock_widget.x==-1 && pSpeedo->clock_widget.y==-1))
			pOLED->draw_clock(pSpeedo->clock_widget.x*3,pSpeedo->clock_widget.y*8);

		pSpeedo->reset_bak(); // alle disp_zeile_bak auf -99 setzen

		// wenn wir nicht in der navigation sind, die tasten für hoch und runter deaktiveren
		if(!pSensors->m_gps->navi_active){
			button_oben_valid=false;
			button_unten_valid=false;
		};
	}
	//////////////////////// Einmaliges Setup des Sprint Tacho ////////////////////////////
	else if(floor(state/10)==2){
		if(MENU_DEBUG){Serial.println("Menustate=000002X, Bin jetzt im Sprint Tacho menu");};
		pOLED->clear_screen();
		pSprint->done=false;
		pSprint->lock=false;
		button_rechts_valid=false;
		button_oben_valid=false;
		button_unten_valid=false;
		pSpeedo->reset_bak(); // alle disp_zeile_bak auf -99 setzen
	}
	///////////////////// skin menu //////////////////////////
	else if(floor(state/10)==3){
		// open SD
		SdFile root;
		root.openRoot(&pSD->volume);
		SdFile subdir;
		if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  Serial.println("open subdir /config failed"); };

		// generate filename
		char filename[10];
		sprintf(filename,"SKIN%i.TXT",int((state%10)-1));

		// check if file exists
		SdFile file;
		if (file.open(&subdir, filename, O_READ)){ //kann ich offentsichtlich ändern, datei auslesen
			int n=1;
			int byte_read=0;
			// reserve buffer space
			char *buffer;
			buffer = (char*) malloc (65);
			if (buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
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
			sprintf(title,"Skinfile Nr: %i/8",(state%10)-1);
			if(buffer[0]=='#'){
				pOLED->show_storry(buffer,byte_read,title,sizeof(title)/sizeof(title[0]));
			} else {
				pOLED->clear_screen();
				pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string(STD_SMALL_1X_FONT,title,2,0,DISP_BRIGHTNESS,0,0);
				pOLED->string_P(STD_SMALL_1X_FONT,PSTR("No first line comment"),0,3,0,DISP_BRIGHTNESS,0);
			}
			// noch mal kurz den buffer missbrauchen
			sprintf(buffer,"  Preview -    Save");
			buffer[0]=127;
			buffer[12]=127;
			buffer[13]=127;
			pOLED->string(STD_SMALL_1X_FONT,buffer,0,7,0,DISP_BRIGHTNESS,0);
			// free buffer
			free(buffer);
			// end of draw to display
		}
		else { // if there is no file with that number
			pOLED->clear_screen();
			char title[17];
			sprintf(title,"Skinfile Nr: %i/8",(state%10)-1);
			pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
			pOLED->string(STD_SMALL_1X_FONT,title,2,0,DISP_BRIGHTNESS,0,0);
			pOLED->string_P(STD_SMALL_1X_FONT,PSTR("File not found"),2,3,0,DISP_BRIGHTNESS,0);
		};
		subdir.close();
		root.close();

		// keep loading current skinfile, because without changing
		sprintf(filename,"SKIN%i.TXT",(pConfig->skin_file+10)%10);
		pConfig->read(filename);
	}
	//////////////////////// skin laden ////////////////////////////
	else if(floor(state/100)==3) { // 0031X
		// load this skinfile
		char filename[10];
		sprintf(filename,"SKIN%i.TXT",(int(floor(state/10))%10)-1);
		pConfig->read(filename);
		pOLED->clear_screen();
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Preview"),6,3);
		delay(300);

		// set buttons
		pMenu->button_links_valid=true;
		pMenu->button_rechts_valid=true;
		pMenu->button_oben_valid=false;
		pMenu->button_unten_valid=false;

		// save,change,restore,and show one preview image
		unsigned long save_state=state;
		state=11;
		display();
		state=save_state;
		pSpeedo->loop(0);
	}
	//////////////////////// skin speichern -> zum menüpunkt 11 gehen und alles ist gut ////////////////////////////
	else if(floor(state/1000)==3) { // 0031X
		pOLED->clear_screen();
		pConfig->skin_file=(int(floor(state/100))%10)-1;
		pConfig->storage_outdated=true;
		pConfig->write("BASE.TXT"); // save config

		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Saved"),7,3);
		delay(300);
		pOLED->clear_screen();

		pMenu->button_links_valid=true;
		pMenu->button_rechts_valid=true;
		pMenu->button_oben_valid=true;
		pMenu->button_unten_valid=true;
		state=11;
		display();
	}//////////////////////// Menü für die Navigation ////////////////////////////
	/* hier noch ein "welche datei wollen sie, ding"
	 *
	 */
	else if(floor(state/10)==4){
		// Menu vorbereiten
		draw(&menu_navi[0],sizeof(menu_navi)/sizeof(menu_navi[0]));
	}
	//////////////////////// Navigation an/aus schalten ////////////////////////////
	else if(floor(state/10)==41){
		if((state%10)==9){
			pSensors->m_gps->navi_active=true;
			pSensors->m_gps->generate_new_order();
			byte tempByte = (1 & 0xFF);
			EEPROM.write(146,tempByte);
		} else if((state%10)==2){
			pSensors->m_gps->navi_active=false;
			byte tempByte = (0 & 0xFF);
			EEPROM.write(146,tempByte);
		};
		state=411;
		pOLED->clear_screen();
		if(pSensors->m_gps->navi_active){
			sprintf(char_buffer," active");
		} else {
			sprintf(char_buffer,"inactive");
		};
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->highlight_bar(0,8*3-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Navigation"),4,3,15,0,0);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,5,4,15,0,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Down = inactive"),4,7,0,DISP_BRIGHTNESS,0);

		button_rechts_valid=false; // nach rechts gehen geht nicht
	}
	//////////////////////// Pointer für die Navigation ////////////////////////////
	else if(floor(state/10)==42){
		if((state%10)==2){ // runter gedrückt
			pSensors->m_gps->navi_point++;
			state=421;
		} else if((state%10)==9){ // hoch gedrückt
			pSensors->m_gps->navi_point--;
			if(pSensors->m_gps->navi_point<1) { pSensors->m_gps->navi_point=0; };
			state=421;
		};

		pOLED->clear_screen();
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Navi Track Pointer"),0,0,0,DISP_BRIGHTNESS,0);
		pSensors->m_gps->generate_new_order(); // vor ausgabe von pSensors->m_gps->navi_point, wenn pSensors->m_gps->navi_point nach dem knopfdruck zu hoch war wirds in dieser fkt zurück gesetzt
		sprintf(char_buffer,"#%3i:",pSensors->m_gps->navi_point);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,0,2,0,DISP_BRIGHTNESS,0);
		pOLED->string(STD_SMALL_1X_FONT,pSensors->m_gps->navi_ziel_name,7,2,0,DISP_BRIGHTNESS,0);        // hier vielleicht die jeweilige straße einblenden
		sprintf(char_buffer,"Long: %05i%04i",int(floor(pSensors->m_gps->navi_ziel_long/10000)),int(pSensors->m_gps->mod(pSensors->m_gps->navi_ziel_lati,10000)));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,2,6,0,DISP_BRIGHTNESS,0);        // hier vielleicht die jeweilige straße einblenden
		sprintf(char_buffer,"Lati: %05i%04i",int(floor(pSensors->m_gps->navi_ziel_lati/10000)),int(pSensors->m_gps->mod(pSensors->m_gps->navi_ziel_lati,10000))); // typisch 052033224 => 5203,3224 => kann pro feld bis zu 32767 => 3.276.732.767 => 3.276° was quasi 8 mal um die welt geht
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,2,7,0,DISP_BRIGHTNESS,0);        // hier vielleicht die jeweilige straße einblenden

		button_rechts_valid=false; // nach rechts gehen geht nicht
	}
	////////////////////////// Dateien listen und highlighten... irgendwie //////////////
	else if(floor(state/10)==43){
		// Umschalten
		if(int(state%10)==9){ // schalter nach oben, abziehen
			if(pSensors->m_gps->active_file>0)
				pSensors->m_gps->active_file--;
		} else  if(int(state%10)==2){
			if(pSensors->m_gps->active_file < 9) // wir haben 0,1,2,3,4,5,6,7,8,9 ... max 10 Files
				pSensors->m_gps->active_file++;
		};
		// immer wieder zurückschuppsen
		state=431;
		pConfig->storage_outdated=true;
		pConfig->write("BASE.TXT"); // save config
		// open SD
		SdFile root;
		root.openRoot(&pSD->volume);
		SdFile subdir;
		if(!subdir.open(&root, NAVI_FOLDER, O_READ)) {  Serial.println("open subdir /config failed"); };
		// generate filename
		char navi_filename[13];
		sprintf(navi_filename,"NAVI%i.TXT",pSensors->m_gps->active_file);
		char buffer[22]; // 21 zeichen + \0
		sprintf(buffer,"Navifile Nr: %i",pSensors->m_gps->active_file);
		// file exists
		SdFile file;
		if (file.open(&subdir, navi_filename, O_READ)){ //kann ich offentsichtlich ändern, datei auslesen
			int n=1;
			int byte_read=0;
			// reserve buffer space
			char buffer_big[65];
			//memset(buffer_big,'\0',65);

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
				pOLED->string(STD_SMALL_1X_FONT,title,2,0,DISP_BRIGHTNESS,0,0);
				pOLED->string_P(STD_SMALL_1X_FONT,PSTR("No first line comment"),0,3,0,DISP_BRIGHTNESS,0);
			}
		} else { // if there is no file with that number
			pOLED->clear_screen();
			char title[17];
			sprintf(title,"Navifile Nr: %i/8",pSensors->m_gps->active_file);
			pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
			pOLED->string(STD_SMALL_1X_FONT,title,2,0,DISP_BRIGHTNESS,0,0);
			pOLED->string_P(STD_SMALL_1X_FONT,PSTR("File not found"),2,3,0,DISP_BRIGHTNESS,0);
		};
		subdir.close();
		root.close();
	}
	//////////////////////// ausgeben wieviele points geschrieben wurden //////////////
	else if(floor(state/10)==44){
		char buffer[10];
		sprintf(buffer,"%i",pSensors->m_gps->written_gps_points);
		pOLED->clear_screen();
		pOLED->string(STD_SMALL_1X_FONT,buffer,5,3,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Points written"),3,4,0,DISP_BRIGHTNESS,0);
	}
	//////////////////////// show animation ////////////////////////
	else if(floor(state/10)==47){
		pOLED->animation(state%10);
	}
	//////////////////////// TEST des arrows////////////////////////
	else if(floor(state/10)==48){
		pOLED->clear_screen();

		//#define STD_SMALL_4X_FONT		4
		//#define STD_MEDIUM_1X_FONT		5
		//#define STD_MEDIUM_2X_FONT		6
		//#define STD_BIG_1X_FONT			7
		//#define SANS_SMALL_4X_FONT		11
		//#define SANS_MEDIUM_1X_FONT		12
		//#define SANS_MEDIUM_2X_FONT		13
		//#define SANS_BIG_1X_FONT		14
		//#define VISITOR_SMALL_4X_FONT	18


		//			for(int x=0;x<21;x++){
		//				for(int y=0;y<7;y++){
		//					char buffer[2];
		//					sprintf(buffer,"%c",char(y*21+x+'!'-1));
		//					pOLED->string(tempfont,buffer,x,y,0,15,0);
		//				}
		//			}
		pOLED->string_P(VISITOR_SMALL_1X_FONT,PSTR("HALLO"),0,0);


	}
	//////////////////////// TEST des watchdogs durch absitzen ////////////////////////
	else if(floor(state/10)==49){
		delay(29999); // knapp 30 sec
	}
	//////////////////////// Einmaliges Setup des GPS Tacho ////////////////////////////
	else if(floor(state/10)==5) {
		pOLED->clear_screen();
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("GPS_time"),0,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("GPS_date"),0,1);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("GPS_long"),0,2);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("GPS_lati"),0,3);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("GPS_alti"),0,4);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("GPS_speed"),0,5);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("GPS_sats"),0,6);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Counter"),0,7);

		button_rechts_valid=false; // nach rechts gehen geht nicht
		button_oben_valid=false;
		button_unten_valid=false;

		pSpeedo->reset_bak(); // alle disp_zeile_bak auf -99 setzen
	}
	//////////////////////// trip|avg|max Menu ////////////////////////////
	else if(floor(state/10)==6) {
		// Menu vorbereiten
		draw(&menu_trip_setup[0],sizeof(menu_trip_setup)/sizeof(menu_trip_setup[0]));
	}
	///////////////////// ansicht eines stands //////////////////////////
	else if(floor(state/100)==6 && state%10==1) {
		pOLED->clear_screen();

		int speicher=(int(floor(state/10))%10)-1;
		strcpy_P(char_buffer, (char*)pgm_read_word(&(menu_trip_setup[(int(floor(state/10))%10)-1])));
		pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,5,0,DISP_BRIGHTNESS,0,0);
		sprintf(char_buffer,"Avg:      %3i km/h",(int)round(pSpeedo->trip_dist[speicher]*3.6/pSpeedo->avg_timebase[speicher]));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,0,2,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,"Time:    %02i:%02i:%02i",(int)round(pSpeedo->avg_timebase[speicher]/3600),(int)round((pSpeedo->avg_timebase[speicher]%3600)/60),(int)round(pSpeedo->avg_timebase[speicher]%60));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,0,3,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,"Trip: %5lu,%02i km",(unsigned long)floor(pSpeedo->trip_dist[speicher]/1000),int(floor((pSpeedo->trip_dist[speicher]%1000)/10)));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,0,4,0,DISP_BRIGHTNESS,0);

		sprintf(char_buffer,"Max:       %3i km/h",(int)pSpeedo->max_speed[speicher]);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,0,5,0,DISP_BRIGHTNESS,0);
	}
	//////////////////// quick jump to next storage //////////////////////
	else if(floor(state/100)==6) { // für alle 0006XX also im menü umgeschaltet, aber die root ist 6 also 611 + unten = 612 und wird hier zu 621 gebaut
		if((state%10)==2){
			if(MENU_DEBUG){  Serial.print("Menustate alt:"); Serial.println(state);  };
			state+=9;
			if(MENU_DEBUG){
				Serial.print("Menustate neu:"); Serial.println(state);
				Serial.println("Menustate % 10 == 2, -> +9");
			};
		} else if((state%10)==9){
			if(MENU_DEBUG){  Serial.print("Menustate alt:"); Serial.println(state); };
			state-=18;
			if(MENU_DEBUG){
				Serial.print("Menustate neu:"); Serial.println(state);
				Serial.println("Menustate % 10 == 9, -> -8");
			};
		}
		if(state>700){ // von 9 auf 1 springen
			state=611;
		}else if(state<610){ // von 1 auf 9 springen
			state=691;
		};
		display();
	}
	//////////////////// quick jump to delete storages //////////////////////
	// state 006X11
	else if((state/1000)%10==6){
		// aus 006[X]11 -> 009[X]1 machen
		state=901;
		state+=(int(floor(state/100))%10)*10;
	}
	//////////////////////// fuel added /////////////////////////////
	else if(state==71){
		pOLED->clear_screen();
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Added fuel amount:"),1,3,0,DISP_BRIGHTNESS,4);
		sprintf(char_buffer,"%2i,%0i l",(int)floor(fuel_added/10),(int)fuel_added%10);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,7,5,0,DISP_BRIGHTNESS,0);
	}
	else if(state==79 || state==72){
		if(state==79) {
			fuel_added++;
		}
		else {
			fuel_added--;
		};
		if(fuel_added<1){
			fuel_added=1;
		}
		else if(fuel_added>800) {
			fuel_added=800;
		};

		sprintf(char_buffer,"%2i,%0i l",(int)floor(fuel_added/10),(int)fuel_added%10);
		pOLED->filled_rect(30,40,60,10,0);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,7,5,0,DISP_BRIGHTNESS,0);
		state=71;
	}
	else if(floor(state/100)==7){
		pOLED->clear_screen();
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Average need: "),4,2,0,DISP_BRIGHTNESS,0);
		sprintf(char_buffer,"%2i,%0i l/100km",(int)floor(fuel_added*10/(pSpeedo->trip_dist[3]/1000)),(int)(fuel_added*100/(pSpeedo->trip_dist[3]/1000))%10);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,6,3,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("All Fuel"),7,6,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("storeages reseted"),3,7,0,DISP_BRIGHTNESS,0);
		pSpeedo->trip_dist[5]=0;
		pSpeedo->avg_timebase[5]=0;
		pSpeedo->max_speed[5]=0;
		state=71;
		pConfig->storage_outdated=true;
		pConfig->write("speedo.txt");
		state=1; // damit kann man durch rechts/links drücken wieder zum tacho springen
	}
	//////////////////////// Setup Menu ////////////////////////////
	else if(floor(state/10)==8) { //00008X
		// Menu vorbereiten
		draw(&menu_setup[0],sizeof(menu_setup)/sizeof(menu_setup[0]));
		menu_preload=true;
	}
	///////////////////// Helligkeitseinstellung //////////////////////////
	else if(floor(state/10)==81) { // 00081X
		pOLED->clear_screen();
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Filemanager"),4,3,0,DISP_BRIGHTNESS,0);
		button_oben_valid=false;
		button_unten_valid=false;
		button_rechts_valid=false; // nach rechts gehen geht nicht

		pFilemanager->run();
	}
	//////////////////// calibration message ///////////////////
	else if(floor(state/10)==82){
		if(state%10>6){	// gänge höher als 6
			state=826;
			display();
		} else {		// erstmal ne message
			pOLED->clear_screen();
			pOLED->highlight_bar(0,0,128,8);
			pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Calibrate Gear"),2,0,15,0,0);
			char temp[2];
			sprintf(temp,"%i",state%10);
			pOLED->string(STD_SMALL_1X_FONT,temp,17,0,15,0,0);
			pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Change Gear up/down"),0,3);
			pOLED->string_P(STD_SMALL_1X_FONT,PSTR("!Keep driving!"),3,4);
			pOLED->string_P(STD_SMALL_1X_FONT,PSTR("  continue"),0,7);
			sprintf(temp,"%c",127);
			pOLED->string(STD_SMALL_1X_FONT,temp,0,7);
			pSensors->m_gear->faktor_counter=0;
		};
	}
	//////////////////// calibration start -> reset values, call in loop the gear_calc fkt ///////////////////
	else if(floor(state/100)==82){
		button_oben_valid=false;
		button_unten_valid=false;
		pOLED->clear_screen();
		pSpeedo->reset_bak();
	}
	//////////////////// calibration done, save it now.  ///////////////////
	else if(floor(state/1000)==82){
		button_links_valid=false;
		button_rechts_valid=false;
		button_oben_valid=false;
		button_unten_valid=false;

		pSensors->m_gear->n_gang[int(pSensors->m_gps->mod(pMenu->state,1000))/100]=pSensors->m_gear->faktor_flat;
		pConfig->storage_outdated=true;
		pConfig->write("GANG.TXT");

		pOLED->clear_screen();
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Saved"),7,3);
		delay(300);
		state=821;
		display();
	}
	///////////////////// trip show setup //////////////////////////
	else if(floor(state/10)==83) {
		if(menu_preload){ // wird im Menü davor auf true gesetzt, erlaubt vorpositionieren des menüs.
			menu_preload=false; // damit beim ändern der preload nicht ausgeführt wird.
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
			pSpeedo->m_trip_mode=state%10;
			byte tempByte = (pSpeedo->m_trip_mode & 0xFF);
			EEPROM.write(2,tempByte);
		};
		draw(&menu_trip_setup[0],sizeof(menu_trip_setup)/sizeof(menu_trip_setup[0]));
		// hier noch 2do
	}
	///////////////////// speed calibration //////////////////////////
	else if(floor(state/10)==84) {
		pOLED->clear_screen();
		pSpeedo->reset_bak(); // dadurch wird auch der counter resetet
	}
	/////////////// save new value ////////////
	else if(pMenu->state==8411){
		pOLED->clear_screen();
		pSensors->m_speed->reifen_umfang=pSensors->m_speed->flat_value_calibrate_umfang/100;
		pConfig->storage_outdated=true;
		pConfig->write("BASE.TXT");
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("saved"),6,3);
		delay(300);
		state=841;
		display();
	}
	////////// display phase ////////////////
	else if(floor(state/10)==85 || floor(state/10)==851|| floor(state/10)==8511 || floor(state/10)==85111) {
		if(state%10==9){
			if(floor(state/10)==85) { pOLED->phase=pOLED->phase+16; }
			else if(floor(state/10)==851) {pOLED->phase++;}
			else if(floor(state/10)==8511) {pOLED->ref=pOLED->ref+16;}
			else if(floor(state/10)==85111) {pOLED->ref++;}

			pOLED->init(pOLED->phase,pOLED->ref);
			byte tempByte = (pOLED->phase & 0xFF);
			EEPROM.write(144,tempByte);
			tempByte = (pOLED->ref & 0xFF);
			EEPROM.write(145,tempByte);

		} else if(state%10==2){
			if(floor(state/10)==85) { pOLED->phase=pOLED->phase-16; }
			else if(floor(state/10)==851) {pOLED->phase--;}
			else if(floor(state/10)==8511) {pOLED->ref=pOLED->ref-16;}
			else if(floor(state/10)==85111) {pOLED->ref--;}

			pOLED->init(pOLED->phase,pOLED->ref);
			byte tempByte = (pOLED->phase & 0xFF);
			EEPROM.write(144,tempByte);
			tempByte = (pOLED->ref & 0xFF);
			EEPROM.write(145,tempByte);
		};

		int olc=0,orc=0;
		int ulc=0,urc=0;
		if(floor(state/10)==85)       { state=851; olc=1; orc=0; ulc=0; urc=0;}
		else if(floor(state/10)==851) {state=8511; olc=0; orc=1; ulc=0; urc=0;}
		else if(floor(state/10)==8511) {state=85111; olc=0; orc=0; ulc=1; urc=0;}
		else if(floor(state/10)==85111) {state=851111; olc=0; orc=0; ulc=0; urc=1;};

		pOLED->clear_screen();
		pOLED->highlight_bar(0,8*1-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Choose phase"),4,1,15,0,0);
		sprintf(char_buffer," %02i ",int(floor(int(pOLED->phase)/16)));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,4,2,abs(olc-1)*15,olc*15,0);
		pOLED->string(STD_SMALL_1X_FONT," // ",8,2,15,0,0);
		sprintf(char_buffer," %02i ",int(int(pOLED->phase)%16));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,12,2,abs(orc-1)*15,orc*15,0);

		pOLED->highlight_bar(0,8*5-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Choose ref V"),4,5,15,0,0);
		sprintf(char_buffer," %02i ",int(floor(int(pOLED->ref)/16)));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,4,6,abs(ulc-1)*15,ulc*15,0);
		pOLED->string(STD_SMALL_1X_FONT," // ",8,6,15,0,0);
		sprintf(char_buffer," %02i ",int(int(pOLED->ref)%16));
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,12,6,abs(urc-1)*15,urc*15,0);
	}
	///////////////////// reset setup //////////////////////////
	else if(state==861) {
		yesno("Really reset","total EEPROM","");
	}
	else if(state==862) {
		byte tempByte;
		tempByte = ((int)1 & 0xFF);
		EEPROM.write(2, tempByte);   // tripmode=1
		tempByte = ((int)15 & 0xFF);
		EEPROM.write(147, tempByte); // Navi_pos=0
		EEPROM.write(148, tempByte); // Winterzeit=0
		EEPROM.write(149, tempByte); // reset_enable=0
		tempByte = ((int)92 & 0xFF);
		EEPROM.write(150, tempByte); // fuel_max=9.2 l
		tempByte = ((int)0 & 0xFF);
		EEPROM.write(151, tempByte); // fuel_step=0
		EEPROM.write(152, tempByte); // fuel_counter=0

		del_conf("The memory","has been reset");
		state=11;
	}
	/////////// adjust  //////////
	////////// dz flahser ////////////////
	else if(floor(state/10)==87) {
		button_rechts_valid=false;
		if(state%10==9){
			pSensors->m_dz->blitz_en=true;
			byte tempByte = (1 & 0xFF);
			EEPROM.write(146,tempByte);
		} else if(state%10==2){
			pSensors->m_dz->blitz_en=false;
			byte tempByte = (0 & 0xFF);
			EEPROM.write(146,tempByte);
		};
		state=871;
		pOLED->clear_screen();
		if(pSensors->m_dz->blitz_en){
			sprintf(char_buffer," active");
		} else {
			sprintf(char_buffer,"inactive");
		};
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->highlight_bar(0,8*3-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("DZ Flasher"),5,3,15,0,0);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,5,4,15,0,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Down = inactive"),4,7,0,DISP_BRIGHTNESS,0);
	}
	/////////// daylightsaving  //////////
	else if(floor(state/10)==88) { // 00088X - hier jetzt winterzeit an aus
		button_rechts_valid=false;
		if(state%10==9){           pSensors->m_clock->set_date_time(-1,-1,-1,-1,-1,-1,-1,1); /* nur noch gmt+2 -1 */  }
		else if(state%10==2){      pSensors->m_clock->set_date_time(-1,-1,-1,-1,-1,-1,-1,2); /* gmt+2 */              };

		if(state%10==9 || state%10==2 ) { // has changed, save it
			byte tempByte = (pSensors->m_clock->get_dayls() & 0xFF);
			EEPROM.write(148,tempByte);
		};

		state=881; // resume state
		pOLED->clear_screen();
		if(pSensors->m_clock->get_dayls()==1){
			sprintf(char_buffer," active");
		} else if(pSensors->m_clock->get_dayls()==2) {
			sprintf(char_buffer,"inactive");
		} else {
			sprintf(char_buffer,"error");
		};
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->highlight_bar(0,8*3-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Daylight saving"),3,3,15,0,0);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,5,4,15,0,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Down = inactive"),4,7,0,DISP_BRIGHTNESS,0);
	}
	/////////// bt reset state  //////////
	else if(floor(state/10)==89) { // 00089X
		button_rechts_valid=false;
		if(state%10==9){
			pSensors->m_reset->set_active(true,true); // set eeprom,set var
		} else if(state%10==2){
			pSensors->m_reset->set_deactive(true,true);  // set eeprom,set var
		};
		state=891;
		pOLED->clear_screen();
		if(pSensors->m_reset->reset_enabled){
			sprintf(char_buffer," active");
		} else {
			sprintf(char_buffer,"inactive");
		};
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Up = active"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->highlight_bar(0,8*3-1,128,17); // mit hintergrundfarbe nen kasten malen
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("BT-Reset"),5,3,15,0,0);
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,5,4,15,0,0);
		pOLED->string_P(STD_SMALL_1X_FONT,PSTR("Down = inactive"),4,7,0,DISP_BRIGHTNESS,0);

		// show reason why last reset happend
		pOLED->string(STD_SMALL_1X_FONT,"Last was ",4,1,0,DISP_BRIGHTNESS,0);
		if(pSensors->m_reset->last_reset==0 || pSensors->m_reset->last_reset==-1){
			pOLED->string_P(STD_SMALL_1X_FONT,PSTR("power"),13,1,0,DISP_BRIGHTNESS,0);
		} else if(pSensors->m_reset->last_reset==1){
			pOLED->string(STD_SMALL_1X_FONT,"avr",13,1,0,DISP_BRIGHTNESS,0);
		} else if(pSensors->m_reset->last_reset==2){
			pOLED->string(STD_SMALL_1X_FONT,"bt",13,1,0,DISP_BRIGHTNESS,0);
		}
		// show reason why last reset happend
	}
	//////////////////// quick start //////////////////////////
	else if(floor(state/10)==9){ // 00009X
		draw(&menu_trip_setup[0],sizeof(menu_trip_setup)/sizeof(menu_trip_setup[0]));
	}
	else if(floor(state/100)==9){ // 0009XX, jetzt alles löschen
		pSpeedo->max_speed[(int(floor(state/10))%10)-1]=0;
		pSpeedo->avg_timebase[(int(floor(state/10))%10)-1]=0;
		pSpeedo->trip_dist[(int(floor(state/10))%10)-1]=0;
		// prepare save
		pConfig->storage_outdated=true;
		pConfig->write("speedo.txt");

		pOLED->clear_screen();
		// show the title
		char temp[30];
		strcpy_P(temp, (char*)pgm_read_word(&(menu_trip_setup[(int(floor(state/10))%10)])));
		sprintf(char_buffer,"%s cleared.",temp); // hier noch die bezeichnung
		pOLED->string(STD_SMALL_1X_FONT,char_buffer,2,3,0,DISP_BRIGHTNESS,0);
		delay(800);
		state=11;
		menu_max=8;
		display();
	}
	///////////////////// "notfall" //////////////////////////
	else {
		back();
	};
	free(char_buffer);
	///////////////////// E N D E //////////////////////////
};
////// bei veränderung des state => einmaliges zeichen des menüs ///////

//// ein rückschritt im menü machen ////////
void speedo_menu::back(){
	int return_value=floor(state/10);
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
	if(floor(state/100)==0 && ((int)floor(state/10))%10 == 0){
		button_links_valid=false;
	};
};
//// ein rückschritt im menü machen ////////

/// eine rückfrage: soll ich wirklich löschen ////
void speedo_menu::yesno(char first[30],char second[30],char third[30]){
	// Menu vorbereiten
	pOLED->clear_screen();
	pOLED->string(STD_SMALL_1X_FONT,first,4,1,0,DISP_BRIGHTNESS,0);
	pOLED->string(STD_SMALL_1X_FONT,second,4,2,0,DISP_BRIGHTNESS,0);
	pOLED->string(STD_SMALL_1X_FONT,third,0,3,0,DISP_BRIGHTNESS,0);
	pOLED->string_P(STD_SMALL_1X_FONT,PSTR("yes=>down    no=>left"),0,7,0,DISP_BRIGHTNESS,0);
	button_rechts_valid=false;
	button_oben_valid=false;
};
/// eine rückfrage: soll ich wirklich löschen ////

/// bestätigen eines löschens ///
void speedo_menu::del_conf(char first[20],char second[20]){
	pOLED->clear_screen();
	pOLED->string(STD_SMALL_1X_FONT,first,6,2,0,DISP_BRIGHTNESS,0);
	pOLED->string(STD_SMALL_1X_FONT,second,2,4,0,DISP_BRIGHTNESS,0);

	delay(1200);
	back();
	display();
};
/// bestätigen eines löschens ///

///// ein text menü zeichnen ////////////
void speedo_menu::draw(const char** menu, int entries){
	int   level_1=(state/10)%10;

	if(MENU_DEBUG){
		Serial.print("Bin im menue, menu_marker:");
		Serial.print(menu_marker);
		Serial.print(", menu_ende:");
		Serial.print(menu_ende);
		Serial.print(", menu_start:");
		Serial.println(menu_start);
	};
	////////// Menu Caption /////////////
	pOLED->filled_rect(0,0,128,12,0);
	char char_buffer[22];

	if(int(floor(state/100))%10==8){ // menu 8 hat eigene Titel
		strcpy_P(char_buffer, (char*)pgm_read_word(&(setup_titel[level_1])));
	} else {
		strcpy_P(char_buffer, (char*)pgm_read_word(&(menu_titel[level_1])));
	};
	pOLED->string(STD_SMALL_1X_FONT,char_buffer,0,0,0,DISP_BRIGHTNESS,0);

	sprintf(char_buffer,"%04lu",state);
	pOLED->string(STD_SMALL_1X_FONT,char_buffer,16,0,0,DISP_BRIGHTNESS,0);
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
	if (buffer==NULL) Serial.println("Malloc failed");
	else memset(buffer,'\0',30);

	for(int k=menu_start;k<=menu_ende;k++){
		if(y==(menu_marker+1)){
			fg=0;
			hg=DISP_BRIGHTNESS;
			pOLED->highlight_bar(0,8*y-1,128,9); // mit hintergrundfarbe nen kasten malen
		} else {
			fg=DISP_BRIGHTNESS;
			hg=0;
			pOLED->filled_rect(0,8*y,128,8,0); // mit hintergrundfarbe nen kasten malen
		};
		// copy string vom flash
		strcpy_P(buffer, (char*)pgm_read_word(&(menu[k])));
		pOLED->string(STD_SMALL_1X_FONT,buffer,2,y,hg,fg,0);
		// copy string vom flash
		y++; // abstand festlegen
	};
	// free text buffer
	free(buffer);

	while(y<8){ // die zeilen unter dem Menü ausmmalen
		pOLED->filled_rect(0,8*y,128,8,0); // mit hintergrundfarbe nen kasten malen
		y++;
	}
	///////// Menu ausgeben ////////////
};
///// ein text menü zeichnen ////////////

///// zyklisches abfragen der buttons ////////////
bool speedo_menu::button_test(bool bt_keys_en){
	// also entweder ist der letzte button_down schon länger als der timeout her
	// oder zumindest länger als fast_timeout UND der first push ist ausreichend lang her
	// oder per serielle konsole
	int input=0;
	if(bt_keys_en){ // auf keinen Fall hier lesen, wenn wir gerade im Import sind, sonder haut uns jeder Straßenname mit w||a||s||d raus
		if(Serial.available()>10){ // wenns zuviele sind flushen
			Serial.flush();
		} else if(Serial.available()>0){ // an sonsten gern
			input=Serial.read(); // sowohl pfeiltasten wie auch /w/asd zulassen

			// einen neuen menustate empfangen
			if(char(input)=='*'){
				int new_menustate=0;

				char *char_buffer;
				char_buffer = (char*) malloc (22);
				if (char_buffer==NULL) Serial.println("Malloc failed");

				pFilemanager->get_filename(&char_buffer[0]); // missbraucht als "get_new_menustate"
				bool found_end=false; // zum abbrechen der schleife
				for(int a=0; !found_end && a<8;a++){ // menustates max 8 stellen
					if(char(char_buffer[a])=='*'){ // enden mit einem weitern stern
						found_end=true;
					} else { // ansonsten, wenn es kein end stern ist, aber eine valide zahl dann packs in den menustate
						if(char_buffer[a]>=48 && char_buffer[a]<=57){
							new_menustate=new_menustate*10+(char_buffer[a]-48);
						};
					};
				}
				// wenn es eine eingabe war, dann hau die hier rein
				if(new_menustate!=0){
					state=new_menustate;
					return true;
				};
				free(char_buffer);
			};
			// ende des empfangs eine neues menustate

			if(input!=65 && input!=66 && input!=67 && input!=68 && char(input)!='a' && char(input)!='w' && char(input)!='d' && char(input)!='s'){
				input=0;
			};
		};
	};

	if(millis()>(button_time+menu_button_timeout) ||
			(button_first_push>0 && millis()>(button_first_push+menu_button_fast_delay)) && millis()>(menu_button_fast_timeout+button_time) ||
			input>0){ // halbe sek timeout
		//////////////////////// rechts ist gedrückt ////////////////////////
		if((digitalRead(menu_button_rechts)==menu_active || input==67 || char(input)=='d') && button_rechts_valid){
			if(button_first_push==0){
				button_first_push=millis();
			};
			if(MENU_DEBUG){      Serial.println("menu_button_rechts");  };
			delay(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
			// erst wenn nach dem delay noch der pegel anliegt
			if((digitalRead(menu_button_rechts)==menu_active || input==67 || char(input)=='d')){ // wenn nach der Wartezeit der button immernoch gedrückt ist
				// menu var umsetzen
				menu_start=0; // rechts -> leeres menu
				menu_ende=menu_lines-1;
				menu_marker=0;
				state=(state*10)+1;
				button_time=millis();
				return true;
			} else {
				return false;
			};
		}

		//////////////////////// links ist gedrückt ////////////////////////
		else if((digitalRead(menu_button_links)==menu_active || input==68 || char(input)=='a')  && button_links_valid){
			if(button_first_push==0){
				button_first_push=millis();
			};
			if(MENU_DEBUG){  Serial.println("menu_button_links");  };
			delay(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
			// erst wenn nach dem delay noch der pegel anliegt
			if((digitalRead(menu_button_links)==menu_active || input==68 || char(input)=='a')){ // wenn nach der Wartezeit der button immernoch gedrückt ist
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
				return true;
				// wenn der pegel doch nicht mehr anliegt ( spike )
			} else {
				return false;
			};
		}

		//////////////////////// oben ist gedrückt ////////////////////////
		else if((digitalRead(menu_button_oben)==menu_active || input==65 || char(input)=='w') && button_oben_valid){
			if(button_first_push==0){
				button_first_push=millis();
			};
			if(MENU_DEBUG){  Serial.println("menu_button_oben");  };
			// move menu
			delay(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
			// erst wenn nach dem delay noch der pegel anliegt
			if((digitalRead(menu_button_oben)==menu_active || input==65 || char(input)=='w')){ // wenn nach der Wartezeit der button immernoch gedrückt ist
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
				};

				//recalc level
				// z.b. 00061 -> 00060 => 60%10=0 -> soll 69
				state--;
				if(state%10==0){
					state+=menu_max+1; // macht aus 0 dann 9 => 8+1
				};
				button_time=millis();
				return true;
			} else {
				return false;
			};
		}

		//////////////////////// unten ist gedrückt ////////////////////////
		else if((digitalRead(menu_button_unten)==menu_active || input==66 || char(input)=='s') && button_unten_valid){
			if(button_first_push==0){
				button_first_push=millis();
			};
			if(MENU_DEBUG){  Serial.println("menu_button_unten");  };
			// move menu
			delay(menu_second_wait); // warte ein backup intervall um einen spike zu unterdrücken
			// erst wenn nach dem delay noch der pegel anliegt
			if((digitalRead(menu_button_unten)==menu_active || input==66 || char(input)=='s')){ // wenn nach der Wartezeit der button immernoch gedrückt ist
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
				};

				//recalc level
				state++;
				// wenn wir bei xxx9 sind und ++ machen landen wir bei xx10
				// xx10%10 == 0, -> xx01
				if(state%10==0){
					state-=9; // macht aus [xx10] => 1
				};
				button_time=millis();
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
		// menu_button_unten zumenu_ende
	};
	// return false wenn die zeit nicht um war
	return false;
};
///// zyklisches abfragen der buttons ////////////

void speedo_menu::init(){
	pinMode(menu_button_rechts, INPUT);
	pinMode(menu_button_links, INPUT);
	pinMode(menu_button_oben, INPUT);
	pinMode(menu_button_unten, INPUT);

	// interne pullups aktivieren
	digitalWrite(menu_button_rechts,HIGH);
	digitalWrite(menu_button_links,HIGH);
	digitalWrite(menu_button_oben,HIGH);
	digitalWrite(menu_button_unten,HIGH);

	state=11;
	button_time=millis();
	button_first_push=millis();

	Serial.println("Menu init done");
};


