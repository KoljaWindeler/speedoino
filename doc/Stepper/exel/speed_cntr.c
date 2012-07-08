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
#include <avr/signal.h>
#include "global.h"
#include "sm_driver.h"
#include "speed_cntr.h"
#include "uart.h"

//! Cointains data for timer interrupt.
speedRampData srd;

int running_value;

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
void speed_cntr_Move(signed int soll_pos, unsigned int accel, unsigned long speed)
{
	//! Number of steps before we hit max speed.
	unsigned int step_to_max_speed;
	//! Number of steps before we must start deceleration (if accel does not hit max speed).
	unsigned int accel_lim;
	unsigned int differ_steps;

	// Set direction from differ of step value.
	if(soll_pos < srd.position){
		srd.dir = CCW;
		differ_steps=srd.position-soll_pos;
	} else {
		srd.dir = CW;
		differ_steps=soll_pos-srd.position;
	}
	

	// If moving only 1 step.
	if(differ_steps == 1){
		// Move one step...
		srd.accel_steps = -1;
		// ...in DECEL state.
		srd.run_state = DECEL;
		// Just a short delay so main() can act on 'running'.
		srd.step_delay = 1000;
		status.running = TRUE;
		OCR1A = 10;
		// Run Timer/Counter 1 with prescaler = 1.
		TCCR1B |= (1<<CS10);
	}
	// Only move if number of steps to move is not zero.
	else if(differ_steps != 0){
		// Refer to documentation for detailed information about these calculations.
		
		if(srd.run_state==STOP){
			// Set max speed limit, by calc min_delay to use in timer.
			// min_delay = (alpha / tt)/ w
			srd.min_delay = A_T_x100 / speed;

			// Set accelration by calc the first (c0) step delay .
			// step_delay = 1/tt * sqrt(2*alpha/accel)
			// step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
			srd.step_delay = (T1_FREQ_148 * sqrt_2(A_SQ / accel))/100;
			
			// Find out after how many steps does the speed hit the max speed limit.
			// step_to_max_speed = speed^2 / (2*alpha*accel)
			step_to_max_speed = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
			// If we hit max speed limit before 0,5 step it will round to 0.
			// But in practice we need to move atleast 1 step to get any speed at all.
			if(step_to_max_speed == 0){
				step_to_max_speed = 1;
			}

			// wenn wir für beschleunigung und entschleunigung zusammen MEHR als die gesamt schritte bräuchten
			if((step_to_max_speed<<1) >= differ_steps){
				srd.decel_steps_neg = -(differ_steps>>1); // negativ soviel schritte, wie ich positiv zum speedup gebraucht hab
				if(srd.dir==CCW){
					srd.decel_start = srd.position - (differ_steps>>1); 
				} else { // der motor dreht hoch, dann ist 
					srd.decel_start = srd.position + (differ_steps>>1);
				};
			} else { // sonst können wir einfach mal annehmen das wir zum entschleunigen genausoviel brauchen wie für beschleunigen
				srd.decel_steps_neg = -step_to_max_speed;
			}
			// We must decelrate at least 1 step to stop.
			if(srd.decel_steps_neg == 0){
				srd.decel_steps_neg = -1;
			}

			// If the maximum speed is so low that we dont need to go via accelration state.
			if(srd.step_delay <= srd.min_delay){
				srd.step_delay = srd.min_delay;
				srd.run_state = RUN;
			} else {
				srd.run_state = ACCEL;
			}

			// Reset counter.
			srd.accel_steps = 0;// Set Timer/Counter to divide clock by 1
			status.running = TRUE;
			OCR1A = 10;
			// Set Timer/Counter to divide clock by 1
			TCCR1B |= (1<<CS10);
			// Set Timer/Counter to divide clock by 8
			//TCCR1B |= (1<<CS11);
		/*} else if(srd.run_state==ACCEL){
			if(srd.dir==CW){
				if(srd.position+(-srd.decel_steps_neg)<=soll_pos){
					// TODO, richtig aufwendig, hier sollten wir alles neu berechnen und bedenken das wir ja schon fahren!!
					// wir sind also in dem modus, das wir zwar gerade beschleunigen,
				} else { // also ist der neue endpunkt doch weiter weg als wir alle dachten, das ist easy, dann schieben wir einfach den stopper weiter nach hinten
					// TODO: wenn wir vorher keine richtige rampge gefahren sind .... wird das hier sehr langsam :D
					srd.decel_start = soll_pos+std.decel_steps_neg;
				}
			} else {
				if(srd.position+srd.decel_steps_neg>=soll_pos){ // unsere soll position liegt noch weiter weg als wir fahren wollten, easy
					// TODO: wenn wir vorher keine richtige rampge gefahren sind .... wird das hier sehr langsam :D
					srd.decel_start = soll_pos+(-std.decel_steps_neg);
				} else {
					// TODO, richtig aufwendig, hier sollten wir alles neu berechnen und bedenken das wir ja schon fahren!!
				}
			};
		} else if(srd.run_state==DECEL){
			if(srd.dir==CW){
				if(srd.position+(-srd.accel_steps)>=soll_pos){ // position + bremsschritte => zu weit gefahren
					srd.run_state==DECEL; // bleib im bremsmodus, indem du bist
					// sonst mach nix
				} else { // wir müssen noch mal weiter fahren
					srd.run_state=ACCEL; // weiterfahren, daher gleich wieder in accel wechseln
					srd.accel_steps=-srd.accel_steps; // wieder da weiter machen wo wir gerade waren, durchstarten, liftoff
					
					// wieviele schritte brauchen wir zum anhalten? wann machen wir das ? 
					srd.decel_steps_neg = -((long)speed*speed/(long)(((long)A_x20000*accel)/100)); // soviele schritte (*-1) brauchen wir zum bremsen
					step_to_max_speed = -decel_steps_neg-srd.accel_steps; // wir sind ja schon auf speed daher brauchen wir viel weniger schritte
					
					if(step_to_max_speed+(-srd.decel_steps_neg) >= differ_steps){ // wir schaffen es nicht mehr innerhalb der vorgegebenen Zeit auf FULLspeed + Bremsen
						unsigned long fehlende_schritte=((step_to_max_speed+(-srd.decel_steps_neg))-differ_steps)>>1; 
						srd.decel_steps_neg+=fehlende_schritte; // da decel_steps negativ sind wird das kleiner in dem wir einfach die anzahl an fehlenden schritte draufrechnen
						srd.decel_start=srd.position+step_to_max_speed-fehlende_schritte;
					}
					else{ // wenn wir noch genügend schritte haben um auf vollgas zu kommen machen wir das 
						// bremsschritte sind vorgegeben
						// bremspunkt noch nicht
						srd.decel_start=srd.position+differ_steps-(-srd.decel_steps_neg);
					}
					srd.decel_steps_neg = -step_to_max_speed;
				};
			} else {
				if(srd.position-(-srd.accel_steps)<=soll_pos){ // position + bremsschritte => zu weit gefahren
					srd.run_state==DECEL; // bleib im bremsmodus, indem du bist
					// sonst mach nix
				} else { // wir müssen noch mal weiter fahren
					srd.run_state=ACCEL; // weiterfahren, daher gleich wieder in accel wechseln
					srd.accel_steps=-srd.accel_steps; // wieder da weiter machen wo wir gerade waren, durchstarten, liftoff
					
					// wieviele schritte brauchen wir zum anhalten? wann machen wir das ? 
					srd.decel_steps_neg = -((long)speed*speed/(long)(((long)A_x20000*accel)/100)); // soviele schritte (*-1) brauchen wir zum bremsen
					step_to_max_speed = -decel_steps_neg-srd.accel_steps; // wir sind ja schon auf speed daher brauchen wir viel weniger schritte
					
					if(step_to_max_speed+(-srd.decel_steps_neg) >= differ_steps){ // wir schaffen es nicht mehr innerhalb der vorgegebenen Zeit auf FULLspeed + Bremsen
						unsigned long fehlende_schritte=((step_to_max_speed+(-srd.decel_steps_neg))-differ_steps)>>1; 
						srd.decel_steps_neg+=fehlende_schritte; // da decel_steps negativ sind wird das kleiner in dem wir einfach die anzahl an fehlenden schritte draufrechnen
						srd.decel_start=srd.position-step_to_max_speed+fehlende_schritte;
					} else{ // wenn wir noch genügend schritte haben um auf vollgas zu kommen machen wir das 
						// bremsschritte sind vorgegeben
						// bremspunkt noch nicht
						srd.decel_start=srd.position-differ_steps+(-srd.decel_steps_neg);
					}
					srd.decel_steps_neg = -step_to_max_speed;
				};			
			};*/
		} else if(srd.run_state==RUN){
			// is it neccesary to speed up?
			int min_delay = A_T_x100 / speed;
			if(min_delay<srd.min_delay){
				srd.min_delay = min_delay;
				srd.run_state = ACCEL;
			}
			
			if(srd.dir==CW){
				if(srd.position+(-srd.decel_steps_neg)>=soll_pos){	// wenn wir mit der entschleunigungsrampe das gerade noch so, oder schon gar nicht mehr schaffen, dann sofort abbremsen
					srd.run_state = DECEL;
					srd.accel_steps = srd.decel_steps_neg;
				} else { // wenn wir noch easy Zeit haben zum bremsen
					srd.decel_start = soll_pos-(-srd.decel_steps_neg);
				};
			} else {
				if(srd.position+srd.decel_steps_neq<=soll_pos){
					srd.run_state = DECEL;
					srd.accel_steps = srd.decel_steps_neg;
				} else {
					srd.decel_start = soll_pos+(-srd.decel_steps_neg);
				};
			};
		}
	}
}

/*! \brief Init of Timer/Counter1.
 *
 *  Set up Timer/Counter1 to use mode 1 CTC and
 *  enable Output Compare A Match Interrupt.
 */
void speed_cntr_Init_Timer1(void)
{
	// Tells what part of speed ramp we are in.
	srd.run_state = STOP;
	srd.position = 0;
	// Timer/Counter 1 in mode 4 CTC (Not running).
	TCCR1B = (1<<WGM12);
	// Timer/Counter 1 Output Compare A Match Interrupt enable.
	TIMSK |= (1<<OCIE1A);
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
 */
ISR(TIMER1_COMPA_vect){
	// Holds next delay period.
	unsigned int new_step_delay;
	// Remember the last step delay used when accelrating.
	static int last_accel_delay;
	// Keep track of remainder from new_step-delay calculation to incrase accurancy
	static unsigned int rest = 0;

	OCR1A = srd.step_delay;

	switch(srd.run_state) {
	case STOP:
		rest = 0;
		// Stop Timer/Counter 1.
		TCCR1B &= ~(1<<CS10 | 1<<CS11);
		status.running = FALSE;
		break;
	case ACCEL:
		sm_driver_StepCounter(srd.dir);
		srd.accel_steps++;
		// keep eye on position
		if(srd.dir==CCW){
			srd.position--;
		} else {
			srd.position++;
		}
		new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_steps + 1));
		rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_steps + 1);
		// Chech if we should start decelration.
		if(srd.position == srd.decel_start) {
			srd.accel_steps = srd.decel_steps_neg;
			srd.run_state = DECEL;
		}
		// Chech if we hitted max speed.
		else if(new_step_delay <= srd.min_delay) {
			last_accel_delay = new_step_delay;
			new_step_delay = srd.min_delay;
			rest = 0;
			srd.run_state = RUN;
		}
		break;
	case RUN:
		sm_driver_StepCounter(srd.dir);
		// keep eye on position
		if(srd.dir==CCW){
			srd.position--;
		} else {
			srd.position++;
		}
		new_step_delay = srd.min_delay;
		// Chech if we should start decelration.
		if(srd.position == srd.decel_start) {
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
			srd.position--;
		} else {
			srd.position++;
		}
		srd.accel_steps++;
		new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_steps + 1));
		rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_steps + 1);
		// Check if we at last step
		if(srd.accel_steps >= 0){
			running_value=0;
			srd.run_state = STOP;
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
unsigned long sqrt_2(unsigned long x)
{
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

/*! \brief Find minimum value.
 *
 *  Returns the smallest value.
 *
 *  \return  Min(x,y).
 */
unsigned int min(unsigned int x, unsigned int y)
{
	if(x < y){
		return x;
	}
	else{
		return y;
	}
}

void set_stopper(){
	running_value=1;
}

int get_stopper(){
	return srd.run_state;
}
