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
speedo_filemanager_v2*	pFilemanager_v2=new speedo_filemanager_v2();
debugging*				pDebug=new debugging();
configuration*			pConfig=new configuration();
speedo_menu* 			pMenu=new speedo_menu();		// pins aktivieren, sonst nix
speedo_speedo*			pSpeedo=new speedo_speedo();
Speedo_sensors*			pSensors=new Speedo_sensors();
Speedo_aktors*			pAktors=new Speedo_aktors();
speedo_timer*   		pTimer=new speedo_timer(); 		// brauch ich ja nur hier, den braucht sonst keiner
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

void init_speedo(void){
	Serial.begin(TARGET_UART_SPEED);

	pDebug->sprintlnp(PSTR("=== SmartSpeedo ==="));
	pDebug->sprintp(PSTR(GIT_REV));				// print Software release
	pDebug->sprintp(PSTR(" HW:"));
	Serial.println(pConfig->get_hw_version());	// print Hardware release

	// first, set all variables to a zero value
	pSensors->init(); 			// start every init sequence of each sensor

	// set can on/off
	pSensors->check_vars();		// check if config read was successful
	pSensors->single_read();	// read all sensor values once to ensure we are ready to show them
	pAktors->init();			// Start outer LEDs // ausschlag des zeigers // Motorausschlag und block bis motor voll ausgeschlagen, solange das letzte intro bild halten
	pSpeedo->reset_bak(); 		// reset all storages, to force the redraw of the speedo
	//	pSensors->m_CAN->init();	// hast to be the very last to ensure startup and to load values from SD to configure correct filter
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
#ifdef LOAD_CALC
	unsigned long load_calc=0;
	unsigned long lasttime_calc=0;
#endif

//	pAktors->check_bt_connection();

	for (;;) {
//		pSensors->m_speed->get_mag_speed();
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
		pSensors->pull_values();			// very important, updates all the sensor values

		/************************* timer *********************/
		pTimer->every_sec(pConfig);		// 1000 ms
		pTimer->every_qsec();			// 250  ms
		pTimer->every_custom();  		// one custom timer, redrawing the speedo, time is defined by "refresh_cycle" in the base.txt
		/************************* push buttons *********************
		 * using true as argument, this will activate bluetooth input as well
		 ************************* push buttons*********************/
		pMenu->button_test();     // important!! if we have a pushed button we will draw something, depending on the menustate
		pFilemanager_v2->check_input(); // check serial input from bluetooth
		/************************ every deamon activity is clear, now draw speedo ********************
		 * we are round about 0000[1]1 - 0000[1]9
		 ************************ every deamon activity is clear, now draw speedo ********************/
		pSensors->m_CAN->check_message();

		// send the data via bluetooth
		pSpeedo->loop(previousMillis);

#ifdef LOAD_CALC
		load_calc++;
		if(millis()-lasttime_calc>1000){
			Serial.print(load_calc);
			Serial.println(" cps"); // 182 w/o interrupts, 175 w/ interrupts, 172 w/ much interrupts
			load_calc=0;
			lasttime_calc=millis();
		}
#endif
	} // end for
} // end main

