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
	bool go_to(int winkel);
	int get_pos();
	int init_steps_to_go;

private:
	void overwrite_pos(int new_pos);
};

#endif /* STEPPER_H_ */


