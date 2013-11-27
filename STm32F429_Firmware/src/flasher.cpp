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

flasher::flasher(){
	flasher_active=false;
	last_toggle_time=0;

	high_speed_add=0;
	dist_to_warn=0;
	start=0;
}

flasher::~flasher(){};


void flasher::pin_toogled(){
	last_toggle_time=Millis.get();
	if(!flasher_active){ // first time called, note actual trip
		set_start(Speedo.trip_dist[8],Sensors.get_speed(false));
	}
	flasher_active=true;
};


void flasher::check(){ // called every 100ms from pSensors->pull_values()
	if(flasher_active){ // yep, we are flashing
		if((Millis.get()-last_toggle_time)>1500){ // 1.5 sec no change on pin => flasher off! could be down to 500ms?
			flasher_active=false;
		} else if(Sensors.get_speed(false)==0){ // still flashing but now standing
			start=Speedo.trip_dist[8]; // reset length
		}
	}
};

void flasher::set_start(unsigned long dist,int kmh){
	start=dist;
	if(kmh>80){ // wenn man bei Ã¼ber 80 km/h den flasher anmacht
		start+=high_speed_add-80+kmh; // bekommt man 200 meter mehr, bevor die Warnung kommt
		// + etwas mehr durch den speed
	};
}

bool flasher::warn(unsigned long dist){
	if(flasher_active && FLASHER_ENABLED){
		if(dist>(start+dist_to_warn)){ // mal nachfragen wenn nach 200 metern der flasher noch an ist
			//Serial.println("return true");
			//_delay_ms(100);
			return true;
		};
	};
	return false;
}

void flasher::init(){
	Serial.puts_ln(USART1,("Flasher init done"));
};


int flasher::check_vars(){
	if(dist_to_warn==0){
		Serial.puts_ln(USART1,("Flasher failed"));
		high_speed_add=200;
		dist_to_warn=200;
		return 1;
	}
	return 0;
};

