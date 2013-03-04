/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2011 Kolja Windeler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "global.h"

Speedo_CAN::Speedo_CAN(){
	failed=false;
	message_available=false;
};

Speedo_CAN::~Speedo_CAN(){
};

// Already in use in sensor class...
//ISR(PCINT2_vect ){
//	pSensors->check_inputs();
//}

void Speedo_CAN::init(){
	// Interrupt for CAN Interface active
	//TODO
	// interrupt auf pk4, pcint20
	DDRK &= ~(1<<PK4); // input

	/********************************************* MCP2515 SETUP ***********************************/
	// MCP2515 per Software Reset zuruecksetzten,
	// danach ist der MCP2515 im Configuration Mode
	PORT_CS &= ~(1<<P_CS);
	spi_putc( SPI_CMD_RESET );
	_delay_ms(1);
	PORT_CS |= (1<<P_CS);

	// etwas warten bis sich der MCP2515 zurueckgesetzt hat
	_delay_ms(10);

	/*
	 *  Einstellen des Bit Timings
	 *
	 *  Fosc       = 16MHz
	 *  BRP        = 7                (teilen durch 8)
	 *  TQ = 2 * (BRP + 1) / Fosc  (=> 1 uS)
	 *
	 *  Sync Seg   = 1TQ
	 *  Prop Seg   = (PRSEG + 1) * TQ  = 1 TQ
	 *  Phase Seg1 = (PHSEG1 + 1) * TQ = 3 TQ
	 *  Phase Seg2 = (PHSEG2 + 1) * TQ = 3 TQ
	 *
	 *  Bus speed  = 1 / (Total # of TQ) * TQ
	 *             = 1 / 8 * TQ = 125 kHz TODO das Hier sollte 500kHz sein!!
	 */

	// BRP = 7
	mcp2515_write_register( CNF1, (1<<BRP0)|(1<<BRP1)|(1<<BRP2) );

	// Prop Seg und Phase Seg1 einstellen
	mcp2515_write_register( CNF2, (1<<BTLMODE)|(1<<PHSEG11) );

	// Wake-up Filter deaktivieren, Phase Seg2 einstellen
	mcp2515_write_register( CNF3, (1<<PHSEG21) );

	// Aktivieren der Rx Buffer Interrupts
	mcp2515_write_register( CANINTE, (1<<RX1IE)|(1<<RX0IE) );

	/*
	 *  Einstellen der Filter
	 */

	// Buffer 0 : Empfangen aller Nachrichten TODO: Filter reduzieren auf id 7e?
	mcp2515_write_register( RXB0CTRL, (1<<RXM1)|(1<<RXM0) );

	// Buffer 1 : Empfangen aller Nachrichten TODO: Filter reduzieren auf id 7e?
	mcp2515_write_register( RXB1CTRL, (1<<RXM1)|(1<<RXM0) );

	// Alle Bits der Empfangsmaske loeschen,
	// damit werden alle Nachrichten empfangen
	mcp2515_write_register( RXM0SIDH, 0 );
	mcp2515_write_register( RXM0SIDL, 0 );
	mcp2515_write_register( RXM0EID8, 0 );
	mcp2515_write_register( RXM0EID0, 0 );

	mcp2515_write_register( RXM1SIDH, 0 );
	mcp2515_write_register( RXM1SIDL, 0 );
	mcp2515_write_register( RXM1EID8, 0 );
	mcp2515_write_register( RXM1EID0, 0 );

	/*
	 *  Einstellen der Pin Funktionen
	 */

	// Deaktivieren der Pins RXnBF Pins (High Impedance State)
	mcp2515_write_register( BFPCTRL, 0 );

	// TXnRTS Bits als Inputs schalten
	mcp2515_write_register( TXRTSCTRL, 0 );

	// Device zurueck in den normalen Modus versetzten
	mcp2515_bit_modify( CANCTRL, 0xE0, 0);
	/********************************************* MCP2515 SETUP ***********************************/

	last_received=millis();
};

void Speedo_CAN::shutdown(){

};

bool Speedo_CAN::check_vars(){
	return true; // returns true if a failure occures
};

/********************************************* CAN VALUE GETTER ***********************************/
int Speedo_CAN::get_air_temp(){
	if(millis()-last_received<1000){
		return can_air_temp;
	}
	return 0;
};

int Speedo_CAN::get_oil_temp(){
	return 0; // not in use
};

int Speedo_CAN::get_water_temp(){
	if(millis()-last_received<1000){
		return can_water_temp;
	}
	return 0;
};

unsigned int Speedo_CAN::get_Speed(){
	if(millis()-last_received<1000){
		return can_speed;
	}
	return 0;
};

unsigned int Speedo_CAN::get_RPM(){
	if(millis()-last_received<1000){
		return can_rpm;
	}
	return 0;
};
/********************************************* CAN VALUE GETTER ***********************************/

/********************************************* CAN FUNCTIONS ***********************************/
void Speedo_CAN::request(char MSG){
	//check valid msg
	if(MSG!=CAN_AIR_TEMP && MSG!=CAN_WATER_TEMP && MSG!=CAN_RPM && MSG!=CAN_SPEED){
		return;
	}

	message.id = 0x07DF; // motor steuergerät .. TODO: Ist die fix?
	message.rtr = 0;

	message.length = 3;
	message.data[0] = 0x02; //frame im Datenstrom
	message.data[1] = 0x01; //mode 01
	message.data[2] = MSG;

	// Nachricht verschicken
	can_send_message(&message);
};

void Speedo_CAN::process_incoming_messages(){
	while(can_get_message(&message)!=0xff){ //0xff=no more frames available
		if(message.id==0x07E8){ // TODO: is this fix?
			if(message.length>2){ // must be at least 2 chars to check [1]
				if(message.data[1]==0x41){ // Rainer said it must be 0x41!!
					last_received=millis();
					if(message.data[2]==CAN_RPM){
						can_rpm=(message.data[3]<<6)|(message.data[4]>>2);
					} else if(message.data[2]==CAN_SPEED){
						can_speed=message.data[3];
					} else if(message.data[2]==CAN_AIR_TEMP){
						can_air_temp=message.data[3]-40;
					} else if(message.data[2]==CAN_WATER_TEMP){
						can_water_temp=message.data[3]-40;
					}
				}
			}
		}
	}
}

uint8_t Speedo_CAN::spi_putc( uint8_t data ){
	// Sendet ein Byte
	SPDR = data;

	// Wartet bis Byte gesendet wurde
	while( !( SPSR & (1<<SPIF) ) ){};

	return SPDR;
}

void Speedo_CAN::mcp2515_write_register( uint8_t adress, uint8_t data ){
	// /CS des MCP2515 auf Low ziehen
	PORT_CS &= ~(1<<P_CS);

	spi_putc(SPI_CMD_WRITE);
	spi_putc(adress);
	spi_putc(data);

	// /CS Leitung wieder freigeben
	PORT_CS |= (1<<P_CS);
}

uint8_t Speedo_CAN::mcp2515_read_register(uint8_t adress){
	uint8_t data;

	// /CS des MCP2515 auf Low ziehen
	PORT_CS &= ~(1<<P_CS);

	spi_putc(SPI_CMD_READ);
	spi_putc(adress);

	data = spi_putc(0xff);

	// /CS Leitung wieder freigeben
	PORT_CS |= (1<<P_CS);

	return data;
}

void Speedo_CAN::mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data){
	// /CS des MCP2515 auf Low ziehen
	PORT_CS &= ~(1<<P_CS);

	spi_putc(SPI_CMD_BIT_MODIFY);
	spi_putc(adress);
	spi_putc(mask);
	spi_putc(data);

	// /CS Leitung wieder freigeben
	PORT_CS |= (1<<P_CS);
}


uint8_t Speedo_CAN::can_send_message(CANMessage *p_message){
	uint8_t status, address;

	// Status des MCP2515 auslesen
	PORT_CS &= ~(1<<P_CS);
	spi_putc(SPI_CMD_READ_STATUS);
	status = spi_putc(0xff);
	spi_putc(0xff);
	PORT_CS |= (1<<P_CS);

	/* Statusbyte:
	 *
	 * Bit  Funktion
	 *  2   TXB0CNTRL.TXREQ
	 *  4   TXB1CNTRL.TXREQ
	 *  6   TXB2CNTRL.TXREQ
	 */

	if (bit_is_clear(status, 2)) {
		address = 0x00;
	}
	else if (bit_is_clear(status, 4)) {
		address = 0x02;
	}
	else if (bit_is_clear(status, 6)) {
		address = 0x04;
	}
	else {
		/* Alle Puffer sind belegt,
           Nachricht kann nicht verschickt werden */
		return 0;
	}

	PORT_CS &= ~(1<<P_CS);    // CS Low
	spi_putc(SPI_CMD_WRITE_TX | address);

	// Standard ID einstellen
	spi_putc((uint8_t) (p_message->id>>3));
	spi_putc((uint8_t) (p_message->id<<5));

	// Extended ID
	spi_putc(0x00);
	spi_putc(0x00);

	uint8_t length = p_message->length;

	if (length > 8) {
		length = 8;
	}

	// Ist die Nachricht ein "Remote Transmit Request" ?
	if (p_message->rtr)
	{
		/* Ein RTR hat zwar eine Laenge,
           aber enthaelt keine Daten */

		// Nachrichten Laenge + RTR einstellen
		spi_putc((1<<RTR) | length);
	}
	else
	{
		// Nachrichten Laenge einstellen
		spi_putc(length);

		// Daten
		for (uint8_t i=0;i<length;i++) {
			spi_putc(p_message->data[i]);
		}
	}
	PORT_CS |= (1<<P_CS);      // CS auf High

	asm volatile ("nop");

	/* CAN Nachricht verschicken
       die letzten drei Bit im RTS Kommando geben an welcher
       Puffer gesendet werden soll */
	PORT_CS &= ~(1<<P_CS);    // CS wieder Low
	if (address == 0x00) {
		spi_putc(SPI_CMD_RTS | 0x01);
	} else {
		spi_putc(SPI_CMD_RTS | address);
	}
	PORT_CS |= (1<<P_CS);      // CS auf High

	return 1;
}

uint8_t Speedo_CAN::mcp2515_read_rx_status(void){
	uint8_t data;

	// /CS des MCP2515 auf Low ziehen
	PORT_CS &= ~(1<<P_CS);

	spi_putc(SPI_CMD_RX_STATUS);
	data = spi_putc(0xff);

	// Die Daten werden noch einmal wiederholt gesendet,
	// man braucht also nur eins der beiden Bytes auswerten.
	spi_putc(0xff);

	// /CS Leitung wieder freigeben
	PORT_CS |= (1<<P_CS);

	return data;
}

uint8_t Speedo_CAN::can_get_message(CANMessage *p_message){
	// Status auslesen
	uint8_t status = mcp2515_read_rx_status();

	if (bit_is_set(status,6))
	{
		// Nachricht in Puffer 0

		PORT_CS &= ~(1<<P_CS);    // CS Low
		spi_putc(SPI_CMD_READ_RX);
	}
	else if (bit_is_set(status,7))
	{
		// Nachricht in Puffer 1

		PORT_CS &= ~(1<<P_CS);    // CS Low
		spi_putc(SPI_CMD_READ_RX | 0x04);
	}
	else {// TODO was ist denn mit Puffer 2 ?
		/* Fehler: Keine neue Nachricht vorhanden */
		return 0xff;
	}

	// Standard ID auslesen
	p_message->id =  (uint16_t) spi_putc(0xff) << 3; // 11 bit adressen
	p_message->id |= (uint16_t) spi_putc(0xff) >> 5;

	spi_putc(0xff);
	spi_putc(0xff);

	// Laenge auslesen
	uint8_t length = spi_putc(0xff) & 0x0f;
	p_message->length = length;

	// Daten auslesen
	for (uint8_t i=0;i<length;i++) {
		p_message->data[i] = spi_putc(0xff);
	}

	PORT_CS |= (1<<P_CS);

	if (bit_is_set(status,3)) {
		p_message->rtr = 1;
	} else {
		p_message->rtr = 0;
	}

	// Interrupt Flag loeschen
	if (bit_is_set(status,6)) {
		mcp2515_bit_modify(CANINTF, (1<<RX0IF), 0);
	} else {
		mcp2515_bit_modify(CANINTF, (1<<RX1IF), 0);
	}

	return (status & 0x07);
}
/********************************************* CAN FUNCTIONS ***********************************/
