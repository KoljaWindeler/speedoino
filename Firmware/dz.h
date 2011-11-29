/*
 * dz.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef DZ_H_
#define DZ_H_

/**************** DZ *******************/
void helper();
class speedo_dz{
public:
	speedo_dz(void);
	~speedo_dz(void);
	void counter();
	void calc();
	void init();

	unsigned int rounded;                 // to show on display, rounded by 50
	unsigned int exact;                 // real rotation speed
	int blitz_dz;
	bool blitz_en;
private:
	volatile unsigned int peak_count; // max 64k => bei 15krpm sind das 256 sek ... sollte reichen
	unsigned long previous_time;
	unsigned int previous_peaks;		// z�hlerstand vom peak
	unsigned int previous_dz;
	volatile unsigned int peak_helper;
	bool hme_light_active;
};
/**************** DZ *******************/

#endif /* DZ_H_ */
