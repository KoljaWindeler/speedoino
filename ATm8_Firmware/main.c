/*
 * Wenn ich das richtig sehe bräuchten wir nur noch die beiden übrigen timer0 und timer2
 * dazu zu nehmen.
 * ISR(PIN_BT/AVR) resetet dann eben den entsprechenden Timer
 * ISR(timer0/2) checkt dann ob resets erlaubt sind und führt ihn gegenbenerweise aus
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "global.h"
#include "uart.h"
#include "sm_driver.h"
#include "speed_cntr.h"
#include "reset.h"

//! Global status flags
struct GLOBAL_FLAGS status = {FALSE, FALSE, 0};

//void ShowHelp(void);
//void ShowData(int position, int acceleration, int deceleration, int speed, int steps);
//void led_off();
//void led_on();
//void led_setup();
/*! \brief Init of peripheral devices.
 *
 *  Setup IO, uart, stepper, timer and interrupt.
 */
void Init(void)
{
	// Init of motor driver IO pins
	sm_driver_Init_IO();
	// Init of uart
	InitUART();
	// Init Reset,LED Pins
	reset_init();


	// Set stepper motor driver output
	sm_driver_StepOutput(0);

	// Init of Timer/Counter1
	speed_cntr_Init_Timer1();

	// activate LED
	//led_setup();

	sei();
}

/*! \brief Demo of linear speed controller.
 *
 *  Serial interface frontend to test linear speed controller.
 */
int main(void)
{

	// Tells if the received string was a valid command.
	char okCmd = FALSE;

	Init();


	/* wir reduzieren hier die möglichkeiten:
	 * - entweder wollte der große wissen ob wir noch da sind
	 * - oder warum wir neu gestartet sind
	 * - oder er sagt uns eine Position an.. was wohl fast zu 100% der Fall sein wird
	 */

	while(1) {
		// If a command is received, check the command and act on it.
		if(status.cmd == TRUE){
			if(UART_RxBuffer[1] == 'm'){
				// Move with...
				// ...number of steps given.
				int steps = 1000*(UART_RxBuffer[2]-'0') + 100*(UART_RxBuffer[3]-'0') + 10*(UART_RxBuffer[4]-'0') + (UART_RxBuffer[5]-'0');
				speed_cntr_Move(steps, 60, 100, 1000);
				okCmd = TRUE;
				uart_SendString("$k*"); // ACK
			}
			else if(UART_RxBuffer[1] == 'y'){
				uart_SendByte('$');
				uart_SendByte('y');
				uart_SendByte(last_rst+'0');
				uart_SendByte('*');
				last_rst=0; // setze den status zurück damit wir immer einen frischen abfragen, wenn der große jetzt neustartet aber es steht da power, dann wissen wir, das war nicht der kleine, solange wie nicht wirklich einen powerlost hatten
			}

			// Clear RXbuffer.
			status.cmd = FALSE;
		}//end if(cmd)
	}//end while(1)
}
