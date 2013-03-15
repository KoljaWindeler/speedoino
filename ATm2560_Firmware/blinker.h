/*
 * blinker.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef BLINKER_H_
#define BLINKER_H_
//////////////// blinker //////////////////////////////
class moped_blinker{
#define BLINKER_PIN 5
#define BLINKER_ENABLED true    // show blinker on warning
public:
	moped_blinker(void);
	~moped_blinker();
	void init();
	int check_vars();
	void pin_toogled();
	void check();
	bool warn(unsigned long dist);
	int dist_to_warn;
	int high_speed_add;
private:
	void set_start(unsigned long dist,int kmh);
	bool      		flasher_active;  // wir sind der meinung der blinker ist an/aus
	unsigned long 	start; // bei welchem meter stand wurde der blinker angeschmissen
	unsigned long 	last_toggle_time;
};
//////////////// blinker //////////////////////////////

#endif /* BLINKER_H_ */
