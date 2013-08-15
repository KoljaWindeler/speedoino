
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

// init & clean buffer
speedo_speedcams::speedo_speedcams(){

}

void speedo_speedcams::test(){
	////////////////////////////////////////// TESTING //////////////////////////////////////////
	while(pSensors->m_gps->get_info(9)>3){
		// wait
	};
	unsigned char filename[20]; // "/NAVI/HOCKENHE.SST"
	strcpy_P((char *)filename,PSTR("NAVI/HANNOVER.SST")); // static file for the moment

	uint32_t dist,dist2,dist3;
	uint32_t time=millis();
	for(int loop=0;loop<1000;loop++){
		dist=pSensors->m_gps->calc_dist(2904720,51214280);
		dist2=pSensors->m_gps->calc_dist(904720,51214280);
		dist3=pSensors->m_gps->calc_dist(2904720,1214280);
	}
	time=millis()-time;
	Serial.print("1000 Berechnungen der Entfernungen zu 2904720/51214280 kosten ");

	Serial.print(time);
	Serial.println(" ms");
	Serial.println(dist);
	// reale Distanz 488,848 km
	// 	757ms mit boardmitteln 	           = 494,709 km => 1,198%
	// 757µs
	////////////////////////////////////////////

	time=millis();
	for(int loop=0; loop<1000; loop++){
		//get_sector_data(loop%3, &dist,&dist,&dist,filename);
	};

	time=millis()-time;
	Serial.print("1000x Laden eines POI kostet ");
	Serial.print(time);
	Serial.println(" ms");

	////////////////////////////////////////////

	unsigned char temp[31];
	SdFile folder;
	SdFile file;
	int sector_id=1;

	time=millis();
	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_READ)<0){
		file.close();
		folder.close();
		Serial.println("DAMN 1");
	} else {
		for(int loop=0;loop<1000;loop++){

			// move cursor to sector line
			if(!file.seekSet(sector_id*30)){
				file.close();
				folder.close();
				Serial.println("DAMN 2");
			};

			// read line
			int n=file.read(temp, 30);
			if(n<30){
				file.close();
				folder.close();
				Serial.println("DAMN 3");
			}

			// get latitude
			uint32_t temp_value=0;
			for(int i=0;i<9;i++){ // 8-0 = 9 Chars
				if(temp[i]>='0' && temp[i]<='9'){
					temp_value=temp_value*10+(temp[i]-'0');
				}
			}

			// get longitude
			temp_value=0;
			for(int i=10;i<19;i++){ // 21-11 = 10 Chars
				if(temp[i]>='0' && temp[i]<='9'){
					temp_value=temp_value*10+(temp[i]-'0');
				}
			}

			// get sector_time
			temp_value=0;
			for(int i=20;i<29;i++){ // 30-22 = 8 Chars
				if(temp[i]>='0' && temp[i]<='9'){
					temp_value=temp_value*10+(temp[i]-'0');
				}
			}

			if(temp_value==0){ // 0 = nonsense, you are not that quick :D
				temp_value=99999999;
			}
		}

		file.close();
		folder.close();

		time=millis()-time;
		Serial.print("1x Laden von 1000 POI kostet ");
		Serial.print(time);
		Serial.println(" ms");
	};

	//////////////////////////////////////// TESTING //////////////////////////////////////////
}


int speedo_speedcams::test2(){
	////////////////////////////////////////// TESTING //////////////////////////////////////////
	if(pSensors->m_gps->get_info(9)>3){ return -1; };

	unsigned char source_filename[20]; // "/NAVI/HOCKENHE.SST"
	unsigned char dest_filename[20]; // "/NAVI/HOCKENHE.SST"
	strcpy_P((char *)source_filename,PSTR("CONFIG/POI.TXT")); // static file for the moment
	strcpy_P((char *)dest_filename,PSTR("CONFIG/POI_N.TXT")); // static file for the moment
	uint32_t current_latitude=pSensors->m_gps->nmea_to_dec(pSensors->m_gps->get_info(3));
	uint32_t current_longitude=pSensors->m_gps->nmea_to_dec(pSensors->m_gps->get_info(2));

	//uint32_t dist=pSensors->m_gps->calc_dist(2904720,51214280);
	uint32_t time=millis();
	unsigned char temp[25];

	uint16_t lines=0;
	uint16_t found=0;
	uint32_t loaded_latitude;
	uint32_t loaded_longitude;
	uint32_t lati_diff;
	uint32_t long_diff;
	SdFile read_file;
	SdFile write_file;


	time=millis();
	if(pFilemanager_v2->get_file_handle(source_filename,&read_file,O_READ)<0){
		read_file.close();
		Serial.println("DAMN 1");
		return -3;
	} else {
		if(pFilemanager_v2->get_file_handle(dest_filename,&write_file,O_RDWR|O_CREAT|O_TRUNC)<0){
			write_file.close();
			Serial.println("DAMN 1");
			return -2;
		}
		Serial.print("File open, length:");
		Serial.println(read_file.fileSize());
		int n=99;
		while(n>=0){
			// read line
			n=read_file.read(temp, 20);
			if(n<20){
				read_file.close();
			} else {
				lines++;
				// get longitude
				loaded_longitude=0;
				for(int i=0;i<9;i++){ // 8-0 = 9 Chars
					if(temp[i]>='0' && temp[i]<='9'){
						loaded_longitude=loaded_longitude*10+(temp[i]-'0');
					}
				}

				// get latitude
				loaded_latitude=0;
				for(int i=10;i<19;i++){ // 21-11 = 10 Chars
					if(temp[i]>='0' && temp[i]<='9'){
						loaded_latitude=loaded_latitude*10+(temp[i]-'0');
					}
				}

				if(current_latitude>loaded_latitude){
					lati_diff=current_latitude-loaded_latitude;
				} else {
					lati_diff=loaded_latitude-current_latitude;
				}

				if(current_longitude>loaded_longitude){
					long_diff=current_longitude-loaded_longitude;
				} else {
					long_diff=loaded_longitude-current_longitude;
				}

				if(long_diff<300000 && lati_diff<200000){ // ca 20km in E<->W and N<->S
					//					Serial.print(lines);
					//					Serial.print(" seems to be near to us:");
					//					Serial.print(loaded_latitude);
					//					Serial.print(" / ");
					//					Serial.print(loaded_longitude);
					//					Serial.print(" | Dist: ");
					//					Serial.print(pSensors->m_gps->calc_dist(loaded_longitude,loaded_latitude));
					//					Serial.println(" m");
					write_file.write(temp,20);
					found++;
				}
			}
		}
		time=millis()-time;
		Serial.print(lines);
		Serial.print(" Points parsed in ");
		Serial.print(time);
		Serial.println(" ms");
		Serial.print(found);
		Serial.println(" are near");
	}

	read_file.close();
	write_file.close();
	time=millis()-time;
	//	Serial.print("1x Laden von 1000 POI kostet ");
	//	Serial.print(time);
	Serial.println("Fertig");
	while(1){};
	return 0;
};

