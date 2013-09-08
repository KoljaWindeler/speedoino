/*
 * display.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

////////////// DISPLAY ///////////////////////////////////
#include      <ssd0323.h>
class speedo_disp : public ssd0323 {
#define DISP_BRIGHTNESS 15
#define DIALOG_NO_YES 1
#define DIALOG_GO_LEFT_200MS 2
#define DIALOG_GO_RIGHT_200MS 3
#define DIALOG_GO_LEFT_500MS 4
#define DIALOG_GO_RIGHT_500MS 5
#define DIALOG_GO_LEFT_1000MS 6
#define DIALOG_GO_RIGHT_1000MS 7
#define DIALOG_GO_LEFT_2000MS 8
#define DIALOG_GO_RIGHT_2000MS 9
#define DIALOG_GO_LEFT_5000MS 10
#define DIALOG_GO_RIGHT_5000MS 11
#define DIALOG_SHOW_500MS 12

public:
	unsigned char 	phase;//=0x62;
	unsigned char 	ref;//=0x3F;
	unsigned char startup[35]; // asdfghjk.asd,1234,1234,1234\0 == 28
	void init_speedo();
	speedo_disp(void);
	~speedo_disp();
	void clear_screen();
	void draw_oil(unsigned char x,unsigned char y);
	void draw_water(unsigned char x,unsigned char y);
	void draw_air(unsigned char x,unsigned char y);
	void draw_fuel(unsigned char x,unsigned char y);
	void draw_clock(unsigned char x,unsigned char y);
	void draw_gps(unsigned char x,unsigned char y, unsigned char sats);
	void draw_blitzer(unsigned char x,unsigned char y);
	void draw_arrow(int arrow, int spalte, int zeile);
	int  sd2ssd(char filename[10],int frame);
	void show_animation(unsigned char *command);
	void disp_waiting(int position,unsigned char spalte,unsigned char zeile);
	int animation(int a);
	void show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length);
	void show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length, uint8_t type);
	void show_storry(const char* storry,const char* title);
	void show_storry(const char* storry,const char* title, uint8_t type);
	void string_P_centered(const char* text, uint8_t line);
	void string_P_centered(const char* text, uint8_t line, bool inverted);
private:
	bool reinit_display;

};
extern speedo_disp* pOLED;
////////////// DISPLAY ///////////////////////////////////

#endif /* DISPLAY_H_ */
