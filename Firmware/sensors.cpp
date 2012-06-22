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

void Speedo_sensors::loop(){
	char *char_buffer;
	char_buffer = (char*) malloc (22);
	////////////////////// water //////////////
	if(pSensors->m_temperature->get_water_temp()!=pSpeedo->disp_zeile_bak[2]){
		pSpeedo->disp_zeile_bak[2]=pSensors->m_temperature->get_water_temp();

		if(pSensors->m_temperature->get_water_temp()==8888){
			sprintf(char_buffer," -     "); // error occored -> no sensor
		} else if(pSensors->m_temperature->get_water_temp()==9999){
			sprintf(char_buffer," --    "); // error occored -> short to gnd
		} else 	if(pSensors->m_temperature->get_water_temp()>1100){
			sprintf(char_buffer,">110{C  "); // more then 110°C add a space to have 5 chars
		} else 	{
			sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->m_temperature->get_water_temp()/10))%1000,pSensors->m_temperature->get_water_temp()%10); // _32.3°C  7 stellen
		};
		// depend on skinsettings
		pOLED->string(pSpeedo->default_font,char_buffer,9,2,0,DISP_BRIGHTNESS,-4);
	};

	////////////////////// air //////////////
	if(pSensors->m_temperature->get_air_temp()!=pSpeedo->disp_zeile_bak[3]){

		pSpeedo->disp_zeile_bak[3]=pSensors->m_temperature->get_air_temp();
		sprintf(char_buffer,"%2i.%i{C",int(floor(pSensors->m_temperature->get_air_temp()/10))%100,pSensors->m_temperature->get_air_temp()%10);
		// depend on skinsettings
		pOLED->string(pSpeedo->default_font,char_buffer,9,3,0,DISP_BRIGHTNESS,2);
	};

	////////////////////// oil //////////////
	if(pSensors->m_temperature->get_oil_temp()!=pSpeedo->disp_zeile_bak[4]){

		pSpeedo->disp_zeile_bak[4]=pSensors->m_temperature->get_oil_temp();

		if(pSensors->m_temperature->get_oil_temp()==8888){
			sprintf(char_buffer," -     "); // error occored -> no sensor
		} else if(pSensors->m_temperature->get_oil_temp()==9999){
			sprintf(char_buffer," --    "); // error occored -> short to gnd
		} else {
			sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->m_temperature->get_oil_temp()/10))%1000,pSensors->m_temperature->get_oil_temp()%10); // _32.3°C  7 stellen
		};
		// depend on skinsettings
		pOLED->string(pSpeedo->default_font,char_buffer,9,4,0,DISP_BRIGHTNESS,-4);
	};

	////////////////////// voltage //////////////
	if(pSpeedo->disp_zeile_bak[0]!=1){
		pOLED->string_P(pSpeedo->default_font,PSTR("Voltage "),1,7);
		pSpeedo->disp_zeile_bak[0]=1;
	}
	int voltage=pSensors->m_voltage->get();
	if(voltage!=pSpeedo->disp_zeile_bak[1]){
		pSpeedo->disp_zeile_bak[1]=voltage;
		char temp[6];
		sprintf(temp,"%2i,%iV",int(floor(voltage/10)),int(voltage%10));
		pOLED->string(pSpeedo->default_font,temp,9,7);
	};
	free(char_buffer);
};



