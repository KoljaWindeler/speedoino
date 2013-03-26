/*
 * sprint.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef SPRINT_H_
#define SPRINT_H_

///////////// sprint ///////////////////////////////////
class speedo_sprint{

#define SPRINT_WAIT_ON_STOP 0x00
#define SPRINT_READY_TO_GO 0x01
#define SPRINT_ACCEL 0x02
#define SPRINT_DONE 0x03
#define	REFRESH_INTERVAL 400                           // display refresh für den sprinttacho


public:
	speedo_sprint(void);
	~speedo_sprint();
	void loop();
	void prepare_startup();
private:
	char    status;
	bool	blink_show;                        // anzeigen oder nicht
	unsigned long start;                                   // start des sprints
	unsigned long end;                                     // ende des sprints
	unsigned long previousMillis;
};
extern speedo_sprint* pSprint;
///////////// sprint ///////////////////////////////////

#endif /* SPRINT_H_ */
