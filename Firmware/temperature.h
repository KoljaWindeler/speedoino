/*
 * temperatur.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

class speedo_temperature{
#define OIL_TEMP_PIN 1
#define WATER_TEMP_PIN 0

public:
	speedo_temperature();
	~speedo_temperature();
	int get_air_temp();
	int get_water_temp();
	int get_oil_temp();
	void read_oil_temp();
	void read_water_temp();
	void read_air_temp();
	void init();
	int oil_r_werte[19];
	int oil_t_werte[19];
	int water_r_werte[19];
	int water_t_werte[19];

private:

	int max_values;
	float oil_temp_value;
	short int oil_temp_value_counter;
	float water_temp_value;
	short int water_temp_value_counter;
	int air_temp_value;
};

#endif /* TEMPERATURE_H_ */
