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
class menu {

#define	menu_button_timeout		300                        // wartezeit zwischen 2 steps
#define menu_button_fast_delay		menu_button_timeout*4   // wartezeit bis schneller
#define menu_button_fast_timeout	100                   // wartezeit zwischen 2 steps -> schnell
#define menu_button_links PINJ4 // 26
#define menu_button_unten PINJ5 // 22
#define menu_button_rechts PINJ6 // 28
#define menu_button_oben PINJ3 // 24

#define MENU_SPRINT 2

#define M_LAP_T 3
//#define SM_LAP_T_

#define M_TOUR_ASSISTS 4
#define SM_TOUR_ASSISTS_ON_OFF 1
#define SM_TOUR_ASSISTS_POINTER 2
#define SM_TOUR_ASSISTS_FILE 3
#define SM_TOUR_ASSISTS_POI_FINDER 4
#define SM_TOUR_ASSISTS_WRITEN_POINTS_CHECK 7
#define SM_TOUR_ASSISTS_SPEEDCAM_STATUS 8
#define SM_TOUR_ASSISTS_SPEEDCAM_ON_OFF 9



public:
	menu(void);
	~menu();
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
	void set_buttons(bool left,bool up,bool down,bool right);

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
	void color_select_menu(unsigned long base_state,led_simple *led_from, led_simple *led_to, int16_t *min, int16_t *max, int upper_limit, bool button_state, const char *name, const char *unit, char set_led_mode, bool just_one_line_mode);
	void storage_update_guard(unsigned long* state, unsigned long old_state,bool storage_outdated, bool* updated_display);
	void set_value_dialog(int8_t* value,const char* title,const char* opt0,const char* opt1,const char* opt2,const char* opt3);
	void set_value_dialog(int8_t* value,const char* title,const char* opt0,const char* opt1,const char* opt2);
	void set_value_dialog(int8_t* value,const char* title,const char* opt0,const char* opt1);
	void set_value_dialog(int8_t* value,const char* title);
};
extern menu Menu;
////////////// menu ///////////////////////
#endif /* MENU_H_ */
