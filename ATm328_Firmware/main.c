/*
 * main.c
 *
 *  Created on: 12.06.2011
 *      Author: kolja
 *
 *  Aufgaben des ATMega:
 *  1. Sobald eine Bluetoothverbindung aufgebaut wird einen Reset ausluesen
 *  2. Wenn der grouee Prozessor nichts mehr sagt diesen Resetten kuennen ( Watchdog Funktion )
 *  3. Den Reset global unterdruecken kuennen
 *  4. Per UART antworten kuennen warum resettet wurde
 *  5. Im Falle eines BT Reset den Bootloader des groueen daran hindern schneller durchzulaufen als Daten gesendet werden kuennen
 *
 *  Konzept:
 *  Wir haben 2 Counter, counter_bt und counter_avr, die werden hochgezuehlt,
 *  momentan etwas(!) langsamer als 1kHz ( 1ms warten + if's )
 *  Wenn einer von beiden den Schwellwert aus CYCLES_FOR_* erreicht dann wird ein RESET
 *  ausgeluest. Das heisst eine HIGH - LOW - HIGH Kombination am Reset Pin.
 *  Wenn es ein BT Reset ist dann sollte die ganze geschichte erst dann wieder freigegeben
 *  werden, wenn eine toggelnde Flanke am Connect Pin gefunden wird.
 *
 *  FALLS der AVR ueberhaupt abschmiert, soll hier erguenzt werden:
 *  Der AVR sollte in einem Takt uehnlich dem des Bluetooth "togglen",
 *  sobald er mal nicht togglet lueuft sein zuehler auch ueber und resettet wie bei BT
 *  eigentlicht das gleiche.
 *
 *  zusuetzlich sollte der avr bestimmen kuennen das die ganze geschichte deaktiviert wird,
 *  z.B. der Filemanager verweilt sehr lang ohne loop
 *  Daher ist der pin PinB0 vom AVR auf LOW zu ziehen um den Reset zu deaktivieren,
 *  wir ziehen den dann unsererseits intern per pullup auf high und somit kuennen wir resetten
 *  wenn der AVR nix sagt, also z.B. kein Programm hat.
 *
 *  Startup: zum testen einmal mit allen augen zwinkern. Alle LED's an, nach 1000ms programmstart
 *  und damit wieder aus.
 *
 *  Wenn wir einen Bluetooth reset haben, schalten wir den PinB0 als Ausgang und ziehen ihn auf Masse
 *  da der Bootloader des groueen einen Input mit Pullup schaltet. In der Zeit die der Pin auf Masse liegt
 *  wird der Count der des Bootloader deaktiviert.
 */
// includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "global.h"
#include "uart.h"
#include "sm_driver.h"
#include "speed_cntr.h"
#include "reset.h"
#include "rpm_calc.h"
#include <avr/wdt.h>

speedRampData srd;
#define ACC 400//600
#define SPEED 1000

/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////
volatile struct GLOBAL_FLAGS status = {FALSE, FALSE, 0};
bool emergency_shutdown;
bool emergency_extra_pos_offset_set;
unsigned char voltage_down_counter=0;
unsigned char voltage_up_counter=0;

/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////
void Init(void)
{
	wdt_enable(WDTO_1S);

	speed=SPEED;
	accel=ACC;
	emergency_shutdown=false;
	emergency_extra_pos_offset_set=false;

	DDRB=0x00;
	DDRC=0x00;
	DDRD=0x00;
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

	sei();

	// reset timer aktivieren
	config_timer0();


	//Just to be sure
	speed=SPEED;
	accel=ACC;

}


int main(){
	Init();
#ifdef STAND_ALONE_MODE
	motor_cal(accel,speed);
	init_rpm_calculation();
#endif
	while(1) {
		wdt_reset();
#ifdef STAND_ALONE_MODE
		check_goto();
#endif
		//check_power_state();
		// If a command is received, check the command and act on it.
		if(status.cmd == TRUE){
			/////////////////////////// MOVE /////////////////////////////////////
			// $m1200* $m0*
			if(UART_RxBuffer[0] == 'm'){
				status.cmd = FALSE;
				int set_pos=UART_RxBuffer[1]-'0';
				int i=2;
				int pos0_accel1_speed2=0;
				while(UART_RxBuffer[i]!=0x00){
					wdt_reset();
					if(UART_RxBuffer[i]>='0' && UART_RxBuffer[i]<='9'){
						if(pos0_accel1_speed2==0){
							set_pos=set_pos*10+UART_RxBuffer[i]-'0';
						} else if(pos0_accel1_speed2==1){
							accel=accel*10+UART_RxBuffer[i]-'0';
						} else if(pos0_accel1_speed2==2){
							speed=speed*10+UART_RxBuffer[i]-'0';
						}
					} else if(UART_RxBuffer[i]==','){
						pos0_accel1_speed2++;
						if(pos0_accel1_speed2==1){
							accel=0;
						} else if(pos0_accel1_speed2==2){
							speed=0;
						}
					}
					i++;
				}

				speed_cntr_Move(set_pos,accel,speed);
				/////////////////////////// MOVE /////////////////////////////////////
				///////////////////// ask reset reason ///////////////////////////////
			} else if(UART_RxBuffer[0] == 'y'){ // ask why we reseted
				uart_SendByte('$');
				uart_SendByte('y');
				uart_SendByte(last_rst+'0');
				uart_SendByte('*');
				last_rst=0; // setze den status zurueck damit wir immer einen frischen abfragen, wenn der grouee jetzt neustartet aber es steht da power, dann wissen wir, das war nicht der kleine, solange wie nicht wirklich einen powerlost hatten
				status.cmd = FALSE;
				///////////////////// ask reset reason ///////////////////////////////
				///////////////////// do calibration ///////////////////////////////
			} else if(UART_RxBuffer[0]=='c'){ // get position
				motor_cal(ACC,SPEED);
				status.cmd = FALSE;
				///////////////////// do calibration///////////////////////////////
				///////////////////// ask for position ///////////////////////////////
			} else if(UART_RxBuffer[0]=='p'){ // get position
				uart_SendByte('$');
				uart_SendByte('p');
				uart_SendInt(srd.position);
				uart_SendByte('*');
				status.cmd = FALSE;
				///////////////////// ask for position ///////////////////////////////
				///////////////////// ask for status ///////////////////////////////
			} else if(UART_RxBuffer[0]=='g'){ // get status
				uart_SendByte('$');
				uart_SendByte('g');
				uart_SendInt(srd.position);
				uart_SendByte(',');
				uart_SendInt(srd.step_delay);
				//				uart_SendByte(',');
				//				uart_SendInt(srd.run_state);
				uart_SendByte('*');
				status.cmd = FALSE;
				///////////////////// ask for status ///////////////////////////////
				//////////////////// (de)activate reset //////////////////////////////
			} else if(UART_RxBuffer[0]=='r'){ // set reset status
				if(UART_RxBuffer[1]=='0'){
					reset_global_active=0;
				} else if(UART_RxBuffer[1]=='1'){
					reset_global_active=1;
				};
				status.cmd = FALSE;
				//////////////////// (de)activate reset //////////////////////////////
				///////////////////// self running mode ///////////////////////////////
			} else if(UART_RxBuffer[0]=='s'){ // self running mode
				motor_cal(ACC,SPEED);	// run calibration
				init_rpm_calculation(); // activate timer
				status.cmd = FALSE;		// mark message as received
				///////////////////// self running mode ///////////////////////////////
				//////////////////// set pointer offset //////////////////////////////
			} else if(UART_RxBuffer[0] == 'o'){ // set offset
				status.cmd = FALSE;
				int new_position=UART_RxBuffer[1]-'0';
				int i=2;
				while(UART_RxBuffer[i]!=0x00){
					new_position=new_position*10+UART_RxBuffer[i]-'0';
					i++;
				}
				if(new_position>2000) new_position=2000;
				if(new_position<0) new_position=0;

				//				if(srd.run_state==STOP){
				//					srd.position=new_position;
				//				};

			}
			//////////////////// set pointer offset //////////////////////////////

		}
	}//end while(1)
}


void check_power_state(){
	if(bit_is_clear(PIND,4)){ // kein Spannung mehr auf dem Pin, vor der Diode
		//		uart_SendString("$");
		//voltage_up_counter=0;
		voltage_down_counter++;
		// wenn der counter nicht geht, erstmal mit 100nF probieren und dann vielleicht als PullUp auslegen?
		if(voltage_down_counter>50){ // ~ 50µsec direkt nacheinander
			//			uart_SendString("%");
			// input interrupts für reset abwerfen
			EIMSK  &= ~((1<<INT0) | (1<<INT1));	//Global Interrupt Flag deaktivieren fuer INT0 und INT1
			TIMSK0 &= ~(1<<TOIE0); // Timer overrun aus

			// Kommunikation weg
			disable_uart();

			// LED's ausklinken
			DDRB = 0x00;
			PORTB = 0x00;
			//		PORTC &= 0x00; // motor abwerfen ...  ahh dumme idee
			DDRD = 0x00;
			PORTD = 0x00;

			// merken das wir im shutdown sind
			emergency_shutdown=true;

			// fahrt anstarten
			srd.position=250;
			speed_cntr_Move(0,accel,speed);
			//} else {
			// wir sind im normal modus, haben aber kein strom von der vorsorgung, könnte
			// sein das wir ein "repower" haben, nach einem Spike, oder weil der kondensator
			// nur wieder spannung aufgebaut hat .. um zu testen ob es der Kondensator ist
			// bestromen wir die Wicklungen der Spulen, das sollte ein sehr leerer Kondensator
			// nicht lang schaffen können

			//			sm_driver_Init_IO();
			//			sm_driver_StepOutput(0);
			//			voltage_down_counter++;
		}
	} else {
		voltage_down_counter=0;
		// solange up noch nicht bei 10 ist, zählen wir hoch und bestromen die windungen
		// erst wenn er 10 zyklen lang die spannung halten kann fahren wir den Zeiger zurück

		if(emergency_shutdown){// && voltage_up_counter>=10){
			// ups, doch strom wieder da
			//			uart_SendString("(");
			Init(); // setzt dann auch emergency_shutdown zurück
			srd.position=200;
			speed_cntr_Move(0,accel,speed);
		} /*else if(emergency_shutdown){
//			uart_SendString(")");
			// wir sind im notfall modus, haben wieder strom von der vorsorgung, könnte
			// sein das wir ein "repower" haben, nach einem Spike, oder weil der kondensator
			// nur wieder spannung aufgebaut hat .. um zu testen ob es der Kondensator ist
			// bestromen wir die Wicklungen der Spulen, das sollte ein sehr leerer Kondensator
			// nicht lang schaffen können
//			sm_driver_Init_IO();
//			sm_driver_StepOutput(0);
			voltage_up_counter++;
		}*/
	} // voltage is up
};
