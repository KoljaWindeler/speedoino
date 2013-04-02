/*
 * clock_me.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef CLOCK_ME_H_
#define CLOCK_ME_H_

////////////// DCF 77 / RTC_DS1307 ///////////////////////
class speedo_clock  // shell class for the clock
{
#define GMT_TIME_CORRECTION 1
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define LEAP_YEAR(Y)  (((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ))

#define CLOCKMODE_COL 3
#define CLOCKMODE_ROW 2

private:
	char decToBcd(char val);
	char bcdToDec(char val);
	volatile unsigned int m_ss;
	volatile unsigned int m_mm;
	volatile unsigned int m_hh;
	volatile unsigned int m_day;
	volatile unsigned int m_mon;
	volatile unsigned int m_year;
	volatile unsigned int m_dayOfWeek;
	volatile bool wintertime;
	void inc_hours();
public:
	speedo_clock(void);
	~speedo_clock();
	void init();
	void loop();
	int check_vars();
	void copy(char* buffer);
	bool changed(int* storage);
	//void store();
	int getdate();
	unsigned long get_long_date();
	void set_date_time(int year,int mon,int day,int hh,int mm,int ss, bool check_winter);
	bool is_winter_time(unsigned int year,unsigned int month,unsigned int day,unsigned int hour,unsigned int minute,unsigned int second);
	short int get_ss();
	void inc();
};
////////////// DCF 77 / RTC_DS1307 ///////////////////////


#endif /* CLOCK_ME_H_ */
