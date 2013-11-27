/*
 * temperatur.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

class temperature{

/* Configure PB0 (ADC Channel ADC12_IN8) as analog input */
/* ADC12_IN8 means channel 8 for ADC1 OR ADC2 */
#define OIL_TEMP_PIN GPIO_Pin_0
#define OIL_TEMP_PORT GPIOB


#define WATER_TEMP_PIN 0

public:
	temperature();
	~temperature();
	int get_air_temp();
	int get_water_temp();
	int get_oil_temp();
	void read_oil_temp();
	void read_water_temp();
	void read_air_temp();
	int check_vars();
	void init();
	int16_t oil_r_werte[19];
	char oil_t_werte[19];
	int16_t water_r_werte[19];
	char water_t_werte[19];
	int16_t	oil_warning_temp;
	int16_t water_warning_temp;
	unsigned char water_temp_fail_status;
	unsigned char oil_temp_fail_status;

private:
	int air_temp_value;

	float oil_temp_value;
	unsigned char oil_temp_value_counter;

	float water_temp_value;
	unsigned char water_temp_value_counter;
};

#endif /* TEMPERATURE_H_ */
