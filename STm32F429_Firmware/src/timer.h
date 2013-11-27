/*
 * timer.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef TIMER_H_
#define TIMER_H_

///////////// timer ///////////////
class timer{
public:
	timer(void);
	~timer();
	void every_qsec();
	void every_sec();
	void every_custom();

	void init_mem (void);
private:
	unsigned long every_second_timer;   // Zeitstempel vom letzten ausführen der "every_sec" funktion
	unsigned long every_qsecond_timer;   // Zeitstempel vom letzten ausführen der "every_qsec" funktion
	unsigned long every_custom_timer;   // Zeitstempel vom letzten ausführen der "every_qsec" funktion
};
extern timer Timer;
///////////// timer ///////////////

#endif /* TIMER_H_ */
