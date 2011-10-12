/*
 * temperatur.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

class speedo_temperature{
#define oil_temp_pin 1

public:
	speedo_temperature();
	~speedo_temperature();
	int get_air_temp();
	int get_oil_temp();
	void read_oil_temp();
	void read_air_temp();
	void init();
	int r_werte[19];
	int t_werte[19];

private:

	int max_values;
	float oil_temp_value;
	short int oil_temp_value_counter;
	int air_temp_value;
};

#endif /* TEMPERATURE_H_ */
