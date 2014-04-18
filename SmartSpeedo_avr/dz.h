/*
 * dz.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef DZ_H_
#define DZ_H_

#define FAST_SPEED 500
#define FAST_ACCEL 900
#define SLOW_ACCEL 600


/**************** DZ *******************/
void helper();
class speedo_dz{
public:
	speedo_dz(void);
	~speedo_dz(void);
	void counter();
	void shutdown();
	void init();
	uint16_t get_dz();
	void set_exact(uint16_t i);

	bool blitz_en;

	volatile uint32_t last_pulse_ts;
	volatile uint16_t pulse_count;

private:
	uint32_t last_time_read;
	uint32_t last_calc_pulse_ts;
	volatile uint16_t analog_dz;
};
/**************** DZ *******************/

#endif /* DZ_H_ */
