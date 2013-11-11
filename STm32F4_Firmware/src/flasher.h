/*
 * flasher.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef flasher_H_
#define flasher_H_
//////////////// flasher //////////////////////////////
class flasher{
#define flasher_PIN 5
#define FLASHER_ENABLED true    // show flasher on warning
public:
	flasher(void);
	~flasher();
	void init();
	int check_vars();
	void pin_toogled();
	void check();
	bool warn(unsigned long dist);
	int16_t dist_to_warn;
	int16_t high_speed_add;
private:
	void set_start(unsigned long dist,int kmh);
	bool      		flasher_active;  // wir sind der meinung der flasher ist an/aus
	unsigned long 	start; // bei welchem meter stand wurde der flasher angeschmissen
	unsigned long 	last_toggle_time;
};
//////////////// flasher //////////////////////////////

#endif /* flasher_H_ */
