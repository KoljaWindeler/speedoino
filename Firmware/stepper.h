/*
 * stepper.h
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#ifndef STEPPER_H_
#define STEPPER_H_

class speedo_stepper{

public:
	#define MOTOR_OVERWRITE_END_POS 1600
	speedo_stepper();
	~speedo_stepper();
	void init();
	void loop();
	bool go_to(int winkel,int accel,int speed);
	bool go_to(int winkel);
	int get_pos();
	void overwrite_pos(int new_pos);
	void get_motor_status(int* ist_pos, int* delay, int* status);
	int init_steps_to_go;

private:

};

#endif /* STEPPER_H_ */


