/*
 * Rpm.h
 *
 *  Created on: 25.10.2013
 *      Author: jkw
 */

#ifndef RPM_H_
#define RPM_H_

class Rpm {

#define FAST_SPEED 500
#define FAST_ACCEL 900
#define SLOW_ACCEL 600


public:
	Rpm();
	virtual ~Rpm();
	void init();
	void shutdown();
	uint16_t check_vars();
	void overflow();
	void set_exact(int16_t i);
	uint16_t get_exact();
	volatile int16_t overflows;
	volatile uint16_t old_timerValue;
	bool blitz_en;
	uint16_t blitz_dz;
private:
	volatile int16_t exact;
	volatile int16_t e_sum;
	volatile int16_t e_old;
};

#endif /* RPM_H_ */
