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
#include "global.h"
#include "uart.h"
#include "sm_driver.h"
#include "speed_cntr.h"
#include "reset.h"

speedRampData srd;

/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////
volatile struct GLOBAL_FLAGS status = {FALSE, FALSE, 0};


/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////
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


	sei();
	//_delay_us(1000);
	config_timer0();
}


int main(){
	Init();
	while(1) {
		// If a command is received, check the command and act on it.

		if(status.cmd == TRUE){
			if(UART_RxBuffer[0] == 'm'){
				status.cmd = FALSE;
				int steps=UART_RxBuffer[1]-'0';
				int i=2;
				while(UART_RxBuffer[i]!=0x00){
					steps=steps*10+UART_RxBuffer[i]-'0';
					i++;
				}
				if(steps>2000) steps=2000;
				if(steps<0) steps=0;

				speed_cntr_Move(steps);

			} else if(UART_RxBuffer[0] == 'y'){ // ask why we reseted
				uart_SendByte('$');
				uart_SendByte('y');
				uart_SendByte(last_rst+'0');
				uart_SendByte('*');
				last_rst=0; // setze den status zurueck damit wir immer einen frischen abfragen, wenn der grouee jetzt neustartet aber es steht da power, dann wissen wir, das war nicht der kleine, solange wie nicht wirklich einen powerlost hatten
				status.cmd = FALSE;
			} else if(UART_RxBuffer[0]=='p'){ // get position
				uart_SendByte('$');
				uart_SendByte('p');
				uart_SendInt(srd.position);
				uart_SendByte('*');
				status.cmd = FALSE;
			} else if(UART_RxBuffer[0]=='r'){ // set reset status
				if(UART_RxBuffer[1]=='0'){
					reset_global_active=0;
				} else {
					reset_global_active=1;
				};
				status.cmd = FALSE;
			};
		}
	}//end while(1)
}
