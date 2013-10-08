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
	volatile uint8_t m_ss;
	volatile uint8_t m_mm;
	volatile uint8_t m_hh;
	volatile uint8_t m_day;
	volatile uint8_t m_mon;
	volatile uint8_t m_year;
	volatile bool wintertime;
	void inc_hours();
public:
	speedo_clock(void);
	~speedo_clock();
	void init();
	void loop();
	int16_t check_vars();
	void copy(char* buffer);
	bool changed(uint8_t* storage);
	//void store();
	int16_t getdate();
	unsigned long get_long_date();
	void set_date_time(uint8_t year,uint8_t mon,uint8_t day,uint8_t hh,uint8_t mm,uint8_t ss, bool check_winter);
	bool is_winter_time(uint8_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second);
	uint8_t get_ss();
	void inc();
};
////////////// DCF 77 / RTC_DS1307 ///////////////////////


#endif /* CLOCK_ME_H_ */
