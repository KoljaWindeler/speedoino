/*
 * Speedoino - This file is part of the firmware.
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


//// check each and every sensor end return result
void sensing::check_vars(){
	int any_failed=0;
	// wenn ein test einen fehler meldet wird der return wert "true"
	if(sensor_source==SENSOR_AUTO || sensor_source==SENSOR_FORCE_CAN){
		if(sensor_source==SENSOR_AUTO){
			Serial.puts_ln(USART1,("CAN auto detect"));
		} else {
			Serial.puts_ln(USART1,("Force CAN mode"));
		}
		CAN_active=true;
		any_failed+=mCAN.check_vars();
		mSpeed.shutdown();
		mRpm.shutdown();
	} else {
		Serial.puts_ln(USART1,("Analog sensor mode"));
		CAN_active=false;
		any_failed+=mRpm.check_vars();
		any_failed+=mSpeed.check_vars();
		mCAN.shutdown();
	}

	/* CHECK ALL sensing */

	any_failed+=mFlasher.check_vars();
	any_failed+=mClock.check_vars();
	any_failed+=mGPS.check_vars();
	any_failed+=mTemperature.check_vars(); // needed for oiltemp
	any_failed+=mFuel.check_vars();
	any_failed+=mReset.check_vars();
	any_failed+=mGear.check_vars();
	any_failed+=mVoltage.check_vars();

	if(any_failed){
		Serial.puts_ln(USART1,("!!!! WARNING !!!!"));
		Serial.puts_ln(USART1,("SD access strange"));
		Serial.puts_ln(USART1,("!!!! WARNING !!!!"));

		Serial.puts(USART1,any_failed);
		Serial.puts_ln(USART1,(" failures"));
		SD.sd_failed=true;
//				_delay_ms(5000);
		TFT.clear_screen();
	}
	/* CHECK ALL sensing */
};

///********************************** GET section *************************************
// * Since the Infomation could be provided by CAN or by
// * convertional sensing, we have to build wrappers for
// * the requests. These wrappers are handling the function calls.
// * we need wrappers for all infos, available on the CAN Bus:
// * - Engine RPM
// * - Verhicle Speed
// * - Air intake Temperature
// * - Coolant Temparture
// ********************************** GET section *************************************/
uint16_t sensing::get_RPM(int mode){ // 0=exact, 1=flated, 2=hard
#ifdef DEMO_MODE
	return Demo.get_RPM();
#endif
	unsigned int exact_value=0;

	// get exact value
	if(CAN_active && !mCAN.failed){
		exact_value=mCAN.get_RPM(); // just get the number, no calculation
	} else {
		exact_value=mRpm.get_exact(); // just get the number, no calculation
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
//
uint16_t sensing::get_speed(bool mag_if_possible){
#ifdef DEMO_MODE
	return Demo.get_speed();
#endif
	unsigned int return_value;
	if(CAN_active && !mCAN.failed){
		return_value=mCAN.get_Speed();
	} else {
		if(mag_if_possible){
			return_value=(unsigned)mSpeed.get_mag_speed();
		}else {
			return_value=(unsigned)mSpeed.getSpeed();
		};
	}
	return return_value;
};

int16_t sensing::get_water_temperature(){
#ifdef DEMO_MODE
	return Demo.get_water_temperature();
#endif
	if(CAN_active && !mCAN.failed){
		return mCAN.get_water_temp();
	} else {
		return mTemperature.get_water_temp();
	}
};

// 0 if everything allright
// return 5 on short to gnd
// 9 on no sensor value (no responses on CAN)
int16_t sensing::get_water_temperature_fail_status(){
#ifdef DEMO_MODE
	return 0;
#endif
	if(CAN_active && !mCAN.failed){
		return mCAN.get_CAN_missed_count();
	} else {
		return mTemperature.water_temp_fail_status;
	}
};

int16_t sensing::get_air_temperature(){
	//	if(CAN_active && !mCAN.failed){ // CAN airintake is much to warm NOTODO
	//		return mCAN.get_air_temp();
	//	} else {
	return mTemperature.get_air_temp();
	//	}
};

int16_t sensing::get_oil_temperature(){
#ifdef DEMO_MODE
	return Demo.get_oil_temperature();
#endif
	//	if(CAN_active && !mCAN.failed){ // no can sensor available  NOTODO
	//		return mCAN.get_oil_temp();
	//	} else {
	return mTemperature.get_oil_temp();
	//	}
};
/********************************** GET section *************************************/

/********************************** READ section *************************************
 * The values must be read from the sensing, eighter by reading the analog sensing
 * or by sending a CAN Bus request
 * Single_read() should only provide startup Values for the inital screen.
 * after that, the pull_values() does the work for us
 ********************************** READ section *************************************/

void sensing::single_read(){
	Serial.puts_ln(USART1,("Sensor single read ... "));
	Serial.puts(USART1,("Reading: clock ... "));
	mClock.inc();  // sekunden hochzählen
	Serial.puts(USART1,("Done\r\nReading: GPS ... "));
	mGPS.valid++;  // vor wievielen sekunden war es das letzte mal gültig
	Serial.puts(USART1,("Done\r\nReading: Air temp ... "));
	mTemperature.read_air_temp();  // temperaturen aktualisieren
	Serial.puts(USART1,("Done\r\nReading: Oil temp ... "));
	mTemperature.read_oil_temp();  // temperaturen aktualisieren
	Serial.puts(USART1,("Done\r\nReading: Water temp ... "));
	mTemperature.read_water_temp();  // temperaturen aktualisieren, useless if we use CAN but no problem
	Serial.puts(USART1,("Done\r\nReading: Voltages ... "));
	mVoltage.calc(false); // spannungscheck TODO
	char temp[6];
	sprintf(temp,"%2i,%iV",int(floor(mVoltage.get()/100)),int((mVoltage.get()/10)%10));
	Serial.puts(USART1,temp);
	//	Serial.puts(USART1,(" Done\r\nReading: Control lights ... "));
	//	check_inputs();
	//	Serial.puts_ln(USART1,(" Done\r\nSensor single read ... Done"));
};


/***************************************************** PULL values ********************************************************
 * Pull values is a process, called within every main loop cycle, so we have to decide, based on our own timer ten_Hz_timer
 *
 * the bool vars "update_required" becomes true every 100ms. In addition a counter "ten_Hz_counter" tells you if its the
 * first, second, ..., tenth call within this second
 *
 * ============== INDEPENDENT ON CAN MODE ========================
 *
 * 1000ms: sensing independent on CAN or no CAN, called every second just once: (by update_required && ten_Hz_counter==0)
 * 		mClock.inc(); // just counts seconds
 * 		m_gps->valid++; // counts second from last valid data
 * 		m_voltage->calc(); // update voltage value, maybe startup (if we started in clock_mode)
 * 		mTemperature.read_oil_temp(); // update analog oil temperature sensor
 *
 * 100ms: sensing independent on CAN or no CAN, called every 200 ms: (by update_required && ten_Hz_counter%2==0)
 * 		m_blinker->check(); // check if the flashers are working
 * 		m_gear->calc(); // calculate gear based on speed and rpm
 * 		rpm_flatted=flatIt(get_RPM(RPM_TYPE_DIRECT),&rpm_flatted_counter,2,get_RPM(RPM_TYPE_FLAT));
 * 		Aktors.rgb_action(get_RPM(RPM_TYPE_FLAT));
 *
 *  25ms:
 *  	Aktors.mStepper.go_to(get_RPM(Aktors.mStepper.shown_mode)); //3.10.2013 ran good
 *
 * ============== ONLY IN NON CAN MODE ========================
 * 	1000ms:
 *		mTemperature.read_air_temp();  // temperaturen aktualisieren
 *		mTemperature.read_water_temp();  // temperaturen aktualisieren
 *
 * ============== ONLY IN CAN MODE ========================
 * 	1000ms:
 * 		mCAN.request(CAN_WATER_TEMP); // generate a CAN message to ask for speed
 *
 * 	500ms:
 * 		mCAN.request(CAN_SPEED); // generate a CAN message to ask for speed
 *
 * 	200ms:
 * 		mCAN.request(CAN_RPM); // generate a CAN message to ask for RPM
 *
 * 	0ms:
 * 		mCAN.process_incoming_messages(); // call if the interrupt has set our flag
 *
 ***************************************************** PULL values *******************************************************/

void sensing::pull_values(){
	// is an update required?
	boolean update_required=false;
	if((Millis.get()-fourty_Hz_timer)>=24){ // 25ms

#ifdef TACHO_SMALLDEBUG
		Serial.puts(USART1,("-s"));
#endif
		fourty_Hz_timer=Millis.get();
		fourty_Hz_counter=(fourty_Hz_counter+1)%40;
		update_required=true;

		if(fourty_Hz_counter==0){ // 1 Hz
			//			m_voltage->calc(false); // spannungscheck	TODO
			mTemperature.read_oil_temp();  // temperaturen aktualisieren
			mClock.inc();  // sekunden hochzählen
			mGPS.valid++;  // vor wievielen sekunden war es das letzte mal gültig
			// auto CAN detection ...
			if(sensor_source==SENSOR_AUTO){
				if(!mCAN.init_comm_possible(&CAN_active)){ // returns always true, exept the communcation was NOT possible even if it should
					Serial.puts_ln(USART1,("=== CAN timed out ==="));
					Serial.puts_ln(USART1,("falling back to analog sensing"));
					mCAN.shutdown();
					mRpm.init();
					mSpeed.init();
					Serial.puts_ln(USART1,("=== CAN timed out ==="));
				}
			};
		}

		if(fourty_Hz_counter%4==0){			//do this, every 10Hz, 100ms
			mFlasher.check();    // blinken wir? TODO
			mGear.calc();// blockt intern alle aufrufe die vor ablauf von 250 ms kommen


			// IIR mit Rückführungsfaktor 3 für Anzeige, 20*4 Pulse, 1400U/min = 2,5 sec | 14000U/min = 0,25 sec
			rpm_flatted=flatIt(get_RPM(RPM_TYPE_DIRECT),&rpm_flatted_counter,2,get_RPM(RPM_TYPE_FLAT));
			//						Aktors.rgb_action(get_RPM(RPM_TYPE_FLAT)); TODO

			//			////////////// TODO
			//			char rpm_buffer[20];
			//			sprintf(rpm_buffer,"%i,%i",get_RPM(RPM_TYPE_DIRECT),get_RPM(RPM_TYPE_FLAT));
			//			Serial.println(rpm_buffer);
			//			////////////// TODO
		}

		if(Aktors.mStepper.init_steps_to_go==0){
			// to this with 40hz, 25ms
			// in addtion to the message above: handling of RPM and aktor
			Aktors.mStepper.go_to(get_RPM(Aktors.mStepper.shown_mode)); //3.10.2013 ran good
		//Aktors.mStepper.go_to(get_RPM(RPM_TYPE_DIRECT));
		}
	}


	/********************* CAN **************************
	 * we have to deciede from where we receive our infomation,
	 * from CAN or from converntianal sensing
	 * infos from CAN-Bus, could be:
	 * - Engine RPM
	 * - Verhicle Speed
	 * - Air intake Temperature
	 * - Coolant Temparture
	 *********************** CAN *************************/
	//	 CAN is present and should be used
	if(CAN_active && !mCAN.failed){
		// fetch new message first
		if(mCAN.message_available){ // muss hier die pin abfrage rein? dafür gibts doch den interrupt
			mCAN.process_incoming_messages();
		}
		if(update_required && mCAN.get_active_can_type()==CAN_TYPE_OBD2 && fourty_Hz_counter%4==0){ // 10Hz, request can OBD2 msg
			if(fourty_Hz_counter==0 || fourty_Hz_counter==2 || fourty_Hz_counter==4 || fourty_Hz_counter==6 || fourty_Hz_counter==8){ // 200ms
				mCAN.request(CAN_CURRENT_INFO,CAN_RPM);
			} else if(fourty_Hz_counter==1 || fourty_Hz_counter==5 ){ // 500ms
				mCAN.request(CAN_CURRENT_INFO,CAN_SPEED);
			} else if(fourty_Hz_counter==7){ // 1000ms
				mCAN.request(CAN_CURRENT_INFO,CAN_WATER_TEMPERATURE);
			} else if(fourty_Hz_counter==9 ){  // 1000ms one free slot
				//mCAN.request(?);
			};
		}
#ifdef TACHO_SMALLDEBUG
		Serial.puts_ln(USART1,("."));
#endif
	}

	/***************** no can available, *********************
	 * ask analog sensing
	 ***************** no can available, *********************/
	else {
		if(update_required){ // 100ms spacing
			// do this, once a second
			if(fourty_Hz_counter==0){
				mTemperature.read_air_temp();  // temperaturen aktualisieren
				mTemperature.read_water_temp();  // temperaturen aktualisieren
			}
#ifdef TACHO_SMALLDEBUG
			Serial.puts_ln(USART1,("."));
#endif
		}
	}
}
/********************************** READ section *************************************/

void sensing::addinfo_show_loop(){
	char *char_buffer;
	char_buffer = (char*)malloc(22);
	////////////////////// water //////////////
	if(get_water_temperature()!=Speedo.disp_zeile_bak[2]){
		Speedo.disp_zeile_bak[2]=get_water_temperature();

		if(get_water_temperature()==8888){
			sprintf(char_buffer," -     "); // error occored -> no sensor
		} else if(get_water_temperature()==9999){
			sprintf(char_buffer," --    "); // error occored -> short to gnd
		} else 	if(get_water_temperature()>1100){
			sprintf(char_buffer,">110{C  "); // more then 110°C add a space to have 5 chars
		} else 	{
			sprintf(char_buffer,"%3i.%i{C",int(floor(get_water_temperature()/10))%1000,get_water_temperature()%10); // _32.3°C  7 stellen
		};
		// depend on skinsettings
		TFT.string(Speedo.default_font,char_buffer,9,2,0,DISP_BRIGHTNESS,-4);
	};

	////////////////////// air //////////////
	if(get_air_temperature()!=Speedo.disp_zeile_bak[3]){

		Speedo.disp_zeile_bak[3]=get_air_temperature();
		sprintf(char_buffer,"%2i.%i{C",int(floor(get_air_temperature()/10))%100,get_air_temperature()%10);
		// depend on skinsettings
		TFT.string(Speedo.default_font,char_buffer,9,3,0,DISP_BRIGHTNESS,2);
	};

	////////////////////// oil //////////////
	if(get_oil_temperature()!=Speedo.disp_zeile_bak[4]){

		Speedo.disp_zeile_bak[4]=get_oil_temperature();

		if(get_oil_temperature()==8888){
			sprintf(char_buffer," -     "); // error occored -> no sensor
		} else if(get_oil_temperature()==9999){
			sprintf(char_buffer," --    "); // error occored -> short to gnd
		} else {
			sprintf(char_buffer,"%3i.%i{C",int(floor(get_oil_temperature()/10))%1000,get_oil_temperature()%10); // _32.3°C  7 stellen
		};
		// depend on skinsettings
		TFT.string(Speedo.default_font,char_buffer,9,4,0,DISP_BRIGHTNESS,-4);
	};

	////////////////////// voltage //////////////
	if(Speedo.disp_zeile_bak[0]!=1){
		TFT.string(Speedo.default_font,("Voltage "),1,7);
		Speedo.disp_zeile_bak[0]=1;
	}
	int voltage=mVoltage.get();
	if(voltage!=Speedo.disp_zeile_bak[1]){
		Speedo.disp_zeile_bak[1]=voltage;
		char temp[6];
		sprintf(temp,"%2i,%iV",int(floor(voltage/100)),int(voltage%100));
		TFT.string(Speedo.default_font,temp,9,7);
	};
	free(char_buffer);
};
//
///********************************** WARN light section *************************************
// * all light depeding infos are processed here
// ********************************** WARN light section *************************************/
//// interrupt to update sensing
//ISR(INT6_vect ){
//	check_inputs();
//}
//// interrupt to update sensing
//ISR(INT7_vect ){
//	check_inputs();
//}
//// interrupt to update sensing
//ISR(PCINT2_vect ){
//	// check if its the right version before test the pin
//	if(pConfig->get_hw_version()==7){
//		if(mCAN.check_message()){
//			// processing is in CAN class
//		} else if(mCAN.get_active_can_type()!=CAN_TYPE_TRIUMPH){ // if version 7, but not triumpf check light, tritumpf is way to heavy traffic
//			check_inputs();
//		}
//	} else if(pConfig->get_hw_version()>7) { // from version higher than 7, CAN Interrupt is not on this interrupt port
//		check_inputs();
//	}
//}
//
////ISR(PCINT0_vect){
////	// check if its the right version before test the pin
////	if(pConfig->get_hw_version()>7){
////		if(!(CAN_INTERRUPT_PIN_PORT_V8&(1<<CAN_INTERRUPT_PIN_FROM_V8))){	 // if the CAN pin is low, low active interrupt
////			if(CAN_active){		 // is the CAN mode active
////				mCAN.message_available=true;
////#ifdef CAN_DEBUG
////				Serial.println("Interrupt: Msg available");
////#endif
////			};
////		};
////	}
////}
//
//void sensing::check_inputs(){
//	unsigned char high_beam=0x00;
//	unsigned char flasher_left=0x00;
//	unsigned char flasher_right=0x00;
//	unsigned char oil_pressure=0x00; // low active
//	unsigned char neutral_gear=0x01; // low active
//
//	if(PINK&(1<<HIGH_BEAM_PIN)){  // pin is high
//		if(mCAN.get_active_can_type()!=CAN_TYPE_TRIUMPH){ // on "non triumph" -> on means on :D
//			high_beam=0x01;
//		} else if(last_highbeam_on==0){ // addition keep off for triumph
//			last_highbeam_on=Millis.get();
//			high_beam=0x00; // not now
//		} else if(Millis.get()-last_highbeam_on>500) { // last active >1sec
//			high_beam=0x01;
//		}
//	} else { // pin is low, High beam is off
//		last_highbeam_on=0;
//		high_beam=0x00; // switch off
//	}
//
//	if(!(PINK&(1<<OIL_PRESSURE_PIN))){  // pin is low, (low active)
//		if(last_oil_off==0){
//			last_oil_off=Millis.get();
//			oil_pressure=0x00; // not now
//		} else if(Millis.get()-last_oil_off>500) { // last active >1sec
//			oil_pressure=0x01;
//		}
//	} else { // pin is high, oil is off
//		last_oil_off=0;
//		oil_pressure=0x00; // switch off
//	}
//
//	// depending on CAN or non CAN mode ... and if CAN on CAN TYPE
//	if(CAN_active && mCAN.get_active_can_type()==CAN_TYPE_TRIUMPH){
//		neutral_gear=mCAN.get_neutral_gear_state();
//		m_gear->set_neutral(neutral_gear);
//	} else {
//		if(PINK&(1<<NEUTRAL_GEAR_PIN)){	 // if the pin is still high, the pulldown is not active, signal is not active
//			neutral_gear=0x00;
//			m_gear->set_neutral(false);
//		} else {
//			neutral_gear=0x01;
//			m_gear->set_neutral(true);
//		}
//	}
//
//	if(PINE&(1<<FLASHER_LEFT_PIN)){
//		flasher_left=0x01;
//		m_blinker->pin_toogled();
//	}
//
//	if(PINE&(1<<FLASHER_RIGHT_PIN)){
//		flasher_right=0x01;
//		m_blinker->pin_toogled();
//	}
//
//	Aktors.set_controll_lights(oil_pressure,flasher_left,neutral_gear,flasher_right,high_beam,false);
//}
///********************************** WARN light section *************************************/

sensing::sensing() {
	// TODO Auto-generated constructor stub
	fourty_Hz_counter=0;
	fourty_Hz_timer=Millis.get();
	CAN_active=true; // don't care, will change it in init
	sensor_source=0;
	last_highbeam_on=0;
	last_oil_off=0;
	rpm_flatted_counter=0;
	last_int_state=0;


	Rpm mRpm;
	flasher mFlasher;
	temperature mTemperature;
	Speed mSpeed;
	oiler mOiler;
}

sensing::~sensing() {
	// TODO Auto-generated destructor stub
}


//// initialize the sensor class, by using the build in init
//// seqence of each sensor
void sensing::init(){
//	mRpm.init();
	mFlasher.init();
	mTemperature.init();
	mClock.init();
	mFuel.init();
	mSpeed.init();
	mReset.init();
	mGear.init();
	mVoltage.init();
	mGPS.init();
	mCAN.init();  // done later in main startup
	//
	//	cli(); //  ... unschön, aber gps macht interrupts an	 TODO
	//	// Blinker LINKS Interrupt
	//	EIMSK |= (1<<INT6); // Enable Interrupt
	//	EICRB |= (1<<ISC60); // any change on INT5
	//
	//	// Blinker RECHTS Interrupt
	//	EIMSK |= (1<<INT7); // Enable Interrupt
	//	EICRB |= (1<<ISC70); // any change on INT5
	//
	//	// Neutral Gear Interrupt
	//	DDRK  &=~(1<<PK1); // ensure its an input
	//	PORTK |=(1<<PK1); // activate Pull UP
	//
	//	// Oil Pressure Interrupt
	//	DDRK  &=~(1<<PK0); // ensure its an input
	//	PORTK |=(1<<PK0); // activate Pull UP
	//	sei();
	//	// High Beam
	//	PCMSK2|=(1<<PCINT18) | (1<<PCINT17) | (1<<PCINT16); //Oil in, High beam in, Neutral gear
	//	PCICR |=(1<<PCIE2); // general interrupt PC aktivieren für SK2
	//
	rpm_flatted_counter=0;
	Serial.puts_ln(USART1,("sensing init done"));
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
float sensing::flatIt(int actual, unsigned char *counter, char max_counter, float old_flat){
	if(*counter==max_counter){
		return (float)((old_flat*(max_counter-1)+actual)/(max_counter));
	} else if(*counter<max_counter && *counter>=0){
		*counter=*counter+1;
		return (float)((old_flat*(*counter-1)+actual)/(*counter));
	} else {
		*counter=1;
		return actual;
	}
	// hier besteht die gefahr das ein messwert nur [INTmax]/max_counter groß sein darf
	// bei 20 Werten also nur 3276,8
}

float sensing::flatIt_shift(int actual, uint8_t *counter, uint8_t shift, float old_flat){
	if(*counter==1<<(shift+1)){ // e.G. shift == 4 (2⁴=16) = (0b10000=16)
		return (float)(old_flat+((int)(actual-old_flat)>>shift)); // e.g. shift = 2, old_flat=1, actual=2, result should be 1.25: (old_flat*3+actual)/4=5/4=1.25 OR BY SHIFT: 1+((2-1)>>2=1+1>>2=1+0.25=1.25
	} else if(*counter<1<<shift && *counter>=0){
		*counter=*counter+1;
		return (float)((old_flat*(*counter-1)+actual)/(*counter));
	} else {
		*counter=1;
		return actual;
	}
	// hier besteht die gefahr das ein messwert nur [INTmax]/max_counter groß sein darf
	// bei 20 Werten also nur 3276,8
}

uint16_t sensing::flatIt_shift_mask(uint16_t actual, uint8_t shift, uint16_t old_flat, uint16_t nmask){
	return old_flat+(((int)(actual-old_flat)>>shift)&~nmask); // e.g. shift = 2, old_flat=1, actual=2, result should be 1.25: (old_flat*3+actual)/4=5/4=1.25 OR BY SHIFT: 1+((2-1)>>2=1+1>>2=1+0.25=1.25
	// hier besteht die gefahr das ein messwert nur [INTmax]/max_counter groß sein darf
	// bei 20 Werten also nur 3276,8
}
