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
	m_dz=new speedo_dz;
	m_temperature=new speedo_temperature();
	m_speed=new speedo_speed();
	m_reset=new speedo_reset();
	m_voltage=new speedo_voltage();
	m_CAN=new Speedo_CAN();

	fourty_Hz_counter=0;
	fourty_Hz_timer=millis();
	CAN_active=true; // don't care, will change it in init
	sensor_source=SENSOR_ANALOG;
	last_highbeam_on=0;
	last_oil_off=0;
	rpm_flatted_counter=0;
	rpm_flatted=0;
	last_int_state=0;
};

// destructor just for nuts
Speedo_sensors::~Speedo_sensors(){
};

// initialize the sensor class, by using the build in init
// seqence of each sensor
void Speedo_sensors::init(){
	m_dz->init();
	m_temperature->init();
	m_speed->init();
	//	m_reset->init();
	m_voltage->init();
	//	m_CAN->init(); // done later in main startup

	cli(); //  ... unschÃ¶n, aber gps macht interrupts an
	// Blinker LINKS Interrupt
	EIMSK |= (1<<INT5); // Enable Interrupt
	EICRB |= (1<<ISC50); // any change on INT5

	// Blinker RECHTS Interrupt
	EIMSK |= (1<<INT7); // Enable Interrupt
	EICRB |= (1<<ISC70); // any change on INT5

	// Neutral Gear Interrupt
	DDRK  &=~(1<<PK1); // ensure its an input
	PORTK |=(1<<PK1); // activate Pull UP

	// Oil Pressure Interrupt
	DDRK  &=~(1<<PK0); // ensure its an input
	PORTK |=(1<<PK0); // activate Pull UP
	sei();
	// High Beam
	PCMSK2|=(1<<PCINT18) | (1<<PCINT17) | (1<<PCINT16); //Oil in, High beam in, Neutral gear
	PCICR |=(1<<PCIE2); // general interrupt PC aktivieren fÃ¼r SK2

	rpm_flatted_counter=0;
	pDebug->sprintlnp(PSTR("Sensors init done"));
}


// check each and every sensor end return result
void Speedo_sensors::check_vars(){
	int any_failed=0;
	// wenn ein test einen fehler meldet wird der return wert "true"
	if(sensor_source==SENSOR_AUTO || sensor_source==SENSOR_FORCE_CAN){
		if(sensor_source==SENSOR_AUTO){
			pDebug->sprintlnp(PSTR("CAN auto detect"));
		} else {
			pDebug->sprintlnp(PSTR("Force CAN mode"));
		}
		CAN_active=true;
		any_failed+=m_CAN->check_vars();
		m_speed->shutdown();
		m_dz->shutdown();
	} else {
		pDebug->sprintlnp(PSTR("Analog sensor mode"));
		CAN_active=false;
		m_CAN->shutdown();
	}

	/* CHECK ALL SENSORS */

	any_failed+=m_reset->check_vars();
	any_failed+=m_voltage->check_vars();

	/* CHECK ALL SENSORS */
};

/********************************** GET section *************************************
 * Since the Infomation could be provided by CAN or by
 * convertional sensors, we have to build wrappers for
 * the requests. These wrappers are handling the function calls.
 * we need wrappers for all infos, available on the CAN Bus:
 * - Engine RPM
 * - Verhicle Speed
 * - Air intake Temperature
 * - Coolant Temparture
 ********************************** GET section *************************************/
unsigned int Speedo_sensors::get_RPM(int mode){ // 0=exact, 1=flated, 2=hard
#ifdef DEMO_MODE
	return pDemo->get_RPM();
#endif
	unsigned int exact_value=0;

	// get exact value
	if(CAN_active && !m_CAN->failed){
		exact_value=m_CAN->get_RPM(); // just get the number, no calculation
	} else {
		exact_value=m_dz->get_dz(); // just get the number, no calculation
	}

	// return value based on mode,
	if(mode==RPM_TYPE_FLAT){ // used often
		return rpm_flatted;	// will be calculated with 10Hz in pull values
	} else if(mode==RPM_TYPE_FLAT_ROUNDED){ // used in speedo
		return (rpm_flatted&~15); // will be calculated with 10Hz in pull values
	} else if(mode==RPM_TYPE_ROUNDED){
		return (exact_value&~63); // == floor(exact_value/64)*64
	}

	return exact_value;
};

unsigned int Speedo_sensors::get_speed(){
#ifdef DEMO_MODE
	return pDemo->get_speed();
#endif
	unsigned int return_value;
	if(CAN_active && !m_CAN->failed){
		return_value=m_CAN->get_Speed();
	} else {
		return_value=(unsigned)m_speed->get_mag_speed();
	};
	return return_value;
};

int Speedo_sensors::get_water_temperature(){
#ifdef DEMO_MODE
	return pDemo->get_water_temperature();
#endif
	if(CAN_active && !m_CAN->failed){
		return m_CAN->get_water_temp();
	} else {
		return m_temperature->get_water_r();
	}
};

// 0 if everything allright
// return 5 on short to gnd
// 9 on no sensor value (no responses on CAN)
int Speedo_sensors::get_water_temperature_fail_status(){
#ifdef DEMO_MODE
	return 0;
#endif
	if(CAN_active && !m_CAN->failed){
		return m_CAN->get_CAN_missed_count();
	} else {
		return m_temperature->water_r_fail_status;
	}
};

int Speedo_sensors::get_air_temperature(){
	//	if(CAN_active && !m_CAN->failed){ // CAN airintake is much to warm
	//		return m_CAN->get_air_temp();
	//	} else {
	return m_temperature->get_air_temp();
	//	}
};

int Speedo_sensors::get_oil_r(){
	//	if(CAN_active && !m_CAN->failed){ // no can sensor available
	//		return m_CAN->get_oil_temp();
	//	} else {
	return m_temperature->get_oil_r();
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
	pDebug->sprintp(PSTR("Done\r\nReading: Air temp ... "));
	pSensors->m_temperature->read_air_temp();  // temperaturen aktualisieren
	pDebug->sprintp(PSTR("Done\r\nReading: Oil temp ... "));
	pSensors->m_temperature->read_oil_r();  // temperaturen aktualisieren
	pDebug->sprintp(PSTR("Done\r\nReading: Water temp ... "));
	pSensors->m_temperature->read_water_r();  // temperaturen aktualisieren, useless if we use CAN but no problem
	pDebug->sprintp(PSTR("Done\r\nReading: Voltages ... "));
	pSensors->m_voltage->calc(false); // spannungscheck
	char temp[6];
	sprintf(temp,"%2i,%iV",int(floor(m_voltage->get()/100)),int((m_voltage->get()/10)%10));
	Serial.print(temp);
	pDebug->sprintp(PSTR(" Done\r\nReading: Control lights ... "));
	check_inputs();
	pDebug->sprintlnp(PSTR(" Done\r\nSensor single read ... Done"));
};


/***************************************************** PULL values ********************************************************
 * Pull values is a process, called within every main loop cycle, so we have to decide, based on our own timer ten_Hz_timer
 *
 * the bool vars "update_required" becomes true every 100ms. In addition a counter "ten_Hz_counter" tells you if its the
 * first, second, ..., tenth call within this second
 *
 * ============== INDEPENDENT ON CAN MODE ========================
 *
 * 1000ms: Sensors independent on CAN or no CAN, called every second just once: (by update_required && ten_Hz_counter==0)
 * 		m_clock->inc(); // just counts seconds
 * 		m_gps->valid++; // counts second from last valid data
 * 		m_voltage->calc(); // update voltage value, maybe startup (if we started in clock_mode)
 * 		m_temperature->read_oil_temp(); // update analog oil temperature sensor
 *
 * 100ms: Sensors independent on CAN or no CAN, called every 200 ms: (by update_required && ten_Hz_counter%2==0)
 * 		m_blinker->check(); // check if the flashers are working
 * 		pSensors->m_gear->calc(); // calculate gear based on speed and rpm
 * 		rpm_flatted=flatIt(get_RPM(RPM_TYPE_DIRECT),&rpm_flatted_counter,2,get_RPM(RPM_TYPE_FLAT));
 * 		pAktors->rgb_action(get_RPM(RPM_TYPE_FLAT));
 *
 *  25ms:
 *  	pAktors->m_stepper->go_to(get_RPM(pAktors->m_stepper->shown_mode)); //3.10.2013 ran good
 *
 * ============== ONLY IN NON CAN MODE ========================
 * 	1000ms:
 *		m_temperature->read_air_temp();  // temperaturen aktualisieren
 *		m_temperature->read_water_temp();  // temperaturen aktualisieren
 *
 * ============== ONLY IN CAN MODE ========================
 * 	1000ms:
 * 		m_CAN->request(CAN_WATER_TEMP); // generate a CAN message to ask for speed
 *
 * 	500ms:
 * 		m_CAN->request(CAN_SPEED); // generate a CAN message to ask for speed
 *
 * 	200ms:
 * 		m_CAN->request(CAN_RPM); // generate a CAN message to ask for RPM
 *
 * 	0ms:
 * 		m_CAN->process_incoming_messages(); // call if the interrupt has set our flag
 *
 ***************************************************** PULL values *******************************************************/

void Speedo_sensors::pull_values(){
	// is an update required?
	boolean update_required=false;
	if((millis()-fourty_Hz_timer)>=24){ // 25ms
#ifdef TACHO_SMALLDEBUG
		pDebug->sprintp(PSTR("-s"));
#endif
		fourty_Hz_timer=millis();
		fourty_Hz_counter=(fourty_Hz_counter+1)%40;
		update_required=true;

		if(fourty_Hz_counter==0){ // 1 Hz
			m_voltage->calc(false); // spannungscheck
			m_temperature->read_oil_r();  // temperaturen aktualisieren
			// auto CAN detection ...
			if(sensor_source==SENSOR_AUTO){
				if(!m_CAN->init_comm_possible(&CAN_active)){ // returns always true, exept the communcation was NOT possible even if it should
					pDebug->sprintlnp(PSTR("=== CAN timed out ==="));
					pDebug->sprintlnp(PSTR("falling back to analog sensors"));
					m_CAN->shutdown();
					m_dz->init();
					m_speed->init();
					pDebug->sprintlnp(PSTR("=== CAN timed out ==="));
				}
			};
		}

		if(fourty_Hz_counter%4==0){			//do this, every 10Hz, 100ms
			// IIR mit RÃ¼ckfÃ¼hrungsfaktor 3 fÃ¼r Anzeige, 20*4 Pulse, 1400U/min = 2,5 sec | 14000U/min = 0,25 sec
			rpm_flatted=flatIt(get_RPM(RPM_TYPE_DIRECT),&rpm_flatted_counter,2,get_RPM(RPM_TYPE_FLAT));

			//			////////////// TODO
			//			char rpm_buffer[20];
			//			sprintf(rpm_buffer,"%i,%i",get_RPM(RPM_TYPE_DIRECT),get_RPM(RPM_TYPE_FLAT));
			//			Serial.println(rpm_buffer);
			//			////////////// TODO
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
		// fetch new message first
		if(m_CAN->message_available){ // muss hier die pin abfrage rein? dafÃ¼r gibts doch den interrupt
			m_CAN->process_incoming_messages();
		}
		if(update_required && m_CAN->get_active_can_type()==CAN_TYPE_OBD2 && fourty_Hz_counter%4==0){ // 10Hz, request can OBD2 msg
			if(fourty_Hz_counter==0 || fourty_Hz_counter==2 || fourty_Hz_counter==4 || fourty_Hz_counter==6 || fourty_Hz_counter==8){ // 200ms
				m_CAN->request(CAN_CURRENT_INFO,CAN_RPM);
			} else if(fourty_Hz_counter==1 || fourty_Hz_counter==5 ){ // 500ms
				m_CAN->request(CAN_CURRENT_INFO,CAN_SPEED);
			} else if(fourty_Hz_counter==7){ // 1000ms
				m_CAN->request(CAN_CURRENT_INFO,CAN_WATER_TEMPERATURE);
			} else if(fourty_Hz_counter==9 ){  // 1000ms one free slot
				//m_CAN->request(?);
			};
		}
#ifdef TACHO_SMALLDEBUG
		pDebug->sprintlnp(PSTR("."));
#endif
	}

	/***************** no can available, *********************
	 * ask analog sensors
	 ***************** no can available, *********************/
	else {
		if(update_required){ // 100ms spacing
			// do this, once a second
			if(fourty_Hz_counter==0){
				m_temperature->read_air_temp();  // temperaturen aktualisieren
				m_temperature->read_water_r();  // temperaturen aktualisieren
			}
#ifdef TACHO_SMALLDEBUG
			pDebug->sprintlnp(PSTR("."));
#endif
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
	if(*counter==max_counter){
		return (float)((old_flat*(max_counter-1)+actual)/(max_counter));
	} else if(*counter<max_counter && *counter>=0){
		*counter=*counter+1;
		return (float)((old_flat*(*counter-1)+actual)/(*counter));
	} else {
		*counter=1;
		return actual;
	}
	// hier besteht die gefahr das ein messwert nur [INTmax]/max_counter groÃŸ sein darf
	// bei 20 Werten also nur 3276,8
}

float Speedo_sensors::flatIt_shift(int actual, uint8_t *counter, uint8_t shift, float old_flat){
	if(*counter==1<<(shift+1)){ // e.G. shift == 4 (2â�´=16) = (0b10000=16)
		return (float)(old_flat+((int)(actual-old_flat)>>shift)); // e.g. shift = 2, old_flat=1, actual=2, result should be 1.25: (old_flat*3+actual)/4=5/4=1.25 OR BY SHIFT: 1+((2-1)>>2=1+1>>2=1+0.25=1.25
	} else if(*counter<1<<shift && *counter>=0){
		*counter=*counter+1;
		return (float)((old_flat*(*counter-1)+actual)/(*counter));
	} else {
		*counter=1;
		return actual;
	}
	// hier besteht die gefahr das ein messwert nur [INTmax]/max_counter groÃŸ sein darf
	// bei 20 Werten also nur 3276,8
}

uint16_t Speedo_sensors::flatIt_shift_mask(uint16_t actual, uint8_t shift, uint16_t old_flat, uint16_t nmask){
	return old_flat+(((int)(actual-old_flat)>>shift)&~nmask); // e.g. shift = 2, old_flat=1, actual=2, result should be 1.25: (old_flat*3+actual)/4=5/4=1.25 OR BY SHIFT: 1+((2-1)>>2=1+1>>2=1+0.25=1.25
	// hier besteht die gefahr das ein messwert nur [INTmax]/max_counter groÃŸ sein darf
	// bei 20 Werten also nur 3276,8
}




/********************************** WARN light section *************************************
 * all light depeding infos are processed here
 ********************************** WARN light section *************************************/
// interrupt to update sensors
ISR(INT5_vect ){
	pSensors->check_inputs();
}
// interrupt to update sensors
ISR(INT7_vect ){
	pSensors->check_inputs();
}
// interrupt to update sensors
ISR(PCINT2_vect ){
	// check if its the right version before test the pin
	if(pConfig->get_hw_version()==7){
		if(pSensors->m_CAN->check_message()){
			// processing is in CAN class
		} else if(pSensors->m_CAN->get_active_can_type()!=CAN_TYPE_TRIUMPH){ // if version 7, but not triumpf check light, tritumpf is way to heavy traffic
			pSensors->check_inputs();
		}
	} else if(pConfig->get_hw_version()>7) { // from version higher than 7, CAN Interrupt is not on this interrupt port
		pSensors->check_inputs();
	}
}

//ISR(PCINT0_vect){
//	// check if its the right version before test the pin
//	if(pConfig->get_hw_version()>7){
//		if(!(CAN_INTERRUPT_PIN_PORT_V8&(1<<CAN_INTERRUPT_PIN_FROM_V8))){	 // if the CAN pin is low, low active interrupt
//			if(pSensors->CAN_active){		 // is the CAN mode active
//				pSensors->m_CAN->message_available=true;
//#ifdef CAN_DEBUG
//				Serial.println("Interrupt: Msg available");
//#endif
//			};
//		};
//	}
//}

void Speedo_sensors::check_inputs(){
	unsigned char high_beam=0x00;
	unsigned char flasher_left=0x00;
	unsigned char flasher_right=0x00;
	unsigned char oil_pressure=0x00; // low active
	unsigned char neutral_gear=0x01; // low active

	if(PINK&(1<<HIGH_BEAM_PIN)){  // pin is high
		if(m_CAN->get_active_can_type()!=CAN_TYPE_TRIUMPH){ // on "non triumph" -> on means on :D
			high_beam=0x01;
		} else if(last_highbeam_on==0){ // addition keep off for triumph
			last_highbeam_on=millis();
			high_beam=0x00; // not now
		} else if(millis()-last_highbeam_on>500) { // last active >1sec
			high_beam=0x01;
		}
	} else { // pin is low, High beam is off
		last_highbeam_on=0;
		high_beam=0x00; // switch off
	}

	if(!(PINK&(1<<OIL_PRESSURE_PIN))){  // pin is low, (low active)
		if(last_oil_off==0){
			last_oil_off=millis();
			oil_pressure=0x00; // not now
		} else if(millis()-last_oil_off>500) { // last active >1sec
			oil_pressure=0x01;
		}
	} else { // pin is high, oil is off
		last_oil_off=0;
		oil_pressure=0x00; // switch off
	}

	// depending on CAN or non CAN mode ... and if CAN on CAN TYPE
	if(CAN_active && m_CAN->get_active_can_type()==CAN_TYPE_TRIUMPH){
		neutral_gear=m_CAN->get_neutral_gear_state();
	} else {
		if(PINK&(1<<NEUTRAL_GEAR_PIN)){	 // if the pin is still high, the pulldown is not active, signal is not active
			neutral_gear=0x00;
		} else {
			neutral_gear=0x01;
		}
	}

	if(PINE&(1<<FLASHER_LEFT_PIN)){// TODO Layout has pin moved to 5, check board
		flasher_left=0x01;
	}

	if(PINE&(1<<FLASHER_RIGHT_PIN)){
		flasher_right=0x01;
	}


	sensor_state = flasher_left << FLASHER_LEFT_SHIFT;
	sensor_state|= flasher_right << FLASHER_RIGHT_SHIFT;
	sensor_state|= high_beam << HIGH_BEAM_SHIFT;
	sensor_state|= oil_pressure << OIL_PRESSURE_SHIFT;
	sensor_state|= neutral_gear << NEUTRAL_GEAR_SHIFT;
}
/********************************** WARN light section *************************************/

uint16_t Speedo_sensors::pulldown_of_divider(uint32_t mV_supply, uint16_t mV_center, uint16_t pull_up){
	uint32_t uV_center=((uint32_t)1000)*((uint32_t)mV_center);		// e.g. uV_center=2500*1000=2.500.000
	uint32_t pull_up_mV=mV_supply-mV_center; 	// e.g. pull_up_mV=12000-2500=9500
	uint32_t current_uA=(pull_up_mV*1000)/pull_up;	// e.g. current_uA=9.500.000/3000=3166
	return (uint16_t)(uV_center/current_uA); 	// e.g.	2.500.000/3166=789 Ohm
}
