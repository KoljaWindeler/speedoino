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


Speedo_sensors::Speedo_sensors(){
	m_blinker=new moped_blinker;
	m_clock=new speedo_clock;
	m_dz=new speedo_dz;
	m_gps=new speedo_gps();
	m_temperature=new speedo_temperature();
	m_fuel=new speedo_fuel();
	m_speed=new speedo_speed();
	m_reset=new speedo_reset();
	m_gear=new speedo_gear();
	m_oiler=new speedo_oiler();
	m_voltage=new speedo_voltage();
};

Speedo_sensors::~Speedo_sensors(){
};

void Speedo_sensors::clear_vars(){
	m_blinker->clear_vars();
	m_clock->clear_vars();
	m_dz->clear_vars();
	m_gps->clear_vars();
	m_temperature->clear_vars();
	m_fuel->clear_vars();
	m_speed->clear_vars();
	m_reset->clear_vars();
	m_gear->clear_vars();
	m_oiler->clear_vars();
	m_voltage->clear_vars();
};

void Speedo_sensors::check_vars(){
	bool any_failed=false;
	// wenn ein test einen fehler meldet wird der return wert "true"

	any_failed|=m_blinker->check_vars();
	any_failed|=m_clock->check_vars();
	any_failed|=m_dz->check_vars();
	any_failed|=m_gps->check_vars();
	any_failed|=m_temperature->check_vars();
	any_failed|=m_fuel->check_vars();
	any_failed|=m_speed->check_vars();
	any_failed|=m_reset->check_vars();
	any_failed|=m_gear->check_vars();
	any_failed|=m_oiler->check_vars();
	any_failed|=m_voltage->check_vars();

	if(any_failed){

		pDebug->sprintp(PSTR("!!!! WARNING !!!!"));
		pDebug->sprintp(PSTR("SD access strange"));
		pDebug->sprintp(PSTR("!!!! WARNING !!!!"));

		pSD->sd_failed=true;
		//_delay_ms(5000);
		//pOLED->clear_screen();
	}
};

void Speedo_sensors::single_read(){
	pSensors->m_clock->inc();  // sekunden hochzählen
	pSensors->m_gps->valid++;  // vor wievielen sekunden war es das letzte mal gültig
	pSensors->m_temperature->read_air_temp();  // temperaturen aktualisieren
	pSensors->m_temperature->read_oil_temp();  // temperaturen aktualisieren
	pSensors->m_temperature->read_water_temp();  // temperaturen aktualisieren
	pSensors->m_oiler->check_value(); // gucken ob wir ölen müssten
	pSensors->m_voltage->calc(); // spannungscheck
};


void Speedo_sensors::init(){
	m_blinker->init();
	m_clock->init();
	m_dz->init();
	m_gps->init();
	m_temperature->init();
	m_fuel->init();
	m_speed->init();
	m_reset->init();
	m_gear->init();
	m_oiler->init();
	m_voltage->init();
	pDebug->sprintlnp(PSTR("Sensors init done"));
}

float Speedo_sensors::flatIt(int actual, short* counter, int max_counter, float old_flat){
	if(*counter<max_counter && *counter>=0){
		*counter=*counter+1;
		return (float)((old_flat*(*counter-1)+actual)/(*counter));
	} else if(*counter==max_counter){
		return (float)((old_flat*(max_counter-1)+actual)/(max_counter));
	} else {
		*counter=1;
		return actual;
	}
	// hier besteht die gefahr das ein messwert nur [INTmax]/max_counter groß sein darf
	// bei 20 Werten also nur 3276,8
}

