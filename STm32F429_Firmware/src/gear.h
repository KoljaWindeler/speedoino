/*
 * gear.h
 *
 *  Created on: 03.09.2011
 *      Author: kolja
 */

#ifndef GEAR_H_
#define GEAR_H_

class gear{

#define KUPPLUNGS_PIN PB6
#define KUPPLUNGS_PORT PORTB
#define KUPPLUNGS_DIRC DDRB

#define kupplungs_pin 12 // TODO

public:
	gear(void);
	~gear();
	void init();
	int get();
	void set_neutral(bool is_neutral);
	void calc();
	int check_vars();
	void calibrate();

	int16_t n_gang[7];
	int gang;
	bool neutral_set;
	// used to calibration mode && to calc to gear in speedo mode
	float faktor_flat;
	unsigned char faktor_counter;

private:
	unsigned long last_time_executed;
};


#endif /* GEAR_H_ */
