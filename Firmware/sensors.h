/*
 * sensors.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef SENSORS_H_
#define SENSORS_H_

class Speedo_sensors{
public:
	Speedo_sensors(void);
	~Speedo_sensors();
	void init();
	float flatIt(int actual, short* counter, int max_counter, float old_flat);

	speedo_clock* m_clock;
	speedo_dz* m_dz;
	moped_blinker* m_blinker;
	speedo_gps* m_gps;
	speedo_temperature* m_temperature;
	speedo_fuel* m_fuel;
	speedo_speed* m_speed;
	speedo_reset* m_reset;
	speedo_gear* m_gear;
	speedo_oiler* m_oiler;
	speedo_voltage* m_voltage;

};
extern Speedo_sensors* pSensors;

#endif /* SENSORS_H_ */
