/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for uart.c.
 *
 * - File:               uart.h
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
 * $RCSfile: uart.h,v $
 * $Date: 2006/05/08 12:25:58 $
 *****************************************************************************/
#ifndef UART_H
#define UART_H

// UART Buffer Defines
#define UART_RX_BUFFER_SIZE 32 // 2,4,8,16,32,64,128 or 256 bytes
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
  #error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_SIZE 64 // 2,4,8,16,32,64,128 or 256 bytes
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
  #error TX buffer size is not a power of 2
#endif
 // UDR Empty Interrupt
#define SET_UDRIE (UCSR0B |=  (1<<UDRIE0))
#define CLR_UDRIE (UCSR0B &= ~(1<<UDRIE0))

void InitUART(void);
void uart_SendByte(unsigned char data);
void uart_SendString(unsigned char Tab[]);
void uart_SendInt(int Tall);
void uart_FlushRxBuffer(void);

//! Buffer with received string from uart.
extern unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];

#pragma vector=USART_RX_vect
__interrupt void UART_RX_interrupt( void );
#pragma vector=USART_UDRE_vect
__interrupt void UART_TX_interrupt( void );

#endif
