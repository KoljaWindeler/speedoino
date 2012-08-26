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

speedo_oiler::speedo_oiler(void){
}

speedo_oiler::~speedo_oiler(){
};

void speedo_oiler::init(){
	pinMode(OILER_PIN,OUTPUT);
	digitalWrite(OILER_PIN,LOW);
	pDebug->sprintlnp(PSTR("Oiler init done"));
};

bool speedo_oiler::check_vars(){
	if(grenze==0){
		grenze=4000;
		pDebug->sprintp(PSTR("oiler failed"));
		return true;
	}
	return false;
};

void speedo_oiler::clear_vars(){
	grenze=0;
};

int speedo_oiler::send_impulse(){
	if(pSensors->m_speed->getSpeed()>30){ // nur ölen wenn wir über 30 kmh sind
		digitalWrite(OILER_PIN,HIGH); // 150ms high
		_delay_ms(150);
		digitalWrite(OILER_PIN,LOW);
		pConfig->write("speedo.txt"); // store this
		return 0;
	} else {
		return -1;
	}
};

void speedo_oiler::check_value(){
	/* hier noch die geschwindigkeitsprogression einbauen
	 * sowas wie
	 * if(pSpeedo->trip_dist[6]/pSpeedo->avg_timebase[6]*3.6>140 && pSpeedo->trip_dist[6]>grenze*0.85){
	 * da kann man noch ne ganze Latte hinterlegen wenn man das umbedingt wollen würde
	 */

	if(pSpeedo->trip_dist[6]>unsigned(grenze)){
		if(send_impulse()>=0){ // nur ölen wenn wir über 30 kmh sind
			pSpeedo->trip_dist[6]=0;
			pSpeedo->max_speed[6]=0;
			pSpeedo->avg_timebase[6]=0;
		};
	};
}
