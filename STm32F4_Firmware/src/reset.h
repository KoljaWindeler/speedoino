/*
 * reset.h
 *
 *  Created on: 02.06.2011
 *      Author: kolja
 */

#ifndef RESET_H_
#define RESET_H_
class reset{
#define reset_toogle_pin 6
#define rst_blink_freq 330
public:
	reset(void);
	~reset();
	void init();
	int check_vars();
	void set_active(bool save_to_eeprom,bool save_to_var);
	void set_deactive(bool save_to_eeprom,bool save_to_var);
	void restore();
	void toggle();
	void ask_reset();

	int last_reset;
	unsigned long last_time;
	bool toggle_high;
	bool reset_enabled;
	int reboots_caused_by_sd_problems;
};

#endif /* RESET_H_ */
