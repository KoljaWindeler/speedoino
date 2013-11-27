/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2013 Kolja Windeler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"
/** constructor
 * init time to 00:00:00
 */
clock::clock(){
	m_year=0;
	m_mon=0;
	m_day=0;
	m_hh=0;
	m_mm=0;
	m_ss=0;
	wintertime=true;
};

clock::~clock(){};



/** changed - check if the seconds have changed .This method can be called with a pointer reference to a var
 * to find out if the last call was ago longer than 1 sec
 *
 * Input: pointer to var
 * Returns: true, if pointed value != current second
 */
bool clock::changed(int16_t* storage){
	if(*storage==int16_t(m_ss)){
		return false;
	}
	*storage=m_ss;
	return true;
};


void clock::set_date_time(uint8_t year,uint8_t mon,uint8_t day,uint8_t hh,uint8_t mm,uint8_t ss, bool check_winter){
#ifdef CLOCK_DEBUG // das schon brutal nervige 1hz meldung
	Serial.println("Setting Clock:");
	char char_buffer[60];
	sprintf(char_buffer,"%i,%i,%i,%i,%i,%i",year%100,mon%100,day%100,hh%100,mm%100,ss%100);
	Serial.println(char_buffer);
#endif

	// accept only if min 10 sec difference
	int16_t internal_clock_timestamp=(m_mm*60)+m_ss;
	int16_t setter_clock_timestamp=(mm*60)+ss;
	if( (internal_clock_timestamp-setter_clock_timestamp)>10 || (internal_clock_timestamp-setter_clock_timestamp)<-10 ){ m_ss=ss; };

	// accept anything else
	if(mm<60) { m_mm=mm; };
	if(hh<24) { m_hh=hh; };
	if(day<32){ m_day=day; };
	if(mon<13){ m_mon=mon; };
	if(year>0){ m_year=year; };

	// calculate the daylight_saving with raw_data
	if(check_winter){
		if(!is_winter_time(m_year,m_mon,m_day,m_hh,m_mm,m_ss)){ // is wintertime?
			wintertime=false;
		}
	};

	// add gmt and daylight_saving time
	inc_hours();
};

/** inc_hours is used to correct the hours, based on daylight_saving + GMT_TIME_CORRECTION
 * any checks if that changes the current day and year
 */
void clock::inc_hours(){
	char inc=GMT_TIME_CORRECTION;
	if(!wintertime){
		inc++;
	}
	// correct hour and check if we have to correct the date as well
	m_hh+=inc;
	if(m_hh>23){
		m_day++;
		m_hh=m_hh%24;
		static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
		if(m_day>monthDays[m_mon-1]){
			m_day=m_day%monthDays[m_mon-1];
			m_mon++;
			if(m_mon>12){
				// wer fährt denn bitte an sylvester?
				m_year++;
				m_mon=m_mon%12;
			};
		};
	}
}

/** calc daylight_saving
 * calculate once on startup if we have to add one addition hour
 */
bool clock::is_winter_time(uint8_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second){
	// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
	// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
	bool winter;
	// Sommerzeit berechnen
	if (month < 3 || month > 10) {// 11, 12, 1 und 2 haben keine Sommerzeit
		winter = true;
	} else {
		uint16_t i;
		uint32_t seconds;
		// leap year calulator expects year argument as years offset from 1970

		static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

		// GPS commits 13 for 2013, lear year expects offset 1970 => for 2013: 13+30 = 43
		year+=30;

		// seconds from 1970 till 1 jan 00:00:00 of the given year, including all COMPLETED leap years
		seconds= year*(SECS_PER_DAY * 365);
		for (i = 0; i < year; i++) {
			if (LEAP_YEAR(i)) {
				seconds += SECS_PER_DAY;   // add extra days for leap years
			}
		}

		// add days for THIS year, months start from 1
		for (i = 1; i < month; i++) {
			seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0

			if ((i == 2) && LEAP_YEAR(year)) {
				seconds += SECS_PER_DAY;   // add extra day
			}
		}

		// add seconds for this day
		seconds+= (day-1) * SECS_PER_DAY;
		//seconds+= hour * SECS_PER_HOUR;
		//seconds+= minute * SECS_PER_MIN;
		//seconds+= second;
		hour+=GMT_TIME_CORRECTION;
		uint8_t weekday= (seconds / SECS_PER_DAY + 4) % 7;

		winter = false; // gehen wir mal davon aus das sommerzeit ist
		if (month == 3) {
			if ((((int8_t)day - (int8_t)weekday) >= 25) && (weekday || hour >= 2)) {
				winter = false; // Sommerzeit
			} else {
				winter = true;
			}
		} else if (month == 10) {
			if ((((int8_t)day - (int8_t)weekday) >= 25) && (weekday || hour >= 3)) {
				winter = true;
			} else {
				winter = false;
			}
		}
	}
	return winter;					// add DST
}


/**
 * initialize the variables,
 * configure the interrupt behaviour.
 */
void clock::init() {
	Serial.puts_ln(USART1,("Clock init done"));
}


int16_t clock::check_vars(){
	return 0;
};

/** getDate returns a integer representation of the date
 * 22.12. will return 1222, so leading month and trailing day.
 * for compatiblity to get_long_date()
 */
int16_t clock::getdate(){
	return m_day+m_mon*100;
};

/** get_long_date returns a integer representation of the date
 * 06.12.2013 will return 131206, so leading year, than month and trailing day.
 */
unsigned long clock::get_long_date(){
	if(m_year<99 && m_mon<=12 && m_day<=31){
		return (unsigned long)((unsigned long)((unsigned long)(m_year*100)+m_mon)*100)+m_day;
	}
	return 999999;
}


uint8_t clock::get_ss(){
	return m_ss;
}

void clock::inc(){
	m_ss++;
	if (m_ss>=60) {
		m_ss=0;
		m_mm++;
		if (m_mm>=60) {
			m_mm=0;
			m_hh++;
			if(m_hh>=24)
				m_hh=0;
		}
	}
};


void clock::copy(char* buffer){
	sprintf(buffer,"%02i:%02i:%02i",m_hh%100,m_mm%100,m_ss%100);
};


/** loop() - to show a beautiful clock in "off"-Mode
 * will be shown, if the user pushes the "left" button for 1 sec
 * and the ignition is off. A permanent Voltage supply is
 * required for this clock
 */
void clock::loop(){
	if(Speedo.disp_zeile_bak[0]!=1){
		Speedo.disp_zeile_bak[0]=1;
		TFT.clear_screen();
		char temp[9];
		sprintf(temp,"%02i:%02i:%02i",m_hh,m_mm,m_ss);
		TFT.string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL,CLOCKMODE_ROW);
	};

	// hour
	if((Speedo.disp_zeile_bak[1])!=m_hh){
		Speedo.disp_zeile_bak[1]=m_hh;
		char temp[3];
		sprintf(temp,"%02i",m_hh);
		TFT.string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL,CLOCKMODE_ROW);
	};

	// min
	if((Speedo.disp_zeile_bak[2])!=m_mm){
		Speedo.disp_zeile_bak[2]=m_mm;
		char temp[3];
		sprintf(temp,"%02i",m_mm);
		TFT.string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL+6,CLOCKMODE_ROW);
	};

	// sec
	if((Speedo.disp_zeile_bak[3])!=get_ss()){
		Speedo.disp_zeile_bak[3]=get_ss();
		char temp[3];
		sprintf(temp,"%02i",m_ss);
		TFT.string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL+12,CLOCKMODE_ROW);
	};

};
