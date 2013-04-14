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
#define CLOCK_UP_TIME 5
public:
	speedo_voltage();
	~speedo_voltage();
	void init();
	int check_vars();
	int get();
	void calc(bool first_start);
	bool bat_empty;
private:
	int value;
	unsigned char value_counter;
	unsigned int start_time;
	void keep_me_alive(bool active);
};


#endif /* VOLTAGE_H_ */
