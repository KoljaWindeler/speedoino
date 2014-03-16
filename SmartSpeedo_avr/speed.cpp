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

/* estimate number of pulses
 * given that there are x pulses per wheel rotation (ca 2m), at high speed (250km/h = 70m/s) we'll have
 * 70[m/s]/2[m/rotation]*x[pulse/rotation]=35*x[pulse/s]
 *
 * :: 250km/h ::
 * for a reed: 35 Hz
 * 8 pulses (T3): 280 Hz
 * 64 Pulses (Hornet): 2240 Hz (<- 8 Bit timer + overflow char?)
 *
 * :: 10 km/h ::
 * Reed: 1.38 Hz
 * 8 Pulses: 11.11 Hz
 * 64 Pulses: 89 Hz
 *
 * Possible ways to count:
 *
 * 1. On every Peak well jump in the interrupt and substact the time between two pulses ..
 * Pro: Easy to calclate, slow speed is very precise
 * Contra: very un-precise if time between pulses is short (<100ms), lots of interrupts!
 *
 * 2. Let a time run, grab timer value on interrupt
 * Pro: done in RPM calculation, working stable
 * Contra: lots of interrupts!
 *
 * 3. count peaks with a timer, ask on a fixed time base for values
 * Pro: very well suited for high pulse speed
 * contra: might be slow?
 * -> Read every 500ms + store millis of last read + only read if at least two pulses captured
 * 			Szenario 2hz:
 * 				Read at time 0: 0 pulses
 * 				Pulse at time 500ms
 * 				Read at time 600ms, not enought pulses
 * 				Pulse at time 1000ms
 * 				Read at 1200: two pulses -> 600ms per pulse ... grrr, reset counter
 * 				Pulse at 1500ms,2000ms
 * 				Read at 2400 -> 600ms/Pulse ... grrr
 * 				Pulse at 2500,3000
 * 				Read at 3000 -> 250ms/Pulse ... grrr
 *
 *			Szenario 2hz, read every 100ms
 * 				Worst Case: pulse at 1,501,1001,1501,2001
 * 							read at 0,100..600 -> 600/2 -> 300
 * 							read at 1600 -> 500
 * 							read at 2600 -> 500
 *
 */

speedo_speed::speedo_speed(){
	last_time_read=millis();
	reed_speed=0;
};

speedo_speed::~speedo_speed(){
};

void speedo_speed::init (){
	// timer
	TCNT3=0x00; // reset value
	TIMSK3=(0<<TOIE3); // <- deactivate Timer overflow interrupt
	TCCR3B=(1<<CS32) | (1<<CS31) | (1<<CS30); // selects external clock on rising edge
	TCCR3A=0; // WGM=0 -> normal mode
	last_time_read=millis(); //prevent calculation of rpm

	pDebug->sprintlnp(PSTR("Speed init done"));
};

void speedo_speed::shutdown(){
	TCCR3B=(0<<CS32) | (0<<CS31) | (0<<CS30); // off
}


// für die gang berechnung ist der speed am magnet geiler als der vom gps weil beim beschleunigen sonst fehler kommen
int speedo_speed::get_mag_speed(){
	if(millis()-last_time_read>100){
		uint8_t sreg;
		uint16_t timerValue=0;
		sreg = SREG;		/* Save global interrupt flag */
		cli();				/* Disable interrupts */
		if(TCNT3>1){
			timerValue = TCNT3;	/* Read TCNTn into i */
			TCNT3=0;			/* Reset Timer value */
		}
		SREG = sreg;		/* Restore global interrupt flag */
		if(timerValue>1){
			unsigned long differ=millis()-last_time_read; // value around 2000ms to 100ms
			reed_speed=(((unsigned long)timerValue)<<11)/differ; // this could be 2 to 240

			// ticks per time:
			// reed low speed: 2/2000, hornet high speed: 240/100
			// scale by 2048 -> 2 .. 2400 [ticks per 2.048sec]
			// to be recalculated by: value*[way/tick]/(2,048*3,6)

			// one digit represets 1*[way/tick]/(2,048*3,6)= [Hornet]: 1[Tick]* 2[m/Tick] / 7,3728 = 0,27 km/h
			// one digit represets 1*[way/tick]/(2,048*3,6)= [T3]: 1[Tick]* 0,25[m/Tick] / 7,3728 = 0,016875 km/h

			last_time_read=millis();
		} else {
			reed_speed=0;
		}
	}
	return reed_speed;
};


