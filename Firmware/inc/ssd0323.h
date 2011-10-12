/*
  ssd0323.h - Arduino library support for ssd0323 and compatable graphic LCDs   
*/

#include <inttypes.h>
typedef uint8_t boolean;
typedef uint8_t byte;
#include <avr/pgmspace.h>

#ifndef	ssd0323_H // include once
#define ssd0323_H

	// Chip specific includes
	#include "ssd0323_mega.h"  // include this for the Arduino Mega other ATmega2560 boards

	// Colors
	#define BLACK				0x00
	#define WHITE				0xFF

	// Font
	#define STD_SMALL_1X_FONT		1
	#define STD_SMALL_2X_FONT		2
	#define STD_SMALL_3X_FONT		3
	#define STD_SMALL_4X_FONT		4
	#define STD_MEDIUM_1X_FONT		5
	#define STD_MEDIUM_2X_FONT		6
	#define STD_BIG_1X_FONT			7
	#define SANS_SMALL_1X_FONT		8
	#define SANS_SMALL_2X_FONT		9
	#define SANS_SMALL_3X_FONT		10
	#define SANS_SMALL_4X_FONT		11
	#define SANS_MEDIUM_1X_FONT		12
	#define SANS_MEDIUM_2X_FONT		13
	#define SANS_BIG_1X_FONT		14
	#define VISITOR_SMALL_1X_FONT	15
	#define VISITOR_SMALL_2X_FONT	16
	#define VISITOR_SMALL_3X_FONT	17
	#define VISITOR_SMALL_4X_FONT	18


	class ssd0323  // shell class for ssd0323 glcd code
	{
	  private:
		void senden_spi(unsigned char zeichen);
	  public:
			ssd0323();
		// Control functions
		void init(unsigned char phase,unsigned char ref);
		void clear_screen();
		void send_char(unsigned char zeichen);
		void send_command(unsigned char theCommand);
		// Graphic Functions
		void set2pixels(unsigned char x, unsigned char y,unsigned char first,unsigned char second);
		void filled_rect(unsigned char x,unsigned char y,unsigned char width,unsigned char height,unsigned char color);
		void highlight_bar(unsigned char x,unsigned char y,unsigned char width,unsigned char height);
		void line(unsigned char x,unsigned char y,unsigned char x2,unsigned char y2,unsigned char color);
		// Font Functions
		void string(short font,char str[],unsigned char spalte, unsigned char zeile);
		void string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile);
		void string(short font,char str[],unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset);
		void string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset);


		void zeichen_small_1x(uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_small_2x(uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_small_3x(uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_small_4x(uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_medium_1x(uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // warte auf schrift
		void zeichen_medium_2x(uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // warte auf schrift
		void zeichen_big_1x(uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // fertig
	};

//	extern ssd0323 OLED;
#endif

