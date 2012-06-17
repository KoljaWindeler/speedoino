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
public:
	speedo_voltage();
	~speedo_voltage();
	void init();
	void clear_vars();
	bool check_vars();
	int get();
	void calc();
private:
	int value;
	short int value_counter;
};


#endif /* VOLTAGE_H_ */
