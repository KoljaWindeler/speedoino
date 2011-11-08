/*
 * dz.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef DZ_H_
#define DZ_H_

#define RGB_OUT_R 7
#define RGB_OUT_G 8
#define RGB_OUT_B 9
#define RGB_IN_R 13
#define RGB_IN_G 10
#define RGB_IN_B 11
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
	bool blitz_en;
private:
	volatile unsigned int peak_count; // max 64k => bei 15krpm sind das 256 sek ... sollte reichen
	unsigned long previous_time;
	unsigned int previous_peaks;		// z�hlerstand vom peak
	unsigned int previous_dz;
	volatile unsigned int peak_helper;

};
/**************** DZ *******************/

#endif /* DZ_H_ */
