/*
 * voltage.h
 *
 *  Created on: 19.11.2011
 *      Author: jkw
 */

#ifndef VOLTAGE_H_
#define VOLTAGE_H_

class speedo_voltage{
#define VOLTAGE_PIN 2
#define CLOCK_UP_TIME 10
public:
	speedo_voltage();
	~speedo_voltage();
	void init();
	void clear_vars();
	bool check_vars();
	int get();
	void calc();
	bool bat_empty;
private:
	int value;
	unsigned char value_counter;
	unsigned int start_time;
};


#endif /* VOLTAGE_H_ */
