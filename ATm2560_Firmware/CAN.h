/*
 * CAN.h
 *
 *  Created on: 8.3.2013
 *      Author: jkw
 */

#ifndef CAN_H_
#define CAN_H_
#include "stdint.h"

#define CAN_CURRENT_INFO 0x01
#define CAN_DTC 0x03

#define CAN_MIL_STATUS 0x01
#define CAN_RPM 0x0C
#define CAN_SPEED 0x0D
#define CAN_AIR_TEMP 0x0F
#define CAN_WATER_TEMPERATURE 0x05

// damn it, I had to move the connection from
// CS PK5 to PB5
// INT PK4 to PB4

#define CAN_DDR_CS_TILL_V7      	DDRK
#define CAN_PORT_CS_TILL_V7     	PORTK
#define CAN_PIN_CS_TILL_V7      	5
#define CAN_INTERRUPT_PIN_V7		PK4
#define CAN_INTERRUPT_PIN_PORT_V7 	PINK



#define CAN_DDR_CS_FROM_V8        DDRB
#define CAN_PORT_CS_FROM_V8       PORTB
#define CAN_PIN_CS_FROM_V8        5
#define CAN_INTERRUPT_PIN_FROM_V8 PB4
#define CAN_INTERRUPT_PIN_PORT_V8 PINB

#define SPEED_TRIPPLE 0


typedef struct
{
	uint16_t  id;
	uint8_t   rtr;
	uint8_t   length;
	uint8_t   data[8];
} CANMessage;

class Speedo_CAN{
public:
	Speedo_CAN(void);
	~Speedo_CAN();
	void init();
	int check_vars();
	int get_air_temp();
	int get_oil_temp();
	int get_water_temp();
	int get_CAN_missed_count();
	bool get_mil_active();
	int get_dtc_error_count();
	int get_dtc_error(int nr);
	unsigned int get_RPM();
	unsigned int get_Speed();
	void shutdown();
	void request(char mode,char PID);
	bool failed;
	bool message_available;
	void process_incoming_messages();
	uint8_t can_get_message(CANMessage *p_message);// move me back to private
	bool decode_dtc(char* char_buffer,char ECU_type);
	bool init_comm_possible(bool* CAN_active);

private:

	uint8_t can_send_message(CANMessage *p_message);

	uint8_t spi_putc( uint8_t data );

	uint8_t mcp2515_read_rx_status(void);
	uint8_t mcp2515_read_register(uint8_t adress);
	void mcp2515_write_register( uint8_t adress, uint8_t data );
	void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data);

	void set_cs_high(bool high);

	CANMessage message;
	int can_water_temp;
	unsigned int can_missed_count;
	unsigned int can_rpm;
	unsigned int can_speed;

	unsigned long last_received;
	unsigned long last_request;
	unsigned int can_dtc_errors_processed;
	char can_dtc_nr;
	unsigned int can_dtc_value;
	char can_dtc_error_count;
	bool can_mil_active;
};
extern Speedo_CAN* pCAN;

#endif /* CAN_H_ */
