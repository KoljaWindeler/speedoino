/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for global variables and defines.
 *
 * - File:               global.h
 * - Compiler:           IAR EWAAVR 4.11A
 * - Supported devices:  All devices with a 16 bit timer can be used.
 *                       The example is written for ATmega48
 * - AppNote:            AVR446 - Linear speed control of stepper motor
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name: RELEASE_1_0 $
 * $Revision: 1.2 $
 * $RCSfile: global.h,v $
 * $Date: 2006/05/08 12:25:58 $
 *****************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#define TRUE 1
#define FALSE 0

/*! \brief Status flags
 */
struct GLOBAL_FLAGS {
  //! True when stepper motor is running.
  unsigned char running:1;
  //! True when uart has received a string (ended with '/r').
  unsigned char cmd:1;
  //! Dummy bits to fill up a byte.
  unsigned char dummy:6;
};

#endif
