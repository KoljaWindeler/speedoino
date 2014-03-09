/*
 * sensors.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#define HIGH_BEAM_PIN PK2
#define NEUTRAL_GEAR_PIN PK1
#define OIL_PRESSURE_PIN PK0

#define FLASHER_LEFT_PIN PE6
#define FLASHER_RIGHT_PIN PE7

#define SENSOR_AUTO 1
#define SENSOR_FORCE_CAN 2

#define SENSOR_SHORT_TO_GND 5
#define SENSOR_OPEN 9

#define RPM_TYPE_DIRECT 0
#define RPM_TYPE_FLAT 1
#define RPM_TYPE_FLAT_ROUNDED 2
#define RPM_TYPE_ROUNDED 3
#define RPM_TYPE_ROUNDED_FLAT 4

class Speedo_sensors{
public:
	Speedo_sensors(void);
	~Speedo_sensors();
	void init();
	void clear_vars();
	void check_vars();
	void single_read();
	void addinfo_show_loop();
	void check_inputs();
	float flatIt(int actual,unsigned char *counter, char max_counter, float old_flat);
	float flatIt_shift(int actual, uint8_t *counter, uint8_t shift, float old_flat);
	uint16_t flatIt_shift_mask(uint16_t actual, uint8_t shift, uint16_t old_flat, uint16_t nmask);
	void pull_values();

	unsigned int get_RPM(int mode); // 0=exact, 1=flated, 2=flatted_display_ready
	unsigned int get_speed(bool mag_if_possible);
	int get_water_temperature();
	int get_water_temperature_fail_status();
	int get_air_temperature();
	int get_oil_temperature();

	speedo_clock* m_clock;
	speedo_dz* m_dz;
	moped_blinker* m_blinker;
	speedo_gps* m_gps;
	speedo_temperature* m_temperature;
	speedo_fuel* m_fuel;
	speedo_speed* m_speed;
	speedo_reset* m_reset;
	speedo_gear* m_gear;
	speedo_voltage* m_voltage;
	Speedo_CAN* m_CAN;
	int8_t sensor_source;
	bool CAN_active;

	unsigned char last_int_state;
private:
	unsigned long fourty_Hz_timer;
	short fourty_Hz_counter;

	uint16_t rpm_flatted;
	uint8_t rpm_flatted_counter;

	unsigned long last_highbeam_on;
	unsigned long last_oil_off;
};
extern Speedo_sensors* pSensors;

#endif /* SENSORS_H_ */
