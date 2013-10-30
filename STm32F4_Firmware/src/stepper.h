/*
 * stepper.h
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#define ATM_DIV_FACTOR 23.148148148

class speedo_stepper{

public:
#define MOTOR_OVERWRITE_END_POS 864
#define TMC222_DIV_FACTOR 1.45 // rechnerisch 1.48
	speedo_stepper();
	~speedo_stepper();
	void init();
	void loop();
	bool go_to(int winkel,int accel,int speed);
	bool go_to(int winkel);
	int get_pos();
	void overwrite_pos(int new_pos);
	void get_motor_status(int* ist_pos, int* delay, int* status);
	void get_atm_motor_status(int* ist_pos, int* delay, int* status);
	void run_calibration(void);
	int init_steps_to_go;
	void startup();
	int shown_mode;

private:
};

#endif /* STEPPER_H_ */


