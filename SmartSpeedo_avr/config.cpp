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
configuration::configuration(){
	hw_version=0x00;
};
configuration::~configuration(){};


int configuration::get_hw_version(){
	if(hw_version!=0x00){
		return (int)hw_version;
	}
	// HW: 6 + (int)(not( 0x00 | (PC0<<2) | (PG1<<1) | (PG0<<0)))
	int return_value=0;

	// PG0 -> Bit0
	// PG1 -> Bit1
	// PC0 -> Bit2

	// set all to input
	// Pin 3 und 4 auf Eingang und andere im ursprünglichen Zustand belassen:
	// DDRB &= ~((1 << DDB3) | (1 << DDB4));
	// Pin 0 und 3 wieder auf Ausgang und andere im ursprünglichen Zustand belassen:
	// DDRB |= (1 << DDB0) | (1 << DDB3);
	DDRG &= ~((1<<DDG0) | (1<<DDG1));
	DDRC &= ~((1<<DDC0));

	// set pull up to all inputs
	// PORTC |= (1<<PC7);    /* internen Pull-Up an PC7 aktivieren */
	PORTG |= ((1<<PG1) | (1<<PG0));
	PORTC |= ((1<<PC0));
	// read inputs
	return_value= 6 + (unsigned char)(not(PING & (1<<PG0))<<0 | not(PING & (1<<PG1))<<1 | not(PINC & (1<<PC0))<<2);

	// disable all PULLUPS
	PORTG &= ~((1<<PG1) | (1<<PG0));
	PORTC &= ~((1<<PC0));

	// return that value
	hw_version=(char)return_value;
	return return_value;
}

void configuration::ram_info() {
	int size = 8192; // Use 2048 with ATmega328
	byte *buf;
	while ((buf = (byte *) malloc(--size)) == NULL);
	free(buf);
	Serial.print(size);

	pDebug->sprintlnp(PSTR(" Byte heap free"));
}


