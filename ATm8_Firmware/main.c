/*
 * main.c
 *
 *  Created on: 12.06.2011
 *      Author: kolja
 *
 *  Aufgaben des ATMega:
 *  1. Sobald eine Bluetoothverbindung aufgebaut wird einen Reset auslösen
 *  2. Wenn der große Prozessor nichts mehr sagt diesen Resetten können ( Watchdog Funktion )
 *  3. Den Reset global unterdrücken können
 *  4. Per UART antworten können warum resettet wurde
 *  5. Im Falle eines BT Reset den Bootloader des großen daran hindern schneller durchzulaufen als Daten gesendet werden können
 *
 *  Konzept:
 *  Wir haben 2 Counter, counter_bt und counter_avr, die werden hochgezählt,
 *  momentan etwas(!) langsamer als 1kHz ( 1ms warten + if's )
 *  Wenn einer von beiden den Schwellwert aus CYCLES_FOR_* erreicht dann wird ein RESET
 *  ausgelöst. Das heisst eine HIGH - LOW - HIGH Kombination am Reset Pin.
 *  Wenn es ein BT Reset ist dann sollte die ganze geschichte erst dann wieder freigegeben
 *  werden, wenn eine toggelnde Flanke am Connect Pin gefunden wird.
 *
 *  FALLS der AVR überhaupt abschmiert, soll hier ergänzt werden:
 *  Der AVR sollte in einem Takt ähnlich dem des Bluetooth "togglen",
 *  sobald er mal nicht togglet läuft sein zähler auch über und resettet wie bei BT
 *  eigentlicht das gleiche.
 *
 *  zusätzlich sollte der avr bestimmen können das die ganze geschichte deaktiviert wird,
 *  z.B. der Filemanager verweilt sehr lang ohne loop
 *  Daher ist der pin PinB0 vom AVR auf LOW zu ziehen um den Reset zu deaktivieren,
 *  wir ziehen den dann unsererseits intern per pullup auf high und somit können wir resetten
 *  wenn der AVR nix sagt, also z.B. kein Programm hat.
 *
 *  Startup: zum testen einmal mit allen augen zwinkern. Alle LED's an, nach 1000ms programmstart
 *  und damit wieder aus.
 *
 *  Wenn wir einen Bluetooth reset haben, schalten wir den PinB0 als Ausgang und ziehen ihn auf Masse
 *  da der Bootloader des großen einen Input mit Pullup schaltet. In der Zeit die der Pin auf Masse liegt
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


/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////
struct GLOBAL_FLAGS status = {FALSE, FALSE, 0};


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
	_delay_us(1000);
	config_timer0();
}


int main(){
	Init();
	command_received=0;
	soll_pos=0;
	int accel=70;
	int speed=5000;
	speed_cntr_Move(-1800,70,100,1000);
	while(get_stopper()!=STOP){};

	while(1) {
		// If a command is received, check the command and act on it.
		if(command_received){
			if(UART_RxBuffer[0] == 'm'){
				// Move with...
				// ...number of steps given.
				int steps = 0;

				for(int i=1; i<UART_RX_BUFFER_SIZE && UART_RxBuffer[i]!='\0';i++){
					steps=steps*10+UART_RxBuffer[i]-'0';
				};
				steps=steps-soll_pos;
				soll_pos+=steps;

				speed_cntr_Move(steps, accel, 100, speed);
				while(get_stopper()!=STOP){};
				uart_SendString("$k*"); // ACK
			}
			else if(UART_RxBuffer[0] == 'y'){
				uart_SendByte('$');
				uart_SendByte('y');
				uart_SendByte(last_rst+'0');
				uart_SendByte('*');
				last_rst=0; // setze den status zurück damit wir immer einen frischen abfragen, wenn der große jetzt neustartet aber es steht da power, dann wissen wir, das war nicht der kleine, solange wie nicht wirklich einen powerlost hatten
			}
			else if(UART_RxBuffer[0] == 'a'){
				int accel = 0;
				for(int i=1; i<UART_RX_BUFFER_SIZE && UART_RxBuffer[i]!='\0';i++){
					accel=accel*10+UART_RxBuffer[i]-'0';
				};
				uart_SendString("$k*"); // ACK
			}
			else if(UART_RxBuffer[0] == 's'){
				int speed = 0;
				for(int i=1; i<UART_RX_BUFFER_SIZE && UART_RxBuffer[i]!='\0';i++){
					speed=speed*10+UART_RxBuffer[i]-'0';
				};
				uart_SendString("$k*"); // ACK
			}

			// Clear RXbuffer.
			command_received=0;
		}//end if(cmd)
	}//end while(1)
}
