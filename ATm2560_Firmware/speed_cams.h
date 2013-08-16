/*
 * SpeedCams.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef SPEEDCAMS_H_
#define SPEEDCAMS_H_

class speedo_speedcams{

public:
	speedo_speedcams();
	void test();
	int test2();
	void calc_gps_goodies();

private:
	unsigned long gps_lati_dec_deg, gps_long_dec_deg;
	double gps_lati_cos;

};
extern speedo_speedcams* pSpeedCams;

#endif /* SPEEDCAMS_H_ */
