/*
 * fuel.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef FUEL_H_
#define FUEL_H_

class speedo_fuel{
private:
	unsigned long last_time;
public:
	speedo_fuel();
	~speedo_fuel();
	float get_fuel(char char_buffer[]);
	void init();
	int check_vars();
	int blink_start; // bei mehr als 140 km auf der Tankuhr fngt die an zu blinken
	int blink_freq; // blinkt mit 500ms Periodenlaenge
};

#endif /* FUEL_H_ */
