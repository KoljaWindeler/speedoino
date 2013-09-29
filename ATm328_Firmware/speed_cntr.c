/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Linear speed ramp controller.
 *
 * Stepper motor driver, increment/decrement the position and outputs the
 * correct signals to stepper motor.
 *
 * - File:               speed_cntr.c
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
 * $RCSfile: speed_cntr.c,v $
 * $Date: 2006/05/08 12:25:58 $
 *****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "global.h"
#include "math.h"
#include "sm_driver.h"
#include "speed_cntr.h"
#include "uart.h"
#include <util/delay.h>
#include <avr/wdt.h>

//! Cointains data for timer interrupt.
extern speedRampData srd;


/*! \brief Move the stepper motor a given number of steps.
 *
 *  Makes the stepper motor move the given number of steps.
 *  It accelrate with given accelration up to maximum speed and decelerate
 *  with given deceleration so it stops at the given step.
 *  If accel/decel is to small and steps to move is to few, speed might not
 *  reach the max speed limit before deceleration starts.
 *
 *  \param step  Number of steps to move (pos - CW, neg - CCW).
 *  \param accel  Accelration to use, in 0.01*rad/sec^2.
 *  \param speed  Max speed, in 0.01*rad/sec.
 */
void speed_cntr_Move(signed int target, unsigned int accel, unsigned int speed){
	if (target > MAX_POS) target = MAX_POS;
	if(target<=0) target=0;
	srd.targetPosition = target;
	if (accel<1) { accel=1; }
	else if(accel<108) { accel=108; };
	if (speed<1) { speed=1; }
	else if(speed<44){ speed=44; };

	//if((TCCR1B && (~T1_STOP))!=0x00) { // if timer is in stop position -> restart ? geht das ?
	if(srd.state==0){
		srd.state=1;
		srd.min_delay = A_T_x100 / speed;
		// 1454439 / speed, wobei  min_delay 0...32767
		// 1454439/ speed < 32767
		// speed > 1454439/32767 = 44
		srd.step_delay = (T1_FREQ_148 * sqrt_2(A_SQ / accel))/240; // changed 5/7/2012 from /100
		srd.max_step_delay = srd.step_delay;
		// step_delay@accel=1
		// (13520 * sqrt_2(145781438,515081206 / accel))/240
		// 145781438,515081206 / accel< 4294967295
		// 145781438,515081206 / 4294967295 < accel
		// 0,3 < accel ...
		// für accel=1 ...
		// srd.step_delay ist 0 ⋯ 65535, wobei 65535 auch das max des timer ist
		// 65535 > (T1_FREQ_148 * sqrt_2(A_SQ / accel))/240
		// 65535 > (13520 * sqrt_2(145781438,515081206 / accel))/240
		// 145781438,515081206/(( 65535*240 / 13520 )²) < accel
		// accel > 108


		srd.accel=accel;
		OCR1A = 10;
		// Run Timer/Counter 1 with prescaler = 8.
		TCCR1B |= T1_RUN;
		TCNT1=0;   //reset the counter
	}
}


void motor_cal( unsigned int accel, unsigned int speed){
	srd.position=0;
	speed_cntr_Move(MAX_POS,accel, speed);
	while(srd.state==1){
		wdt_reset();
	}
	speed_cntr_Move(0,accel, speed);
	while(srd.state==1){
		wdt_reset();
	}
	srd.position=100;
	speed_cntr_Move(0,0, 0);
	while(srd.state==1){
		wdt_reset();
	}
}



/*! \brief Init of Timer/Counter1.
 *
 *  Set up Timer/Counter1 to use mode 1 CTC and
 *  enable Output Compare A Match Interrupt.
 */
void speed_cntr_Init_Timer1(void){
	// Tells what part of speed ramp we are in.
	srd.position = 0;
	srd.dir=CW;
	srd.accel_steps = 0;// Set Timer/Counter to divide clock by 1
	srd.state=0;
	// Timer/Counter 1 in mode 4 CTC (Not running).
	TCCR1B = (1<<WGM12);
	// Timer/Counter 1 Output Compare A Match Interrupt enable.
	TIMSK1 |= (1<<OCIE1A);
}

/*! \brief Timer/Counter1 Output Compare A Match Interrupt.
 *
 *  Timer/Counter1 Output Compare A Match Interrupt.
 *  Increments/decrements the position of the stepper motor
 *  exept after last position, when it stops.
 *  The \ref step_delay defines the period of this interrupt
 *  and controls the speed of the stepper motor.
 *  A new step delay is calculated to follow wanted speed profile
 *  on basis of accel/decel parameters.
 *
 *  Timer1 lÃ¤uft als 16-Bit Timer.
 *  Max 65535
 *  Speed: Prescale 8 => F_CPU=16Mhz/8 = 2,000000MHz
 *  Timer max timeout = 0xffff/2000000=0,0327675 sec
 *  Timer Step=1/2MHz = 500ns
 */
ISR(TIMER1_COMPA_vect){
	// Holds next delay period.
	static unsigned int new_step_delay;
	static unsigned int rest;
//	uart_SendByte('.');

	OCR1A = srd.step_delay;

	// detect stopped state
	if (srd.position==srd.targetPosition && srd.accel_steps<=1 && srd.accel_steps>=-1) { // if we have only one three steps to drive pos is 0,1,2 differ is 3,2,1 accel is 1,0,-1
		// Stop Timer/Counter 1.
		TCCR1B &= T1_STOP;
		srd.state=0;
		// Reset counter.
		srd.accel_steps = 0;// Set Timer/Counter to divide clock by 1
		return;
	} else {
		if (srd.accel_steps==0) { // if we stop, but not on that target position check direction
			if(srd.targetPosition>srd.position){
				srd.dir=CW;
			} else {
				srd.dir=CCW;
			}
		} else { // this else is used to have one step pause before changeing the direction
			sm_driver_StepCounter(srd.dir); // go one step in that direction
		}

		// calculate position + remaining steps
		int delta;
		if(srd.dir==CW){ // CW -> normal Rotation, target should be bigger than current Position
			if(srd.position<MAX_POS){
				if(srd.accel_steps!=0){
					srd.position++;
				}
			}
			delta=srd.targetPosition-srd.position; // steps to go: normal positiv
		} else {
			if(srd.position>0){
				if(srd.accel_steps!=0){
					srd.position--;
				}
			}
			delta=srd.position-srd.targetPosition; // CCW
		}

		if (delta>0) { 	// case 1 : moving towards target (maybe under accel or decel)
			if (delta < srd.accel_steps) { 	// remaining steps are less than we took to accelerate: its time to declerate
				srd.accel_steps--;
				new_step_delay = srd.step_delay + ((2 * (long)srd.step_delay + rest)/(CHANGER * srd.accel_steps + 1));
				rest = ((2 * (long)srd.step_delay)+rest)%(CHANGER * srd.accel_steps + 1);
			} else if (srd.min_delay < new_step_delay || srd.accel_steps==0) { // right now we haven't taken enough steps to accell -> go on with the acceleration
				srd.accel_steps++;
				new_step_delay = srd.step_delay - ((2 * (long)srd.step_delay + rest)/(CHANGER * srd.accel_steps + 1));
				rest = ((2 * (long)srd.step_delay)+rest)%(CHANGER * srd.accel_steps + 1);
			} else {
				// we are full speed - stay there
			}
		} else {
			// case 2 : at or moving away from target (slow down!)
			srd.accel_steps--;
			new_step_delay = srd.step_delay + ((2 * (long)srd.step_delay + rest)/(CHANGER * srd.accel_steps + 1)); // decelerate
			rest = ((2 * (long)srd.step_delay)+rest)%(CHANGER * srd.accel_steps + 1);
		}

		if(srd.accel_steps==0){ // reset speed
			new_step_delay=srd.max_step_delay;
		}
	}
	srd.step_delay = new_step_delay;
}

/*! \brief Square root routine.
 *
 * sqrt routine 'grupe', from comp.sys.ibm.pc.programmer
 * Subject: Summary: SQRT(int) algorithm (with profiling)
 *    From: warwick@cs.uq.oz.au (Warwick Allison)
 *    Date: Tue Oct 8 09:16:35 1991
 *
 *  \param x  Value to find square root of.
 *  \return  Square root of x.
 */
unsigned long sqrt_2(unsigned long x){
	register unsigned long xr;  // result register
	register unsigned long q2;  // scan-bit register
	register unsigned char f;   // flag (one bit)

	xr = 0;                     // clear result
	q2 = 0x40000000L;           // higest possible result bit
	do
	{
		if((xr + q2) <= x)
		{
			x -= xr + q2;
			f = 1;                  // set flag
		}
		else{
			f = 0;                  // clear flag
		}
		xr >>= 1;
		if(f){
			xr += q2;               // test flag
		}
	} while(q2 >>= 2);          // shift twice
	if(xr < x){
		return xr +1;             // add for rounding
	}
	else{
		return xr;
	}
}
