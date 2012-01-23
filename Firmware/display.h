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
public:
	unsigned char 	phase;//=0x62;
	unsigned char 	ref;//=0x3F;
	bool       		disp_invert;
	unsigned long 	disp_last_invert;
	char startup[200];
	void init_speedo();
	speedo_disp(void);
	~speedo_disp();
	void draw_oil(unsigned char x,unsigned char y);
	void draw_air(unsigned char x,unsigned char y);
	void draw_fuel(unsigned char x,unsigned char y);
	void draw_clock(unsigned char x,unsigned char y);
	void draw_blitzer(unsigned char x,unsigned char y);
	void draw_arrow(int arrow, int spalte, int zeile);
	int  sd2ssd(char filename[10],int frame);
	void show_animation(const char command[]);
	void disp_waiting(int position,unsigned char spalte,unsigned char zeile);
	void animation(int a);
	void show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length);
};
extern speedo_disp* pOLED;
////////////// DISPLAY ///////////////////////////////////

#endif /* DISPLAY_H_ */
