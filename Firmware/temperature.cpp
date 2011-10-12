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
};

speedo_temperature::~speedo_temperature(){
};

void speedo_temperature::init(){
	oil_temp_value_counter=0;
	oil_temp_value=0;
	// values for
	for(unsigned int j=0; j<sizeof(r_werte)/sizeof(r_werte[0]); j++){
		r_werte[j]=0;
		t_werte[j]=0;
	};

	Serial.println("Temp init done.");
}

void speedo_temperature::read_oil_temp() {
	// werte in array speichern in °C*10 für Nachkommastelle
	if(false){ Serial.println("\nTemp: Beginne zu lesen"); }

	// werte auslesen
	unsigned int oil_value=analogRead(oil_temp_pin);
	// kann bis zu 225.060 werden bei 40°C etwa 470 ohm: 470+220=690 Ohm, U/R=I => 5/690=0,007246377A, R*I=U, 1,594202899V, Wert=326
	int temp=(1024-oil_value)/10; // max 102
	if(temp>0){
		int r_temp=round((oil_value*22)/temp); // 22*1024 < 32000
		//Serial.print("Oel Wert eingelesen: "); Serial.print(oil_value); Serial.print(" zwischenschritt "); Serial.println(r_temp);
		// LUT  wert ist z.B. 94°C somit 102 ohm => dann wird hier in der for schleife bei i=13 ausgelößt, also guck ich mir den her + den davor an
		for(int i=0;i<19;i++){ // 0 .. 18 müssen durchsucht werden das sind die LUT positionen
			if(r_temp>=r_werte[i]){ // den einen richtigen raussuchen
				int j=i-1;  if(j<0) j=0; // j=12
				int offset=r_temp-r_werte[i]; // wieviel höher ist mein messwert als den, den ich nicht wollte => 102R-100R => 2R
				int differ_r=r_werte[j]-r_werte[i]; // wie weit sind die realen widerstands werte auseinander 10R
				int differ_t=t_werte[i]-t_werte[j]; // wie weit sind die realen temp werte auseinander 5°C
				int aktueller_wert=round(10*(t_werte[i]-offset*differ_t/differ_r));
				oil_temp_value=pSensors->flatIt(aktueller_wert,&oil_temp_value_counter,20,oil_temp_value);

				if(false){
					Serial.print("Oel Wert eingelesen: ");
					Serial.print(oil_value);
					Serial.print(" und interpretiert ");
					Serial.print(aktueller_wert);
					Serial.print(" und geplaettet: ");
					Serial.println(int(round(oil_temp_value)));
				}

				break; // break the for loop
			};
		};
	};
};

void speedo_temperature::read_air_temp() {
	// get i2c tmp102 //
	int sensorAddress = 0x91 >> 1;  // From datasheet sensor address is 0x91 shift the address 1 bit right, the Wire library only needs the 7 most significant bits for the address
	byte msb;
	byte lsb;
	Wire.requestFrom(sensorAddress,2);
	if(TEMP_DEBUG){ Serial.println("request abgeschickt"); }
	if (Wire.available() >= 2)  // if two bytes were received
	{
		if(TEMP_DEBUG){ Serial.println("2 Byte im Puffer"); }
		msb = Wire.receive();  // receive high byte (full degrees)
		lsb = Wire.receive();  // receive low byte (fraction degrees)
		air_temp_value = ((msb) << 4);  // MSB
		air_temp_value|= (lsb >> 4);    // LSB
		air_temp_value = round(air_temp_value*0.625); // round and save
		if(TEMP_DEBUG){ Serial.print("Air value "); Serial.println(air_temp_value); }
	} else {
		air_temp_value = 999;
	};
};


int speedo_temperature::get_air_temp(){
	if(DEMO_MODE)
		return (32+((millis()/1000)%10))*10+((millis()/1000)%10);
	else if(air_temp_value>999) // notfall
		return 0;
	else
		return air_temp_value;
}

int speedo_temperature::get_oil_temp(){
	if(DEMO_MODE)
		return (102+((millis()/1000)%10))*10+((millis()/1000)%10);
	else if(pSpeedo->trip_dist[1]==0) // wir sind heute noch exakt gar nicht gefahren
		return get_air_temp()-1;
	else
		return int(round(oil_temp_value));
}
// alte verfahren
// air_temp_values[position] = round((10 * air_value * 5  * 100.0)/1024.0); //Luft
// oil_temp_values[position] = round((oil_value*-0.2+148.94)*10); //kolja öl neu nach Datenblatt mit akzeptierter nicht Linearitä
// oil_temp_values[position] = round((oil_value*-0.147032+128.25)*10); //kolja öl
