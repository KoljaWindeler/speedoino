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
configuration::configuration(){
	last_speed_value=0;
	storage_outdated=false;
	skin_file=0;
	hw_version=0x00;
};
configuration::~configuration(){};


int configuration::get_hw_version(){
	if(hw_version!=0x00){
		return (int)hw_version;
	}
	// HW: 6 + (int)(not( 0x00 | (PC0<<2) | (PG1<<1) | (PG0<<0)))
	int return_value=0;

	// PG0 -> Bit0
	// PG1 -> Bit1
	// PC0 -> Bit2

	// set all to input
	// Pin 3 und 4 auf Eingang und andere im ursprünglichen Zustand belassen:
	// DDRB &= ~((1 << DDB3) | (1 << DDB4));
	// Pin 0 und 3 wieder auf Ausgang und andere im ursprünglichen Zustand belassen:
	// DDRB |= (1 << DDB0) | (1 << DDB3);
	DDRG &= ~((1<<DDG0) | (1<<DDG1));
	DDRC &= ~((1<<DDC0));

	// set pull up to all inputs
	// PORTC |= (1<<PC7);    /* internen Pull-Up an PC7 aktivieren */
	PORTG |= ((1<<PG1) | (1<<PG0));
	PORTC |= ((1<<PC0));
	// read inputs
	return_value=6 + (unsigned char)(not(PING & (1<<PG0))<<0 | not(PING & (1<<PG1))<<1 | not(PINC & (1<<PC0))<<2);

	// disable all PULLUPS
	PORTG &= ~((1<<PG1) | (1<<PG0));
	PORTC &= ~((1<<PC0));

	// return that value
	hw_version=(char)return_value;
	return return_value;
}

void configuration::ram_info() {
	int size = 8192; // Use 2048 with ATmega328
	byte *buf;
	while ((buf = (byte *) malloc(--size)) == NULL);
	free(buf);
	Serial.print(size);

	pDebug->sprintlnp(PSTR(" Byte heap free"));
}


/*********** write config *******************
 * hier die config datei schreiben, und anhand des
 * Dateinamens checken welche var hier gespeichert
 * wird. vorher checken ob sd_failed nicht true ist
 *********** write config ******************/
int configuration::write(const char *filename){
#ifdef SD_DEBUG
	Serial.print("Writing:");
	Serial.println(filename);
#endif
	pSensors->m_gps->gps_write_status=4;
	if(!pSD->sd_failed){
		pSensors->m_gps->gps_write_status=5;
		if(storage_outdated){ // jede Minute
#ifdef SD_DEBUG
			Serial.println("Schreibe wirklich");
#endif
			pSensors->m_gps->gps_write_status=6;
			SdFile root;
			SdFile file;
			SdFile subdir;
			root.openRoot(&pSD->volume);
			/*************** SPEEDO.TXT **************************
			 * Write those fast changing things like 
			 * avg, dist, time for each of the 10 storages
			 ****************************************************/
			//if(strncmp_P(filename, PSTR("speedo.txt"),10) && storage_outdated){
			if(strncmp("speedo.txt",filename,10)==0 && storage_outdated){
				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  pDebug->sprintlnp(PSTR("open subdir /config failed")); return -1; };
				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
					//Serial.print("platzhalter1");
					return -2;
				} else {
					// get some buffer
					char buffer[35];
					memset(buffer,'\0',35);

					// write heading to file
					strcpy_P(buffer, PSTR("#This is an auto-generated file from (MonDay) "));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_clock->getdate());
					pSD->writeString(file, buffer);
					for(unsigned int i=0;i<sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]);i++){
						if(pSpeedo->max_speed[i]>350) pSpeedo->max_speed[i]=200;
						sprintf(buffer,"max_%i=%i;\n",i+1,pSpeedo->max_speed[i]%1000); // 12 chars max: max_1=300\n\0
						pSD->writeString(file, buffer);
						sprintf(buffer,"avg_time_%i=%lu;\n",i+1,pSpeedo->avg_timebase[i]); // 23 chars max: avg_time_1=4294967295\n\0
						pSD->writeString(file, buffer);
						sprintf(buffer,"trip_%i=%lu;\n\n",i+1,pSpeedo->trip_dist[i]); // 19 chars max: trip_1=4294967295\n\0
						pSD->writeString(file, buffer);
					};

					file.close();
					storage_outdated=false;
				}
				/*************** GANG.TXT **************************
				 * as soon as the user has calibrated at least one gear
				 ****************************************************/
			} else if(strncmp("GANG.TXT",filename,8)==0){
				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  pDebug->sprintlnp(PSTR("open subdir /config failed")); return -1; };
				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
					//Serial.print("platzhalter2");
					return -2;
				} else {
					// get some buffer
					char buffer[20];
					memset(buffer,'\0',20);

					// write heading to file
					strcpy_P(buffer, PSTR("#This is an auto-generated file from (MonDay) "));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_clock->getdate());
					pSD->writeString(file, buffer);


					for(unsigned int i=1;i<=6;i++){
						sprintf(buffer,"n_gang_%i=%i;\n",i,pSensors->m_gear->n_gang[i]); // 12 chars max: max_1=300\n\0
						pSD->writeString(file, buffer);
					};

					file.close();
					storage_outdated=false;
				}
				/*************** 311211.GPS **************************
				 * this function is triggered by the m_gps Sensor, every 30 sec
				 * we will the other way round ask the m_gps for information
				 * about the logged points as long as they aren't empty
				 ****************************************************/
			} else if(filename[6]=='.' && filename[7]=='G' && filename[8]=='P' && filename[9]=='S'){
				/*pSD->power_down();
				delay(20);
				pSD->power_up();*/
				//				Serial.println("Save Point start");
				// buffer for dir name /GPS/2013/06/130612.GPS
				unsigned char full_filename[24+1]; // 24 CHARS for filename + 3 for compatiblity
				// create filename
				full_filename[0]='/';
				full_filename[1]='G';
				full_filename[2]='P';
				full_filename[3]='S';
				full_filename[4]='/';
				full_filename[5]='2';
				full_filename[6]='0';
				full_filename[7]=filename[0];
				full_filename[8]=filename[1];
				full_filename[9]='/';
				full_filename[10]=filename[2];
				full_filename[11]=filename[3];
				full_filename[12]='/';
				for (int i=0;i<=9; i++){
					full_filename[i+13]=filename[i];
				}
				full_filename[23]='\0';

#ifdef SD_DEBUG
				Serial.print("*** Openning: ");
				Serial.println((char*)full_filename);
#endif
				// now open it up
				if(!pFilemanager_v2->get_file_handle(full_filename,&file,O_WRITE|O_CREAT|O_APPEND)<0){
					pDebug->sprintlnp(PSTR("open subdir /config failed"));
#ifdef SD_DEBUG
					Serial.print("*** Could not open: ");
					Serial.println((char*)full_filename);
#endif
					return -1;
				} else {
					pSensors->m_gps->gps_write_status=7;
					// get some buffer
					char buffer[63];
					memset(buffer,'\0',63);

					// get the info from the gps class
#ifdef SD_DEBUG
					Serial.print("*** vor get_logged_points ist im puffer: ");
					Serial.println(buffer);
#endif
					int i=0;
					int nbytes=0;
					while(i<100 && pSensors->m_gps->get_logged_points(buffer,i,&nbytes)>=0){
#ifdef SD_DEBUG
						Serial.print("*** get_logged_points liefert: ");
						Serial.println(buffer);
						int result=file.write((uint8_t *)buffer, nbytes);
						if(result<0){
							Serial.print("*** writeString error: ");
							Serial.println(result);
						}
#else
						file.write((uint8_t *)buffer, nbytes);
#endif
						i++;
					}
					// free buffer and close file
					file.close();
					//Serial.println("Save Point done");
					storage_outdated=false;
				}	
				/*************** BASE.TXT **************************
				 * save the basic stuff like tire outline aka meters
				 * per tick, flasher warning distance and so on
				 ****************************************************/
			} else if(strncmp("BASE.TXT",filename,8)==0){
				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {
					pDebug->sprintlnp(PSTR("open subdir /config failed"));
					return -1;
				};
				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
					//Serial.print("platzhalter4");
					return -2;
				} else {
					// get some buffer
					char buffer[45];
					memset(buffer,'\0',45);

					// write heading to file
					strcpy_P(buffer, PSTR("#This is an auto-generated file from (MonDay) "));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_clock->getdate());
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("startup="));
					pSD->writeString(file, buffer);
					pSD->writeString(file, (char *)pOLED->startup);
					pSD->writeString(file, (char *)";\n");

					strcpy_P(buffer, PSTR("blinker="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_blinker->dist_to_warn%10000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("blinker_high="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_blinker->high_speed_add%10000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("fuel_blink_start="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_fuel->blink_start%1000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("fuel_blink_freq="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_fuel->blink_freq%10000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("reifen_umfang="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i.%i\n",int(floor(pSensors->m_speed->reifen_umfang)),int(floor(pSensors->m_speed->reifen_umfang*100))%100);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("gps_takeover="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_speed->gps_takeover%1000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("refresh_cyle="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSpeedo->refresh_cycle);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("active_navi_file="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_gps->active_file%10); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("navi_active="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_gps->navi_active);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("navi_point="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pSensors->m_gps->navi_point));
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("disp_phase="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pOLED->phase));
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("disp_ref="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pOLED->ref));
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("mode_trip="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSpeedo->m_trip_mode%10); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("storage_trip="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSpeedo->m_trip_storage%10); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("skin_file="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pConfig->skin_file%10); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("oil_dist="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->m_oiler->grenze%10000); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("dz_flash="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_dz->blitz_dz); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("dz_flash_en="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_dz->blitz_en);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("dz_min="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->dz_min_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("dz_max="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->dz_max_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("oil_min="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->oil_min_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("oil_max="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->oil_max_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("water_min="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->water_min_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("water_max="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->water_max_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("kmh_min="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->kmh_min_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("kmh_max="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->kmh_max_value); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					////////////// static color /////////////
					strcpy_P(buffer, PSTR("rgb_out_static_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->static_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_static_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->static_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_static_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->static_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					////////////// dz color /////////////
					strcpy_P(buffer, PSTR("rgb_out_dz_start_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_start_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_dz_start_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_start_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_dz_start_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_start_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_dz_end_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_end_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_dz_end_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_end_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_dz_end_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_end_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);


					////////////// kmh color /////////////
					strcpy_P(buffer, PSTR("rgb_out_kmh_start_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->kmh_start_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_kmh_start_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->kmh_start_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_kmh_start_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->kmh_start_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_kmh_end_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->kmh_end_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_kmh_end_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->kmh_end_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_kmh_end_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->kmh_end_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					////////////// oil color /////////////
					strcpy_P(buffer, PSTR("rgb_out_oil_start_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->oil_start_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_oil_start_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->oil_start_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_oil_start_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->oil_start_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_oil_end_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->oil_end_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_oil_end_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->oil_end_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_oil_end_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->oil_end_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					////////////// water color /////////////
					strcpy_P(buffer, PSTR("rgb_out_water_start_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->water_start_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_water_start_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->water_start_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_water_start_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->water_start_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_water_end_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->water_end_color.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_water_end_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->water_end_color.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_water_end_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->water_end_color.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("led_mode="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->led_mode)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					//////// flash //////////
					strcpy_P(buffer, PSTR("rgb_flash_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.r)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_flash_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.g)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_flash_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.b)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);
					// RGB LEDs

					strcpy_P(buffer, PSTR("water_warning="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pSensors->m_temperature->water_warning_temp)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("oil_warning="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pSensors->m_temperature->oil_warning_temp)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("bt_pin="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%04i\n",pAktors->bt_pin); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("sensor_source="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->sensor_source); // SENSOR_AUTO 0x01, SENSOR_FORCE_CAN 0x02, SENSOR_NO_CAN --- everything else
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("CAN_type="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_CAN->get_active_can_type()); // triumph 0x01, OBD2 0x02
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("GPS_format="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_gps->use_compressed_log_format);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("LT_realtime="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pLapTimer->use_realtime_not_calculated);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("POI_active="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSpeedCams->get_active());
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("P_highl="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pAktors->pointer_highlight_mode);
					pSD->writeString(file, buffer);

					// ==============================================================================================================//


					file.close();
					storage_outdated=false;
				};
			};
			subdir.close();
			root.close();
			return 0;
		}; // storeage outdated
	}; // sd_failed
	return -3;
};


/********** prozedure zum lesen des skinfiles **************
 * mit der absicherung das nur files im richtigen bereich
 * oder notfalls das defaultfile geladen wird
 */
int configuration::read_skin(){
	if(skin_file>-1 && skin_file<9){
		char filename[10];
		sprintf(filename,"SKIN%i.SSF",skin_file);
		return read(CONFIG_FOLDER,filename,READ_MODE_CONFIGFILE,"");
	} else {
		return read(CONFIG_FOLDER,"SKIN0.SSF",READ_MODE_CONFIGFILE,""); // load default
	}
}


/******* configurations datei einlesen *************
 * filename = Dateiname im config.fOLDER verzeichniss, kein Unterverzeichniss
 * Liest zeichenweise (max 200B) ein und ruft  mit jeder Zeile parse_config auf
 ***************************************************/
int configuration::read( const char* folder, const char* filename, int read_mode, char* search_string){
	SdFile root;
	SdFile file;
	SdFile subdir;
	pDebug->sprintp(PSTR("Reading: "));
	Serial.print(filename);
	pDebug->sprintp(PSTR(" ... "));

	root.openRoot(&pSD->volume);
	if(!subdir.open(&root, folder, O_READ)) {  pDebug->sprintlnp(PSTR("open subdir /config failed")); return -1; };
	// datei im lese Modus oeffnen da config hier nicht geschrieben wird
	if (file.open(&subdir, filename, O_READ)) {
		int n,i=0;
		boolean skip_row=false;
		// store string
		char *buf;
		buf = (char*) malloc (200);
		if (buf==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
		else memset(buf,'\0',200);

		uint8_t* buffer; // buffer fuer jedes Zeichen
		buffer = (uint8_t*) malloc (2);
		if (buffer==NULL) pDebug->sprintp(PSTR("Malloc failed"));
		else memset(buffer,'\0',2);

		while ((n = file.read(buffer, 1)) > 0) { // n=1/0=wieviele byte gelesen wurden
			if(char(buffer[0])=='\n' || char(buffer[0])=='\r') {  // auswerten
				buf[i]='\0'; // eigentlich unnoetig da das ganze array mit den dinger voll ist .. oder ?
				if(i>0){
					// wir haben mehr als kein zeichen gelesen, und einen Zeilenumbruch gefunden => attake
					if(read_mode==READ_MODE_CONFIGFILE){
						int return_value=parse(&buf[0]);
						if(return_value<0) {
							pDebug->sprintp(PSTR("parse_config erzeugte Fehlercode "));
							Serial.print(return_value);
							pDebug->sprintp(PSTR(".\r\nEingabe war:"));
							Serial.println(buf);
						};
					} else if(read_mode==READ_MODE_TEXTREPLACEMENT){
						int return_value=parse_textreplacement(buf,search_string);
						if(return_value<0) {
							pDebug->sprintp(PSTR("parse_textreplacement erzeugte Fehlercode "));
							Serial.print(return_value);
							pDebug->sprintp(PSTR(". Eingabe war:"));
							Serial.println(buf);
						};
					};
				};
				i=0; // buffer leeren
				memset(buf,'\0',200); // warum geht hier nicht memset(buf,'1',200) ?
				skip_row=false; // grundstzlich geh ich mal davon aus das keine Kommentarzeile kommt
			} else if(char(buffer[0])=='#' && i==0){ // wenn wir ein # drin finden und das ganz vorne dann skippen wir die row
				skip_row=true;
			} else if(skip_row) {
				i=0;
			} else { // einlesen
				// copy it to buffer
				buf[i]=(char) buffer[0];
				i++;
				if(i>=200){ i=0; };
			};
			memset(buffer,'\0',2);
		};
		free(buf);
	} else {
		pDebug->sprintlnp(PSTR("!! File open failed !!"));
		Serial.println(filename);
		pDebug->sprintlnp(PSTR("!! File open failed !!"));
		return -1;
	};
	file.close();
	subdir.close();
	root.close();
	pDebug->sprintlnp(PSTR("Done"));
	return 0;
}

/******* configurationszeile auswerten *************
 * bekommt eine Zeile als char-array und sucht nach dem "=" und dem \0
 * ruft dann entsprechend dessen was vor dem "=" steht den entsprechenden parser auf
 * schreibt dann den wert in config.f,config.i,config.a
 ***************************************************/
int configuration::parse(char* buffer){
	char *name; // store string
	name = (char*)malloc (50);
	if (name==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));

	int seperator=0;
	while(1){ // der name soll maximal 50 zeichen lang sein
		if(char(buffer[seperator])!='='){
			name[seperator]=buffer[seperator];
		} else {
			name[seperator]='\0';
			break;
		};
		seperator++;
		if(seperator>49){ free(name); return -1; }; // programmabbruch
	};
	pDebug->parse(0,name);
#ifdef PARSE_SHORT
	Serial.print(name);
	Serial.print("=");
#endif

	int temp=0;
	int return_value=0;

	// hier wissen wir wie der name ist
	if(strcmp_P(name,PSTR("startup"))==0){
		parse_a(buffer,seperator,(char *)pOLED->startup,sizeof(pOLED->startup)/sizeof(pOLED->startup[0]));
	} else if(strcmp_P(name,PSTR("active_navi_file"))==0){ // welche datei ist die aktive ?
		parse_int(buffer,seperator,&pSensors->m_gps->active_file);
	} else if(strcmp_P(name,PSTR("navi_active"))==0){
		parse_bool(buffer,seperator,&pSensors->m_gps->navi_active);
	} else if(strcmp_P(name,PSTR("navi_point"))==0){
		parse_int(buffer,seperator,&pSensors->m_gps->navi_point);
	} else if(strcmp_P(name,PSTR("disp_phase"))==0){
		parse_uint8_t(buffer,seperator,&pOLED->phase);
	} else if(strcmp_P(name,PSTR("disp_ref"))==0){
		parse_uint8_t(buffer,seperator,&pOLED->ref);
	} else if(strcmp_P(name,PSTR("skin_file"))==0){ // welche datei ist der aktive skin
		parse_int(buffer,seperator,&pConfig->skin_file);
	} else if(strcmp_P(name,PSTR("oil_dist"))==0){ // distanz in meter nachder ge...lt wird
		parse_int(buffer,seperator,&pAktors->m_oiler->grenze);
	} else if(strncmp("oil_temp_r_",name,11)==0){ // ganzen Block auslesen, alle temp_rXXX gehen hier rein
		char var_name[14]; // watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings von temp_r_0 bis temp_r_18 erzeugen
			sprintf(var_name,"oil_temp_r_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSensors->m_temperature->oil_r_werte[j]);
			};
		};
	} else if(strncmp("oil_temp_t_",name,11)==0){// ganzen Block auslesen, alle temp_tXXX gehen hier rein
		char var_name[14];// watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"oil_temp_t_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_uint8_t(buffer,seperator,(uint8_t*)&pSensors->m_temperature->oil_t_werte[j]);
			};
		};
	} else if(strncmp("water_temp_r_",name,13)==0){ // ganzen Block auslesen, alle temp_rXXX gehen hier rein
		char var_name[16];// watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings von temp_r_0 bis temp_r_18 erzeugen
			sprintf(var_name,"water_temp_r_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSensors->m_temperature->water_r_werte[j]);
			};
		};
	} else if(strncmp("water_temp_t_",name,13)==0){// ganzen Block auslesen, alle temp_tXXX gehen hier rein
		char var_name[16];// watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"water_temp_t_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_uint8_t(buffer,seperator,(uint8_t*)&pSensors->m_temperature->water_t_werte[j]);
			};
		};
	} else if(strncmp("n_gang_",name,7)==0){// ganzen Block auslesen, alle n_gangXXX gehen hier rein
		char var_name[9];
		for(int j=1;j<=6;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"n_gang_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSensors->m_gear->n_gang[j]);
			};
		};
	} else if(strncmp("max_",name,4)==0){// ganzen Block auslesen, alle max_XXX gehen hier rein
		char var_name[6];
		for(int j=0;j<9;j++){ // alle mglichen strings erzeugen: max_1 ... max_8
			sprintf(var_name,"max_%i",j+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSpeedo->max_speed[j]);
			};
		};
	} else if(strncmp("avg_time_",name,9)==0){// ganzen Block auslesen, alle avg_time_XXX gehen hier rein
		char var_name[11];
		for(int i=0;i<9;i++){ // alle mglichen strings erzeugen: avg_time_1 ... avg_time_8
			sprintf(var_name,"avg_time_%i",i+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_ul(buffer,seperator,&pSpeedo->avg_timebase[i]);
			};
		};
	} else if(strncmp("trip_",name,5)==0){// ganzen Block auslesen, alle trip_XXX gehen hier rein
		char var_name[7];
		for(int i=0;i<9;i++){ // alle mglichen strings erzeugen: trip_1 ... trip_8
			sprintf(var_name,"trip_%i",i+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_ul(buffer,seperator,&pSpeedo->trip_dist[i]);
			};
		};
	} else if(strcmp_P(name,PSTR("blinker"))==0){
		parse_int(buffer,seperator,&pSensors->m_blinker->dist_to_warn);
	} else if(strcmp_P(name,PSTR("blinker_high"))==0){
		parse_int(buffer,seperator,&pSensors->m_blinker->high_speed_add);
	} else if(strcmp_P(name,PSTR("fuel_blink_start"))==0){
		parse_int(buffer,seperator,&pSensors->m_fuel->blink_start);
	} else if(strcmp_P(name,PSTR("fuel_blink_freq"))==0){
		parse_int(buffer,seperator,&pSensors->m_fuel->blink_freq);
	} else if(strcmp_P(name,PSTR("reifen_umfang"))==0){
		float save_me=pSensors->m_speed->reifen_umfang;
		parse_float(buffer,seperator,&pSensors->m_speed->reifen_umfang);
		if(pSensors->m_speed->reifen_umfang>4 || pSensors->m_speed->reifen_umfang<0) pSensors->m_speed->reifen_umfang=save_me;
	} else if(strcmp_P(name,PSTR("gps_takeover"))==0){
		parse_int(buffer,seperator,&pSensors->m_speed->gps_takeover);
	} else if(strcmp_P(name,PSTR("refresh_cyle"))==0){
		parse_int(buffer,seperator,&pSpeedo->refresh_cycle);
	} else if(strcmp_P(name,PSTR("mode_trip"))==0){
		parse_int(buffer,seperator,&pSpeedo->m_trip_mode);
	} else if(strcmp_P(name,PSTR("storage_trip"))==0){
		parse_int(buffer,seperator,&pSpeedo->m_trip_storage);
	} else if(strcmp_P(name,PSTR("oil_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->oil_widget.x);
	} else if(strcmp_P(name,PSTR("oil_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->oil_widget.y);
	} else if(strcmp_P(name,PSTR("oil_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->oil_widget.symbol);
	} else if(strcmp_P(name,PSTR("oil_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->oil_widget.font);
	} else if(strcmp_P(name,PSTR("water_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->water_widget.x);
	} else if(strcmp_P(name,PSTR("water_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->water_widget.y);
	} else if(strcmp_P(name,PSTR("water_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->water_widget.symbol);
	} else if(strcmp_P(name,PSTR("water_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->water_widget.font);
	} else if(strcmp_P(name,PSTR("air_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->air_widget.x);
	} else if(strcmp_P(name,PSTR("air_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->air_widget.y);
	} else if(strcmp_P(name,PSTR("air_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->air_widget.symbol);
	} else if(strcmp_P(name,PSTR("air_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->air_widget.font);
	} else if(strcmp_P(name,PSTR("kmh_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->kmh_widget.x);
	} else if(strcmp_P(name,PSTR("kmh_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->kmh_widget.y);
	} else if(strcmp_P(name,PSTR("kmh_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->kmh_widget.font);
	} else if(strcmp_P(name,PSTR("kmhchar_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->kmhchar_widget.x);
	} else if(strcmp_P(name,PSTR("kmhchar_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->kmhchar_widget.y);
	} else if(strcmp_P(name,PSTR("kmhchar_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->kmhchar_widget.font);
	} else if(strcmp_P(name,PSTR("arrow_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->arrow_widget.x);
	} else if(strcmp_P(name,PSTR("arrow_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->arrow_widget.y);
	} else if(strcmp_P(name,PSTR("dz_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->dz_widget.x);
	} else if(strcmp_P(name,PSTR("dz_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->dz_widget.y);
	} else if(strcmp_P(name,PSTR("dz_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->dz_widget.font);
	} else if(strcmp_P(name,PSTR("gps_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->gps_widget.x);
	} else if(strcmp_P(name,PSTR("gps_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->gps_widget.y);
	} else if(strcmp_P(name,PSTR("gps_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->gps_widget.font);
	} else if(strcmp_P(name,PSTR("gps_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->gps_widget.symbol);
	} else if(strcmp_P(name,PSTR("addinfo_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->addinfo_widget.x);
	} else if(strcmp_P(name,PSTR("addinfo_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->addinfo_widget.y);
	} else if(strcmp_P(name,PSTR("addinfo_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->addinfo_widget.font);
	} else if(strcmp_P(name,PSTR("addinfo2_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->addinfo2_widget.x);
	} else if(strcmp_P(name,PSTR("addinfo2_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->addinfo2_widget.y);
	} else if(strcmp_P(name,PSTR("addinfo2_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->addinfo2_widget.font);
	} else if(strcmp_P(name,PSTR("clock_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->clock_widget.x);
	} else if(strcmp_P(name,PSTR("clock_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->clock_widget.y);
	} else if(strcmp_P(name,PSTR("clock_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->clock_widget.symbol);
	} else if(strcmp_P(name,PSTR("clock_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->clock_widget.font);
	} else if(strcmp_P(name,PSTR("gear_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->gear_widget.x);
	} else if(strcmp_P(name,PSTR("gear_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->gear_widget.y);
	} else if(strcmp_P(name,PSTR("gear_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->gear_widget.font);
	} else if(strcmp_P(name,PSTR("fuel_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->fuel_widget.x);
	} else if(strcmp_P(name,PSTR("fuel_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->fuel_widget.y);
	} else if(strcmp_P(name,PSTR("fuel_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->fuel_widget.symbol);
	} else if(strcmp_P(name,PSTR("fuel_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->fuel_widget.font);
	} else if(strcmp_P(name,PSTR("default_font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSpeedo->default_font);
	} else if(strcmp_P(name,PSTR("dz_flash"))==0){
		parse_int(buffer,seperator,&pSensors->m_dz->blitz_dz);
	} else if(strcmp_P(name,PSTR("dz_flash_en"))==0){
		parse_bool(buffer,seperator,&pSensors->m_dz->blitz_en);
	} else if(strcmp_P(name,PSTR("water_warning"))==0){
		parse_int(buffer,seperator,&pSensors->m_temperature->water_warning_temp);
	} else if(strcmp_P(name,PSTR("oil_warning"))==0){
		parse_int(buffer,seperator,&pSensors->m_temperature->oil_warning_temp);

		// die grenzen fuer die farben
	} else if(strcmp_P(name,PSTR("dz_min"))==0){
		parse_int(buffer,seperator,&pAktors->dz_min_value);
	} else if(strcmp_P(name,PSTR("dz_max"))==0){
		parse_int(buffer,seperator,&pAktors->dz_max_value);
	} else if(strcmp_P(name,PSTR("oil_min"))==0){
		parse_int(buffer,seperator,&pAktors->oil_min_value);
	} else if(strcmp_P(name,PSTR("oil_max"))==0){
		parse_int(buffer,seperator,&pAktors->oil_max_value);
	} else if(strcmp_P(name,PSTR("water_min"))==0){
		parse_int(buffer,seperator,&pAktors->water_min_value);
	} else if(strcmp_P(name,PSTR("water_max"))==0){
		parse_int(buffer,seperator,&pAktors->water_max_value);
	} else if(strcmp_P(name,PSTR("kmh_min"))==0){
		parse_int(buffer,seperator,&pAktors->kmh_min_value);
	} else if(strcmp_P(name,PSTR("kmh_max"))==0){
		parse_int(buffer,seperator,&pAktors->kmh_max_value);

		/////// RGB LEDs /////////
	} else if(strcmp_P(name,PSTR("rgb_flash_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_flasher.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_flash_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_flasher.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_flash_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_flasher.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_static_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->static_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_static_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->static_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_static_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->static_color.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_oil_start_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->oil_start_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_oil_start_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->oil_start_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_oil_start_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->oil_start_color.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_oil_end_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->oil_end_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_oil_end_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->oil_end_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_oil_end_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->oil_end_color.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_water_start_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->water_start_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_water_start_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->water_start_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_water_start_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->water_start_color.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_water_end_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->water_end_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_water_end_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->water_end_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_water_end_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->water_end_color.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_kmh_start_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->kmh_start_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_kmh_start_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->kmh_start_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_kmh_start_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->kmh_start_color.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_kmh_end_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->kmh_end_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_kmh_end_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->kmh_end_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_kmh_end_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->kmh_end_color.b=temp;

	} else if(strcmp_P(name,PSTR("rgb_out_dz_start_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_start_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_dz_start_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_start_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_dz_start_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_start_color.b=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_dz_end_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_end_color.r=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_dz_end_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_end_color.g=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_dz_end_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_end_color.b=temp;
	} else if(strcmp_P(name,PSTR("led_mode"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pAktors->led_mode);
		/////// RGB LEDs /////////
	} else if(strcmp_P(name,PSTR("bt_pin"))==0){
		parse_int(buffer,seperator,&pAktors->bt_pin);
	} else if(strcmp_P(name,PSTR("sensor_source"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pSensors->sensor_source);
	} else if(strcmp_P(name,PSTR("CAN_type"))==0){
		int temp=0x00;
		parse_int(buffer,seperator,&temp);
		pSensors->m_CAN->set_active_can_type((unsigned char)(temp&0xff));
	} else if(strcmp_P(name,PSTR("GPS_format"))==0){
		parse_bool(buffer,seperator,&pSensors->m_gps->use_compressed_log_format);
	} else if(strcmp_P(name,PSTR("LT_realtime"))==0){
		parse_bool(buffer,seperator,&pLapTimer->use_realtime_not_calculated);
	} else if(strcmp_P(name,PSTR("POI_active"))==0){
		bool temp;
		parse_bool(buffer,seperator,&temp);
		pSpeedCams->set_active(temp);
	} else if(strcmp_P(name,PSTR("P_highl"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&pAktors->pointer_highlight_mode);
	} else {
		return_value=-2; // ungltiger identifier
	}
	free(name);
	return return_value; // alles bestens?
};



/******* float parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab dem gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * gleichzeitig wird berprft ob der zahlenbereich hinkommen kann
 ***************************************************/
int configuration::parse_float(char* buffer,int i,float* wert){
	pDebug->parse_float(0,buffer,0,0,0);

	float t_wert=0;
	boolean decade_active=false; // false=vorkommabetrieb
	int decade_count=0; // anzahl an decadenstellen
	int vz=1; // um neg zahl zu parsen
	i++; // das "=" weglesen
	while(1){
		pDebug->parse_float(1,buffer,i,0,0);

		if(char(buffer[i])=='\0' || char(buffer[i])=='\n' || char(buffer[i])==';'){
			break;
		}
		// unterscheiden zwischen punkten, minus und zahlen und ungueltigen
		if(int(buffer[i])==46){
			decade_active=true;
		} else if(int(buffer[i])==45){ // "-" TODO: das checken, scheint als obs nicht klappt
			pDebug->parse_float(2,buffer,i,0,0);
			vz=-1;
		} else if(  int(buffer[i])>57 || int(buffer[i])<48   ){
			return -1; // keine zahl
		} else {
			if(decade_active){  decade_count++;        };
			t_wert=10*t_wert+(int(buffer[i])-48);
		};
		i++;
		if(i>198){
			return -2; // ich glaub soweit kann float gar nicht, aber solang wre das array max
		};
	}

	pDebug->parse_float(3,buffer,t_wert,decade_count,vz);
	t_wert=t_wert/(pow(10,decade_count))*vz;
	pDebug->parse_float(4,buffer,t_wert,decade_count,vz);

	*wert=t_wert;
#ifdef PARSE_SHORT
	Serial.print(*wert);
	Serial.println("<<--");
#endif
	//debug
	return 0;
};

/******* float parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab dem gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * leerzeichen werden ignoriert
 ***************************************************/
int configuration::parse_bool(char* buffer,int i,bool* wert){
	bool t_wert = false;
	i++; // das "=" weglesen
	while(1){ 
		if(char(buffer[i])=='\0' || char(buffer[i])=='\n' || char(buffer[i])==';') 
			break;
		if(int(buffer[i])==49){ // ASCII(49) = "1"
			t_wert=true;
			break;
		} else if(int(buffer[i])!=32) { // Alles was nicht "1" und nicht " " ist ist "0" ;-)
			t_wert=false;
			break;
		};
		i++;
		if(i>198){
			break;
		};
	}

	*wert=t_wert; 
	//debug
#ifdef PARSE_SHORT
	Serial.print(*wert);
	Serial.println("<<--");
#endif
	//debug
	return 0;
};

/******* int parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab wo gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * wird von parse_float bedient und gecastet ..
 ***************************************************/
int configuration::parse_int(char* buffer,int i,int* wert){
	float temp;
	int return_value=parse_float(buffer,i,&temp);
	//	Serial.println(temp);
	*wert=int(temp);

	pDebug->parse_int();
	return return_value;
};


/******* short uint8_t *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab wo gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * wird von parse_float bedient und gecastet ..
 ***************************************************/
int configuration::parse_uint8_t(char* buffer,int i,uint8_t* wert){
	float temp;
	int return_value=parse_float(buffer,i,&temp);
	*wert=int(temp)&0xff;	// cast to int to get ridge of the ".xxx" and than take lowest byte

	return return_value;
};

/******* chararray aus chararray parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab wo gesucht werden soll
 ***************************************************/
int configuration::parse_a(char* buffer,int i,char* wert,int max_length){
	int j=0;
	i++;
	while(j<max_length-1){
		if(char(buffer[i])=='\0' || char(buffer[i])=='\n'  || char(buffer[i])==';') break;
		wert[j]=buffer[i];
		j++; i++;
		if(i>198){ // buffer got 200 byte, last should be '\0' so read max 0 to 198
			return -2;
		}
	}
	wert[j]='\0';

	pDebug->parse_a(0,wert);
	return 0;
};

/******* unsigned long parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab dem gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * gleichzeitig wird ueberprueft ob der zahlenbereich hinkommen kann
 * Im prinzip wie parse_float, nur ohne vorzeichen und ohne punkt interpretation
 ***************************************************/
int configuration::parse_ul(char* buffer,int i,unsigned long* wert){
	unsigned long t_wert=0;
	i++; // das "=" weglesen
	while(1){
		if(char(buffer[i])=='\0' || char(buffer[i])=='\n' || char(buffer[i])==';') break;
		// unterscheiden zwischen punkten, minus und zahlen und ungueltigen
		if(  int(buffer[i])>57 || int(buffer[i])<48   ){
			return -1; // keine zahl
		} else {
			t_wert=10*t_wert+(int(buffer[i])-48);
		};
		i++;
		if(i>198){
			return -2; // ich glaub soweit kann float gar nicht, aber solang wre das array max
		};
	}
	*wert=t_wert;

#ifdef PARSE_SHORT
	Serial.print(*wert);
	Serial.println("<<--");
#endif
	pDebug->parse_ul(0,*wert);
	return 0;
};

// in diese routine werden die km hochgezaehlt und eventuell das auf die karte/eeprom speichern veranlasst
void configuration::km_save(){
	int speed_value=pSensors->get_speed(false);
	// debug
#ifdef STORAGE_DEBUG
	pDebug->sprintp(PSTR("calling km_save"));
#endif
	// debug
	if(pSensors->get_RPM(RPM_TYPE_DIRECT)>0){ // if motor is running
		// debug
#ifdef STORAGE_DEBUG
		pDebug->sprintlnp(PSTR("speed>0 => storage outdated"));
#endif
		// debug
		storage_outdated=true;
		unsigned int strecke_m=round(speed_value/3.6*1); // km/h => m/1sec
		// debug
#ifdef STORAGE_DEBUG
		Serial.print("Prozentualer Unterschied zum letzen mal (muss zwischen 95 und 105 liegen): "); Serial.println(round(last_speed_value*100/speed_value));
		Serial.print("Speed value: "); Serial.println(speed_value);
#endif
		// debug
		for(unsigned int a=0;a<sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]);a++){
			// rescue after error
			if(pSpeedo->max_speed[a]>300){
				pSpeedo->max_speed[a]=speed_value;
			};
			// trips hochzaehlen, die sekunden basis +1 und die avg_speed + den aktuellen speed
			pSpeedo->trip_dist[a]+=strecke_m;
			pSpeedo->avg_timebase[a]++;
			// falls die geschwindigkeit zwischen 95% und 105% liegt
			// das dient dazu das ein peak gefiltert wird, wenn man tatsaechlich so schnell ist, wird man auch 1 sek lang aehnlich schnell bleiben
			if((last_speed_value*100/speed_value)>=95 && (last_speed_value*100/speed_value)<=105){
				// nur uebernehmen wenn unter 300 aber ueber der letzten max
				if(speed_value>pSpeedo->max_speed[a] && speed_value<256){
					pSpeedo->max_speed[a]=speed_value;
					// debug
#ifdef STORAGE_DEBUG
					Serial.print("====> Max updated: ");
					Serial.print(pSpeedo->max_speed[a]);
					Serial.println(" <==========");
#endif
					// debug
				};
			};
		};
		// save as backup
		last_speed_value=speed_value;

	};
	//save it
	if(pSensors->m_clock->get_ss()==59){
		write("speedo.txt");
	};
};


void configuration::day_trip_check(){
	//load date_of_today
	int temp =eeprom_read_byte((const uint8_t *)3);
	int temp2=eeprom_read_byte((const uint8_t *)4);
	int date_of_today=temp+100*temp2;
	// reset day trip at next day
	// wenn keine RTC verbunden, dann gibt clock_getdate() 0 zurueck.
	// wenn nun noch gps verbunden ist, dann wird ein richtiges datum
	// abgespeichert und das hier bei jedem start durchlaufen.
	// aber da clock_getdate()==0 ist, wird keine datei geloescht werden.
#ifdef STORAGE_DEBUG
	Serial.print("Laut speicher war das Datum zuletzt der ");
	Serial.print(date_of_today);
	Serial.print(" und heute ist der ");
	Serial.println(pSensors->m_clock->getdate());
#endif
	if(pSensors->m_clock->getdate()!=date_of_today){
		pSpeedo->trip_dist[2]=0;
		pSpeedo->max_speed[2]=0;
		pSpeedo->avg_timebase[2]=0;

		if(pSensors->m_clock->getdate()!=0){ // wenn wir heute einen anderen tag haben als im "heute" vom eeprom steht dann schreiben wir das heute in den eeprom
			// write "today"
			byte tempByte = ((int)floor(pSensors->m_clock->getdate()/100) & 0xFF);
			eeprom_write_byte((uint8_t *)4,tempByte);
			tempByte = ((int)floor(pSensors->m_clock->getdate()%100) & 0xFF);
			eeprom_write_byte((uint8_t *)3,tempByte);

			storage_outdated=true; // zum speichern zwingen
			write("speedo.txt"); // und ab dafuer
		};
	};
}


int configuration::parse_textreplacement(char* buffer, char* search_recopy_string){
	int seperator=0;
	while(1){ // der name soll maximal 50 zeichen lang sein
		if(char(buffer[seperator])=='='){
			if(seperator==0){ // if the "=" is on pos 0 [=bla] ond nothing left of it
				return -2;
			};
			break;
		} else {
			seperator++;
		};
		if(seperator>49){ return -1; }; // programmabbruch
	};

	int return_value=0;

	// hier wissen wir wie der name ist
	int search_recopy_pointer=0;
	int buffer_pointer=seperator+1; // skip "="
	if(strncmp(buffer,search_recopy_string,seperator)==0){
		while(1){
			if(search_recopy_pointer>=22){
				search_recopy_string[search_recopy_pointer]=0x00;
				break;
			} else if(buffer[buffer_pointer]=='\n' || buffer[buffer_pointer]==0x00 || buffer[buffer_pointer]>=0x7F || buffer[buffer_pointer]<=0x20 || buffer[buffer_pointer]==';' || buffer[buffer_pointer]=='#'){
				search_recopy_string[search_recopy_pointer]=0x00;
				break;
			} else {
				search_recopy_string[search_recopy_pointer]=buffer[buffer_pointer];
			}
			buffer_pointer++;
			search_recopy_pointer++;
		}
		return_value=1; //found
	}
	return return_value;
}
