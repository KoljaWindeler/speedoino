/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for speed_cntr.c.
 *
 * - File:               speed_cntr.h
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
 * $RCSfile: speed_cntr.h,v $
 * $Date: 2006/05/08 12:25:58 $
 *****************************************************************************/

#ifndef SPEED_CNTR_H
#define SPEED_CNTR_H

/*! \brief Holding data used by timer interrupt for speed ramp calculation.
 *
 *  Contains data used by timer interrupt to calculate speed profile.
 *  Data is written to it by move(), when stepper motor is moving (timer
 *  interrupt running) data is read/updated when calculating a new step_delay
 */
typedef struct {
  volatile unsigned char dir : 1;
  volatile unsigned char state : 1;
  volatile signed int targetPosition;
  //! Minimum time delay (max speed)
  volatile signed int min_delay;
  //! Peroid of next timer delay. At start this value set the accelration rate.
  volatile unsigned int step_delay;
  volatile signed int position;
  //! Counter used when accelerateing/decelerateing to calculate step_delay.
  volatile signed int accel_steps;
  volatile signed int accel;
} speedRampData;

/*! \Brief Frequency of timer1 in [Hz].
 *
 * Modify this according to frequency used. Because of the prescaler setting,
 * the timer1 frequency is the clock frequency divided by 8.
 */
// Timer/Counter 1 running on 8mhz
#define T1_FREQ 160000 // ? hÃ¤h ?

//! Number of (full)steps per round on stepper motor in use.
#define FSPR 781

#ifdef HALFSTEPS
  #define SPR (FSPR*2)
  #pragma message("[speed_cntr.c] *** Using Halfsteps ***")
#endif
#ifdef FULLSTEPS
  #define SPR FSPR
  #pragma message("[speed_cntr.c] *** Using Fullsteps ***")
#endif
#ifndef HALFSTEPS
  #ifndef FULLSTEPS
    #error FULLSTEPS/HALFSTEPS not defined!
  #endif
#endif

// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr || 2*3.1459/(781*1)=0,008056082
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100 || 0,008056082*160000*100  = 128897,311139565
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // ALPHA*2*10000000000=161121640
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
#define MAX_POS 20000/11.73
#define T1_STOP ~(1<<CS10 | 1<<CS11 | 1<<CS12)	  // 000
#define T1_RUN ((0<<CS12)|(1<<CS11)|(0<<CS10))	  // 010

void speed_cntr_Move(signed int soll_pos, unsigned int accel, unsigned int max_speed);
void speed_cntr_Init_Timer1(void);
unsigned long sqrt_2(unsigned long v);
unsigned int min(unsigned int x, unsigned int y);
int get_stopper();
void set_stopper();
void motor_cal( unsigned int accel, unsigned int speed);


#endif
