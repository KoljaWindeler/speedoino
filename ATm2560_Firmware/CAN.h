/*
 * CAN.h
 *
 *  Created on: 8.3.2013
 *      Author: jkw
 */

#ifndef CAN_H_
#define CAN_H_

#define CAN_RPM 0
#define CAN_SPEED 1
#define CAN_AIR_TEMP 2
#define CAN_WATER_TEMP 3

class Speedo_CAN{
public:
	Speedo_CAN(void);
	~Speedo_CAN();
	void init();
	void clear_vars();
	bool check_vars();
	int get_air_temp();
	int get_oil_temp();
	int get_water_temp();
	unsigned int get_RPM();
	unsigned int get_Speed();
	void shutdown();
	void request(char MSG);
	bool failed;
private:

};
extern Speedo_CAN* pCAN;

#endif /* CAN_H_ */
