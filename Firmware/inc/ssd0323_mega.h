/*
  ssd0323_Arduino.h - Arduino library support for ssd0323 and compatable graphic LCDs
  Copyright (c)2008 Michael Margolis All right reserved

  This is the configuration file for mapping Arduino (ATmega168) pins to the ssd0323 Graphics LCD library

*/

#ifndef	ssd0323_CONFIG_H
#define ssd0323_CONFIG_H

/*********************************************************/
/*  Configuration for assigning LCD bits to Arduino Pins */
/*********************************************************/
/* Arduino pins used for Commands - these must be within the range of 23-29
 */

#define SPI_RESET			32		// D/I Bit
#define SPI_CD				34		// von 34 auf 37 geändert ... 34 für platine, 37 für defektes Board
#define SPI_POWER			33		// hmm brauch ich das? 
#define SPI_GND				31		// nötig!!
#define SPI_CS				30		// D/I Bit 
#define SPI_DATA			36		// EN Bit
#define SPI_CLK				35		// EN Bit

/*******************************************************/
/*     end of Arduino configuration                    */
/*******************************************************/

// macros to fast write data to pins 31-36
#define fastWriteHigh(_pin) (PORTC |= 1   << ((7-(_pin -30)) & 0x07)) 
#define fastWriteLow(_pin)  (PORTC &= ~(1 << ((7-(_pin -30)) & 0x07)))

#endif
