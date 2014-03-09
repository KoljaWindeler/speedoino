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
speedo_sd::speedo_sd(){
};

speedo_sd::~speedo_sd(){
};

// store error strings in flash to save RAM
void speedo_sd::error_P(const char* str) {
	PgmPrint("error: ");
	SerialPrintln_P(str);
	if (card.errorCode()) {
		PgmPrint("SD error: ");
		Serial.print(card.errorCode(), HEX);
		PgmPrint(",");
		Serial.println(card.errorData(), HEX);
	}
}
/*
 * Write CR LF to a file
 */
void speedo_sd::writeCRLF(SdFile& f) {
	f.write((uint8_t*)"\r\n", 2);
}
/*
 * Write a string to a file
 */
int speedo_sd::writeString(SdFile& f, char *str) {
	uint8_t n;
	for (n = 0; str[n]; n++); // n=zähler, hier sind keine klammern
	return f.write((uint8_t *)str, n);
}

void speedo_sd::init(){
	// p kanal runterziehen damit er leitend wird
	power_up(3);
	pDebug->sprintlnp(PSTR(" done"));
};

void speedo_sd::power_down(){
	pinMode(SD_EN,INPUT);
	digitalWrite(SD_EN,HIGH);
}

void speedo_sd::power_up(unsigned char tries){
	pinMode(SD_EN,OUTPUT);
	digitalWrite(SD_EN,LOW);
	sd_failed=false;
	bool allright=true;
	pDebug->sprintp(PSTR("SD try:"));
	while(tries>0){ //maximal 3 versuche die sd karte zu öffnen
		Serial.print(3-tries);

		allright=true;
		if (card.init((unsigned char)SPI_HALF_SPEED)==0){	allright=false; };
		if (!volume.init(&card))						{ 	allright=false; }; // initialize a FAT volume
		if(allright){
			break;
		} else {
			tries--;
			pDebug->sprintp(PSTR(","));
		};
		pSensors->m_reset->toggle();
	};
	if(!allright){
		sd_failed=true;
	};

};

