/*
  ssd0323.h - Arduino library support for ssd0323 and compatable graphic LCDs   
 */

#include <inttypes.h>
typedef uint8_t boolean;
typedef uint8_t byte;

#ifndef	ssd0323_H // include once
#define ssd0323_H

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

// Font
#define STD_SMALL_1X_FONT		1
#define STD_SMALL_2X_FONT		2
#define STD_SMALL_3X_FONT		3
#define STD_SMALL_4X_FONT		4
#define SANS_SMALL_1X_FONT		5
#define SANS_SMALL_2X_FONT		6
#define SANS_SMALL_3X_FONT		7
#define SANS_SMALL_4X_FONT		8
#define SANS_BIG_1X_FONT		9
#define VISITOR_SMALL_1X_FONT	10
#define VISITOR_SMALL_2X_FONT	11
#define VISITOR_SMALL_3X_FONT	12
#define VISITOR_SMALL_4X_FONT	13
#define VISITOR_SMALL_5X_FONT	14
#define VISITOR_SMALL_6X_FONT	15
#define VISITOR_SMALL_7X_FONT	16
#define VISITOR_SMALL_8X_FONT	17


class ssd0323  // shell class for ssd0323 glcd code
{
private:
	void senden_spi(unsigned char zeichen);
public:
	ssd0323();
	// Control functions
	void init(unsigned char phase,unsigned char ref);
	void send_char(unsigned char zeichen);
	void send_command(unsigned char theCommand);
	// Graphic Functions
	void clear_screen();
	void set2pixels(unsigned char x, unsigned char y,unsigned char first,unsigned char second);
	void filled_rect(unsigned char x,unsigned char y,unsigned char width,unsigned char height,unsigned char color);
	void highlight_bar(unsigned char x,unsigned char y,unsigned char width,unsigned char height);
	void line(unsigned char x,unsigned char y,unsigned char x2,unsigned char y2,unsigned char color);
	// Font Functions
	void string(const char str[],unsigned char spalte, unsigned char zeile);
	void string(short font,const char str[],unsigned char spalte, unsigned char zeile);
	//		void string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile);
	void string(short font,const char str[],unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset);
	//		void string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset);


	void zeichen_small_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
	void zeichen_small_2x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
	void zeichen_small_3x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
	void zeichen_small_4x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
	void zeichen_medium_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // warte auf schrift
	void zeichen_medium_2x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // warte auf schrift
	void zeichen_big_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // fertig

	void start_up();
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
	void string_centered(const char* text, uint8_t line);
	void string_centered(const char* text, uint8_t line, bool inverted);

	unsigned char     phase;//=0x62;
	unsigned char     ref;//=0x3F;
	unsigned char startup[35]; // asdfghjk.asd,1234,1234,1234\0 == 28


private:
	bool reinit_display;
};

//extern ssd0323 OLED;
#endif

