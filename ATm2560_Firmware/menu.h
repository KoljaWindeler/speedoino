/*
 * menu.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef MENU_H_
#define MENU_H_
#include "aktors.h"
////////////// menu ///////////////////////
class speedo_menu {

#define	 menu_button_timeout		300                        // wartezeit zwischen 2 steps
#define menu_button_fast_delay		menu_button_timeout*4   // wartezeit bis schneller
#define menu_button_fast_timeout	100                   // wartezeit zwischen 2 steps -> schnell
#define menu_button_links PINJ4 // 26
#define menu_button_unten PINJ5 // 22
#define menu_button_rechts PINJ6 // 28
#define menu_button_oben PINJ3 // 24

public:
	speedo_menu(void);
	~speedo_menu();
	void init();
	void display();
	bool button_test(bool bt_keys_en, bool hw_keys_en);
	void back();
	void draw(const char* const* menu, int entries);
	void yesno(const char first[30],const char second[30],const char third[30]);
	void copy_storagename_to_chararray(int id,char* array);
	void popup(const char *first,const char *second);
	int  center_me(char* input,int legth);
	bool go_left(bool update_twice);
	bool go_right(bool update_twice);
	bool go_up(bool update_twice);
	bool go_down(bool update_twice);

	unsigned long state,old_state;
	short state_helper;
	bool button_rechts_valid;
	bool button_links_valid;
	bool button_oben_valid;
	bool button_unten_valid;
	bool update_display;

private:
	unsigned long button_time;                      // Zeitpunkt des letzen Button kontakt
	unsigned long button_first_push;                // für "halten". Zeitpunkt des ersten Kontakt
	bool just_marker_update;
	void set_buttons(bool left,bool up,bool down,bool right);
	void color_select_menu(int base_state,led_simple *led_from, led_simple *led_to, int *min, int *max, int upper_limit, bool button_state, const char *name, const char *unit, char set_led_mode, bool just_one_line_mode);
	void storage_update_guard(unsigned long* state, unsigned long old_state,bool storage_outdated, bool* updated_display);
};
extern speedo_menu* pMenu;
////////////// menu ///////////////////////
#endif /* MENU_H_ */
