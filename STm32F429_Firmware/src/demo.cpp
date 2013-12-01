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
/*
 * demo.cpp
 *
 *  Created on: 17.06.2013
 *      Author: kolja
 */


/*
 * Demo length should be 100sec
 * Start is 30 sec
 * === SPEEDO + RPM ===
 * 3sec gear 1
 * 5sec gear 2
 * 10sec gear 3
 * 10sec gear 4
 * 10sec gear 5
 * 62sec gear 6
 *
 */

#ifdef DEMO_MODE

speedo_demo::speedo_demo(){
	timing_sec[0]=3; // 9
	timing_sec[1]=8; // 24
	timing_sec[2]=18; // 54
	timing_sec[3]=28; // 84
	timing_sec[4]=38; // 114
	timing_sec[5]=100;
}

speedo_demo::~speedo_demo(){}

unsigned int  speedo_demo::get_RPM(){
	if(Millis.get()>30000){
		unsigned int max_k_rpm[6]={7,10,10,12,10,12};
		unsigned int min_k_rpm[6]={0,4,6,8,9,8};

		unsigned long my_millis=Sensors.mGPS.mod((Millis.get()-30000),100000); // das wird so ja wahrscheinlich eher nicht gehen
		int i=0;
		while(my_millis>(1000*(unsigned long)timing_sec[i]) && i<5){
			i++;
		};

		int offset=min_k_rpm[i]*1000;
		unsigned long delta_time=timing_sec[i];
		unsigned long delta_rpm=(max_k_rpm[i]-min_k_rpm[i])*1000;
		unsigned long start_time=0;

		if(i>0){
			delta_time=timing_sec[i]-timing_sec[i-1]; // wieviel gesamt zeit hab ich, wie ist mein delta x
			start_time=timing_sec[i-1]; // wann haben wir angefangen also wie war mein offset x
		}
		start_time*=1000;
		unsigned long prefactor=(my_millis-start_time)/delta_time;
		unsigned long return_value=offset+prefactor*delta_rpm/1000; // offset_y+(x-offset_y)/delta_x*delta_y; // wie ist mein offset y
		return (unsigned int) return_value;
	};
	return 0;
}

unsigned int speedo_demo::get_speed(){
	// rpm/speed=n_gang ==> speed = rpm/ngang
	if(Millis.get()>30000){
		unsigned long my_millis=Sensors.mGPS.mod((Millis.get()-30000),100000); // das wird so ja wahrscheinlich eher nicht gehen
		int i=0;
		while(my_millis>1000*timing_sec[i] && i<5){
			i++;
		};
		// Sensors.m_gear->faktor_counter=0; // sollte den tiefpass brücken, ist das gut?
		if(Sensors.mGear.n_gang[i+1]<1){
			return 222;
		}
		return ((unsigned long)(Sensors.get_RPM(RPM_TYPE_FLAT)))/Sensors.mGear.n_gang[i+1];
	}
	return 0;
}

int speedo_demo::get_water_temperature(){
	if(Millis.get()>30000){
		unsigned long my_millis=Sensors.mGPS.mod((Millis.get()-30000),100000); // das wird so ja wahrscheinlich eher nicht gehen
		if(my_millis<50000){
			return (900-200)*my_millis/50000+200; //linear
		} else {
			return 900;
		}
	}
	return 200; //20.0° default
}

int speedo_demo::get_oil_temperature(){
	Sensors.mTemperature.oil_temp_fail_status=1;
	if(Millis.get()>30000){
		unsigned long my_millis=Sensors.mGPS.mod((Millis.get()-30000),100000); // das wird so ja wahrscheinlich eher nicht gehen
		if(my_millis<70000){
			return (950-200)*my_millis/70000+200; //linear
		} else {
			return 950;
		}
	}
	return 200; //20.0° default
}

#endif
