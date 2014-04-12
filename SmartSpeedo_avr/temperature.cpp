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

speedo_temperature::speedo_temperature(){
	oil_r_value_counter=0;
	oil_r_value=0;
	oil_r_fail_status=0; // 0 = no failed read, 1-5 = Number of shorts read, 6-9 = Number of open read

	water_r_value_counter=0;
	water_r_value=0;
	water_r_fail_status=0; // 0 = no failed read, 1-5 = Number of shorts read, 6-9 = Number of open read

	air_temp_value=0;
};

speedo_temperature::~speedo_temperature(){
};


void speedo_temperature::init(){
	I2c.begin();
	I2c.setSpeed(0); /// fast
	I2c.timeOut(100); // 100 ms to get Temperature
	pDebug->sprintlnp(PSTR("Temp init done."));
}

void speedo_temperature::read_oil_r() {
	// werte in array speichern in °C*10 für Nachkommastelle
#ifdef TEMP_DEBUG
	Serial.println("\n\rTemp: Beginne Öl zu lesen");
#endif

	//// Widerstandsberechnung ////
	/*	Spannungsabgriff zwischen 2 Widerständen, R1 ggn 5V da drunter in Reihe "R".
	 *  R=U/I, U=R*I, I=U/R
	 *  Spannung über dem zu messenden Widerstand:  Uabgriff = R*I
	 *  I=5V/(R+R1)
	 *  Uabgriff = 5V*analogValue/1024 = R*5V/(R+R1)
	 *  5*analogValue/1024*R + 5*analogValue/1024*R1 = R*5
	 *  5*analogValue/1024*R1 = R*(5-5*analogValue/1024)
	 *  R= (5*analogValue/1024*R1)/(5-5*analogValue/1024) ; "5" Kürzen
	 *  R= (analogValue/1024*R1)/(1-1*analogValue/1024) ; "1024" nach unten
	 *  R= (analogValue*R1)/(1024-analogValue) ; "1024" nach unten
	 *   Widerstandsberechnung *////

	// werte auslesen
	unsigned int oil_value=analogRead(OIL_TEMP_PIN)*4.8828125;
	if(oil_value>10 && oil_value<4900){ // between 10mV and 4,9V
		int r_oil_temp_value=pSensors->pulldown_of_divider(5000,oil_value,220);
		oil_r_value=pSensors->flatIt(r_oil_temp_value,&oil_r_value_counter,20,oil_r_value);
		oil_r_fail_status=0; //?
	} else if(oil_value>=4900) { // kein Sensor  0=(1024-x)/10		x>=1015
#ifdef TEMP_DEBUG
		Serial.print("@");
		Serial.print(millis());
		Serial.print(": oil Wert -> OPEN");
		Serial.print(" | Oil fail status:");
		Serial.println((int)oil_r_fail_status);
#endif

		if(oil_r_fail_status<6){
			oil_r_fail_status=6;
		} else if(oil_r_fail_status<9){
			oil_r_fail_status++;
		}
	} else { // Kurzschluss nach masse: 102=(1024-x)/10  	x<=4
		if(oil_r_fail_status<5){ // nach sechs maligem fehler => ausgabe!
			oil_r_fail_status++;
		};

#ifdef TEMP_DEBUG
		Serial.print("@");
		Serial.print(millis());
		Serial.print(": oil Wert -> Short to GND");
		Serial.print(" | Oil fail status:");
		Serial.println((int)oil_r_fail_status);
#endif
	}
};

void speedo_temperature::read_water_r() {
	// werte in array speichern in °C*10 für Nachkommastelle
#ifdef TEMP_DEBUG
	Serial.println("\n\rTemp: Beginne Water zu lesen");
#endif

	// werte auslesen
	unsigned int water_value=analogRead(WATER_TEMP_PIN)*4.8828125;
	/* kann bis zu 225.060 werden bei 40°C etwa 470 ohm: 470+220=690 Ohm, U/R=I => 5/690=0,007246377A, R*I=U, 1,594202899V, Wert=326
	 * wenn temp < 102 sein soll, dann ergibt 1020 schon ein error, bei 102=(1024-X)/10 => x <= 4
	 * Da haben wir also 4*5V/1024 über dem PT100 und den Rest über 390Ohm
	 * 0,01953125 V über dem PT100 und 4,98046875V über 390Ohm, -> PT100: 1,529411765 Ohm
	 */
	if(water_value>10 && water_value<4900){ // between 10mV and 4,9V
		int r_temp=pSensors->pulldown_of_divider(5000,water_value,390); // 5V supply, 390 Pullup
		// LUT  wert ist z.B. 94°C somit 102 ohm => dann wird hier in der for schleife bei i=13 ausgelößt, also guck ich mir den her + den davor an
		// wenn unser ermittelter R KLEINER ist als der kleinste R, dann haben wir da mehr grad als maximum und brauchen nicht interpolieren
		water_r_value=pSensors->flatIt(r_temp,&water_r_value_counter,60,water_r_value);
		water_r_fail_status=0;
	} else if(water_value>=4900) { // kein Sensor  0=(1024-x)/10		x>=1015
		if(water_r_fail_status<6){
			water_r_fail_status=6;
		} else if(water_r_fail_status<9){
			water_r_fail_status++;
		}
	} else { // Kurzschluss nach masse: 102=(1024-x)/10  	x<=4
		if(water_r_fail_status<5){ // nach sechs maligem fehler => ausgabe!
			water_r_fail_status++;
		};

#ifdef TEMP_DEBUG
		Serial.print("Water Wert Kurzschluss ggn Masse");
#endif
	}
};

void speedo_temperature::read_air_temp() {
	// get i2c tmp102 //
#ifdef TEMP_DEBUG
	Serial.println("beginne air read");
#endif
	int sensorAddress = 0b01001000;  // From datasheet sensor address is 0x91 shift the address 1 bit right
	byte msb;
	byte lsb;
	I2c.read(sensorAddress,0x00,2);
#ifdef TEMP_DEBUG
	Serial.println("request abgeschickt");
#endif

	if (I2c.available() >= 2){  // if two bytes were received
#ifdef TEMP_DEBUG
		Serial.println("2 Byte im Puffer");
#endif
		msb = I2c.receive();  // receive high byte (full degrees)
		lsb = I2c.receive();  // receive low byte (fraction degrees)
		air_temp_value = ((msb) << 3);  // MSB
		air_temp_value|= (lsb >> 5);    // LSB
		air_temp_value = round(air_temp_value*1.25); // round and save
#ifdef TEMP_DEBUG
		Serial.print("Air value "); Serial.println(air_temp_value);
#endif
	} else {
		air_temp_value = 999;
#ifdef TEMP_DEBUG
		Serial.println("ALARM keine Antwort vom I2C sensor!!");
#endif
	};
};


int speedo_temperature::get_air_temp(){
	if(air_temp_value>999) // notfall
		return 0;
	else
		return air_temp_value;
}

int speedo_temperature::get_oil_r(){
	return int(round(oil_r_value));
}

int speedo_temperature::get_water_r(){
	return int(round(water_r_value));
}
