/*
 * speed.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef SPEED_H_
#define SPEED_H_

class speedo_speed{
#define SPEED_PIN PE5
#define SPEED_REED_OK 0
#define SPEED_REED_ERROR 1
public:
	speedo_speed();
	~speedo_speed();
	void calc();
	int getSpeed();
	void init();
	void shutdown();
	int check_vars();
	void check_umfang();
	int get_sat_speed();
	int get_mag_speed();
	int 		  gps_takeover;
	unsigned char status;
	float 		  reifen_umfang;
	// für die kalibrierung
	float flat_value_calibrate_umfang;
private:
	int           reed_speed;  // speichere speed darin, wenn valid_time noch < 1000 ist gib das hier aus
	unsigned long last_time;  // wenn dieser Zeitpunkt über 1000 msec her ist stehen wir
	unsigned long prevent_double_count;
	volatile unsigned int speed_peaks;
	// für die kalibrierung
	unsigned char flat_counter_calibrate_umfang;
};

#endif /* SPEED_H_ */
