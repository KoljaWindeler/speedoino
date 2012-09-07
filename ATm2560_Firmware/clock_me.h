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
#define DS1307_I2C_ADDRESS 0x68  // This is the I2C address of the rtc

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
	short int m_dayls;                                        // winterzeit oder nicht
	bool lost_data;
public:
	speedo_clock(void);
	~speedo_clock();
	void init();
	void loop();
	void clear_vars();
	bool check_vars();
	void copy(char* buffer);
	bool changed(int* storage);
	//void store();
	int getdate();
	unsigned long get_long_date();
	void set_date_time(int year,int mon,int day,int hh,int mm,int ss,int dayOfWeek,int dayls);
	short int get_dayls();
	short int get_ss();
	void inc();
};
////////////// DCF 77 / RTC_DS1307 ///////////////////////


#endif /* CLOCK_ME_H_ */
