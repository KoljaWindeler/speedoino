#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rpm_calc.h"
#include "speed_cntr.h"
#include "reset.h"
#include "uart.h"

// activate timer + pin interrupts
void init_rpm_calculation(){
	// timer
	TCNT2=0x00; 								// reset value
	TIMSK2=(1<<TOIE2); 							// activate Timer overflow interrupt
	TCCR2B=(0<<CS22) | (1<<CS21) | (0<<CS20); 	// selects clock to "/8" => 16Mhz/8=2MHz,
	TCCR2A=0; 									// WGM=0 -> normal mode
	t2_overruns=0;

	//activate PCint PB0 -> PCINT0
	DDRB &= ~(1<<PB0); 		// input
	PCMSK0|=(1<<PCINT0); 	// CAN interrupt pin v7
	PCICR|=(1<<PCIE0); 		// enable PinChange interrupt
}

// timer runs with 2 MHz
// if the engine runs with 3.000 rpm we should have 3000/60*2(double ignition)=100 pulses per second
// that leads to 10ms per Pulse. Timer5 value will be at 20.000 after 10ms. -> 60.000.000/20.000=3.000
ISR(PCINT0_vect){
	if(PINB&(1<<PB0)){ 			/* rising edge */
		uint8_t sreg;
		uint16_t timerValue;
		sreg = SREG;        	/* Save global interrupt flag */
		cli();               	/* Disable interrupts */
		timerValue = TCNT2;    	/* Read TCNTn into i */
		set_goto(t2_overruns,timerValue); // just save values as quick as possible
		t2_overruns=0;
		TCNT0=2;            	/* Reset Timer value */
		SREG = sreg;        	/* Restore global interrupt flag */
	}
}

// this overflow will occure after 256/2000000=0,000128 sec
ISR(TIMER2_OVF_vect){
	t2_overruns++;
	if(t2_overruns>4*256){ 	// no spark for 4*256*0,128ms=131,072ms -> less than 228 rpm
		set_goto(0,0); 		// just save values as quick as possible
	}
}

// just save values as quick as possible and set the flag GOTO_FLAG
void set_goto(uint16_t overruns, uint8_t timer){
	goto_overruns=overruns;
	goto_timer=timer;
	goto_flag=true;
}

// this should be callen in the non-interrupt space, to avoid long interrupts
// since we call speed_cntr_move (which uses sqrt+...) we shouldn't do it in the interrupt
void check_goto(){
	if(goto_flag){
		//set_exact((uint32_t)60000000UL / (((uint32_t)t0_overruns<<8) + timerValue));
		// Pos 864 = RPM 20.000 -> 20000/864=23,148
		// 60.000.000/23,148148148148148148148148148148 = 2592000
		speed_cntr_Move((uint32_t)2592000UL / (((uint32_t)goto_overruns<<8) + goto_timer),accel,speed);
		goto_flag=false;
	}
}
