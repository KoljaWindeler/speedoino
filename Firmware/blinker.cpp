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

moped_blinker::moped_blinker(){
}

moped_blinker::~moped_blinker(){

};

void moped_blinker::check(){
  short int blinker_state=digitalRead(BLINKER_PIN);
  bool blinker_on;

  if(blinker_state==1){ // quasi high active
    guard_start=millis(); // hier nichts ändern
    blinker_on=true;
    if(BLINKER_DEBUG){ pDebug->sprintlnp(PSTR("Blinker ist wirklich an")); };
  } else if (blinker_state==0 && guard_start+1000>millis()){
    blinker_on=true; // wenn innerhalb der letzten sec der blinker an war
    if(BLINKER_DEBUG){ pDebug->sprintlnp(PSTR("Blinker ist aus aber guard intervall")); };
  } else {
    blinker_on=false;
    if(BLINKER_DEBUG){ pDebug->sprintlnp(PSTR("Blinker ist aus")); };
  };

  if(blinker_on && !lock){
    lock=true;
    set_start(pSpeedo->trip_dist[8],pSensors->m_speed->getSpeed()>80);
  } else if(!blinker_on && lock){
    lock=false;
    start=0;
  };
};

void moped_blinker::set_start(unsigned long dist,int kmh){
	start=dist;
	if(kmh>80){ // wenn man bei über 80 km/h den Blinker anmacht
		start+=high_speed_add; // bekommt man 200 meter mehr, bevor die Warnung kommt
	};
}

bool moped_blinker::warn(unsigned long dist){
	if(dist>(start+dist_to_warn) && lock && BLINKER_ENABLED){ // mal nachfragen wenn nach 200 metern der Blinker noch an ist
		return true;
	};
	return false;
}

void moped_blinker::init(){
  pinMode(BLINKER_PIN, INPUT);
  pDebug->sprintlnp(PSTR("Blinker init done"));
  guard_start=0;
  high_speed_add=0;
};
