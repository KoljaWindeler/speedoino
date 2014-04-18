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
/* timer usage by the speedoino codebase
 * Timer0 -  8 Bit - Used by the Arduino Base to calc the millis() timestamp
 * Timer1 - 16 Bit - Used by the RPM capturing module, normal mode, 2MHz
 * Timer2 -  8 Bit - Used by the Arduino Base to generate Phase correct PWM, 250kHz, Pins: PH6 (outer RGB), PB4 (not used)
 * Timer3 - 16 Bit - Used by the Speed module
 * Timer4 - 16 Bit - Used by the Arduino Base to generate Phase correct PWM, 250kHz, Pins: PH3 (not used), PH4..5 (outer RGB)
 * Timer5 - 16 Bit - not in use now, could be used by the correct Wheel speed detection, OCM on "pulse per rotation" and MATCH ISR on trigger
 */


speedo_dz::speedo_dz(){
	blitz_en=false;
	last_calc_pulse_ts=0;
	last_time_read=0;
}

uint16_t speedo_dz::get_dz(){
	if(millis()-last_time_read>200){ // update this with max 5hz, 200ms
		uint16_t timerValue=0;
		uint8_t sreg = SREG;		/* Save global interrupt flag */
		cli();				/* Disable interrupts */
		if(pulse_count>0){
			timerValue = pulse_count;
			pulse_count=0;
		}
		SREG = sreg;		/* Restore global interrupt flag */
		if(timerValue>1){
			int32_t differ=last_pulse_ts-last_calc_pulse_ts; // value around 2000ms to 100ms
			if(differ<0){
				differ=((uint32_t)-1)-differ;
			}
			last_calc_pulse_ts=last_pulse_ts;
			if(timerValue>=(1<<8)){ // if timer > 256
				analog_dz=(((uint32_t)timerValue)<<16)/(differ>>7); // this could be 2 to 240
			} else {
				analog_dz=(((uint32_t)timerValue)<<23)/(differ); // this could be 2 to 240
			}

//			Serial.print(differ);
//			Serial.print(" | ");
//			Serial.print(timerValue);
//			Serial.print(" | ");
//			Serial.print(analog_dz);
//			Serial.print(" | ");
//			Serial.print(analog_dz/(8.192*1.0225));
//			Serial.print(" | ");
//			Serial.println(analog_dz*3.581525061);

			analog_dz*=3.581525061;
			last_time_read=millis();
		} else {
			// if we haven't even received a pulse in one sec than we are driving
			// reed sensor: less then 2m in one sec, less than 7.2 km/h
			// 8 pulses: less then 2m/8 in one sec, less than 0,9 km/h
			// hornet:  less then 2m/64 in one sec, less than 0,1125 km/h
			if(millis()-last_time_read>1000 && timerValue==0){
				analog_dz=0;
				last_time_read=millis();
			}
		}
	}
	return analog_dz;
}

ISR(INT4_vect){
		pSensors->m_dz->last_pulse_ts = micros();
		pSensors->m_dz->pulse_count++;
};


void speedo_dz::init() {
	EIMSK |= (1<<INT4); // Enable Interrupt
	EICRB |= (1<<ISC41)|(0<<ISC40); // falling edge on INT4 <- rising edge on RPM pin

	pDebug->sprintlnp(PSTR("DZ init done"));
	blitz_en=false;
};

void speedo_dz::shutdown(){
	EIMSK &= ~(1<<INT4); // DISABLE Interrupt
	EICRB &= ~(1<<ISC40) | (1<<ISC41); // no edge on INT4
};


