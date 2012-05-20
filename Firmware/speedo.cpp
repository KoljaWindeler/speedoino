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
void speedo_speedo::reset_bak(){
	for(unsigned int i=0;i<sizeof(disp_zeile_bak)/sizeof(disp_zeile_bak[0]);i++){
		disp_zeile_bak[i]=-99;
	};
}

/************* speedo main loop *************
 * this is THE main loop, default speedo mode
 ************* speedo main loop *************/
void speedo_speedo::loop(unsigned long previousMillis){
	char *char_buffer;
	char_buffer = (char*) malloc (22);
	if (char_buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));

	previousMillis=millis(); // get time for this loop
	pDebug->speedo_loop(0,1,previousMillis,(char *)" "); // debug
	/************************* oil temperature *********************
	 * Oil Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->oil_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a coresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 ************************* oil temperature *********************/
	if(disp_zeile_bak[OIL_TEMP]!=pSensors->m_temperature->get_oil_temp()){
		if(!(oil_widget.x==-1 && oil_widget.y==-1)){ // only show it if pos != -1/-1
			disp_zeile_bak[OIL_TEMP]=int(pSensors->m_temperature->get_oil_temp());
			// below 20 degree the PTC is very antiliear so we won't show it
			if(pSensors->m_temperature->get_oil_temp()>200){
				sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->m_temperature->get_oil_temp()/10))%1000,pSensors->m_temperature->get_oil_temp()%10); // _32.3°C  7 stellen
			} else {
				sprintf(char_buffer,"<20{C  "); // below 20°C add a space to have 5 chars
			};
			pDebug->speedo_loop(1,0,previousMillis," "); // debug
			// depend on skinsettings
			pOLED->string(oil_widget.font,char_buffer,oil_widget.x+4,oil_widget.y,0,DISP_BRIGHTNESS,-4);
		};
	};
	/************************* oil temperature *********************/
	/************************* water temperature *********************
	 * water Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->water_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a coresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 ************************* oil temperature *********************/
	if(disp_zeile_bak[WATER_TEMP]!=pSensors->m_temperature->get_water_temp()){
		if(!(water_widget.x==-1 && water_widget.y==-1)){ // only show it if pos != -1/-1
			disp_zeile_bak[WATER_TEMP]=int(pSensors->m_temperature->get_water_temp());
			// below 20 degree the PTC is very antiliear so we won't show it
			if(pSensors->m_temperature->get_water_temp()>200){
				sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->m_temperature->get_water_temp()/10))%1000,pSensors->m_temperature->get_oil_temp()%10); // _32.3°C  7 stellen
			} else {
				sprintf(char_buffer,"<20{C  "); // below 20°C add a space to have 5 chars
			};
			pDebug->speedo_loop(1,0,previousMillis," "); // debug
			// depend on skinsettings
			pOLED->string(water_widget.font,char_buffer,water_widget.x+4,water_widget.y,0,DISP_BRIGHTNESS,-4);
		};
	};
	/************************* oil temperature *********************/
	pDebug->speedo_loop(22,1,0," ");
	/************************* air temperature *********************
	 * Air Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->air_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a corresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 ************************* air temperature *********************/
	if(disp_zeile_bak[AIR_TEMP]!=pSensors->m_temperature->get_air_temp()){
		if(!(air_widget.x==-1 && air_widget.y==-1)){ // only show it if pos != -1/-1
			disp_zeile_bak[AIR_TEMP]=int(pSensors->m_temperature->get_air_temp());
			sprintf(char_buffer,"%2i.%i{C",int(floor(pSensors->m_temperature->get_air_temp()/10))%100,pSensors->m_temperature->get_air_temp()%10);
			pDebug->speedo_loop(2,0,previousMillis," ");
			// depend on skinsettings
			pOLED->string(air_widget.font,char_buffer,air_widget.x+1,air_widget.y,0,DISP_BRIGHTNESS,2);
		};
	};
	/************************* air temperature *********************/
	pDebug->speedo_loop(23,1,0," ");
	/************************* arrow ******************
	 * if we move the return value of m_gps->get_order will change (will return distance to next goal)
	 * on change we get another arrow to the goal,
	 * A TODO would be a kind of "show north"-Mode, so .. here we go TODO TODO
	 **************************************************/
	if(pSensors->m_gps->navi_active || pSensors->m_gps->navi_active){ // Arrow Mode TODO
		if(!(pSpeedo->arrow_widget.x==-1 && pSpeedo->arrow_widget.y==-1)){
			int result_value=pSensors->m_gps->get_order(char_buffer); // distance will be calculated on the parse routine, this call just copies it
			if(disp_zeile_bak[ARROW]!=result_value+pSensors->m_gps->valid){
				pOLED->draw_arrow(pSensors->m_gps->winkel,arrow_widget.x,arrow_widget.y);
				//pOLED->draw_arrow((pSensors->m_clock->get_ss()*9+1)%360,arrow_widget.x,arrow_widget.y);
				disp_zeile_bak[ARROW]=int(result_value+pSensors->m_gps->valid);
			}
		};
	}
	/************************* speed *********************
	 * getSpeed will return the speed as int, depending on setting this will be the GPS OR Magnitude based speed.
	 * first if checks if the value equals -99, in this case the full display has to be rewritten, draw "km/h"
	 * else just draw the number of km/h, but only refresh it if the difference is higher than 2 or we are very slow (below 10)
	 ************************* geschwindigkeit *********************/
	int temp_speed=pSensors->m_speed->getSpeed();
	if(!(kmh_widget.x==-1 && kmh_widget.y==-1)){ // only show it if pos != -1/-1
		if(disp_zeile_bak[SPEED_VALUE]==-99){ // storage has been resetet, draw everything
			sprintf(char_buffer,"%3i",temp_speed);
			// depend on skinsettings danger, 2 places to edit the kmh size
			pOLED->string(kmh_widget.font,char_buffer,kmh_widget.x,kmh_widget.y,0,DISP_BRIGHTNESS,0); //   6/2
			pOLED->string_P(kmhchar_widget.font,PSTR("km/h"),kmhchar_widget.x,kmhchar_widget.y,0,DISP_BRIGHTNESS,2); //       13/3 // that belongs to the speed
			pDebug->speedo_loop(3,0,previousMillis," ");  // debug
			disp_zeile_bak[SPEED_VALUE]=int(temp_speed+1);
		} else if( (abs(disp_zeile_bak[SPEED_VALUE]-(temp_speed+1))>1) || (abs(disp_zeile_bak[SPEED_VALUE]-(temp_speed+1))==1 && temp_speed<10) ) {
			sprintf(char_buffer,"%3i",temp_speed);
			pDebug->speedo_loop(4,0,previousMillis," "); // debug
			// depend on skinsettings danger, 2 places to edit the kmh size
			pOLED->string(kmh_widget.font,char_buffer,kmh_widget.x,kmh_widget.y,0,DISP_BRIGHTNESS,0); //   6/2
			disp_zeile_bak[SPEED_VALUE]=int(temp_speed+1);
		};
	};
	/************************* speed *********************/
	/********************* additional informations *****************************
	 * first we have to separate between the navigation state
	 * if the menu_state ==1  we will show something like
	 *    102 km/h           <-- speedo, printed with the code on top of this section
	 * 5800 U/min | 4 GPS    <-- engine rpm, gps info  (1)
	 *     Day 342.km        <-- one storage, chosen in the setup section (2)
	 * |||||||||||||||||||   <-- the highlighted "info" field, this could be anything like (flasher/navigation/engine cold) (3)
	 ***********************************************************************/
	// see comment on top, number (1)
	//pSensors->m_dz->calc(); // calc in main loop to run the stepper in every menu
	if(!(dz_widget.x==-1 && dz_widget.y==-1)){ // only show it if pos != -1/-1
		int sats=pSensors->m_gps->get_info(6);
		if(disp_zeile_bak[DZ_VALUE]!=signed(pSensors->m_dz->rounded+1+sats)){
			if(disp_zeile_bak[DZ_VALUE]==-99){ //schreib alles neu, auch die buchstaben
				if(pSensors->m_gps->valid>4 || sats==0){ // GPS is invalid
					sprintf(char_buffer,"%5i U/min | no GPS",pSensors->m_dz->rounded); // auf glatte 250er
				} else {
					sprintf(char_buffer,"%5i U/min | %2i GPS",pSensors->m_dz->rounded,int(sats)%100); // auf glatte 250er
				};
				pDebug->speedo_loop(5,0,previousMillis," ");
				// depend on skinsettings
				pOLED->string(dz_widget.font,char_buffer,dz_widget.x,dz_widget.y,0,DISP_BRIGHTNESS,0);
			} else { // nur eine änderung im wert
				sprintf(char_buffer,"%5i",pSensors->m_dz->rounded);
				pDebug->speedo_loop(6,0,previousMillis," ");
				pOLED->string(dz_widget.font,char_buffer,dz_widget.x,dz_widget.y,0,DISP_BRIGHTNESS,0);
				// GPS //
				if(pSensors->m_gps->valid>=4 || sats==0){ // GPS is invalid wenn wir 3 sec nix gehört haben
					sprintf(char_buffer,"no GPS");
				} else {
					sprintf(char_buffer,"%2i GPS",sats%100);
				};
				// depend on skinsettings
				pOLED->string(dz_widget.font,char_buffer,dz_widget.x+14,dz_widget.y,0,DISP_BRIGHTNESS,0); // TODO
			};
			disp_zeile_bak[DZ_VALUE]=int(pSensors->m_dz->rounded+1+sats);
		};
	};

	if(!(addinfo_widget.x==-1 && addinfo_widget.y==-1)){ // only show it if pos != -1/-1
		// see on top comment, number (2)
		if(unsigned(disp_zeile_bak[ADD_INFO])!=pSensors->m_gps->mod(floor(trip_dist[8]/100),100)){ // immer wenn sich trip_dist ändert den string ausgeben der direkt drüber steht, auf 0.1 km genau
			// trip[8] => gesamt/100 => 100er Meter, 27.342,8 => 28
			disp_zeile_bak[ADD_INFO]=int(pSensors->m_gps->mod(floor(trip_dist[8]/100),100));

			if(m_trip_mode==1)
				sprintf(char_buffer,"   Total %5i.%01ikm   ",(int)floor(trip_dist[0]/1000),(int)floor((trip_dist[0]%1000)/100));
			else if(m_trip_mode==2)
				sprintf(char_buffer,"NonPermanent  %3i.%01ikm",(int)floor(trip_dist[1]/1000),(int)floor((trip_dist[1]%1000)/100));
			else if(m_trip_mode==3)
				sprintf(char_buffer,"     Day %3i.%01ikm",(int)floor(trip_dist[2]/1000),(int)floor((trip_dist[2]%1000)/100));
			else if(m_trip_mode==4)
				sprintf(char_buffer,"    Tour  %3i.%01ikm",(int)floor(trip_dist[3]/1000),(int)floor((trip_dist[3]%1000)/100));
			else if(m_trip_mode==5)
				sprintf(char_buffer,"   Quick   %3i.%01ikm",(int)floor(trip_dist[4]/1000),(int)floor((trip_dist[4]%1000)/100));
			else if(m_trip_mode==6)
				sprintf(char_buffer,"   Fuel     %3i.%01ikm",(int)floor(trip_dist[5]/1000),(int)floor((trip_dist[5]%1000)/100));
			else if(m_trip_mode==7)
				sprintf(char_buffer,"   Oiler    %3i.%01ikm",(int)floor(trip_dist[6]/1000),(int)floor((trip_dist[6]%1000)/100));
			else if(m_trip_mode==8)
				sprintf(char_buffer,"  Saison   %5i.%01ikm",(int)floor(trip_dist[7]/1000),(int)floor((trip_dist[7]%1000)/100));
			else if(m_trip_mode==9)
				sprintf(char_buffer,"   Board %5i.%01ikm",(int)floor(trip_dist[8]/1000),(int)floor((trip_dist[8]%1000)/100));
			pDebug->speedo_loop(16,0,previousMillis,(char *)" ");
			pOLED->string(addinfo_widget.font,char_buffer,addinfo_widget.x,addinfo_widget.y,0,DISP_BRIGHTNESS,0);
		};
	};
	if(!(addinfo2_widget.x==-1 && addinfo2_widget.y==-1)){ // only show it if pos != -1/-1
		// now, print variable messages, additional infos, see top comment (3)
		/// warnung wegen zu hoher Drehzahl
		if(pSensors->m_temperature->get_oil_temp()<600 && pSensors->m_dz->rounded>7000){
			if(disp_zeile_bak[ADD_INFO2]!=101){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=101;
				pDebug->speedo_loop(10,0,previousMillis," ");
				pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string_P(addinfo2_widget.font,PSTR("Motor noch kalt"),addinfo2_widget.x+2,addinfo2_widget.y,15,0,1); // 2,6
			};

		}
		///// Temp to high /////
		else if(pSensors->m_temperature->get_oil_temp()>=pSensors->m_temperature->oil_warning_temp && pSensors->m_temperature->get_oil_temp()<8888){
			if(disp_zeile_bak[ADD_INFO2]!=108){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=108;
				pDebug->speedo_loop(14,0,previousMillis," ");
				pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string_P(addinfo2_widget.font,PSTR("oil temp warning"),addinfo2_widget.x+2,addinfo2_widget.y,15,0,1);
			};

		}
		else if(pSensors->m_temperature->get_water_temp()>=pSensors->m_temperature->water_warning_temp && pSensors->m_temperature->get_water_temp()<8888){
			if(disp_zeile_bak[ADD_INFO2]!=109){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=109;
				pDebug->speedo_loop(14,0,previousMillis," ");
				pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string_P(addinfo2_widget.font,PSTR("water temp warning"),addinfo2_widget.x+2,addinfo2_widget.y,15,0,1);
			};

		}
		///// Blinker ////
		else if(pSensors->m_blinker->warn(trip_dist[8])){ // mal nachfragen wenn nach 200 metern der Blinker noch an ist
			if(disp_zeile_bak[ADD_INFO2]!=102){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=102;
				pDebug->speedo_loop(11,0,previousMillis," ");
				pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string_P(addinfo2_widget.font,PSTR("Blinker vergessen"),addinfo2_widget.x+2,addinfo2_widget.y,15,0,1);
			};

		}
		///// Navi ////
		else if(pSensors->m_gps->navi_active){
			int result_value=pSensors->m_gps->get_order(char_buffer); // result value ist der zahlen wert bis zur nächsten aktion, 11km => 11 | 900m => 900 (10m Schritte)
			if(disp_zeile_bak[ADD_INFO2]!=result_value){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh! // wird also einmal pro sek geschrieben
				disp_zeile_bak[ADD_INFO2]=int(result_value);
				pDebug->speedo_loop(13,0,previousMillis," ");
				if(result_value>-1){ // -1 => no gps
					pOLED->filled_rect(0,8*addinfo2_widget.y,8,128,15); // den bereich am ende der Zeile leeren ??
					pOLED->string(addinfo2_widget.font,char_buffer,addinfo2_widget.x,addinfo2_widget.y,15,0,1); // die Ziel ausgabe
				} else {
					pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8);
					pOLED->string_P(addinfo2_widget.font,PSTR("No GPS"),addinfo2_widget.x+9,addinfo2_widget.y,15,0,1);
					pOLED->draw_arrow(-1,arrow_widget.x,arrow_widget.y); // clears arrow
				};
			};
		}
		///// Sensor in use but not found ///
		else if(oil_widget.x!=-1 && oil_widget.y!=-1 && (pSensors->m_temperature->get_oil_temp()==8888 || pSensors->m_temperature->get_oil_temp()==9999)){
			if(disp_zeile_bak[ADD_INFO2]!=106){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=106;
				pDebug->speedo_loop(14,0,previousMillis," ");
				pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string_P(addinfo2_widget.font,PSTR("Oil read failed"),addinfo2_widget.x+2,addinfo2_widget.y,15,0,1);
			};

		}
		else if(water_widget.x!=-1 && water_widget.y!=-1 && (pSensors->m_temperature->get_water_temp()==8888 || pSensors->m_temperature->get_water_temp()==9999)){
			if(disp_zeile_bak[ADD_INFO2]!=107){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=107;
				pDebug->speedo_loop(14,0,previousMillis," ");
				pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string_P(addinfo2_widget.font,PSTR("Water read failed"),addinfo2_widget.x+2,addinfo2_widget.y,15,0,1);
			};

		}
		///// SD card ////
		else if(pSD->sd_failed){
			if(disp_zeile_bak[ADD_INFO2]!=104){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=104;
				pDebug->speedo_loop(14,0,previousMillis," ");
				pOLED->highlight_bar(0,8*addinfo2_widget.y,128,8); // mit hintergrundfarbe nen kasten malen
				pOLED->string_P(addinfo2_widget.font,PSTR("SD access failed"),addinfo2_widget.x+2,addinfo2_widget.y,15,0,1);
			};

		}
		//// löschen
		else{
			if(disp_zeile_bak[ADD_INFO2]!=105){ // erst die bedingung um den Block abzuklopfen dann gucken ob refresh!
				disp_zeile_bak[ADD_INFO2]=105;
				pDebug->speedo_loop(15,0,previousMillis," ");
				pOLED->string_P(addinfo2_widget.font,PSTR("                      "),0,addinfo2_widget.y,0,0,1);

			};
		};
	};
	/********************* additional informations *****************************/
	/********************** Zeitanzeige nur wenn anders  *******************/
	if(!(clock_widget.x==-1 && clock_widget.y==-1)){ // only show it if pos != -1/-1
		if(pSensors->m_clock->changed(disp_zeile_bak+TIME_BAK)){ // coooooool, we need only one adress of the array, so we take the array header address and add the Field
			pSensors->m_clock->copy(char_buffer);
			pDebug->speedo_loop(17,0,previousMillis,char_buffer);
			pOLED->string(clock_widget.font,char_buffer,clock_widget.x+1,clock_widget.y,0,DISP_BRIGHTNESS,2);
		};
	};
	/********************** Zeitanzeige nur wen anders  *******************/
	/*********************** Ganganzeige unten mitte  *********************
	 * zuerst den faktor berechnen aus drehzahl und dem speed am mmag sensor
	 * da gps und drehzahl nicht perfekt synchron laufen
	 * mit dem faktor gehts in die konfigurierbare LUT
	 * und dann unten in die Anzeige
	 *
	 *********************** Ganganzeige unten mitte  *********************/
	if(!(gear_widget.x==-1 && gear_widget.y==-1)){ // only show it if pos != -1/-1
		int gang=pSensors->m_gear->get();
		// wenn gang anders => anzeigen
		if(gang+2!=disp_zeile_bak[GEAR_VALUE]){
			disp_zeile_bak[GEAR_VALUE]=gang+2;
			if(gang==0){
				sprintf(char_buffer,"N");
			} else if(gang<0){
				sprintf(char_buffer," ");
			} else {
				sprintf(char_buffer,"%i",gang%10);
			};
			pDebug->speedo_loop(18,0,previousMillis," ");
			// depend on skinsettings
			pOLED->string(gear_widget.font,char_buffer,gear_widget.x,gear_widget.y,0,DISP_BRIGHTNESS,2);
		};
	};
	/*********************** Ganganzeige unten mitte  *********************/
	/********************** benzinanzeige unten rechts ********************
	 * hat die kleine besonderheit das sie blinken soll,
	 * daher kann fuel_copy() auch -100 zurueckgeben.
	 * dann wird schlicht nichts angezeigt
	 ********************** benzinanzeige unten rechts ********************/
	if(!(fuel_widget.x==-1 && fuel_widget.y==-1)){ // only show it if pos != -1/-1
		float fuel_temp=pSensors->m_fuel->get_fuel(char_buffer);
		if(disp_zeile_bak[FUEL_VALUE]!=(int)floor(fuel_temp)+2){
			disp_zeile_bak[FUEL_VALUE]=(int)floor(fuel_temp)+2;
			pDebug->speedo_loop(19,0,previousMillis,char_buffer);
			// depend on skinsettings
			pOLED->string(fuel_widget.font,char_buffer,fuel_widget.x+2,fuel_widget.y,0,DISP_BRIGHTNESS,0);
		};
	};
	/********************** benzinanzeige unten rechts ********************/
	/*************** jump navigation points **********************
	 * if the navigation is active, but the menu_state is !=1 than the up/down button has been pushed
	 * during navigation mode => so change the Navipointer!! to jump in the points
	 **************************************************************/
	if(pSensors->m_gps->navi_active && (pMenu->state%10)==2){ // runter gedrückt ( nach hinten )
		pSensors->m_gps->navi_point++;
		// store change
		byte tempByte = (pSensors->m_gps->navi_point & 0xFF);
		EEPROM.write(147,tempByte);
		pMenu->state=11; // reset state
		pSensors->m_gps->generate_new_order();
	} else if(pSensors->m_gps->navi_active && (pMenu->state%10)==9){ // hoch gedrückt
		pSensors->m_gps->navi_point--;
		if(pSensors->m_gps->navi_point<1) { pSensors->m_gps->navi_point=0; };
		// store change
		byte tempByte = (pSensors->m_gps->navi_point & 0xFF);
		EEPROM.write(147,tempByte);
		pMenu->state=11; // reset state
		pSensors->m_gps->generate_new_order();
	}
	/*************** jump navigation points **********************/
	pDebug->speedo_loop(20,1,previousMillis," ");
	free(char_buffer);
}

