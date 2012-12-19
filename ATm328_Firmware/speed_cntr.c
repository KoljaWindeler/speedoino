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
//#include <avr/iom328p.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "math.h"
#include "sm_driver.h"
#include "speed_cntr.h"
#include "uart.h"
#include <util/delay.h>

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
void speed_cntr_Move(signed int soll_pos, unsigned int accel, unsigned int max_speed){
	//! Number of steps before we hit max speed.
	unsigned int step_to_max_speed;
	unsigned int differ_steps;
	//unsigned int accel=10*8;// 21.7.  von 240 -> 180
	int speed=0;
	speedRampData srd_next=srd; // copy all facts

	if(soll_pos>MAX_POS) soll_pos=MAX_POS;
	if(soll_pos<=0) soll_pos=0;

	// Set direction from differ of step value.
	if(soll_pos < srd_next.position){
		srd_next.dir_next = CCW;
		differ_steps=srd_next.position-soll_pos;
		speed=srd_next.position-soll_pos; //"8/3"
	}
	else{
		srd_next.dir_next = CW;
		differ_steps=soll_pos-srd_next.position;
		speed=soll_pos-srd_next.position; //"8/3"
	}


	speed*=2; // 21.7.  von 8/3 -> 2
	if(speed < 6){
		speed = 6;
	} else if(speed>max_speed){
		speed=max_speed;
	}

	if(differ_steps < 0){ differ_steps=0; };

	// If moving only 1 step.
	if(differ_steps == 1){
		// Move one step...
		srd_next.accel_steps = -1;
		// ...in DECEL state.
		srd_next.run_state = DECEL;
		// Just a short delay so main() can act on 'running'.
		srd_next.step_delay = 2000;
		OCR1A = 10;
		// Run Timer/Counter 1 with prescaler = 8.
		TCCR1B |= T1_RUN;
	}
	// Only move if number of steps to move is not zero.
	else if(differ_steps > 1){

		// Refer to documentation for detailed information about these calculations.
		if(srd_next.run_state==STOP){
			srd_next.dir=srd_next.dir_next; // now set previous saved direction
			// Set max speed limit, by calc min_delay to use in timer.
			// min_delay = (alpha / tt)/ w
			srd_next.min_delay = A_T_x100 / speed; // 3222432,8 / speed =
			//sprintf(temp,"%i\r\n",srd_next.min_delay);
			//uart_SendString(temp);

			// Set accelration by calc the first (c0) step delay .
			// step_delay = 1/tt * sqrt(2*alpha/accel)
			// step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
			srd_next.step_delay = (T1_FREQ_148 * sqrt_2(A_SQ / accel))/240; // changed 5/7/2012 from /100

			// Find out after how many steps does the speed hit the max speed limit.
			// step_to_max_speed = speed^2 / (2*alpha*accel)
			step_to_max_speed = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
			// If we hit max speed limit before 0,5 step it will round to 0.
			// But in practice we need to move atleast 1 step to get any speed at all.
			if(step_to_max_speed == 0){
				step_to_max_speed = 1;
			}

			// wenn wir für beschleunigung und entschleunigung zusammen MEHR als die gesamt schritte bräuchten
			if((step_to_max_speed*2) >= differ_steps){
				srd_next.decel_steps_neg = -(differ_steps/2); // negativ soviel schritte, wie ich positiv zum speedup gebraucht hab
			} else { // sonst können wir einfach mal annehmen das wir zum entschleunigen genausoviel brauchen wie für beschleunigen
				srd_next.decel_steps_neg = -step_to_max_speed;
			}

			// berechne bremspunkt abhängig von der drehrichtung
			if(srd_next.dir==CW){
				srd_next.decel_start = srd_next.position + differ_steps + srd_next.decel_steps_neg; // da descel_steps negativ ist
			} else { // der motor dreht hoch, dann ist
				srd_next.decel_start = srd_next.position - differ_steps - srd_next.decel_steps_neg; // pos - gesamt schritte - (- bremsschritte)
			};

			// We must decelrate at least 1 step to stop.
			if(srd_next.decel_steps_neg == 0){
				srd_next.decel_steps_neg = -1;
			}

			// If the maximum speed is so low that we dont need to go via accelration state.
			if(srd_next.step_delay <= srd_next.min_delay){
				srd_next.step_delay = srd_next.min_delay;
				srd_next.run_state = RUN;
			} else {
				srd_next.run_state = ACCEL;
			}
//			uart_SendString("starte timer\n\r");
			OCR1A = 10; 			// mach mal ein paar schritte (10)
			// prescale 8
			TCCR1B |= T1_RUN;
			TCNT1=0;   //reset the counter

		} else if(srd_next.run_state==DECEL){
			if((soll_pos+srd_next.decel_steps_neg>srd_next.decel_start && srd_next.dir==CW) || (soll_pos+srd_next.decel_steps_neg<srd_next.decel_start && srd_next.dir==CCW)){
				srd_next.min_delay = A_T_x100 / speed;
				//srd_step_delay = (T1_FREQ_148 * Sqr(A_SQ /accel)) / 100 DO NOT CHANGE IT
				step_to_max_speed = speed * (speed / ((A_x20000 *accel) / 100));

				if(step_to_max_speed == 0){
					step_to_max_speed = 1;
				};

				// wenn wir für beschleunigung und entschleunigungzusammen MEHR als die gesamt schritte bräuchten
				if((step_to_max_speed * 2 + srd_next.accel_steps) >=differ_steps){ // srd_accel_steps ist im decel mode negativ
					srd_next.decel_steps_neg = -(differ_steps -(differ_steps + srd_next.accel_steps) / 2); // wenn wir ein dreieck haben
				} else {	// sonst können wir einfach mal annehmen das wir zum entschleunigen genausoviel brauchen wie für beschleunigen
					srd_next.decel_steps_neg = -step_to_max_speed;
				};

				// berechne bremspunkt abhängig von der drehrichtung
				if(srd_next.dir_next == CW) {
					srd_next.decel_start = srd_next.position + differ_steps+ srd_next.decel_steps_neg; // da descel_steps negativ ist
				} else { // der motor dreht hoch, dann ist
					srd_next.decel_start = srd_next.position - differ_steps- srd_next.decel_steps_neg; // pos - gesamt schritte - (- bremsschritte)
				};

				srd_next.accel_steps = -srd_next.accel_steps;
				srd_next.accel_steps = 0; // geiler wäre 0

				// We must decelrate at least 1 step to stop.
				if(srd_next.decel_steps_neg == 0){
					srd_next.decel_steps_neg = -1;
				};

				// If the maximum speed is so low that we dont need to go via accelration state.
				if(srd_next.step_delay <= srd_next.min_delay){
					srd_next.step_delay = srd_next.min_delay;
					srd_next.run_state = RUN;
				} else {
					srd_next.run_state = ACCEL;
				};
			};
		} else if(srd_next.run_state==RUN){
			//uart_SendString("3\r\n");
			////////////// is it neccesary to speed up? //////////////
			int min_delay = A_T_x100 / speed; // 1611216,4 / (6..200) = 268536 .. 8056,082
			if(abs(min_delay-srd_next.min_delay)>100){
				// yes, speed up
				srd_next.min_delay = min_delay;
				step_to_max_speed = speed * (speed /((A_x20000 * accel) / 100));

				/* this is danger! wir wissen ja nicht genau, sind wir vielleicht arsch langsam
				* zu unserer jetzigen geschwindigkeit hingeschlichen. oder waren wir mit gleicher
				* beschleunigung unterwegs? Schöner wäre es zu sagen:
				* Wir haben differ_steps bis wir da sind wo wir hin wollen
				* Schaffen wir das auf dem Weg dort hin auf max speed zu kommen ?
				* Ja, wenn die Anzahl an schritten um von der jetzigen Geschwindigkeit auf max_speed zu kommen
				* PLUS die anzahl an bremsschritten ... wenn das mehr ist als differ_steps
				* Die Bremsschritte zu berechnen ist einfach, das ist "steps_to_max_speed" aber wieviele schritte
				* brauchen wir von unserer aktuellen geschwindigkeit (implizit in srd_next.step_delay enthalten) auf max_speed ??
				*
				* Die Zeit zwischen den Steps wird ja erstmal im Startwert nur von ACCELL=240 abhängig "berechnet" (ist eigentlich fix)
				* Und dann wird sie anhand der gefahrenen schritte kleiner gemacht .. hey cool das ist IMMER der gleiche verlauf
				* die deckelung kommt dann über "min_delay" rein
				*
				*/
				if((step_to_max_speed * 2) - srd_next.accel_steps>= differ_steps){ // wir sind ja schon ein teil der rampe gefahren, srd_accel_steps weit
					// hier kommen wir rein wenn differ_steps nicht groß genug ist, also dreieck
					// leider geil :D
					srd_next.decel_steps_neg = -((differ_steps + srd_next.accel_steps) / 2); // zum entschleunigen haben wir unsere aktuelle breite + meine geschwindigkeit
				} else { // sonst können wir einfach mal annehmen das wir zum entschleunigen genausoviel brauchen wie für beschleunigen
					srd_next.decel_steps_neg = -step_to_max_speed;
				};

				// berechne bremspunkt abhängig von der drehrichtung
				if(srd_next.dir_next == CW) {
					srd_next.decel_start = srd_next.position + differ_steps + srd_next.decel_steps_neg; // da descel_steps negativ ist
				} else { // der motor dreht hoch, dann ist
					srd_next.decel_start = srd_next.position -differ_steps - srd_next.decel_steps_neg; // pos - gesamt schritte - (-bremsschritte)
				};

				// We must decelrate at least 1 step to stop.
				if(srd_next.decel_steps_neg == 0){
					srd_next.decel_steps_neg = -1;
				};
				srd_next.run_state = ACCEL; // -> Use STOP to reaccel?
				//uart_SendString("--> GoTo ACCEL\r\n");
			}
			////////////// is it neccesary to speed up? //////////////

			if(srd_next.dir==CW){
				if(srd_next.position+(-srd_next.decel_steps_neg)>=soll_pos){	// wenn wir mit der entschleunigungsrampe das gerade noch so, oder schon gar nicht mehr schaffen, dann sofort abbremsen
					srd_next.run_state = DECEL;
					srd_next.accel_steps = srd_next.decel_steps_neg;
					char temp[30];
					sprintf(temp,"1:%i,%i,%i\r\n",srd_next.accel_steps,srd_next.position,soll_pos);
					//uart_SendString(temp);
				} else { // wenn wir noch easy Zeit haben zum bremsen
					srd_next.decel_start = soll_pos-(-srd_next.decel_steps_neg);
				};
			} else {
				if(srd_next.position+srd_next.decel_steps_neg<=soll_pos){
					srd_next.run_state = DECEL;
					srd_next.accel_steps = srd_next.decel_steps_neg;
				} else {
					srd_next.decel_start = soll_pos+(-srd_next.decel_steps_neg);
				};
			};
		}
	}
	//uart_SendString(temp);
	// now copy all facts to the operation struct, but without getting interrupted
	cli();
	srd=srd_next;
	sei();
	// now copy all facts to the operation struct, but without getting interrupted
}



/*! \brief Init of Timer/Counter1.
 *
 *  Set up Timer/Counter1 to use mode 1 CTC and
 *  enable Output Compare A Match Interrupt.
 */
void speed_cntr_Init_Timer1(void){
	// Tells what part of speed ramp we are in.
	srd.run_state = STOP;
	srd.position = 0;
	srd.dir=CW;
	srd.dir_next=CW;
	srd.accel_steps = 0;// Set Timer/Counter to divide clock by 1
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
 *  Timer1 läuft als 16-Bit Timer.
 *  Max 65535
 *  Speed: Prescale 8 => F_CPU=8Mhz/8 = 1000000Hz
 *  Timer max timeout = 0xffff/1000000=0,065535 sec
 */
ISR(TIMER1_COMPA_vect){
	// Holds next delay period.
	unsigned int new_step_delay = 0;
	// Remember the last step delay used when accelrating.
	static int last_accel_delay;
	// Keep track of remainder from new_step-delay calculation to incrase accurancy
	//static unsigned int rest = 0;

	OCR1A = srd.step_delay;

	switch(srd.run_state) {
	case STOP:

		//rest = 0;
		// Stop Timer/Counter 1.
		TCCR1B &= T1_STOP;

		// Reset counter.
		srd.accel_steps = 0;// Set Timer/Counter to divide clock by 1
		break;
	case ACCEL:
		sm_driver_StepCounter(srd.dir);
		srd.accel_steps++;
		// keep eye on position
		if(srd.dir==CCW){
			if(srd.position>0)
				srd.position--;
		} else {
			if(srd.position<MAX_POS)
				srd.position++;
		}
		new_step_delay = srd.step_delay - ((2 * (long)srd.step_delay)/(4 * srd.accel_steps + 1));
		//rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_steps + 1);
		// Check if we should start decelration.
		if(srd.position == srd.decel_start) {
			srd.accel_steps = -srd.accel_steps;
			srd.run_state = DECEL;
		}
		// Chech if we hitted max speed.
		else if(new_step_delay <= srd.min_delay || srd.accel_steps>(-srd.decel_steps_neg)) { // geht das?
			last_accel_delay = new_step_delay;
			new_step_delay = srd.min_delay;
			//rest = 0;
			srd.run_state = RUN;
		}
		break;
	case RUN:
		sm_driver_StepCounter(srd.dir);
		if(srd.dir==CCW){
			if(srd.position>0)
				srd.position--;
		} else {
			if(srd.position<MAX_POS)
				srd.position++;
		}
		new_step_delay = srd.min_delay;
		// Check if we should start decelration.
		if((srd.dir==CW && srd.position >= srd.decel_start) || ((srd.dir==CCW && srd.position <= srd.decel_start))) {
			srd.accel_steps = srd.decel_steps_neg;
			// Start decelration with same delay as accel ended with.
			new_step_delay = last_accel_delay;
			srd.run_state = DECEL;
		}
		break;
	case DECEL:
		sm_driver_StepCounter(srd.dir);
		// keep eye on position
		if(srd.dir==CCW){
			if(srd.position>0)
				srd.position--;
		} else {
			if(srd.position<MAX_POS)
				srd.position++;
		}
		srd.accel_steps++;
		new_step_delay = srd.step_delay - ((2 * (long)srd.step_delay)/(4 * srd.accel_steps + 1));
		//new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_steps + 1));
		//rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_steps + 1);
		// Check if we at last step
		if(srd.accel_steps >= 0){
			srd.run_state = STOP;
			OCR1A=0x03ff;
		}
		break;
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

