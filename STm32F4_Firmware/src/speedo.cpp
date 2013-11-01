
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

// reset all storages
void speedo::reset_bak(){
	for(unsigned int i=0;i<sizeof(disp_zeile_bak)/sizeof(disp_zeile_bak[0]);i++){
		disp_zeile_bak[i]=-99;
	};
}

/************* speedo main loop *************
 * this is THE main loop, default speedo mode
 ************* speedo main loop *************/
void speedo::loop(unsigned long previousMillis){
	char *char_buffer;
	char_buffer = (char*) malloc(22);

	previousMillis=Millis.get(); // get time for this loop
	Debug.speedo_loop(0,1,previousMillis,(char *)" "); // debug

	/********************* additional informations II *****************************
	 * The following block has to be the first in line, to set the var addinfo2currenctly_shown.
	 * If the var is set to TRUE no other widget should be drawn in the same line
	 * To ensure this the function check_no_collision_with_addinfo2() should be called in the
	 * header of each value check
	 *
	 * ADDITIONAL INFOMATIONS II shows Warnings and Errors
	 * like: Voltage LOW / Temperature TO HIGH / SD Failed / Flasher ..
	 *
	 * disp_zeile_bak values
	 * 101 RPM to high
	 * 102 Flasher
	 * 103 SpeedCam
	 * 104 SD
	 * 105 Clear
	 * 106 Oil read failed
	 * 107 Water read failed
	 * 108 Oil temp to high
	 * 109 Water temp to high
	 * 110 Read failed
	 * 111 Voltage
	 * 112 CAN
	 * ??? Navi, 0-990 (rounded to 10)(m),1-X (km)
	 */
	if(!(addinfo2_widget.x==-1 && addinfo2_widget.y==-1)){ // only show it if pos != -1/-1
		// now, print variable messages, additional infos, see top comment (4)
		// lets assume that we are showing a warning right now, its easier to assume it and set it to false as soon if we are NOT showing a warning
		addinfo2_currently_shown=true;
#ifdef TACHO_SMALLDEBUG
		Serial.puts(USART1,("-a2"));
#endif
		/// warnung wegen zu hoher Drehzahl, erstmal checken ob wir von der drehzahl her in betrachtkommen
//		if(Sensors.get_RPM(RPM_TYPE_DIRECT)>7000 && ((Sensors.get_oil_temperature()<600 && pSensors->m_temperature->oil_temp_fail_status==0) || // temperatur < 60°C
//				(Sensors.get_water_temperature()<600 && pSensors->m_temperature->water_temp_fail_status==0))){ // und keine Fehler beim lesen
//			if(disp_zeile_bak[ADD_INFO2]!=101){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=101;
//				Debug.speedo_loop(10,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Engine cold"),addinfo2_widget.y,true);
//			};
//		}
//
//		///// SPEEDCAM - BABOOM /////
//		else if(pSpeedCams->calc()){ // returns true, if a speedcam is next to us
//			unsigned long dist=pSpeedCams->get_dist_to_next_point();
//			if(disp_zeile_bak[ADD_INFO2]!=(int)floor(dist/10)){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=(int)floor(dist/10);
//				Debug.speedo_loop(14,0,previousMillis," ");
//				OLED.highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
//				if(dist>999) dist=999; // should never happen
//				sprintf_P(char_buffer,PSTR("POI warning %3im"),(int)floor(dist/5)*5); // round to 5
//				pMenu->center_me(char_buffer,17);
//				OLED.string(addinfo2_widget.font,char_buffer,addinfo2_widget.x+2,addinfo2_widget.y,15,0,1); // 2,6
//			};
//		}
//
//		///// Temp to high /////
//		else if(Sensors.get_oil_temperature()>=pSensors->m_temperature->oil_warning_temp && Sensors.get_oil_temperature()<8888 && (pSensors->m_temperature->oil_temp_fail_status!=SENSOR_OPEN || pSensors->m_temperature->oil_temp_fail_status!=SENSOR_SHORT_TO_GND)){
//			if(disp_zeile_bak[ADD_INFO2]!=108){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=108;
//				Debug.speedo_loop(14,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Oil temp warning"),addinfo2_widget.y,true);
//			};
//
//		}
//		else if(Sensors.get_water_temperature()>=pSensors->m_temperature->water_warning_temp && Sensors.get_water_temperature()<8888 && (Sensors.get_water_temperature_fail_status()!=SENSOR_OPEN || Sensors.get_water_temperature_fail_status()!=SENSOR_SHORT_TO_GND)){
//			if(disp_zeile_bak[ADD_INFO2]!=109){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=109;
//				Debug.speedo_loop(14,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Water temp warning"),addinfo2_widget.y,true);
//			};
//
//		}
//		///// Blinker ////
//		else if(pSensors->m_blinker->warn(trip_dist[8])){ // mal nachfragen wenn nach 200 metern der Blinker noch an ist
//			if(disp_zeile_bak[ADD_INFO2]!=102){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=102;
//				Debug.speedo_loop(11,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Flasher?"),addinfo2_widget.y,true);
//			};
//
//		}
//		///// Voltage ////
//		else if(pSensors->m_voltage->get()<1100){ // less than 11.0 Volts
//			if(disp_zeile_bak[ADD_INFO2]!=111 && Millis.get()>20000){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=111;
//				Debug.speedo_loop(11,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Voltage below 11V"),addinfo2_widget.y,true);
//			};
//
//		}
//		////// CAN MIL ////////
//		else if(pSensors->CAN_active && pSensors->m_CAN->get_mil_active()){
//			if(disp_zeile_bak[ADD_INFO2]!=112){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=112;
//				Debug.speedo_loop(11,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("CAN warning"),addinfo2_widget.y,true);
//			};
//		}
//		///// Navi ////
//		else if(pSensors->m_gps->navi_active){
//			int dist=0;
//			int result_value=pSensors->m_gps->get_order(char_buffer,&dist); // result value ist der zahlen wert bis zur nÃ€chsten aktion, 11km => 11 | 900m => 900 (10m Schritte)
//			if(disp_zeile_bak[ADD_INFO2]!=dist){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh! // wird also einmal pro sek geschrieben
//				disp_zeile_bak[ADD_INFO2]=int(dist);
//				Debug.speedo_loop(13,0,previousMillis," ");
//				if(result_value>-1){ // -1 => no gps
//					OLED.filled_rect(0,8*addinfo2_widget.y,128,8,15); // den bereich am ende der Zeile leeren ??
//					OLED.string(addinfo2_widget.font,char_buffer,addinfo2_widget.x,addinfo2_widget.y,15,0,1); // die Ziel ausgabe
//				} else {
//					OLED.highlight_bar(0,8*addinfo2_widget.y,128,8);
//					strcpy_P(char_buffer,PSTR("No GPS"));
//					pMenu->center_me(char_buffer,17);
//					OLED.string(addinfo2_widget.font,char_buffer,addinfo2_widget.x+2,addinfo2_widget.y,15,0,1); // 2,6
//					OLED.draw_arrow(-1,arrow_widget.x,arrow_widget.y); // clears arrow
//				};
//			};
//		}
//		///// Sensor in use but not found ///
//		else if(oil_widget.x!=-1 && oil_widget.y!=-1 && (Sensors.get_oil_temperature()==8888 || Sensors.get_oil_temperature()==9999)){
//			if(disp_zeile_bak[ADD_INFO2]!=106){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=106;
//				Debug.speedo_loop(14,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Oil read failed"),addinfo2_widget.y,true);
//			};
//
//		}
//		else if(water_widget.x!=-1 && water_widget.y!=-1 && (Sensors.get_water_temperature()==8888 || Sensors.get_water_temperature()==9999)){
//			if(disp_zeile_bak[ADD_INFO2]!=107){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=107;
//				Debug.speedo_loop(14,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Water read failed"),addinfo2_widget.y,true);
//			};
//
//		}
//		else if(kmh_widget.x!=-1 && kmh_widget.y!=-1 && pSensors->m_speed->status==SPEED_REED_ERROR){
//			if(disp_zeile_bak[ADD_INFO2]!=110){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=110;
//				Debug.speedo_loop(14,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("Reed sensor error"),addinfo2_widget.y,true);
//			};
//
//		}
//		///// SD card ////
//		else if(pSD->sd_failed){
//			if(disp_zeile_bak[ADD_INFO2]!=104){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//				disp_zeile_bak[ADD_INFO2]=104;
//				Debug.speedo_loop(14,0,previousMillis," ");
//				OLED.string_P_centered(PSTR("SD access failed"),addinfo2_widget.y,true);
//			};
//
//		}
//		//// delete
//		else{
//			addinfo2_currently_shown=false;
//			if(disp_zeile_bak[ADD_INFO2]!=105){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
//
//				Debug.speedo_loop(15,0,previousMillis," ");
//				OLED.string_P(addinfo2_widget.font,PSTR("                      "),0,addinfo2_widget.y,0,0,1);
//				// if any widget is in the same line, reset the particular line, so the next loop will redraw the sensor
//				// inital draw screen will redraw all symbols and reset disp_zeile_bak
//				initial_draw_screen();
//				disp_zeile_bak[ADD_INFO2]=105;
//			};
//		};
#ifdef TACHO_SMALLDEBUG
		Serial.puts_ln(USART1,("."));
#endif
	};
	/********************* additional informations II *****************************/

	/************************* oil temperature *********************
	 * Oil Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->oil_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a coresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 ************************* oil temperature *********************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-o"));
#endif
//	if(disp_zeile_bak[OIL_TEMP]!=Sensors.get_oil_temperature()+pSensors->m_temperature->oil_temp_fail_status){
//		if((!(oil_widget.x==-1 && oil_widget.y==-1)) && check_no_collision_with_addinfo2(oil_widget.y)){ // only show it if pos != -1/-1
//			disp_zeile_bak[OIL_TEMP]=int(Sensors.get_oil_temperature()+pSensors->m_temperature->oil_temp_fail_status);
//			// below 20 degree the PTC is very antiliear so we won't show it
//			if(pSensors->m_temperature->oil_temp_fail_status==SENSOR_OPEN){
//				sprintf_P(char_buffer,PSTR(" -     ")); // error occored -> no sensor
//			} else if(pSensors->m_temperature->oil_temp_fail_status==SENSOR_SHORT_TO_GND){
//				sprintf_P(char_buffer,PSTR(" --    ")); // error occored -> short to gnd
//			} else 	if(Sensors.get_oil_temperature()>200){
//				sprintf_P(char_buffer,PSTR("%3i.%i{C"),int(floor(Sensors.get_oil_temperature()/10))%1000,Sensors.get_oil_temperature()%10); // _32.3Â°C  7 stellen
//			} else {
//				sprintf_P(char_buffer,PSTR("<20{C  ")); // below 20Â°C add a space to have 5 chars
//			};
//			Debug.speedo_loop(1,0,previousMillis," "); // debug
//			// depend on skinsettings
//			OLED.string(oil_widget.font,char_buffer,oil_widget.x+4,oil_widget.y,0,DISP_BRIGHTNESS,-4);
//		};
//	};
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif
	/************************* oil temperature *********************/


	/************************* water temperature *********************
	 * water Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->water_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a coresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 *
	 * 7 Chars
	 * "_-_____" <- Sensor open (based on 5x read 1024 on the analog pin)
	 * "_--____" <- Sensor short to ground (based on 5x read 0 on the analog pin)
	 * "_32.4Â°C" <- regular temp + filling
	 * "102.3Â°C" <- regular temp
	 * ">110Â°C_" <- temp to high
	 * "<30Â°C__" <- temp to low
	 ************************* water temperature *********************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-w"));
#endif
//	if(disp_zeile_bak[WATER_TEMP]!=Sensors.get_water_temperature()+Sensors.get_water_temperature_fail_status()){
//		if((!(water_widget.x==-1 && water_widget.y==-1)) && check_no_collision_with_addinfo2(water_widget.y)){ // only show it if pos != -1/-1
//			disp_zeile_bak[WATER_TEMP]=int(Sensors.get_water_temperature()+Sensors.get_water_temperature_fail_status());
//			// below 20 degree the PTC is very antiliear so we won't show it
//			if(Sensors.get_water_temperature_fail_status()==SENSOR_OPEN){
//				sprintf_P(char_buffer,PSTR(" -     ")); // error occored -> no sensor
//			} else if(Sensors.get_water_temperature_fail_status()==SENSOR_SHORT_TO_GND){	// could of fail reads
//				sprintf_P(char_buffer,PSTR(" --    ")); // error occored -> short to gnd
//			} else { // if no short
//				// check if its a CAN value or a measured value
//				if(pSensors->CAN_active){
//					// CAN temperature is save, display it
//					sprintf_P(char_buffer,PSTR("%3i.%i{C"),int(floor(Sensors.get_water_temperature()/10))%1000,Sensors.get_water_temperature()%10); // _32.3Â°C  7 stellen
//				} else {
//					// Sensor temperature is save, within a certain range ...
//					if(Sensors.get_water_temperature()>1100){
//						sprintf_P(char_buffer,PSTR(">110{C ")); // more then 110Â°C add a space to have 7 chars
//					} else 	if(Sensors.get_water_temperature()>300){
//						sprintf_P(char_buffer,PSTR("%3i.%i{C"),int(floor(Sensors.get_water_temperature()/10))%1000,Sensors.get_water_temperature()%10); // _32.3Â°C  7 stellen
//					} else {
//						sprintf_P(char_buffer,PSTR("<30{C  ")); // below 30Â°C add a space to have 7 chars
//					};
//				}
//
//			};
//
//			Debug.speedo_loop(1,0,previousMillis," "); // debug
//			// depend on skinsettings
//			OLED.string(water_widget.font,char_buffer,water_widget.x+4,water_widget.y,0,DISP_BRIGHTNESS,-4);
//		};
//	};
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif
	/************************* water temperature *********************/
	Debug.speedo_loop(22,1,0," ");


	/************************* air temperature *********************
	 * Air Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->air_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a corresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 ************************* air temperature *********************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-at"));
#endif
	if(disp_zeile_bak[AIR_TEMP]!=Sensors.get_air_temperature()){
		if((!(air_widget.x==-1 && air_widget.y==-1)) && check_no_collision_with_addinfo2(air_widget.y)){ // only show it if pos != -1/-1
			disp_zeile_bak[AIR_TEMP]=int(Sensors.get_air_temperature());
			sprintf(char_buffer,("%2i.%i{C"),int(floor(Sensors.get_air_temperature()/10))%100,Sensors.get_air_temperature()%10);
			Debug.speedo_loop(2,0,previousMillis," ");
			// depend on skinsettings
			OLED.string(air_widget.font,char_buffer,air_widget.x+1,air_widget.y,0,DISP_BRIGHTNESS,2);
		};
	};
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif
	/************************* air temperature *********************/
	Debug.speedo_loop(23,1,0," ");

	/************************* arrow ******************
	 * if we move the return value of m_gps->get_order will change (will return distance to next goal)
	 * on change we get another arrow to the goal,
	 * A TODO would be a kind of "show north"-Mode, so .. here we go TODO TODO
	 **************************************************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-n"));
#endif
//	if(pSensors->m_gps->navi_active){ // Arrow Mode
//		if((!(arrow_widget.x==-1 && arrow_widget.y==-1)) && check_no_collision_with_addinfo2(arrow_widget.y)){
//			int result_value;
//			pSensors->m_gps->get_order(char_buffer,&result_value); // distance will be calculated on the parse routine, this call just copies it
//			if(disp_zeile_bak[ARROW]!=result_value+pSensors->m_gps->valid){
//				OLED.draw_arrow(pSensors->m_gps->winkel,arrow_widget.x,arrow_widget.y);
//				//OLED.draw_arrow((pSensors->m_clock->get_ss()*9+1)%360,arrow_widget.x,arrow_widget.y);
//				disp_zeile_bak[ARROW]=int(result_value+pSensors->m_gps->valid);
//			}
//		};
//	}
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif


	/************************* speed *********************
	 * getSpeed will return the speed as int, depending on setting this will be the GPS OR Magnitude based speed.
	 * first if checks if the value equals -99, in this case the full display has to be rewritten, draw "km/h"
	 * else just draw the number of km/h, but only refresh it if the difference is higher than 2 or we are very slow (below 10)
	 ************************* geschwindigkeit *********************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-gs"));
#endif
//	int temp_speed=Sensors.get_speed(false);
//	int temp_gps_speed=Sensors.get_speed(true); // warum heisst der denn gps, wenns doch der mag ist?
//	if((!(kmh_widget.x==-1 && kmh_widget.y==-1)) && check_no_collision_with_addinfo2(kmh_widget.y)){ // only show it if pos != -1/-1
//		if(disp_zeile_bak[SPEED_VALUE]==-99){ // storage has been resetet, draw everything
//			sprintf_P(char_buffer,PSTR("%3i"),temp_speed);
//			// depend on skinsettings danger, 2 places to edit the kmh size
//			OLED.string(kmh_widget.font,char_buffer,kmh_widget.x,kmh_widget.y,0,DISP_BRIGHTNESS,0); //   6/2
//			OLED.string_P(kmhchar_widget.font,PSTR("km/h"),kmhchar_widget.x,kmhchar_widget.y,0,DISP_BRIGHTNESS,2); //       13/3 // that belongs to the speed
//			Debug.speedo_loop(3,0,previousMillis," ");  // debug
//			disp_zeile_bak[SPEED_VALUE]=int(temp_speed+1);
//		} else if( (abs(disp_zeile_bak[SPEED_VALUE]-(temp_speed+1))>1) || (abs(disp_zeile_bak[SPEED_VALUE]-(temp_speed+1))==1 && temp_speed<10) ) {
//			sprintf_P(char_buffer,PSTR("%3i"),temp_speed);
//			Debug.speedo_loop(4,0,previousMillis," "); // debug
//			// depend on skinsettings danger, 2 places to edit the kmh size
//			OLED.string(kmh_widget.font,char_buffer,kmh_widget.x,kmh_widget.y,0,DISP_BRIGHTNESS,0); //   6/2
//			disp_zeile_bak[SPEED_VALUE]=int(temp_speed+1);
//		} else if(temp_speed==0 && ((abs(disp_zeile_bak[SPEED_VALUE]-(temp_gps_speed+1))>1) || (abs(disp_zeile_bak[SPEED_VALUE]-(temp_gps_speed+1))==1 && temp_gps_speed<10))){
//			sprintf_P(char_buffer,PSTR("%3i"),temp_gps_speed);
//			Debug.speedo_loop(4,0,previousMillis," "); // debug
//			// depend on skinsettings danger, 2 places to edit the kmh size
//			OLED.string(kmh_widget.font,char_buffer,kmh_widget.x,kmh_widget.y,0,DISP_BRIGHTNESS,0); //   6/2
//			disp_zeile_bak[SPEED_VALUE]=int(temp_gps_speed+1);
//		}
//	};
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif
	/************************* speed *********************/


	/********************* additional informations *****************************
	 * first we have to separate between the navigation state
	 * if the menu_state ==1  we will show something like
	 *    102 km/h           <-- speedo, printed with the code on top of this section
	 * 5800 U/min | 4 GPS    <-- engine rpm, gps info  (1),(2)
	 *     Day 342.km        <-- one storage, chosen in the setup section, called addinfo (3)
	 * ||| Flasher ??? |||   <-- the highlighted "addinfo2" field, this could be anything like (flasher/navigation/engine cold) (4)
	 ***********************************************************************/
	// see comment on top, number (1)
	//pSensors->m_dz->calc(); // calc in main loop to run the stepper in every menu
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-dz"));
#endif
//	if((!(dz_widget.x==-1 && dz_widget.y==-1)) && check_no_collision_with_addinfo2(dz_widget.y)){ // only show it if pos != -1/-1
//		if(disp_zeile_bak[DZ_VALUE]!=signed(Sensors.get_RPM(RPM_TYPE_FLAT_ROUNDED)+1)){
//			if(disp_zeile_bak[DZ_VALUE]==-99){ //schreib alles neu, auch die buchstaben
//				sprintf_P(char_buffer,PSTR("%5i U/min"),Sensors.get_RPM(RPM_TYPE_FLAT_ROUNDED)); // auf glatte 50er
//				Debug.speedo_loop(5,0,previousMillis," ");
//				// depend on skinsettings
//				OLED.string(dz_widget.font,char_buffer,dz_widget.x,dz_widget.y,0,DISP_BRIGHTNESS,0);
//			} else { // nur eine Ã€nderung im wert
//				sprintf_P(char_buffer,PSTR("%5i"),Sensors.get_RPM(RPM_TYPE_FLAT_ROUNDED));
//				Debug.speedo_loop(6,0,previousMillis," ");
//				OLED.string(dz_widget.font,char_buffer,dz_widget.x,dz_widget.y,0,DISP_BRIGHTNESS,0);
//			};
//			disp_zeile_bak[DZ_VALUE]=int(Sensors.get_RPM(RPM_TYPE_FLAT_ROUNDED)+1);
//		};
//	};
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif

	// see comment on top (2)
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-gps"));
#endif
//	if((!(gps_widget.x==-1 && gps_widget.y==-1)) && check_no_collision_with_addinfo2(gps_widget.y)){ // only show it if pos != -1/-1
//		int sats=pSensors->m_gps->get_info(6);
//		if(disp_zeile_bak[GPS_VALUE]!=int(sats+1)){
//			if(gps_widget.symbol){ // draw our gps symbol
//				OLED.draw_gps(gps_widget.x*3,gps_widget.y*8,sats);
//			} else {
//				if(disp_zeile_bak[GPS_VALUE]==-99){ //schreib alles neu, auch die buchstaben
//					sprintf_P(char_buffer,PSTR("no GPS"));
//					Debug.speedo_loop(6,0,previousMillis," ");
//				} else { // nur eine Ã€nderung im wert
//					sprintf_P(char_buffer,PSTR("%2i"),int(sats)%100);
//					Debug.speedo_loop(6,0,previousMillis," ");
//				};
//				OLED.string(gps_widget.font,char_buffer,gps_widget.x,gps_widget.y,0,DISP_BRIGHTNESS,0);
//			}
//			disp_zeile_bak[GPS_VALUE]=int(1+sats);
//		};
//	};
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif

	//////////////////////////////////////// ADD INFO WIDGET ////////////////////////////////////////////////////////
	// see on top comment, number (3)
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-ai"));
#endif
//	if((!(addinfo_widget.x==-1 && addinfo_widget.y==-1)) && check_no_collision_with_addinfo2(addinfo_widget.y)){ // only show it if pos != -1/-1
//		if(unsigned(disp_zeile_bak[ADD_INFO])!=pSensors->m_gps->mod(floor(trip_dist[8]/100),100)+pSensors->m_gps->mod(floor(avg_timebase[8]/60),100)){ // immer wenn sich trip_dist Ã€ndert den string ausgeben der direkt drÃŒber steht, auf 0.1 km genau
//			// trip[8] => gesamt/100 => 100er Meter, 27.342,8 => 28
//			disp_zeile_bak[ADD_INFO]=int(pSensors->m_gps->mod(floor(trip_dist[8]/100),100)+pSensors->m_gps->mod(floor(avg_timebase[8]/60),100));
//			// temp buffer
//			unsigned long temp_trip_dist=trip_dist[m_trip_storage-1];
//			unsigned long temp_avg_timebase=avg_timebase[m_trip_storage-1];
//			int temp_max_speed=max_speed[m_trip_storage-1];
//
//
//			// Possible values: e.g. with the longes Name "NonPermanent" (others "Day", "Board", "Tour" ...)
//			// Mode 1 (Distance):         "NonPermanent 7234.3km" or "NonPermanent 41234km_"         | Full Text
//			// Mode 2 (Time):             "_NonPermanent 03:32__"                                    | Full Text
//			// Mode 3 (Avg Speed):        "NonPerma AVG 111km/h_"                                    | Short (7 char) Text
//			// Mode 4 (Max Speed):        "NonPerma MAX 232km/h_"                                    | Short (7 char) Text
//			// Mode 5 (Distance & Time):  "___34543km | 682h____" or "__4543.1km | 68:22___"         | no Text
//			// Mode 6 (Distance & Avg):   "_32799.3km | 111km/h_"                                    | no Text
//			// Mode 7 (Distance & Max):   "_32799.3km | 232km/h_"                                    | no Text
//			// Mode 8 (Time & Avg):       "___345h | 111km/h____" or "___45:04 | 111km/h___"         | no Text
//			// Mode 9 (Time & Max):       "___45:04 | 232km/h___" or "___45:04 | 232km/h___"         | no Text
//			// just one signle value
//			if(m_trip_mode<5){
//				// copy text to char buffer
//				char temp_char_array[22];
//				pMenu->copy_storagename_to_chararray(m_trip_storage-1,temp_char_array);
//				// cut it for longer menu Points
//				if(m_trip_mode>2){
//					temp_char_array[7]=0x00;
//				}
//
//				if(m_trip_mode==1){
//					if(temp_trip_dist>999999){
//						// "NonPermanent 32878km"  20 Chars
//						sprintf_P(char_buffer,PSTR("%s %lukm"),temp_char_array,(unsigned long)floor(temp_trip_dist/1000));
//					}
//					// less then 10000 km
//					else {
//						// "NonPermanent 3288.1km"  21 Chars
//						sprintf_P(char_buffer,PSTR("%s %i.%ikm"),temp_char_array,(int)floor(temp_trip_dist/1000),(int)floor((temp_trip_dist%1000)/100));
//					}
//				} else if(m_trip_mode==2){
//					// "NonPermanent 345:34" 19 Chars
//					sprintf_P(char_buffer,PSTR("%s %02i:%02i"),temp_char_array,(int)floor(temp_avg_timebase/3600),(int)floor((temp_avg_timebase%3600)/60));
//				} else if(m_trip_mode==3){
//					// "NonPerma AVG 111km/h" 20 Chars
//					sprintf_P(char_buffer,PSTR("%s AVG %ikm/h"),temp_char_array,(int)floor(temp_trip_dist*3.6/temp_avg_timebase));
//				} else if(m_trip_mode==4){
//					// "NonPerma MAX 323km/h" 20 Chars
//					sprintf_P(char_buffer,PSTR("%s MAX %ikm/h"),temp_char_array,(int)temp_max_speed);
//				}
//			}
//			// Distance & Time
//			else if(m_trip_mode==5){
//				// more that 10.000 km or more than 100 hours ==>34543km | 682h<=== 14 chars
//				if(temp_trip_dist>999999 || temp_avg_timebase>=360000L){
//					sprintf_P(char_buffer,PSTR("%lukm | %02ih"),(unsigned long)floor(temp_trip_dist/1000),(int)floor(temp_avg_timebase/3600));
//				}
//				// less that 10.000 km and less than 100 hours ==>4543.1km | 68:22h<=== 17 chars
//				else {
//					sprintf_P(char_buffer,PSTR("%i.%ikm | %02i:%02i"),(int)floor(temp_trip_dist/1000),(int)floor((temp_trip_dist%1000)/100),(int)floor(temp_avg_timebase/3600),(int)floor((temp_avg_timebase%3600)/60));
//				};
//			}
//			// Distance & Avg
//			else if(m_trip_mode==6){
//				// ==>32799.3km | 111km/h<== 19 chars
//				sprintf_P(char_buffer,PSTR("%i.%ikm | %ikm/h"),(int)floor(temp_trip_dist/1000),(int)floor((temp_trip_dist%1000)/100),(int)floor(temp_trip_dist*3.6/temp_avg_timebase));
//			}
//			// Distance & Max
//			else if(m_trip_mode==7){
//				// ==>32799.3km | 232km/h<== 19 chars
//				sprintf_P(char_buffer,PSTR("%i.%ikm | %ikm/h"),(int)floor(temp_trip_dist/1000),(int)floor((temp_trip_dist%1000)/100),(int)temp_max_speed);
//			}
//			// Time & Avg
//			else if(m_trip_mode==8){
//				// more than 100h
//				if(temp_avg_timebase>=360000L){
//					// ==>345h | 111km/h<== 14 chars
//					sprintf_P(char_buffer,PSTR("%3ih | %ikm/h"),(int)floor(temp_avg_timebase/3600),(int)floor(temp_trip_dist*3.6/temp_avg_timebase));
//				}
//				// less than 100h
//				else {
//					// ==>45:04 | 111km/h<== 15 chars
//					sprintf_P(char_buffer,PSTR("%02i:%02i | %ikm/h"),(int)floor(temp_avg_timebase/3600),(int)floor((temp_avg_timebase%3600)/60),(int)floor(temp_trip_dist*3.6/temp_avg_timebase));
//				}
//			}
//			// Time & Max
//			else if(m_trip_mode==9){
//				// more than 100h
//				if(temp_avg_timebase>=360000L){
//					// ==>345h | 232km/h<== 14 chars
//					sprintf_P(char_buffer,PSTR("%3ih | %ikm/h"),(int)floor(temp_avg_timebase/3600),(int)temp_max_speed);
//				}
//				// less than 100h
//				else {
//					// ==>45:04 | 232km/h<== 15 chars
//					sprintf_P(char_buffer,PSTR("%02i:%02i | %ikm/h"),(int)floor(temp_avg_timebase/3600),(int)floor((temp_avg_timebase%3600)/60),(int)temp_max_speed);
//				}
//			}
//
//
//			pMenu->center_me(char_buffer,21);
//			Debug.speedo_loop(16,0,previousMillis,(char *)" ");
//			//OLED.string(addinfo_widget.font,char_buffer,addinfo_widget.x,addinfo_widget.y,0,DISP_BRIGHTNESS,0);
//			OLED.string(addinfo_widget.font,char_buffer,0,addinfo_widget.y,0,DISP_BRIGHTNESS,0);
//		};
//	};
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-tt"));
#endif
	/********************** Zeitanzeige nur wenn anders  *******************/
//	if((!(clock_widget.x==-1 && clock_widget.y==-1)) && check_no_collision_with_addinfo2(clock_widget.y)){ // only show it if pos != -1/-1
//		if(pSensors->m_clock->changed(((uint8_t*)disp_zeile_bak)+TIME_BAK)){ // coooooool, we need only one adress of the array, so we take the array header address and add the Field
//			pSensors->m_clock->copy(char_buffer);
//			Debug.speedo_loop(17,0,previousMillis,char_buffer);
//			OLED.string(clock_widget.font,char_buffer,clock_widget.x+1,clock_widget.y,0,DISP_BRIGHTNESS,2);
//		};
//	};
	/********************** Zeitanzeige nur wen anders  *******************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif
#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-gear"));
#endif
	/*********************** Ganganzeige unten mitte  *********************
	 * zuerst den faktor berechnen aus drehzahl und dem speed am mmag sensor
	 * da gps und drehzahl nicht perfekt synchron laufen
	 * mit dem faktor gehts in die konfigurierbare LUT
	 * und dann unten in die Anzeige
	 *
	 *********************** Ganganzeige unten mitte  *********************/
//	if((!(gear_widget.x==-1 && gear_widget.y==-1)) && check_no_collision_with_addinfo2(gear_widget.y)){ // only show it if pos != -1/-1
//		int gang=pSensors->m_gear->get();
//		// wenn gang anders => anzeigen
//		if(gang+2!=disp_zeile_bak[GEAR_VALUE]){
//			disp_zeile_bak[GEAR_VALUE]=gang+2;
//			if(gang==0){
//				sprintf(char_buffer,"N");
//			} else if(gang<0){
//				sprintf(char_buffer," ");
//			} else {
//				sprintf(char_buffer,"%i",gang%10);
//			};
//			Debug.speedo_loop(18,0,previousMillis," ");
//			// depend on skinsettings
//			OLED.string(gear_widget.font,char_buffer,gear_widget.x,gear_widget.y,0,DISP_BRIGHTNESS,2);
//		};
//	};
	/*********************** Ganganzeige unten mitte  *********************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif

#ifdef TACHO_SMALLDEBUG
	Serial.puts(USART1,("-f"));
#endif
	/********************** benzinanzeige unten rechts ********************
	 * hat die kleine besonderheit das sie blinken soll,
	 * daher kann fuel_copy() auch -100 zurueckgeben.
	 * dann wird schlicht nichts angezeigt
	 ********************** benzinanzeige unten rechts ********************/
//	if((!(fuel_widget.x==-1 && fuel_widget.y==-1)) && check_no_collision_with_addinfo2(fuel_widget.y)){ // only show it if pos != -1/-1
//		float fuel_temp=pSensors->m_fuel->get_fuel(char_buffer);
//		if((disp_zeile_bak[FUEL_VALUE]!=(int)floor(fuel_temp)+2) && check_no_collision_with_addinfo2(fuel_widget.y)){
//			disp_zeile_bak[FUEL_VALUE]=(int)floor(fuel_temp)+2;
//			Debug.speedo_loop(19,0,previousMillis,char_buffer);
//			// depend on skinsettings
//			OLED.string(fuel_widget.font,char_buffer,fuel_widget.x+2,fuel_widget.y,0,DISP_BRIGHTNESS,0);
//		};
//	};
	/********************** benzinanzeige unten rechts ********************/
#ifdef TACHO_SMALLDEBUG
	Serial.puts_ln(USART1,("."));
#endif

	/*************** jump navigation points **********************
	 * if the navigation is active, but the menu_state is !=1 than the up/down button has been pushed
	 * during navigation mode => so change the Navipointer!! to jump in the points
	 **************************************************************/
//	if(pSensors->m_gps->navi_active && (pMenu->state%10)==2){ // button down
//		pSensors->m_gps->navi_point++;
//		pConfig->storage_outdated=true;		// store change
//		pConfig->write("BASE.TXT");
//		pMenu->state=11; 					// reset state
//		pSensors->m_gps->generate_new_order();
//	} else if(pSensors->m_gps->navi_active && (pMenu->state%10)==9){ // button up
//		pSensors->m_gps->navi_point--;
//		if(pSensors->m_gps->navi_point<1) { pSensors->m_gps->navi_point=0; };
//		pConfig->storage_outdated=true;		// store change
//		pConfig->write("BASE.TXT");
//		pMenu->state=11; 					// reset state
//		pSensors->m_gps->generate_new_order();
//	}
	/*************** jump navigation points **********************/
	Debug.speedo_loop(20,1,previousMillis," ");
	free(char_buffer);
}

bool speedo::check_no_collision_with_addinfo2(int current_widget_y){
	return (current_widget_y!=addinfo2_widget.y || !addinfo2_currently_shown);
}


void speedo::initial_draw_screen(){
	if(oil_widget.symbol && !(oil_widget.x==-1 && oil_widget.y==-1))
		OLED.draw_oil(oil_widget.x*3,oil_widget.y*8); //3=6/2 weil doppelpixxel
	if(water_widget.symbol && !(water_widget.x==-1 && water_widget.y==-1))
		OLED.draw_water(water_widget.x*3,water_widget.y*8); // bisher oil icon
	if(fuel_widget.symbol && !(fuel_widget.x==-1 && fuel_widget.y==-1))
		OLED.draw_fuel(fuel_widget.x*3,fuel_widget.y*8); // 7 => 56
	if(air_widget.symbol && !(air_widget.x==-1 && air_widget.y==-1))
		OLED.draw_air(air_widget.x*3,air_widget.y*8);
	if(clock_widget.symbol && !(clock_widget.x==-1 && clock_widget.y==-1))
		OLED.draw_clock(clock_widget.x*3,clock_widget.y*8);
//	if(gps_widget.symbol && !(gps_widget.x==-1 && gps_widget.y==-1))			 // TODO
//		OLED.draw_gps(gps_widget.x*3,gps_widget.y*8,pSensors->m_gps->get_info(6));

	reset_bak(); // alle disp_zeile_bak auf -99 setzen
}

void speedo::clear_vars(){
	refresh_cycle=-1; // anzahl an ms nachdem der haupttacho gecleared wird
	m_trip_mode=-1;
	m_trip_storage=-1;
	addinfo2_currently_shown=false; 												// assume that the addinfo2 is not shown
	memset(OLED.startup,'\0',sizeof(OLED.startup)/sizeof(OLED.startup[0]));	// Startup sequence
	strcpy((char *)OLED.startup,("ERROR,0,0,0;"));

	trip_dist[1]=0;																	// always reseten -> non permanent storage
	max_speed[1]=0;
	avg_timebase[1]=0;

	Serial.puts_ln(USART1,("Speedo values clear"));
}

void speedo::check_vars(){
	if(pSD->sd_failed==true){
		// skinning
		water_widget.x=0;
		water_widget.y=0;
		water_widget.symbol=true;
		water_widget.font=VISITOR_SMALL_1X_FONT;

		oil_widget.x=-1;
		oil_widget.y=-1;
		oil_widget.symbol=true;
		oil_widget.font=VISITOR_SMALL_1X_FONT;

		air_widget.x=14;
		air_widget.y=0;
		air_widget.symbol=true;
		air_widget.font=VISITOR_SMALL_1X_FONT;

		arrow_widget.x=0;
		arrow_widget.y=1;

		kmh_widget.x=6;
		kmh_widget.y=2;
		kmh_widget.font=VISITOR_SMALL_3X_FONT;

		kmhchar_widget.x=15;
		kmhchar_widget.y=4;
		kmhchar_widget.font=VISITOR_SMALL_1X_FONT;

		dz_widget.x=-1;
		dz_widget.y=-1;
		dz_widget.font=VISITOR_SMALL_1X_FONT;

		gps_widget.x=8;
		gps_widget.y=5;
		gps_widget.font=VISITOR_SMALL_1X_FONT;

		addinfo_widget.x=-1;
		addinfo_widget.y=-1;
		addinfo_widget.font=VISITOR_SMALL_1X_FONT;

		addinfo2_widget.x=0;
		addinfo2_widget.y=6;
		addinfo2_widget.font=VISITOR_SMALL_1X_FONT;

		clock_widget.x=0;
		clock_widget.y=7;
		clock_widget.symbol=true;
		clock_widget.font=VISITOR_SMALL_1X_FONT;

		gear_widget.x=11;
		gear_widget.y=7;
		gear_widget.font=VISITOR_SMALL_1X_FONT;

		fuel_widget.x=14;
		fuel_widget.y=7;
		fuel_widget.symbol=true;
		fuel_widget.font=VISITOR_SMALL_1X_FONT;

		default_font=VISITOR_SMALL_1X_FONT;
		// skinning
	}

	// fix trip vars if needed
	if(m_trip_mode<0){
		m_trip_mode=1;
	}

	if(m_trip_storage<0){
		m_trip_storage=1;
	}
};
