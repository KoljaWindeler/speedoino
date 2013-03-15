/*
 * CAN.h
 *
 *  Created on: 8.3.2013
 *      Author: jkw
 */

#ifndef CAN_H_
#define CAN_H_
#include "stdint.h"

#define CAN_CURRENT_INFO 0x02
#define CAN_DTC 0x03

#define CAN_RPM 0x0C
#define CAN_SPEED 0x0D
#define CAN_AIR_TEMP 0x0F
#define CAN_WATER_TEMPERATURE 0x05

#define DDR_CS      DDRK
#define PORT_CS     PORTK
#define P_CS        5



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
	unsigned int get_RPM();
	unsigned int get_Speed();
	void shutdown();
	void request(char mode,char PID);
	bool failed;
	bool message_available;
	void process_incoming_messages();
	uint8_t can_get_message(CANMessage *p_message);// move me back to private

private:

	uint8_t can_send_message(CANMessage *p_message);

	uint8_t spi_putc( uint8_t data );

	uint8_t mcp2515_read_rx_status(void);
	uint8_t mcp2515_read_register(uint8_t adress);
	void mcp2515_write_register( uint8_t adress, uint8_t data );
	void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data);

	CANMessage message;
	int can_water_temp;
	unsigned int can_rpm;
	unsigned int can_speed;
	unsigned long last_received;
};
extern Speedo_CAN* pCAN;

#endif /* CAN_H_ */
