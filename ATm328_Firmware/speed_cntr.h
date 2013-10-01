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
	volatile unsigned int max_step_delay;
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
// Timer/Counter 1 running on 2mhz
#define T1_FREQ 2000000 // Hz

#define M6403

#if !defined(M6405)
	#if !defined(M6403)
		#error "Ein Motor braucht das Volk"
	#endif
#endif
// Number of (full)steps per round on stepper motor in use.
#define ROTOR_POL_PAIR 5
#if defined(M6403)
#define GEAR_RATIO 43.2
#elif defined(M6405)
#define GEAR_RATIO 36
#endif
#define FRRPR GEAR_RATIO*ROTOR_POL_PAIR*4 // Full steps per Needle Rotation
// 4 Fullsteps to rotate an "easy rotor" by 360 deg
// ROTOR_POL_PAIR*4 Fullsteps to rotate an motor with ROTOR_POL_PAIR pol pairs
// GEAR_RATIO*ROTOR_POL_PAIR*4 Fullsteps to rotate THE NEEDLE with the given gear ratio of GEAR_RATIO once 360deg

// The rotor consists of 5 PolPairs, using Fullstep mode it takes 20 "STEPS" to rotate the Rotor once
// the rotation is translated by the gearbox with a ratio of 1:43.2 so it takes 862 FULLsteps
//( each 90deg of rotor) to rotate the needle 360deg [SPR]
#ifdef HALFSTEPS
#define SPR (FRRPR*2)
#pragma message("[speed_cntr.c] *** Using Halfsteps ***")
#endif
#ifdef FULLSTEPS
#define SPR (FRRPR)
#pragma message("[speed_cntr.c] *** Using Fullsteps ***")
#endif


// motor max speed is (Datasheet) 600deg/sec [MOTOR_MAX_DEG_PER_SEC]
// => 862FS/360deg*600deg/sec = 1436 FS/sec [MOTOR_MAX_FS_PER_SEC]
// => 696us/fullstep [MOTOR_MIN_TIME_PER_FSTEP_US]
#define MOTOR_MAX_DEG_PER_SEC 600
#define MOTOR_MAX_FS_PER_SEC SPR/360*MOTOR_MAX_DEG_PER_SEC
#define MOTOR_MIN_TIME_PER_FSTEP_US 1000000/MOTOR_MAX_FS_PER_SEC

// our timer is running with T1_FREQ: (2.000.000 [timer_inc/sec]) * MOTOR_MIN_TIME_PER_FSTEP_US/1000000 [sec] = MIN_TIMER_VALUE
// MOTOR_MIN_TIME_PER_FSTEP_US * T1_FREQ < MIN_TIMER_VALUE
// 0,000696 sec/fullstep * 2.000.000 timer_inc/sec = 1392 timer_inc/fullstep
#define TIMER_MIN_VALUE MOTOR_MIN_TIME_PER_FSTEP_US*T1_FREQ/1000000

// the min delay is calculated by A_T_x100/speed
// therefor speed could be < ~1050, accel < ~800

// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr || 2*3.1459/(781*1)=0,008056082
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100 || 0,008056082*160000*100  = 128897,311139565
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // ALPHA*2*10000000000=161121640
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
#if defined(M6403)
	#define MAX_POS FRRPR*325/360 					 //  43,2*20*325/360=780
#elif defined(M6405)
	#define MAX_POS FRRPR*300/360 					 //  36*20*310/360=620
#endif
#define CHANGER 4

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
