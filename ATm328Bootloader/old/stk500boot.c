/*****************************************************************************
Title:     STK500v2 compatible bootloader
Author:    Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:      $Id: stk500boot.c,v 1.15 2008/05/25 09:09:30 peter Exp $
Compiler:  avr-gcc 4.x / avr-libc 1.4.x 
Hardware:  All AVRs with bootloader support, tested with ATmega8
License:   GNU General Public License 

DESCRIPTION:
    This program allows an AVR with bootloader capabilities to 
    read/write its own Flash/EEprom. To enter Programming mode   
    an input pin is checked. If this pin is pulled low, programming mode  
    is entered. If not, normal execution is done from $0000 
    "reset" vector in Application area.
    Size < 500 words, fits into a 512 word bootloader section 
	when compiled with avr-gcc 4.1

USAGE:
    - Set AVR MCU type and clock-frequency (F_CPU) in the Makefile.
    - Set baud rate below (AVRISP only works with 115200 bps)
    - compile/link the bootloader with the supplied Makefile
    - program the "Boot Flash section size" (BOOTSZ fuses),
      for boot-size 512 words:  program BOOTSZ1
    - enable the BOOT Reset Vector (program BOOTRST)
    - Upload the hex file to the AVR using any ISP programmer
    - Program Boot Lock Mode 3 (program BootLock 11 and BootLock 12 lock bits)
    - Reset your AVR while keeping PROG_PIN pulled low
    - Start AVRISP Programmer (AVRStudio/Tools/Program AVR)
    - AVRISP will detect the bootloader
    - Program your application FLASH file and optional EEPROM file using AVRISP

Note: 
    Erasing the device without flashing, through AVRISP GUI button "Erase Device" 
    is not implemented, due to AVRStudio limitations.
    Flash is always erased before programming.

    Normally the bootloader accepts further commands after programming. 
    The bootloader exits and starts applicaton code after programming 
    when ENABLE_LEAVE_BOOTLADER is defined.
    Use Auto Programming mode to programm both flash and eeprom, 
    otherwise bootloader will exit after flash programming.

	AVRdude:
	Please uncomment #define REMOVE_CMD_SPI_MULTI when using AVRdude.
	Comment #define REMOVE_PROGRAM_LOCK_BIT_SUPPORT to reduce code size
	Read Fuse Bits and Read/Write Lock Bits is not supported

NOTES:
    Based on Atmel Application Note AVR109 - Self-programming
    Based on Atmel Application Note AVR068 - STK500v2 Protocol    

LICENSE:
    Copyright (C) 2006 Peter Fleury

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

 *****************************************************************************/
#include <avr/interrupt.h>
//#include <avr/boot.h>
#include <util/delay.h>
#include "command.h"

#include <inttypes.h>
#include <avr/io.h>
//#include <avr/iom328p.h>
#include <avr/pgmspace.h>

// <avr/boot.h> uses sts instructions, but this version uses out instructions
// This saves cycles and program memory.
#include "boot.h"
// We don't use <avr/wdt.h> as those routines have interrupt overhead we don't need.


/*
 * Uncomment the following lines to save code space 
 */
//#define REMOVE_PROG_PIN_PULLUP           // disable internal pullup, use external 
//#define REMOVE_CMD_SPI_MULTI             // disable processing of SPI_MULTI commands

/*
 *  Uncomment to leave bootloader and jump to application after programming.
 */
//#define ENABLE_LEAVE_BOOTLADER           

/* 
 * Pin "PROG_PIN" on port "PROG_PORT" has to be pulled low
 * (active low) to start the bootloader 
 * uncomment #define REMOVE_PROG_PIN_PULLUP if using an external pullup
 */
#define PROG_PORT  PORTD
#define PROG_DDR   DDRD
#define PROG_IN    PIND
#define PROG_PIN   PIND2



/*
 * define CPU frequency in Mhz here if not defined in Makefile 
 */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif



/*
 * UART Baudrate, AVRStudio AVRISP only accepts 115200 bps
 */
#define BAUDRATE 115200


/*
 * HW and SW version, reported to AVRISP, must match version of AVRStudio
 */
#define CONFIG_PARAM_BUILD_NUMBER_LOW	0
#define CONFIG_PARAM_BUILD_NUMBER_HIGH	0
#define CONFIG_PARAM_HW_VER				0x0F
#define CONFIG_PARAM_SW_MAJOR			2
#define CONFIG_PARAM_SW_MINOR			0x0A



/*
 * Calculate the address where the bootloader starts from FLASHEND and BOOTSIZE
 * (adjust BOOTSIZE below and BOOTLOADER_ADDRESS in Makefile if you want to change the size of the bootloader)
 */
#define BOOTSIZE 2048
#define APP_END  (FLASHEND -(2*BOOTSIZE) + 1)



/*
 * Signature bytes are not available in avr-gcc io_xxx.h
 */
#define SIGNATURE_BYTES 0x1E950F



/* 
 *  ATMega with two USART, select USART for bootloader using USE_USART1 define
 */ 
#define	 UART_BAUD_RATE_LOW	     UBRR0L
#define UART_BAUD_RATE_HIGH	     UBRR0H
#define	 UART_STATUS_REG	     UCSR0A
#define	 UART_CONTROL_REG	     UCSR0B
#define	 UART_ENABLE_TRANSMITTER TXEN0
#define	 UART_ENABLE_RECEIVER	 RXEN0
#define	 UART_TRANSMIT_COMPLETE	 TXC0
#define	 UART_RECEIVE_COMPLETE	 RXC0
#define	 UART_DATA_REG	         UDR0
#define UART_DOUBLE_SPEED       U2X0



/*
 * Macros to map the new ATmega88/168 EEPROM bits
 */
#ifdef EEMPE        				        		            
#define EEMWE EEMPE
#define EEWE  EEPE
#endif


/*
 * Macro to calculate UBBR from XTAL and baudrate
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*16.0)-1.0+0.5)


/*
 * States used in the receive state machine
 */
#define	 ST_START		0
#define	 ST_GET_SEQ_NUM	1
#define ST_MSG_SIZE_1	2
#define ST_MSG_SIZE_2	3
#define ST_GET_TOKEN	4
#define ST_GET_DATA		5
#define ST_GET_CHECK	6
#define ST_PROCESS		7


/*
 * use 16bit address variable for ATmegas with <= 64K flash
 */
#if defined(RAMPZ)
typedef uint32_t address_t;
#else
typedef uint16_t address_t;
#endif


/*
 * function prototypes
 */ 
static void sendchar(char c);
static unsigned char recchar(void);


/*
 * since this bootloader is not linked against the avr-gcc crt1 functions,
 * to reduce the code size, we need to provide our own initialization
 */
void __jumpMain     (void) __attribute__ ((naked)) __attribute__ ((section (".init9")));

void __jumpMain(void)
{    
	asm volatile ( ".set __stack, %0" :: "i" (RAMEND) );

	/* init stack here, bug WinAVR 20071221 does not init stack based on __stack */
	asm volatile ("ldi r24,%0":: "M" (RAMEND & 0xFF));
	asm volatile ("ldi r25,%0":: "M" (RAMEND >> 8));
	asm volatile ("out __SP_H__,r25" ::);
	asm volatile ("out __SP_L__,r24" ::);

	asm volatile ( "clr __zero_reg__" );                       // GCC depends on register r1 set to 0
	asm volatile ( "out %0, __zero_reg__" :: "I" (_SFR_IO_ADDR(SREG)) );  // set SREG to 0
#ifndef REMOVE_PROG_PIN_PULLUP	 
	PROG_PORT |= (1<<PROG_PIN);		                           // Enable internal pullup
#endif    
	asm volatile ( "rjmp main");                               // jump to main()
}


/*
 * send single byte to USART, wait until transmission is completed
 */
static void sendchar(char c)
{
	while((UCSR0A & (1<< UDRE0)) == 0) {};
	UDR0=c;
}

//************************************************************************
static int	Serial_Available(void)
{
	return(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE));	// wait for data
}

/*
 * Read single byte from USART, block if no data available
 */
static unsigned char recchar(void)
{
	while(!(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE)));  // wait for data
	return UART_DATA_REG;
}


int main(void) __attribute__ ((OS_main));
int main(void)
{
	address_t       	address = 0;
	address_t       	eraseAddress = 0;
	unsigned char  	msgParseState;
	unsigned int   	i = 0;
	unsigned char   	checksum = 0;
	unsigned char   	seqNum = 0;
	unsigned int    	msgLength = 0;
	unsigned char   	msgBuffer[285];
	unsigned char   	c, *p;
	unsigned char   	isLeave = 0;

	unsigned long		boot_timeout=999;
	unsigned long		boot_timer=0;
	unsigned int		boot_state=0;

	/*
	 * Init UART
	 * set baudrate and enable USART receiver and transmiter without interrupts
	 */

	// Berechnungen
#define BAUD 115200UL // Baudrate
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1) // clever runden // 3,840277778
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1))) // Reale Baudrate // 125000
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler. // 1001,5625
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#warning Systematischer Fehler der Baudrate grsser 1% und damit zu hoch!
#endif

	UBRR0H = UBRR_VAL >> 8;
	UBRR0L = UBRR_VAL & 0xFF;
	UART_CONTROL_REG   = (1 << UART_ENABLE_RECEIVER) | (1 << UART_ENABLE_TRANSMITTER);

	DDRD |= (1<<PIND5) |(1<<PIND6) |(1<<PIND7); // 3x led AUSGANG,
	PORTD |= (1<<PIND5); // 3x led AUSGANG,


	msgLength		=	11;
	msgBuffer[0] 	=	CMD_SIGN_ON;
	msgBuffer[1] 	=	STATUS_CMD_OK;
	msgBuffer[2] 	=	8;
	msgBuffer[3] 	=	'A';
	msgBuffer[4] 	=	'V';
	msgBuffer[5] 	=	'R';
	msgBuffer[6] 	=	'I';
	msgBuffer[7] 	=	'S';
	msgBuffer[8] 	=	'P';
	msgBuffer[9] 	=	'_';
	msgBuffer[10]	=	'2';
	sendchar(MESSAGE_START);
	checksum	=	MESSAGE_START^0;

	sendchar(seqNum);
	checksum	^=	seqNum;

	c			=	((msgLength>>8)&0xFF);
	sendchar(c);
	checksum	^=	c;

	c			=	msgLength&0x00FF;
	sendchar(c);
	checksum ^= c;

	sendchar(TOKEN);
	checksum ^= TOKEN;

	p	=	msgBuffer;
	while ( msgLength )
	{
		c	=	*p++;
		sendchar(c);
		checksum ^=c;
		msgLength--;
	}
	sendchar(checksum);
	seqNum++;
	PORTD |= (1<<PIND6); // 3x led AUSGANG,

	while (boot_state==0)
	{
		while ((!(Serial_Available())) && (boot_state == 0))		// wait for data
		{
			_delay_ms(0.001);
			boot_timer++;

			if(boot_timer<50) PORTD |= (1<<PIND5); // 3x led AUSGANG,
			else PORTD &= ~(1<<PIND5);

			if(boot_timer>100) boot_timer=0;

			if (boot_timer > boot_timeout)
			{
				boot_state	=	1; // (after ++ -> boot_state=2 bootloader timeout, jump to main 0x00000 )
			}
		}
		boot_state++; // ( if boot_state=1 bootloader received byte from UART, enter bootloader mode)
	}

	PORTD |= (1<<PIND7); // 3x led AUSGANG,

	if (boot_state==1)
	{
		/* main loop */
		while(!isLeave)
		{
			/*
			 * Collect received bytes to a complete message
			 */
			msgParseState = ST_START;
			while ( msgParseState != ST_PROCESS )
			{
				c = recchar();
				switch (msgParseState)
				{
				case ST_START:
					if( c == MESSAGE_START )
					{
						msgParseState = ST_GET_SEQ_NUM;
						checksum = MESSAGE_START^0;
					}
					break;

				case ST_GET_SEQ_NUM:
					if ( (c == 1) || (c == seqNum) )
					{
						seqNum = c;
						msgParseState = ST_MSG_SIZE_1;
						checksum ^= c;
					}
					else
					{
						msgParseState = ST_START;
					}
					break;

				case ST_MSG_SIZE_1:
					msgLength = (unsigned int)c<<8;
					msgParseState = ST_MSG_SIZE_2;
					checksum ^= c;
					break;

				case ST_MSG_SIZE_2:
					msgLength |= c;
					msgParseState = ST_GET_TOKEN;
					checksum ^= c;
					break;

				case ST_GET_TOKEN:
					if ( c == TOKEN )
					{
						msgParseState = ST_GET_DATA;
						checksum ^= c;
						i = 0;
					}
					else
					{
						msgParseState = ST_START;
					}
					break;

				case ST_GET_DATA:
					msgBuffer[i++] = c;
					checksum ^= c;
					if ( i == msgLength )
					{
						msgParseState = ST_GET_CHECK;
					}
					break;

				case ST_GET_CHECK:
					if( c == checksum )
					{
						msgParseState = ST_PROCESS;
					}
					else
					{
						msgParseState = ST_START;
					}
					break;
				}//switch
			}//while(msgParseState)

			/*
			 * Now process the STK500 commands, see Atmel Appnote AVR068
			 */

			switch (msgBuffer[0])
			{
			case CMD_SPI_MULTI:
			{
				unsigned char answerByte = 0;

				// only Read Signature Bytes implemented, return dummy value for other instructions
				if ( msgBuffer[4]== 0x30 )
				{
					unsigned char signatureIndex = msgBuffer[6];

					if ( signatureIndex == 0 )
						answerByte = (SIGNATURE_BYTES >>16) & 0x000000FF;
					else if ( signatureIndex == 1 )
						answerByte = (SIGNATURE_BYTES >> 8) & 0x000000FF;
					else
						answerByte = SIGNATURE_BYTES & 0x000000FF;
				}
				msgLength = 7;
				msgBuffer[1] = STATUS_CMD_OK;
				msgBuffer[2] = 0;
				msgBuffer[3] = msgBuffer[4];  // Instruction Byte 1
				msgBuffer[4] = msgBuffer[5];  // Instruction Byte 2
				msgBuffer[5] = answerByte;
				msgBuffer[6] = STATUS_CMD_OK;
			}
			break;
			case CMD_SIGN_ON:
				msgLength = 11;
				msgBuffer[1]  = STATUS_CMD_OK;
				msgBuffer[2]  = 8;
				msgBuffer[3]  = 'A';
				msgBuffer[4]  = 'V';
				msgBuffer[5]  = 'R';
				msgBuffer[6]  = 'I';
				msgBuffer[7]  = 'S';
				msgBuffer[8]  = 'P';
				msgBuffer[9]  = '_';
				msgBuffer[10] = '2';
				break;

			case CMD_GET_PARAMETER:
			{
				unsigned char value;

				switch(msgBuffer[1])
				{
				case PARAM_BUILD_NUMBER_LOW:
					value = CONFIG_PARAM_BUILD_NUMBER_LOW;
					break;
				case PARAM_BUILD_NUMBER_HIGH:
					value = CONFIG_PARAM_BUILD_NUMBER_HIGH;
					break;
				case PARAM_HW_VER:
					value = CONFIG_PARAM_HW_VER;
					break;
				case PARAM_SW_MAJOR:
					value = CONFIG_PARAM_SW_MAJOR;
					break;
				case PARAM_SW_MINOR:
					value = CONFIG_PARAM_SW_MINOR;
					break;
				default:
					value = 0;
					break;
				}
				msgLength = 3;
				msgBuffer[1] = STATUS_CMD_OK;
				msgBuffer[2] = value;
			}
			break;

			case CMD_LEAVE_PROGMODE_ISP:
			case CMD_ENTER_PROGMODE_ISP:
			case CMD_SET_PARAMETER:
				msgLength = 2;
				msgBuffer[1] = STATUS_CMD_OK;
				break;

			case CMD_READ_SIGNATURE_ISP:
			{
				unsigned char signatureIndex = msgBuffer[4];
				unsigned char signature;

				if ( signatureIndex == 0 )
					signature = (SIGNATURE_BYTES >>16) & 0x000000FF;
				else if ( signatureIndex == 1 )
					signature = (SIGNATURE_BYTES >> 8) & 0x000000FF;
				else
					signature = SIGNATURE_BYTES & 0x000000FF;

				msgLength = 4;
				msgBuffer[1] = STATUS_CMD_OK;
				msgBuffer[2] = signature;
				msgBuffer[3] = STATUS_CMD_OK;
			}
			break;

			case CMD_READ_LOCK_ISP:
				msgLength = 4;
				msgBuffer[1] = STATUS_CMD_OK;
				msgBuffer[2] = boot_lock_fuse_bits_get( GET_LOCK_BITS );
				msgBuffer[3] = STATUS_CMD_OK;
				break;

			case CMD_READ_FUSE_ISP:
			{
				unsigned char fuseBits;

				if ( msgBuffer[2] == 0x50 )
				{
					if ( msgBuffer[3] == 0x08 )
						fuseBits = boot_lock_fuse_bits_get( GET_EXTENDED_FUSE_BITS );
					else
						fuseBits = boot_lock_fuse_bits_get( GET_LOW_FUSE_BITS );
				}
				else
				{
					fuseBits = boot_lock_fuse_bits_get( GET_HIGH_FUSE_BITS );
				}
				msgLength = 4;
				msgBuffer[1] = STATUS_CMD_OK;
				msgBuffer[2] = fuseBits;
				msgBuffer[3] = STATUS_CMD_OK;
			}
			break;

			case CMD_CHIP_ERASE_ISP:
				eraseAddress = 0;
				msgLength = 2;
				msgBuffer[1] = STATUS_CMD_OK;
				break;

			case CMD_LOAD_ADDRESS:
				address = ( ((msgBuffer[3])<<8)|(msgBuffer[4]) )<<1;  //convert word to byte address
				msgLength = 2;
				msgBuffer[1] = STATUS_CMD_OK;
				break;

			case CMD_PROGRAM_FLASH_ISP:
			case CMD_PROGRAM_EEPROM_ISP:
			{
				unsigned int  size = (((unsigned int)msgBuffer[1])<<8) | msgBuffer[2];
				unsigned char *p = msgBuffer+10;
				unsigned int  data;
				unsigned char highByte, lowByte;
				address_t     tempaddress = address;


				if ( msgBuffer[0] == CMD_PROGRAM_FLASH_ISP )
				{
					// erase only main section (bootloader protection)
					if  (  eraseAddress < APP_END )
					{
						// If we are in RWW section, immediately start page erase
						__boot_page_erase_short((uint16_t)(void*)address);
						//						boot_page_erase(eraseAddress);	// Perform page erase
						//						boot_spm_busy_wait();		// Wait until the memory is erased.
						//						eraseAddress += SPM_PAGESIZE;    // point to next page to be erase
						msgLength = 2;
						msgBuffer[1] = STATUS_CMD_OK;
					} else {
						msgLength = 2;
						msgBuffer[1] = STATUS_CMD_FAILED;
					}

					do{}while(((SPMCSR)& (1<<SELFPRGEN)));


					// Copy buffer into programming buffer
					uint8_t *bufPtr=msgBuffer+10;
					uint16_t addrPtr = (uint16_t)(void*)address;
					do {
						uint16_t a;
						a = *bufPtr++;
						a |= (*bufPtr++) << 8;
						__boot_page_fill_short((uint16_t)(void*)addrPtr,a);
						addrPtr += 2;
						size -= 2;			// Reduce number of bytes to write by two
					} while (size);

					// Write from programming buffer
					__boot_page_write_short((uint16_t)(void*)address);

					do{}while(((SPMCSR)& (1<<SELFPRGEN)));

#if defined(RWWSRE)
					// Reenable read access to flash
					boot_rww_enable();
#endif
					//
					//					/* Write FLASH */
					//					do {
					//						lowByte   = *p++;
					//						highByte  = *p++;
					//
					//						data =  (highByte << 8) | lowByte;
					//						boot_page_fill(address,data);
					//
					//						address = address + 2;  	// Select next word in memory
					//						size -= 2;			// Reduce number of bytes to write by two
					//					} while(size);			// Loop until all bytes written
					//
					//					boot_page_write(tempaddress);
					//					boot_spm_busy_wait();
					//					boot_rww_enable();				// Re-enable the RWW section
				}
				else
				{
					/* write EEPROM */
					do {
						EEARL = address;			// Setup EEPROM address
						EEARH = (address >> 8);
						address++;					// Select next EEPROM byte

						EEDR= *p++;				    // get byte from buffer
						EECR |= (1<<EEMWE);			// Write data into EEPROM
						EECR |= (1<<EEWE);

						while (EECR & (1<<EEWE));	// Wait for write operation to finish
						size--;						// Decrease number of bytes to write
					} while(size);					// Loop until all bytes written
					msgLength = 2;
					msgBuffer[1] = STATUS_CMD_OK;
				}

			}
			break;

			case CMD_READ_FLASH_ISP:
			case CMD_READ_EEPROM_ISP:
			{
				unsigned int  size = (((unsigned int)msgBuffer[1])<<8) | msgBuffer[2];
				unsigned char *p = msgBuffer+1;
				msgLength = size+3;

				*p++ = STATUS_CMD_OK;
				if (msgBuffer[0] == CMD_READ_FLASH_ISP )
				{
					unsigned int data;

					// Read FLASH
					do {
#if defined(RAMPZ)
						data = pgm_read_word_far(address);
#else
						data = pgm_read_word_near(address);
#endif
						*p++ = (unsigned char)data;         //LSB
						*p++ = (unsigned char)(data >> 8);	//MSB
						address    += 2;  	 // Select next word in memory
						size -= 2;
					}while (size);
				}
				else
				{
					/* Read EEPROM */
					do {
						EEARL = address;			// Setup EEPROM address
						EEARH = ((address >> 8));
						address++;					// Select next EEPROM byte
						EECR |= (1<<EERE);			// Read EEPROM
						*p++ = EEDR;				// Send EEPROM data
						size--;
					}while(size);
				}
				*p++ = STATUS_CMD_OK;
			}
			break;

			default:
				msgLength = 2;
				msgBuffer[1] = STATUS_CMD_FAILED;
				break;
			}

			/*
			 * Now send answer message back
			 */
			sendchar(MESSAGE_START);
			checksum = MESSAGE_START^0;

			sendchar(seqNum);
			checksum ^= seqNum;

			c = ((msgLength>>8)&0xFF);
			sendchar(c);
			checksum ^= c;

			c = msgLength&0x00FF;
			sendchar(c);
			checksum ^= c;

			sendchar(TOKEN);
			checksum ^= TOKEN;

			p = msgBuffer;
			while ( msgLength )
			{
				c = *p++;
				sendchar(c);
				checksum ^=c;
				msgLength--;
			}
			sendchar(checksum);
			seqNum++;

		}//for
	}

	/*
	 * Now leave bootloader
	 */
#ifndef REMOVE_PROG_PIN_PULLUP	
	PROG_PORT &= ~(1<<PROG_PIN);    // set to default
#endif	
	boot_rww_enable();              // enable application section

	// Jump to Reset vector in Application Section
	// (clear register, push this register to the stack twice = adress 0x0000/words, and return to this address) 
	asm volatile (
			"clr r1" "\n\t"
			"push r1" "\n\t"
			"push r1" "\n\t"
			"ret"     "\n\t"
			::);

	/*
	 * Never return to stop GCC to generate exit return code
	 * Actually we will never reach this point, but the compiler doesn't
	 * understand this
	 */
	for(;;);
}
