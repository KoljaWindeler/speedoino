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
/////////////// vars ///////////////////
//unsigned int fuel_count; // preload anzahl von 10tel ml
//int fuel_out; // preload (liter * 10) verbraucht
//int fuel_max; // max
/////////////// vars ///////////////////
speedo_fuel::speedo_fuel(){
	last_time=0;
	blink_freq=0;	 // default value, überschreibt config
	blink_start=0; // default value, überschreibt config
};

speedo_fuel::~speedo_fuel(){
};


float speedo_fuel::get_fuel(char char_buffer[]){
	char local_buffer[6];
	bool should_blink=false;
	float fuel_diff=(float)round(pSpeedo->trip_dist[5]/1000);


	if(pSensors->CAN_active && pSensors->m_CAN->get_active_can_type()==CAN_TYPE_TRIUMPH){
		should_blink=pSensors->m_CAN->get_fuel_blink();
	} else if(fuel_diff>=blink_start){
		should_blink=true;
	}


	if(should_blink){
		if(last_time+2*blink_freq<millis()){ // gib die Zahl aus wie z.B. "119" und schreib in den Buffer "119km"
			last_time=millis();
			sprintf(local_buffer,"%3ikm",((int)fuel_diff)%1000);	// gib die zahl zurück, alles easy
		} else if(last_time+blink_freq<millis()){ // wir sind mit dem Tank weiter gefahren als die Blinkgranze und wollen gerade mal nix anzeigen um das blinken zu symbolisieren
			sprintf(local_buffer,"     ");
			fuel_diff=-100;
		}
	} else {
		sprintf(local_buffer,"%3ikm",((int)fuel_diff)%1000);
	}

	// copy to output
	for(unsigned int i=0; i<6; i++){
		char_buffer[i]=local_buffer[i];
	}
	return fuel_diff;
};
//
void speedo_fuel::init(){

}


int speedo_fuel::check_vars(){
	if(blink_freq==0 || blink_start==0){
		blink_freq=500; // blinkt mit 500ms Periodenlaenge
		blink_start=140; // bei mehr als 140 km auf der Tankuhr fngt die an zu blinken
		pDebug->sprintp(PSTR("fuel failed"));
		return 1;
	}
	return 0;
};
