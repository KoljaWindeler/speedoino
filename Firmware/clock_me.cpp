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

speedo_clock::speedo_clock(void){
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

void speedo_clock::set_date_time(int year,int mon,int day,int hh,int mm,int ss,int dayOfWeek,int dayls){
	if(CLOCK_DEBUG && false){ // das schon brutal nervige 1hz meldung
		Serial.println("Setting Clock:");
		char *char_buffer;
		char_buffer = (char*) malloc (60);
		if (char_buffer==NULL) Serial.println("Malloc failed");
		else memset(char_buffer,'\0',60);

		sprintf(char_buffer,"%i,%i,%i,%i,%i,%i,%i,%i",year%100,mon%100,day%100,hh%100,mm%100,ss%100,dayOfWeek%100,dayls%10);
		Serial.println(char_buffer);
		free(char_buffer);
	}
	if(year>-1){ m_year=year; };
	if(mon>-1){ m_mon=mon; };
	if(dayls>-1 && dayls<4){ m_dayls=dayls; };
	if(hh>-1){ m_hh=(hh+m_dayls)%24; };
	if(mm>-1){ m_mm=mm; };
	if(( (signed(m_ss)-ss)>10 || (signed(m_ss)-ss)<-10) && ss>-1){ m_ss=unsigned(ss); }; // min 10 sec differenz
	if(dayOfWeek>-1){ m_dayOfWeek=dayOfWeek; };
	if(day>-1){ m_day=day+floor((m_hh+m_dayls)/24); }; // eventuell is ja schon morgen

	/*if(lost_data){ // wenn im i2c mist rausgekommen ist, bat gewechselt, kurzschluss, erster aufbau .. watt weiß ich
		if(day>-1 && year>-1 && mon>-1){ // und wir valide gps daten bekommen
			store(); // speichern!!
			lost_data=false;
		}
	}*/
};

/**
 * Initialize the DCF77 routines: initialize the variables,
 * configure the interrupt behaviour.
 */
void speedo_clock::init() {
	m_year=0;
	m_mon=0;
	m_day=0;
	m_hh=0;
	m_mm=0;
	m_ss=0;
	m_dayls=0;
	/*lost_data=false;

	// modul lesen
	Wire.beginTransmission(DS1307_I2C_ADDRESS);   // Open I2C line in write mode
	Wire.send(0x00);                              // Set the register pointer to (0x00)
	Wire.endTransmission();                       // End Write Transmission
	Wire.requestFrom(DS1307_I2C_ADDRESS, 7);      // Open the I2C line in read mode
	m_ss         = bcdToDec(Wire.receive() & 0x7f); // Read seven char of data
	m_mm         = bcdToDec(Wire.receive());
	m_hh         = bcdToDec(Wire.receive() & 0x3f);
	m_dayOfWeek  = bcdToDec(Wire.receive());
	m_day        = bcdToDec(Wire.receive());
	m_mon        = bcdToDec(Wire.receive());
	m_year       = bcdToDec(Wire.receive());
	Serial.print("Die aktuelle Uhrzeit: ");
	Serial.print(m_hh);
	Serial.print(":");
	Serial.print(m_mm);
	Serial.print(":");
	Serial.print(m_ss);
	Serial.print(" ");
	Serial.print(m_day);
	Serial.print(".");
	Serial.print(m_mon);
	Serial.print(".");
	Serial.println(m_year);
	if(m_mon==1 && m_day==1 && m_year==0){
		lost_data=true;
	}*/
	pDebug->sprintlnp(PSTR("Clock init done"));
	// hier vom lustigen modul lesen ende
}


/*void speedo_clock::store(){
	if(CLOCK_DEBUG){
		Serial.println("== Writing to Modul ==");
	};
	if(m_year>0){ // und wir valide gps daten bekommen, mit 0 initialisieren wir
		Wire.beginTransmission(DS1307_I2C_ADDRESS);
		Wire.send(0x00);
		Wire.send(decToBcd(m_ss));
		Wire.send(decToBcd(m_mm));
		Wire.send(decToBcd(m_hh));
		Wire.send(decToBcd(m_dayOfWeek));
		Wire.send(decToBcd(m_day));
		Wire.send(decToBcd(m_mon));
		Wire.send(decToBcd(m_year));
		Wire.endTransmission();                    // End write mode
		if(CLOCK_DEBUG){
			Serial.print("Es wird gespeichert ");
			Serial.print(m_day);
			Serial.print("/");
			Serial.print(m_mon);
			Serial.print("/");
			Serial.print(m_year);
			Serial.print(" ");
			Serial.print(m_hh);
			Serial.print(":");
			Serial.print(m_mm);
			Serial.print(":");
			Serial.println(m_ss);
			Serial.println("== Reading to verify ==");
			Wire.beginTransmission(DS1307_I2C_ADDRESS);   // Open I2C line in write mode
			Wire.send(0x00);                              // Set the register pointer to (0x00)
			Wire.endTransmission();                       // End Write Transmission
			Wire.requestFrom(DS1307_I2C_ADDRESS, 7);      // Open the I2C line in read mode
			volatile unsigned int temp_ss        = bcdToDec(Wire.receive() & 0x7f); // Read seven char of data
			volatile unsigned int temp_mm        = bcdToDec(Wire.receive());
			volatile unsigned int temp_hh        = bcdToDec(Wire.receive() & 0x3f);
			volatile unsigned int temp_dayofweek = bcdToDec(Wire.receive());
			volatile unsigned int temp_day       = bcdToDec(Wire.receive());
			volatile unsigned int temp_mon       = bcdToDec(Wire.receive());
			volatile unsigned int temp_year      = bcdToDec(Wire.receive());
			Serial.print("aus dem speicher ");
			Serial.print(temp_day);
			Serial.print("/");
			Serial.print(temp_mon);
			Serial.print("/");
			Serial.print(temp_year);
			Serial.print(" ");
			Serial.print(temp_hh);
			Serial.print(":");
			Serial.print(temp_mm);
			Serial.print(":");
			Serial.print(temp_ss);
			Serial.print("->");
			Serial.println(temp_dayofweek);
		};
	};
};*/

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

short int speedo_clock::get_dayls(){
	return m_dayls;
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
