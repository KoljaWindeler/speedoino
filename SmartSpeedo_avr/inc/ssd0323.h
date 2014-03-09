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
	/*********************************************************/
	/*  Configuration for assigning LCD bits to Arduino Pins */
	/*********************************************************/
	/* Arduino pins used for Commands - these must be within the range of 23-29
	 */

	#define SPI_RESET			32		// D/I Bit
	#define SPI_CD				30		// von 34 -> 30
	#define SPI_CS				34		// D/I 30 -> 34
	#define SPI_DATA			36		// EN Bit --> 36 -> 35
	#define SPI_CLK				35		// EN Bit --> 35 -> 36

	/*******************************************************/
	/*     end of Arduino configuration                    */
	/*******************************************************/

	// macros to fast write data to pins 31-36
	#define fastWriteHigh(_pin) (PORTC |= 1   << ((7-(_pin -30)) & 0x07))
	#define fastWriteLow(_pin)  (PORTC &= ~(1 << ((7-(_pin -30)) & 0x07)))

	// Colors
	#define BLACK				0x00
	#define WHITE				0xFF

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
		void set2pixels(unsigned char x, unsigned char y,unsigned char first,unsigned char second);
		void filled_rect(unsigned char x,unsigned char y,unsigned char width,unsigned char height,unsigned char color);
		void highlight_bar(unsigned char x,unsigned char y,unsigned char width,unsigned char height);
		void line(unsigned char x,unsigned char y,unsigned char x2,unsigned char y2,unsigned char color);
		// Font Functions
		void string(short font,const char str[],unsigned char spalte, unsigned char zeile);
		void string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile);
		void string(short font,const char str[],unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset);
		void string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset);


		void zeichen_small_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_small_2x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_small_3x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_small_4x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset); // fertig
		void zeichen_medium_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // warte auf schrift
		void zeichen_medium_2x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // warte auf schrift
		void zeichen_big_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset);  // fertig
	};

//	extern ssd0323 OLED;
#endif

