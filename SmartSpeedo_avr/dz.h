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
	int check_vars();
	uint16_t get_dz();
	void set_exact(uint16_t i);

	unsigned char dz_faktor_counter;
	int blitz_dz;
	bool blitz_en;

	volatile uint8_t overruns;
private:
	volatile int16_t exact;                 // real rotation speed
	int16_t e_sum;
	int16_t e_old;
};
/**************** DZ *******************/

#endif /* DZ_H_ */
