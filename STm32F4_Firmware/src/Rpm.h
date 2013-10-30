/*
 * Rpm.h
 *
 *  Created on: 25.10.2013
 *      Author: jkw
 */

#ifndef RPM_H_
#define RPM_H_

class Rpm {
public:
	Rpm();
	virtual ~Rpm();
	void init();
	void overflow();
	void set_exact(int16_t i);
	uint16_t get_exact();
	volatile int16_t overflows;
	volatile uint16_t old_timerValue;
private:
	volatile int16_t exact;
	volatile int16_t e_sum;
	volatile int16_t e_old;
};

#endif /* RPM_H_ */
