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

//** create objects **//
//speedo_filemanager*		pFilemanager=new speedo_filemanager(); // ob das gut geht weiß ich auch nicht ;)

speedo_filemanager_v2*	pFilemanager_v2=new speedo_filemanager_v2();
debugging*				pDebug=new debugging();
speedo_sd*				pSD=new speedo_sd();
configuration*			pConfig=new configuration();
speedo_disp* 			pOLED=new speedo_disp();		// vor menu_disp();
speedo_menu* 			pMenu=new speedo_menu();		// pins aktivieren, sonst nix
speedo_speedo*			pSpeedo=new speedo_speedo();
speedo_sprint* 			pSprint=new speedo_sprint();
Speedo_sensors*			pSensors=new Speedo_sensors();
Speedo_aktors*			pAktors=new Speedo_aktors();
speedo_timer*   		pTimer=new speedo_timer(); 		// brauch ich ja nur hier, den braucht sonst keiner
#ifdef DEMO_MODE
speedo_demo*			pDemo=new speedo_demo();
#endif
//** create objects **//


/******************* Basics *************************
 * INTERRUPTS:
 * GPS will be refresh on its own, calls UART Interrupt and sets a flag on successful receive of a sentence
 * Reed Sensor will call a interrupt as well and count peaks
 * Engine Rotation will call the third interrupt and increase some var, and calucate the rpm in the interrupt ( is that clever ? )
 *
 * SETUP
 * -> Load all settings from files and initialize the sensors etc
 *
 * LOOP (Execution order of MainLoop)
 * --> show activity on Watchdog port to prevent reset
 * --> check if UART based interrupt has set the "valid GPS Data"-flag
 * ---> if data available, call parse() to save data to an array.
 * ----> call navi_calc() if navi is active
 * ----> if the distance to navigation point is small enough
 * -----> generate a new gps order by calling generate_new_order(), this will open the SD card and search the right line
 * ----> every 30 sec, store the data to SD Card
 * --> Check if timer are ready to be executed
 * ---> collect and calculate temperature measurment values
 * ---> increase clock variables
 * ---> calculate speedo km ... so this is not 10000% exact as we to a kind of integration in 1 sec steps ..
 * ---> check if the flashers are (still) active
 * --> check in any button is pushed, if so
 * ---> recalculate state var and display stuff, corresponding to the state -- lets call it "the Menu"
 * --> based on that state: show speedo, a lot of code but basicly easy: check if the values are equal to the values of the last round
 * ---> if not, display them on the screen and store to internal storage ( disp_backup array )
 ******************* Basics *************************/

/******************* TODO List ***************************
 * -> die stelle finden an der mehr als 22 byte in das char array geschrieben werden
 * bzw rausfinden wann und warum manchmal die temperaturanzeige total am rad dreht
 * -> eine stelle finden an der man den sd error zurücksetzen kann, also im menü finden
 * -> bootloader mit verify
 ******************* TODO List ***************************/
void init_speedo(void){
	Serial.begin(115200);

	pDebug->sprintlnp(PSTR("=== Speedoino ==="));
	pDebug->sprintp(PSTR(GIT_REV));				// print Software release
	pDebug->sprintp(PSTR(" HW:"));
	Serial.println(pConfig->get_hw_version());	// print Hardware release

	pSD->init(); 				// try open SD Card
	// first, set all variables to a zero value
	pSensors->init(); 			// start every init sequence of each sensor
	pSensors->clear_vars();		// clear all sensor values;
	pSpeedo->clear_vars();		// refresh cycle
	// read configuration file from sd card
	pConfig->read(CONFIG_FOLDER,"BASE.TXT",READ_MODE_CONFIGFILE,""); 	// load base config
	pConfig->read(CONFIG_FOLDER,"SPEEDO.TXT",READ_MODE_CONFIGFILE,"");	// speedovalues, avg,max,time
	pConfig->read(CONFIG_FOLDER,"GANG.TXT",READ_MODE_CONFIGFILE,"");	// gang
	pConfig->read(CONFIG_FOLDER,"TEMPER.TXT",READ_MODE_CONFIGFILE,"");	// Temperatur
	pConfig->read_skin();		// skinning
	// check if read SD read was okay, if not: load your default backup values
	pAktors->check_vars();		// check if color of outer LED are OK
	pSensors->check_vars();		// check if config read was successful
	pSpeedo->check_vars();		// rettet das Skinning wenn SD_failed von den sensoren auf true gesetzt wird
	pSensors->single_read();	// read all sensor values once to ensure we are ready to show them
	pAktors->init();			// Start outer LEDs // ausschlag des zeigers // Motorausschlag und block bis motor voll ausgeschlagen, solange das letzte intro bild halten
	pConfig->EEPROM_init(); 	// read vars from eeprom, reset Day Based storage etc
	pOLED->init_speedo(); 		// Start Screen //execute this AFTER Config->init(), init() will load  phase,config,startup. PopUp will be shown if sd access fails
	pMenu->init(); 				// Start butons // adds the connection between pins and vars
	pMenu->display(); 			// execute this AFTER pOLED->init_speedo!! this will show the menu and, if state==11, draws speedosymbols
	pSpeedo->reset_bak(); 		// reset all storages, to force the redraw of the speedo
	pSensors->m_CAN->init();	// hast to be the very last to ensure startup and to load values from SD to ser correct filter
	pConfig->ram_info();
	pDebug->sprintlnp(PSTR("=== Setup finished ==="));
	Serial.flush(); // jaja, hallo liebes bluetooth modul, will keiner wissen das du alles echos solange wir nicht mit dem pc verbunden sind ...
}


int main(void) {
	/******************** setup procedure ********************************************
	 * all initialisations must been made before the main loop
	 ******************** setup procedure ********************************************/
	init();
	init_speedo();

	/******************** setup procedure ********************************************
	 * all initialisations must been made before the main loop, before THIS
	 ******************** setup procedure ********************************************/
	unsigned long   previousMillis = 0;

	for (;;) {
		pSensors->m_CAN->check_message();
		//////////////////////////////////////////////////
		//		pSensors->m_reset->set_deactive(false,false);
		//		Serial3.end();
		//		Serial3.begin(115200);
		//		while(true){
		//			while(Serial3.available()>0){
		//				Serial.print(Serial3.read(),BYTE);
		//			}
		//			while(Serial.available()>0){
		//				Serial3.print(Serial.read(),BYTE);
		//			}
		//		}
		//////////////////////////////////////////////////

		pSensors->m_reset->toggle(); 		// toggle pin, if we don't toggle it, the ATmega8 will reset us, kind of watchdog<
		pDebug->speedo_loop(21,1,0," "); 	// intensive debug= EVERY loop access reports the Menustate
		pSensors->m_gps->check_flag();    	// check if a GPS sentence is ready
		pAktors->check_flag(); 				// updated expander
		pSensors->pull_values();			// very important, updates all the sensor values

		/************************* timer *********************/
		pTimer->every_sec(pConfig);		// 1000 ms
		pTimer->every_qsec();			// 250  ms
		pTimer->every_custom();  		// one custom timer, redrawing the speedo, time is defined by "refresh_cycle" in the base.txt
		/************************* push buttons *********************
		 * using true as argument, this will activate bluetooth input as well
		 ************************* push buttons*********************/
		pMenu->button_test(true,false);     // important!! if we have a pushed button we will draw something, depending on the menustate
		/************************ every deamon activity is clear, now draw speedo ********************
		 * we are round about 0000[1]1 - 0000[1]9
		 ************************ every deamon activity is clear, now draw speedo ********************/
		pSensors->m_CAN->check_message();

		if((pMenu->state/10)==1 || pMenu->state==7311111)  {
			pSpeedo->loop(previousMillis);
		}
		//////////////////// Sprint Speedo ///////////////////
		else if( pMenu->state==21 ) {
			pSprint->loop();
		}
		////////////////// Clock Mode ////////////////////////
		else if(pMenu->state==291){
			pSensors->m_clock->loop();
		}
		//////////////////// voltage mode ///////////////////
		else if(pMenu->state==531){
			pSensors->addinfo_show_loop();
		}
		//////////////////// stepper mode ///////////////////
		else if(pMenu->state==541){
			pAktors->m_stepper->loop();
		}
		//////////////////// gps scan ///////////////////
		else if(pMenu->state==511){
			pSensors->m_gps->loop();
		}
		//////////////////// outline scan ///////////////////
		else if(pMenu->state==721){
			pSensors->m_speed->check_umfang();
		}
		////////////////// gear scan ///////////////
		else if(floor(pMenu->state/100)==71){
			pSensors->m_gear->calibrate();
		}
	} // end for
} // end main

