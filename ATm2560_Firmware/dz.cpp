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

speedo_dz::speedo_dz(){
	exact=0;                 // real rotation speed
	blitz_dz=0;
	blitz_en=false;
	dz_faktor_counter=0;
}

unsigned int speedo_dz::get_dz(bool exact_dz){
	return exact;
}

ISR(INT4_vect){
	uint8_t sreg;
	uint16_t timerValue;
	sreg = SREG;		/* Save global interrupt flag */
	cli();				/* Disable interrupts */
	timerValue = TCNT5;	/* Read TCNTn into i */
	TCNT5=0;			/* Reset Timer value */
	SREG = sreg;		/* Restore global interrupt flag */
	// timer runs with 2 MHz
	// if the engine runs with 3.000 rpm we should have 3000/60*2(double ignition)=100 pulses per second
	// that leads to 10ms per Pulse. Timer5 value will be at 20.000 after 10ms.
	pSensors->m_dz->overruns=0;
	pSensors->m_dz->set_exact((uint32_t)60000000UL / (((uint32_t)pSensors->m_dz->overruns<<16) + timerValue)); // division 12 cycles
}

// this overflow will occure after 65536/2000000=0,032768 sec, 0,032768=30/RPM, RPM=30/0,032768=915 min
ISR(TIMER5_OVF_vect){
	uint8_t sreg;
	sreg = SREG;	/* Save global interrupt flag */
	cli();			/* Disable interrupts */
	TCNT5=0;
	SREG = sreg;	/* Restore global interrupt flag */
	pSensors->m_dz->overruns++;
	if(pSensors->m_dz->overruns>4){ // no spark for 164ms -> less than 183 rpm
		pSensors->m_dz->set_exact(0);
	}
}

void speedo_dz::set_exact(uint16_t i){
	if(i<15000){
		exact=i;
	} else {
		exact=15000;
	}
}


void speedo_dz::init() {
	EIMSK |= (1<<INT4); // Enable Interrupt
	EICRB |= (1<<ISC40) | (1<<ISC41); // rising edge on INT4

	// timer
	TCNT5=0x00; // reset value
	TIMSK5=(1<<TOIE5); // <- activate Timer overflow interrupt
	TCCR5B=(0<<CS52) | (1<<CS51) | (0<<CS50); // selects clock to "/8" => 16Mhz/8=2MHz,
	TCCR5A=0; // WGM=0 -> normal mode

	pDebug->sprintlnp(PSTR("DZ init done"));
	blitz_en=false;
	overruns=0;
	Serial3.flush();
};

void speedo_dz::shutdown(){
	EIMSK &= ~(1<<INT4); // DISABLE Interrupt
	EICRB &= ~(1<<ISC40) | (1<<ISC41); // no edge on INT4
};


int speedo_dz::check_vars(){
	if(blitz_dz==0){
		pDebug->sprintp(PSTR("DZ failed"));
		blitz_dz=12500; // hornet maessig
		blitz_en=true; // gehen wir mal von "an" aus
		return 1;
	}
	return 0;
};
