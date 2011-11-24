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

#define			refresh_interval 400                           // display refresh für den sprinttacho
public:
	speedo_sprint(void);
	~speedo_sprint();
	void init();
	void loop();
	bool    done;                              // der sprint ist noch nicht fertig
	bool    lock;                              // ist der sprint am laufen
private:
	bool	blink_show;                        // anzeigen oder nicht
	unsigned long start;                                   // start des sprints
	unsigned long end;                                     // ende des sprints
	unsigned long previousMillis;
};
extern speedo_sprint* pSprint;
///////////// sprint ///////////////////////////////////

#endif /* SPRINT_H_ */
