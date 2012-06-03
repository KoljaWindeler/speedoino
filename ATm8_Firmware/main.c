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
volatile struct GLOBAL_FLAGS status = {FALSE, FALSE, 0};


/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////
void Init(void)
{
	soll_pos=0;
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
		if(get_stopper()!=STOP){
			while(get_stopper()!=STOP){
				_delay_ms(10);
				//uart_SendByte('-');
			};
			uart_SendString("$k*"); // ACK
			status.cmd = FALSE;
		}

		if(status.cmd == TRUE){
			if(UART_RxBuffer[0] == 'm'){
				int steps=UART_RxBuffer[1]-'0';
				int i=2;
				while(UART_RxBuffer[i]!=0x00){
					steps=steps*10+UART_RxBuffer[i]-'0';
					i++;
				}
				if(abs(steps-soll_pos)>0){
					// speed && accel festlegen
					int beschleunigung; // 100
					int geschwindigkeit; //400

					if(abs(steps-soll_pos)>1000){
						beschleunigung=240; // 120
						geschwindigkeit=1100; //800
					} else {
						// wenn die schritweite unter 100 ist -- dann nur 10% der leistung
						beschleunigung=24; // 100
						geschwindigkeit=110; //400
					};


					speed_cntr_Move(steps-soll_pos, beschleunigung, beschleunigung, geschwindigkeit);
					soll_pos=steps;
				} else {
					// wenn schritt zu klein ... nur so bescheid geben
					uart_SendString("$k*"); // ACK
					status.cmd = FALSE;
				}
				//speed_cntr_Move(steps, 40, 80, 200);
			} else if(UART_RxBuffer[0] == 'y'){
				uart_SendByte('$');
				uart_SendByte('y');
				uart_SendByte(last_rst+'0');
				uart_SendByte('*');
				last_rst=0; // setze den status zurück damit wir immer einen frischen abfragen, wenn der große jetzt neustartet aber es steht da power, dann wissen wir, das war nicht der kleine, solange wie nicht wirklich einen powerlost hatten
				status.cmd = FALSE;
			}
		}
	}//end while(1)
}
