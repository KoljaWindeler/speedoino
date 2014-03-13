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
	int get_water_r();
	int get_oil_r();
	void read_oil_r();
	void read_water_r();
	void read_air_temp();
	void init();
	unsigned char water_r_fail_status;
	unsigned char oil_r_fail_status;

private:
	int air_temp_value;

	float oil_r_value;
	unsigned char oil_r_value_counter;

	float water_r_value;
	unsigned char water_r_value_counter;
};

#endif /* TEMPERATURE_H_ */
