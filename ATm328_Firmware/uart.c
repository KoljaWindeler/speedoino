/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Uart routines.
 *
 * - File:               uart.c
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
 * $RCSfile: uart.c,v $
 * $Date: 2006/05/08 12:25:58 $
 *****************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"
#include "uart.h"
#include "sm_driver.h"
#include "speed_cntr.h"
#include <util/delay.h>

//! RX buffer for uart.
unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
//! RX buffer pointer.
unsigned char UART_RxPtr;
unsigned char UART_state;

// Static Variables.
//! TX buffer for uart.
//static unsigned char UART_TxBuffer[UART_TX_BUFFER_SIZE];
//! TX buffer head pointer.
static volatile unsigned char UART_TxHead;
//! TX buffer tail pointer.
static volatile unsigned char UART_TxTail;

/*! \brief Init of uart.
 *
 * Setup uart. The \ref BAUD value must be modified according to clock frqequency.
 * Refer to datasheet for details.
 *
 */
void InitUART(void)
{
	// Set baud rate. 19.2 kbps trasfer speed running at 8 MHz.
	//#define BAUD 25
	// Set baud rate. 19.2 kbps trasfer speed running at 3.6864 MHz.
	//#define BAUD 11

	// interrupt + UART
#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun nachgeholt mit 4Mhz"
#endif

	// Berechnungen
#define BAUD 19200UL // Baudrate
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1) // clever runden // 12,520833333
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1))) // Reale Baudrate // 19230,769230769
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler. // 1001,5625
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#error Systematischer Fehler der Baudrate grsser 1% und damit zu hoch!
#endif

	UBRR0H = UBRR_VAL >> 8;
	UBRR0L = UBRR_VAL & 0xFF;

	//UCSR0A = 1<<UDRE0;
	/* Enable receiver and transmitter, rx and tx int */
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0); // rx,tx,rx interrupt
	/* Set frame format: 8data, 1stop bit */
	//UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00)             ;    // Asynchron 8N1


	// Flush Buffers
	UART_RxPtr = 0;
	UART_TxTail = 0;
	UART_TxHead = 0;
	UART_state = 0;
}

void disable_uart(){
	UBRR0H = 0x00;
	UBRR0L = 0x00;
	UCSR0B = 0x00;
};


/*! \brief send a byte.
 *
 *  Puts a byte in TX buffer and starts uart TX interrupt.
 *  If TX buffer is full it will hang until space.
 *
 *  \param data  Data to be sent.
 */
void uart_SendByte(char data){
	while((UCSR0A & (1<< UDRE0)) == 0) {};
	UDR0=data;
};

/*! \brief Sends a string.
 *
 *  Loops thru a string and send each byte with uart_SendByte.
 *  If TX buffer is full it will hang until space.
 *
 *  \param Str  String to be sent.
 */
void uart_SendString(char Str[])
{
	unsigned char n = 0;
	while(Str[n])
		uart_SendByte(Str[n++]);
}

/*! \brief Sends a integer.
 *
 *  Converts a integer to ASCII and sends it using uart_SendByte.
 *  If TX buffer is full it will hang until space.
 *
 *  \param x  Integer to be sent.
 */
void uart_SendInt(int x)
{
	static const char dec[] = "0123456789";
	unsigned int div_val = 10000;

	if (x < 0){
		x = - x;
		uart_SendByte('-');
	}
	while (div_val > 1 && div_val > x)
		div_val /= 10;
	do{
		uart_SendByte (dec[x / div_val]);
		x %= div_val;
		div_val /= 10;
	} while(div_val);
}


/*! \brief RX interrupt handler.
 *
 *  RX interrupt handler.
 *  RX interrupt always enabled.
 */
ISR(USART_RX_vect){
	unsigned char data;
	// Read the received data.
	data = UDR0;
	//uart_SendByte(data);
	// Put the data into RxBuf
	if(data=='$'){
		UART_RxPtr=0;
	} else if(data=='*') {
		status.cmd = TRUE;
	} else {
		UART_RxBuffer[UART_RxPtr ] = data;
		UART_RxBuffer[UART_RxPtr+1]=0x00;
		UART_RxPtr=(UART_RxPtr+1)%(UART_RX_BUFFER_SIZE-1);
	};

}
