/*
 * gear.h
 *
 *  Created on: 03.09.2011
 *      Author: kolja
 */

#ifndef GEAR_H_
#define GEAR_H_

class speedo_gear{
#define kupplungs_pin 12
public:
	speedo_gear(void);
	~speedo_gear();
	void init();
	int get();
	void calc();
	void calibrate();

	int n_gang[7];
	int gang;
	// used to calibration mode && to calc to gear in speedo mode
	float faktor_flat;
	short faktor_counter;
};


#endif /* GEAR_H_ */
