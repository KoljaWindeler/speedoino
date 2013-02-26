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
	pDebug->sprintlnp(PSTR("Sensors values clear"));
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
	pDebug->sprintlnp(PSTR("Sensor single read ... "));
	pDebug->sprintp(PSTR("Reading: clock ... "));
	pSensors->m_clock->inc();  // sekunden hochzählen
	pDebug->sprintp(PSTR("Done\r\nReading: GPS ... "));
	pSensors->m_gps->valid++;  // vor wievielen sekunden war es das letzte mal gültig
	pDebug->sprintp(PSTR("Done\r\nReading: Air temp ... "));
	pSensors->m_temperature->read_air_temp();  // temperaturen aktualisieren
	pDebug->sprintp(PSTR("Done\r\nReading: Oil temp ... "));
	pSensors->m_temperature->read_oil_temp();  // temperaturen aktualisieren
	pDebug->sprintp(PSTR("Done\r\nReading: Water temp ... "));
	pSensors->m_temperature->read_water_temp();  // temperaturen aktualisieren
	pDebug->sprintp(PSTR("Done\r\nReading: Oiler ... "));
	pSensors->m_oiler->check_value(); // gucken ob wir ölen müssten
	pDebug->sprintp(PSTR("Done\r\nReading: Voltages ... "));
	pSensors->m_voltage->calc(); // spannungscheck
	char temp[6];
	sprintf(temp,"%2i,%iV",int(floor(m_voltage->get()/10)),int(m_voltage->get()%10));
	Serial.print(temp);
	pDebug->sprintp(PSTR(" Done\r\nReading: Control lights ... "));
	check_inputs();
	pDebug->sprintlnp(PSTR(" Done\r\nSensor single read ... Done"));
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

	cli(); // TODO ... unschön, warum reagiert er überhaupt schon auf interrupts?
	// Blinker LINKS Interrupt
	EIMSK |= (1<<INT6); // Enable Interrupt
	EICRB |= (1<<ISC60) | (1<<ISC61); // rising edge on INT5

	// Blinker RECHTS Interrupt
	EIMSK |= (1<<INT7); // Enable Interrupt
	EICRB |= (1<<ISC70) | (1<<ISC71); // rising edge on INT5

	// Neutral Gear Interrupt
	DDRK  &=~(1<<PK1); // ensure its an input
	PORTK |=(1<<PK1); // activate Pull UP

	// Oil Pressure Interrupt
	DDRK  &=~(1<<PK0); // ensure its an input
	PORTK |=(1<<PK0); // activate Pull UP
	sei();
	// High Beam
	PCMSK2|=(1<<PCINT18) | (1<<PCINT17) | (1<<PCINT16);
	PCICR |=(1<<PCIE2); // general interrupt PC aktivieren für SK2


	pDebug->sprintlnp(PSTR("Sensors init done"));
}
/************* IIR Tiefpass ***********************
 * WARNING: max_counter is signed char! max 127
 * This is used to flat values,
 * y(0)=(y(1)*min((counter-1),max_counter-1)+x)/min((counter-1),max_counter-1)
 *
 * input:
 * - actual: integer value of the current measurement
 * - counter: POINTER on char, this value will increase inside
 * - max_counter: FIX char value
 * - old_flat: float value of the OLD calculation
 *
 * output:
 * - float value of IIR
 ************* IIR Tiefpass ***********************/
float Speedo_sensors::flatIt(int actual, unsigned char *counter, char max_counter, float old_flat){
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


// interrupt to update sensors
ISR(INT6_vect ){
	pSensors->check_inputs();
}
// interrupt to update sensors
ISR(INT7_vect ){
	pSensors->check_inputs();
}
// interrupt to update sensors
ISR(PCINT2_vect ){
	pSensors->check_inputs();
}

void Speedo_sensors::check_inputs(){
	unsigned char high_beam=0x00;
	unsigned char flasher_left=0x00;
	unsigned char flasher_right=0x00;
	unsigned char oil_pressure=0x01; // low active
	unsigned char neutral_gear=0x01; // low active

	if(PINK&(1<<HIGH_BEAM_PIN)){
		high_beam=0x01;
	}

	if(PINK&(1<<OIL_PRESSURE_PIN)){	 // if the pin is still high, the pulldown is active, signal is not active
		oil_pressure=0x00;
	}

	if(PINK&(1<<NEUTRAL_GEAR_PIN)){	 // if the pin is still high, the pulldown is active, signal is not active
		neutral_gear=0x00;
	}

	if(PINE&(1<<FLASHER_LEFT_PIN)){
		flasher_left=0x01;
	}

	if(PINE&(1<<FLASHER_RIGHT_PIN)){
		flasher_right=0x01;
	}

	pAktors->set_controll_lights(oil_pressure,flasher_left,neutral_gear,flasher_right,high_beam);
}

