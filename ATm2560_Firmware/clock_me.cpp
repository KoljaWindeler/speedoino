/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2011 Kolja Windeler
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

speedo_clock::speedo_clock(){
	m_year=0;
	m_mon=0;
	m_day=0;
	m_hh=0;
	m_mm=0;
	m_ss=0;
};

speedo_clock::~speedo_clock(){

};

///////////// vars ///////////////////
bool speedo_clock::changed(int* storage){
	if(*storage==signed(m_ss)){
		return false;
	}

	*storage=m_ss;
	return true;
};

// Convert normal decimal numbers to binary coded decimal
char speedo_clock::decToBcd(char val)
{
	return ( (val/10*16) + (val%10) ); // 12 => 16 + 2 // 0001 0010 = 12 <- dec (int) to bcd (one char)
}

// Convert binary coded decimal to normal decimal numbers
char speedo_clock::bcdToDec(char val)
{
	return ( (val/16*10) + (val%16) );
}

void speedo_clock::set_date_time(int year,int mon,int day,int hh,int mm,int ss, bool check_winter){
	if(CLOCK_DEBUG && false){ // das schon brutal nervige 1hz meldung
		Serial.println("Setting Clock:");
		char *char_buffer;
		char_buffer = (char*) malloc (60);
		if (!char_buffer) Serial.println("Malloc failed");
		else memset(char_buffer,'\0',60);

		sprintf(char_buffer,"%i,%i,%i,%i,%i,%i",year%100,mon%100,day%100,hh%100,mm%100,ss%100);
		Serial.println(char_buffer);
		free(char_buffer);
	}

	// gehen wir erstmal von Winterzeit mit GMT offset aus
	if(( (signed(m_ss)-ss)>10 || (signed(m_ss)-ss)<-10) && ss>-1){ m_ss=unsigned(ss); }; // min 10 sec differenz
	if(mm>-1){ m_mm=mm; };
	if(hh>-1){ m_hh=(hh+GMT_TIME_CORRECTION)%24; }; // Überlauf checken
	if(day>-1){ m_day=day+floor(m_hh/24); }; // eventuell is ja schon morgen
	if(mon>-1){ m_mon=mon; };
	if(year>-1){ m_year=year; };

	// berechnung der Winterzeit mit Rohdaten
	if(check_winter){
		if(is_winter_time(m_year,m_mon,m_day,m_hh,m_mm,m_ss)){ // ist winterzeit?
			static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
			// nachziehen
			m_hh++;
			if(m_hh>23){
				m_day++;
				m_hh=m_hh%24;
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
		};
	};
};

unsigned int speedo_clock::is_winter_time(unsigned int year,unsigned int month,unsigned int day,unsigned int hour,unsigned int minute,unsigned int second){
	// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
	// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

	unsigned int i,DST;
	unsigned long seconds;
	// leap year calulator expects year argument as years offset from 1970

	static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

	// seconds from 1970 till 1 jan 00:00:00 of the given year
	year+=30; // GPS commits 13 for 2013, lear year expects offset 1970 => for 2013: 13+30 = 43
	if(year>1970) year-=1970;


	seconds= year*(SECS_PER_DAY * 365);
	for (i = 0; i < year; i++) {

		if (LEAP_YEAR(i)) {
			seconds +=  SECS_PER_DAY;   // add extra days for leap years
		}
	}

	// add days for this year, months start from 1
	for (i = 1; i < month; i++) {
		if ( (i == 2) && LEAP_YEAR(year)) {
			seconds += SECS_PER_DAY * 29;
		} else {
			seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
		}
	}
	seconds+= (day-1) * SECS_PER_DAY;
	//seconds+= hour * SECS_PER_HOUR;
	//seconds+= minute * SECS_PER_MIN;
	//seconds+= second;
	hour+=GMT_TIME_CORRECTION;
	unsigned int weekday= (seconds / SECS_PER_DAY + 4) % 7;

	// Sommerzeit berechnen
	if (month < 3 || month > 10) {// 11, 12, 1 und 2 haben keine Sommerzeit
		DST = 0;
	} else {
		DST = 1; // gehen wir mal davon aus das sommerzeit ist
		if (month == 3) {
			if ((day - weekday >= 25) && (weekday || hour >= 2)) {
				DST = 1; // Sommerzeit
			} else {
				DST = 0;
			}
		} else if (month == 10) {
			if ((day - weekday >= 25) && (weekday || hour >= 3)) {
				DST = 0;
			} else {
				DST = 1;
			}
		}
	}
	return DST;					// add DST
}


/**
 * Initialize the DCF77 routines: initialize the variables,
 * configure the interrupt behaviour.
 */
void speedo_clock::init() {
	pDebug->sprintlnp(PSTR("Clock init done"));
	// hier vom lustigen modul lesen ende
}



bool speedo_clock::check_vars(){
	return false;
};


int speedo_clock::getdate(){
	return m_day+m_mon*100;
};

//20.12.2011  -> 111220 unsigned long
unsigned long speedo_clock::get_long_date(){
	if(m_year+m_mon+m_day==0) // falls die uhr so gar nciht ging geben wir den 99.99.2099 zurück
		return 999999;

	unsigned long date=m_year;
	date=date*100+m_mon;
	date=date*100+m_day;
	if(date>991231) // two digit year
		return 0;
	return date;
}


short int speedo_clock::get_ss(){
	return m_ss;
}

void speedo_clock::inc(){
	m_ss++;
	if (m_ss==60) {
		m_ss=0;
		m_mm++;
		if (m_mm==60) {
			m_mm=0;
			m_hh++;
			if(m_hh==24)
				m_hh=0;
		}
	}
}; // 0==kein signal, >0 = sek seit letztem sync


void speedo_clock::copy(char* buffer){
	sprintf(buffer,"%02i:%02i:%02i",m_hh%100,m_mm%100,m_ss%100);
};

void speedo_clock::loop(){

	if(pSpeedo->disp_zeile_bak[0]!=1){
		pSpeedo->disp_zeile_bak[0]=1;
		pOLED->clear_screen();
		char temp[9];
		sprintf(temp,"%02i:%02i:%02i",m_hh,m_mm,m_ss);
		pOLED->string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL,CLOCKMODE_ROW);
	};

	// hour
	if(pSpeedo->disp_zeile_bak[1]!=signed(m_hh)){
		pSpeedo->disp_zeile_bak[1]=m_hh;
		char temp[3];
		sprintf(temp,"%02i",m_hh);
		pOLED->string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL,CLOCKMODE_ROW);
	};

	// min
	if(pSpeedo->disp_zeile_bak[2]!=signed(m_mm)){
		pSpeedo->disp_zeile_bak[2]=m_mm;
		char temp[3];
		sprintf(temp,"%02i",m_mm);
		pOLED->string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL+6,CLOCKMODE_ROW);
	};

	// sec
	if(pSpeedo->disp_zeile_bak[3]!=get_ss()){
		pSpeedo->disp_zeile_bak[3]=get_ss();
		char temp[3];
		sprintf(temp,"%02i",m_ss);
		pOLED->string(VISITOR_SMALL_2X_FONT,temp,CLOCKMODE_COL+12,CLOCKMODE_ROW);
	};

};


