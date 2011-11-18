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
	speedo_stepper();
	~speedo_stepper();
	void init();
	bool go_to(int winkel,int overwrite);

private:

};

#endif /* STEPPER_H_ */


