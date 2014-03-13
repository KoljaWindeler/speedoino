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

#define MENU_SPRINT 2

#define M_LAP_T 3
//#define SM_LAP_T_


public:
	speedo_menu(void);
	~speedo_menu();
	void init();
	bool button_test();
	bool go_left(bool update_twice);
	bool go_right(bool update_twice);
	bool go_up(bool update_twice);
	bool go_down(bool update_twice);

private:
	unsigned long button_time;                      // Zeitpunkt des letzen Button kontakt
	unsigned long button_first_push;                // für "halten". Zeitpunkt des ersten Kontakt
};
extern speedo_menu* pMenu;
////////////// menu ///////////////////////
#endif /* MENU_H_ */
