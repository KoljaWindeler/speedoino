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

// SENSOR Class holds a member of each Sensor needed
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
	m_voltage=new speedo_voltage();
	m_CAN=new Speedo_CAN();

	ten_Hz_counter=0;
	ten_Hz_timer=millis();
	CAN_active=false;
};

// destructor just for nuts
Speedo_sensors::~Speedo_sensors(){
};

// initialize the sensor class, by using the build in init
// seqence of each sensor
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
	m_voltage->init();
	m_CAN->init();

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


// initialize every var, and write clean blank value to it (base config)
void Speedo_sensors::clear_vars(){
	pDebug->sprintlnp(PSTR("Sensors values clear"));
};

// check each and every sensor end return result
void Speedo_sensors::check_vars(){
	bool any_failed=false;
	// wenn ein test einen fehler meldet wird der return wert "true"

	// a good place to use destructors, or "deactivators"
	if(CAN_active){
		any_failed|=m_CAN->check_vars();
		m_speed->shutdown();
		m_dz->shutdown();
	} else {
		any_failed|=m_dz->check_vars();
		any_failed|=m_speed->check_vars();
		m_CAN->shutdown();
	}

	any_failed|=m_blinker->check_vars();
	any_failed|=m_clock->check_vars();
	any_failed|=m_gps->check_vars();
	any_failed|=m_temperature->check_vars(); // needed for oiltemp
	any_failed|=m_fuel->check_vars();
	any_failed|=m_reset->check_vars();
	any_failed|=m_gear->check_vars();
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

/********************************** GET section *************************************
 * Since the Infomation could be provided by CAN or by
 * convertional sensors, we have to build a wrapper for
 * the request. These wrapper are handling the function calls.
 * we need wrappers for all infos, available on the CAN Bus:
 * - Engine RPM
 * - Verhicle Speed
 * - Air intake Temperature
 * - Coolant Temparture
 ********************************** GET section *************************************/
unsigned int Speedo_sensors::get_RPM(bool exact_dz_needed){
	if(CAN_active && !m_CAN->failed){
		return m_CAN->get_RPM();
	} else {
		return m_dz->get_dz(exact_dz_needed);
	}
};

unsigned int Speedo_sensors::get_speed(bool mag_if_possible){
	unsigned int return_value;
	if(CAN_active && !m_CAN->failed){
		return_value=m_CAN->get_Speed();
	} else {
		if(mag_if_possible){
			return_value=(unsigned)m_speed->get_mag_speed();
		}else {
			return_value=(unsigned)m_speed->getSpeed();
		};
	}
	return return_value;
};

int Speedo_sensors::get_water_temperature(){
	if(CAN_active && !m_CAN->failed){
		return m_CAN->get_water_temp();
	} else {
		return m_temperature->get_water_temp();
	}
};

int Speedo_sensors::get_air_temperature(){
//	if(CAN_active && !m_CAN->failed){ // CAN airintake is much to warm
//		return m_CAN->get_air_temp();
//	} else {
		return m_temperature->get_air_temp();
//	}
};

int Speedo_sensors::get_oil_temperature(){
//	if(CAN_active && !m_CAN->failed){ // no can sensor available
//		return m_CAN->get_oil_temp();
//	} else {
		return m_temperature->get_oil_temp();
//	}
};
/********************************** GET section *************************************/

/********************************** READ section *************************************
 * The values must be read from the sensors, eighter by reading the analog sensors
 * or by sending a CAN Bus request
 * Single_read() should only provide startup Values for the inital screen.
 * after that, the pull_values() does the work for us
 ********************************** READ section *************************************/

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
	pDebug->sprintp(PSTR("Done\r\nReading: Voltages ... "));
	pSensors->m_voltage->calc(); // spannungscheck
	char temp[6];
	sprintf(temp,"%2i,%iV",int(floor(m_voltage->get()/10)),int(m_voltage->get()%10));
	Serial.print(temp);
	pDebug->sprintp(PSTR(" Done\r\nReading: Control lights ... "));
	check_inputs();
	pDebug->sprintlnp(PSTR(" Done\r\nSensor single read ... Done"));
};


void Speedo_sensors::pull_values(){
	// is an update required?
	boolean update_required=false;
	if((millis()-ten_Hz_timer)>=99){ // 100ms
		ten_Hz_timer=millis();
		ten_Hz_counter=(ten_Hz_counter+1)%10;
		update_required=true;

		if(ten_Hz_counter==0){
			m_clock->inc();  // sekunden hochzählen
			m_gps->valid++;  // vor wievielen sekunden war es das letzte mal gültig
			m_voltage->calc(); // spannungscheck
			m_temperature->read_oil_temp();  // temperaturen aktualisieren
		} else if(update_required && ten_Hz_counter%2==0){ // do this, every 5Hz, 200ms
			m_blinker->check();    // blinken wir?
		}
	}


	/********************* CAN **************************
	 * we have to deciede from where we receive our infomation,
	 * from CAN or from converntianal sensors
	 * infos from CAN-Bus, could be:
	 * - Engine RPM
	 * - Verhicle Speed
	 * - Air intake Temperature
	 * - Coolant Temparture
	 *********************** CAN *************************/
	// CAN is present and should be used
	if(CAN_active && !m_CAN->failed){
		if(update_required){ // 10Hz
			if(ten_Hz_counter==0 || ten_Hz_counter==2 || ten_Hz_counter==4 || ten_Hz_counter==6 || ten_Hz_counter==8){ // 200ms
				m_CAN->request(CAN_RPM);
			} else if(ten_Hz_counter==1 || ten_Hz_counter==5 ){ // 500ms
				m_CAN->request(CAN_SPEED);
//			} else if(ten_Hz_counter==3){ // 1000ms
//				m_CAN->request(CAN_AIR_TEMP);
			} else if(ten_Hz_counter==7){ // 1000ms
				m_CAN->request(CAN_WATER_TEMP);
			} else if(ten_Hz_counter==9 ){  // 1000ms one free slot
				//m_CAN->request(?);
			};
		}
	}

	// no can available,
	else {
		if(update_required){ // 100ms spacing
			// do this, with 200ms spacing
			if(ten_Hz_counter%2==0){
				m_dz->calc();
			};

			// do this, once a second
			if(ten_Hz_counter==0){
				m_temperature->read_air_temp();  // temperaturen aktualisieren
				m_temperature->read_water_temp();  // temperaturen aktualisieren
			}
		}
	}
}
/********************************** READ section *************************************/

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

void Speedo_sensors::addinfo_show_loop(){
	char *char_buffer;
	char_buffer = (char*)malloc(22);
	////////////////////// water //////////////
	if(pSensors->get_water_temperature()!=pSpeedo->disp_zeile_bak[2]){
		pSpeedo->disp_zeile_bak[2]=pSensors->get_water_temperature();

		if(pSensors->get_water_temperature()==8888){
			sprintf(char_buffer," -     "); // error occored -> no sensor
		} else if(pSensors->get_water_temperature()==9999){
			sprintf(char_buffer," --    "); // error occored -> short to gnd
		} else 	if(pSensors->get_water_temperature()>1100){
			sprintf(char_buffer,">110{C  "); // more then 110°C add a space to have 5 chars
		} else 	{
			sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->get_water_temperature()/10))%1000,pSensors->get_water_temperature()%10); // _32.3°C  7 stellen
		};
		// depend on skinsettings
		pOLED->string(pSpeedo->default_font,char_buffer,9,2,0,DISP_BRIGHTNESS,-4);
	};

	////////////////////// air //////////////
	if(pSensors->get_air_temperature()!=pSpeedo->disp_zeile_bak[3]){

		pSpeedo->disp_zeile_bak[3]=pSensors->get_air_temperature();
		sprintf(char_buffer,"%2i.%i{C",int(floor(pSensors->get_air_temperature()/10))%100,pSensors->get_air_temperature()%10);
		// depend on skinsettings
		pOLED->string(pSpeedo->default_font,char_buffer,9,3,0,DISP_BRIGHTNESS,2);
	};

	////////////////////// oil //////////////
	if(pSensors->get_oil_temperature()!=pSpeedo->disp_zeile_bak[4]){

		pSpeedo->disp_zeile_bak[4]=pSensors->get_oil_temperature();

		if(pSensors->get_oil_temperature()==8888){
			sprintf(char_buffer," -     "); // error occored -> no sensor
		} else if(pSensors->get_oil_temperature()==9999){
			sprintf(char_buffer," --    "); // error occored -> short to gnd
		} else {
			sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->get_oil_temperature()/10))%1000,pSensors->get_oil_temperature()%10); // _32.3°C  7 stellen
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

/********************************** WARN light section *************************************
 * all light depeding infos are processed here
 ********************************** WARN light section *************************************/
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
	if(PINK&(1<<CAN_INTERRUPT_PIN)){	 // if the CAN pin is high
		if(pSensors->CAN_active){		 // is the CAN mode active
			pSensors->m_CAN->message_available=true;
		};
	};
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
/********************************** WARN light section *************************************/
