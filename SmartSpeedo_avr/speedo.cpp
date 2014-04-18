
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

// reset all storages
void speedo_speedo::reset_bak(){
	for(unsigned int i=0;i<sizeof(disp_zeile_bak)/sizeof(disp_zeile_bak[0]);i++){
		disp_zeile_bak[i]=-99;
	};
}

/************* speedo main loop *************
 * this is THE main loop, default speedo mode
 ************* speedo main loop *************/
void speedo_speedo::loop(unsigned long previousMillis){
	previousMillis=millis(); // get time for this loop
	pDebug->speedo_loop(0,1,previousMillis,(char *)" "); // debug

	/************************* oil temperature *********************
	 * Oil Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->oil_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a coresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 ************************* oil temperature *********************/
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintp(PSTR("-o"));
#endif
	if(disp_zeile_bak[OIL_TEMP]!=pSensors->get_oil_r()+pSensors->m_temperature->oil_r_fail_status){
		disp_zeile_bak[OIL_TEMP]=int(pSensors->get_oil_r()+pSensors->m_temperature->oil_r_fail_status);
		// below 20 degree the PTC is very antiliear so we won't show it
		int value=pSensors->get_oil_r();

		if(pSensors->m_temperature->oil_r_fail_status==SENSOR_OPEN){
			value=0xffff;
		} else if(pSensors->m_temperature->oil_r_fail_status==SENSOR_SHORT_TO_GND){
			value=0xfffe;
		};
		pDebug->speedo_loop(1,0,previousMillis," "); // debug

#ifdef OIL_DEBUG
		char buffer[30];
		sprintf(buffer,"Widerstand Oel:%i",value);
		Serial.println(buffer);
#else
		// send data
		unsigned char data[3];
		data[0]=CMD_POST_OIL_R;
		data[1]=value>>8 & 0xff;
		data[2]=value & 0xff;
		pFilemanager_v2->send_answere(data,3);
#endif
	};
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintlnp(PSTR("."));
#endif
	/************************* oil temperature *********************/


	/************************* water temperature *********************
	 * water Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->water_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a coresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 *
	 * 7 Chars
	 * "_-_____" <- Sensor open (based on 5x read 1024 on the analog pin)
	 * "_--____" <- Sensor short to ground (based on 5x read 0 on the analog pin)
	 * "_32.4Â°C" <- regular temp + filling
	 * "102.3Â°C" <- regular temp
	 * ">110Â°C_" <- temp to high
	 * "<30Â°C__" <- temp to low
	 ************************* water temperature *********************/
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintp(PSTR("-w"));
#endif
	if(disp_zeile_bak[WATER_TEMP]!=pSensors->get_water_temperature()+pSensors->get_water_temperature_fail_status()){
		disp_zeile_bak[WATER_TEMP]=int(pSensors->get_water_temperature()+pSensors->get_water_temperature_fail_status());

		int value=pSensors->get_water_temperature();

		if(pSensors->get_water_temperature_fail_status()==SENSOR_OPEN){
			value=0xffff;
		} else if(pSensors->get_water_temperature_fail_status()==SENSOR_SHORT_TO_GND){
			value=0xfffe;
		};
		pDebug->speedo_loop(1,0,previousMillis," "); // debug

#ifdef WATER_DEBUG
		char buffer[30];
		sprintf(buffer,"Widerstand Wasser:%i",value);
		Serial.println(buffer);
#else
		// send data
		unsigned char data[3];
		if(pSensors->CAN_active){
			data[0]=CMD_POST_WATER_TEMP;
		} else {
			data[0]=CMD_POST_WATER_R;
		}
		data[1]=value>>8 & 0xff;
		data[2]=value & 0xff;
		pFilemanager_v2->send_answere(data,3);
#endif
	};

	pDebug->speedo_loop(1,0,previousMillis," "); // debug

#ifdef TACHO_SMALLDEBUG
	pDebug->sprintlnp(PSTR("."));
#endif
	/************************* water temperature *********************/
	pDebug->speedo_loop(22,1,0," ");


	/************************* air temperature *********************
	 * Air Temperature will be read out in the every_sec routine
	 * the measured value will be available in the variable
	 * pSensors->m_temperature->air_temp_out. That value is degree multiplied by 10.
	 * We prevent the screen from flickering by saving a corresponding
	 * value in disp_zeile_bak[OIL_TEMP] and check that value before redraw
	 ************************* air temperature *********************/
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintp(PSTR("-at"));
#endif
	if(disp_zeile_bak[AIR_TEMP]!=int(pSensors->get_air_temperature())){
		disp_zeile_bak[AIR_TEMP]=int(pSensors->get_air_temperature());
		pDebug->speedo_loop(2,0,previousMillis," ");

#ifdef AIR_DEBUG
		char buffer[30];
		sprintf(buffer,"Aussentemperatur:%i",disp_zeile_bak[AIR_TEMP]);
		Serial.println(buffer);
#else
		// send data
		unsigned char data[3];
		data[0]=CMD_POST_AIR_TEMP;
		data[1]=disp_zeile_bak[AIR_TEMP]>>8 & 0xff;
		data[2]=disp_zeile_bak[AIR_TEMP] & 0xff;
		pFilemanager_v2->send_answere(data,3);
#endif
	};
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintlnp(PSTR("."));
#endif
	/************************* air temperature *********************/
	pDebug->speedo_loop(23,1,0," ");

	/************************* speed *********************
	 * getSpeed will return the speed as int, depending on setting this will be the GPS OR Magnitude based speed.
	 * first if checks if the value equals -99, in this case the full display has to be rewritten, draw "km/h"
	 * else just draw the number of km/h, but only refresh it if the difference is higher than 2 or we are very slow (below 10)
	 ************************* geschwindigkeit *********************/
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintp(PSTR("-gs"));
#endif
	int temp_speed=pSensors->get_speed();
	if( (abs(disp_zeile_bak[SPEED_VALUE]-(temp_speed+1))>1) || (abs(disp_zeile_bak[SPEED_VALUE]-(temp_speed+1))==1 && temp_speed<10) ) {
		pDebug->speedo_loop(4,0,previousMillis," "); // debug
		disp_zeile_bak[SPEED_VALUE]=temp_speed+1;

#ifdef SPEED_DEBUG
		char buffer[30];
		sprintf(buffer,"Speed unscaled:%i",temp_speed);
		Serial.println(buffer);
#else
		// send data
		unsigned char data[3];
		data[0]=CMD_POST_SPEED;
		if(!pSensors->CAN_active){
			data[0]=CMD_POST_SPEED_PSEUDO;
		}
		data[1]=temp_speed>>8 & 0xff;
		data[2]=temp_speed & 0xff;
		pFilemanager_v2->send_answere(data,3);
#endif
	}
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintlnp(PSTR("."));
#endif
	/************************* speed *********************/


	/********************* dz *****************************/
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintp(PSTR("-dz"));
#endif
	if(disp_zeile_bak[DZ_VALUE]!=signed(pSensors->get_RPM(RPM_TYPE_DIRECT)+1)){
		pDebug->speedo_loop(6,0,previousMillis," ");
		disp_zeile_bak[DZ_VALUE]=int(pSensors->get_RPM(RPM_TYPE_DIRECT)+1);
		int value = disp_zeile_bak[DZ_VALUE];

#ifdef DZ_DEBUG
		char buffer[30];
		sprintf(buffer,"DZ :%i",value);
		Serial.println(buffer);
#else
		// send data
		unsigned char data[3];
		data[0]=CMD_POST_RPM;
		data[1]=value>>8 & 0xff;
		data[2]=value & 0xff;
		pFilemanager_v2->send_answere(data,3);
#endif
	};
#ifdef TACHO_SMALLDEBUG
	pDebug->sprintlnp(PSTR("."));
#endif
	/********************* dz *****************************/

	/********************* digi in *****************************/
	if(disp_zeile_bak[LIGHT_VALUES]!=pSensors->sensor_state){
		pDebug->speedo_loop(6,0,previousMillis," ");
		disp_zeile_bak[LIGHT_VALUES]=pSensors->sensor_state;

#ifdef CONTROLLIGHTS_DEBUG
		char output[30];
		sprintf(output,"Digi Input value:%i",pSensors->sensor_state);
		Serial.println(output);
		if(pSensors->sensor_state&(1<<FLASHER_LEFT_SHIFT))	{ Serial.println("1. Flasher links");		};
		if(pSensors->sensor_state&(1<<FLASHER_RIGHT_SHIFT))	{ Serial.println("2. Flasher rechts");		};
		if(pSensors->sensor_state&(1<<HIGH_BEAM_SHIFT))		{ Serial.println("3. Fernlicht ");			};
		if(pSensors->sensor_state&(1<<OIL_PRESSURE_SHIFT))	{ Serial.println("4. Oeldruck ein");		};
		if(pSensors->sensor_state&(1<<NEUTRAL_GEAR_SHIFT))	{ Serial.println("5. Leerlauf eingelegt");	};
#else
		// send data
		unsigned char data[2];
		data[0]=CMD_POST_LIGHTS;
		data[1]=pSensors->sensor_state;
		pFilemanager_v2->send_answere(data,2);
#endif
	};
	/********************* digi in *****************************/

	/********************* analog voltage *****************************/
	if(disp_zeile_bak[VOLTAGE_VALUE]!=pSensors->m_voltage->get()){
		pDebug->speedo_loop(6,0,previousMillis," ");
		disp_zeile_bak[VOLTAGE_VALUE]=pSensors->m_voltage->get();

#ifdef VOLTAGE_DEBUG
		char output[30];
		sprintf(output,"Voltage value:%i",pSensors->m_voltage->get());
		Serial.println(output);
#else
		// send data
		unsigned char data[3];
		int voltage=pSensors->m_voltage->get();
		data[0]=CMD_POST_LIGHTS;
		data[1]=(voltage>>8)&0xff;
		data[1]=(voltage)&0xff;
		pFilemanager_v2->send_answere(data,3);
#endif

	};
	/********************* analog voltage *****************************/

}

