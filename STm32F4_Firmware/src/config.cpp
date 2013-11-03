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
//	if(hw_version!=0x00){
//		return (int)hw_version;
//	}
//	// HW: 6 + (int)(not( 0x00 | (PC0<<2) | (PG1<<1) | (PG0<<0)))
//	int return_value=0;
//
//	// PG0 -> Bit0
//	// PG1 -> Bit1
//	// PC0 -> Bit2
//
//	// set all to input
//	// Pin 3 und 4 auf Eingang und andere im ursprünglichen Zustand belassen:
//	// DDRB &= ~((1 << DDB3) | (1 << DDB4));
//	// Pin 0 und 3 wieder auf Ausgang und andere im ursprünglichen Zustand belassen:
//	// DDRB |= (1 << DDB0) | (1 << DDB3);
//	DDRG &= ~((1<<DDG0) | (1<<DDG1));
//	DDRC &= ~((1<<DDC0));
//
//	// set pull up to all inputs
//	// PORTC |= (1<<PC7);    /* internen Pull-Up an PC7 aktivieren */
//	PORTG |= ((1<<PG1) | (1<<PG0));
//	PORTC |= ((1<<PC0));
//	// read inputs
//	return_value=6 + (unsigned char)(not(PING & (1<<PG0))<<0 | not(PING & (1<<PG1))<<1 | not(PINC & (1<<PC0))<<2);
//
//	// disable all PULLUPS
//	PORTG &= ~((1<<PG1) | (1<<PG0));
//	PORTC &= ~((1<<PC0));
//
//	// return that value
//	hw_version=(char)return_value;
//	return return_value;
}

void configuration::ram_info() {
	uint16_t size = 60000; // Use 2048 with ATmega328
	byte *buf;
	while ((buf = (byte *) malloc(--size)) == NULL);
	free(buf);
	Serial.puts(USART1,int(size/1000));
	Serial.puts(USART1,int(size%1000));

	Serial.puts_ln(USART1,(" Byte heap free"));
}


/*********** write config *******************
 * hier die config datei schreiben, und anhand des
 * Dateinamens checken welche var hier gespeichert
 * wird. vorher checken ob sd_failed nicht true ist
 *********** write config ******************/
int configuration::write(const char *filename){
//#ifdef SD_DEBUG
//	Serial.put(USART,"Writing:");
//	Serial.put_ln(USART,filename);
//#endif
//	Sensors.mGPS.gps_write_status=4;
//	if(!pSD->sd_failed){
//		Sensors.mGPS.gps_write_status=5;
//		if(storage_outdated){ // jede Minute
//#ifdef SD_DEBUG
//			Serial.put_ln(USART,"Schreibe wirklich");
//#endif
//			Sensors.mGPS.gps_write_status=6;
//			SdFile root;
//			SdFile file;
//			SdFile subdir;
//			root.openRoot(&pSD->volume);
//			/*************** SPEEDO.TXT **************************
//			 * Write those fast changing things like
//			 * avg, dist, time for each of the 10 storages
//			 ****************************************************/
//			//if(strncmp_P(filename, ("speedo.txt"),10) && storage_outdated){
//			if(strncmp("speedo.txt",filename,10)==0 && storage_outdated){
//				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  Serial.puts_ln(USART1,("open subdir /config failed")); return -1; };
//				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
//					//Serial.put(USART,"platzhalter1");
//					return -2;
//				} else {
//					// get some buffer
//					char buffer[35];
//					memset(buffer,'\0',35);
//
//					// write heading to file
//					strcpy_P(buffer, ("#This is an auto-generated file from (MonDay) "));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.mClock.getdate());
//					pSD->writeString(file, buffer);
//					for(unsigned int i=0;i<sizeof(Speedo.max_speed)/sizeof(Speedo.max_speed[0]);i++){
//						if(Speedo.max_speed[i]>350) Speedo.max_speed[i]=200;
//						sprintf(buffer,"max_%i=%i;\n",i+1,Speedo.max_speed[i]%1000); // 12 chars max: max_1=300\n\0
//						pSD->writeString(file, buffer);
//						sprintf(buffer,"avg_time_%i=%lu;\n",i+1,Speedo.avg_timebase[i]); // 23 chars max: avg_time_1=4294967295\n\0
//						pSD->writeString(file, buffer);
//						sprintf(buffer,"trip_%i=%lu;\n\n",i+1,Speedo.trip_dist[i]); // 19 chars max: trip_1=4294967295\n\0
//						pSD->writeString(file, buffer);
//					};
//
//					file.close();
//					storage_outdated=false;
//				}
//				/*************** GANG.TXT **************************
//				 * as soon as the user has calibrated at least one gear
//				 ****************************************************/
//			} else if(strncmp("GANG.TXT",filename,8)==0){
//				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  Serial.puts_ln(USART1,("open subdir /config failed")); return -1; };
//				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
//					//Serial.put(USART,"platzhalter2");
//					return -2;
//				} else {
//					// get some buffer
//					char buffer[20];
//					memset(buffer,'\0',20);
//
//					// write heading to file
//					strcpy_P(buffer, ("#This is an auto-generated file from (MonDay) "));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.mClock.getdate());
//					pSD->writeString(file, buffer);
//
//
//					for(unsigned int i=1;i<=6;i++){
//						sprintf(buffer,"n_gang_%i=%i;\n",i,Sensors.m_gear->n_gang[i]); // 12 chars max: max_1=300\n\0
//						pSD->writeString(file, buffer);
//					};
//
//					file.close();
//					storage_outdated=false;
//				}
//				/*************** 311211.GPS **************************
//				 * this function is triggered by the m_gps Sensor, every 30 sec
//				 * we will the other way round ask the m_gps for information
//				 * about the logged points as long as they aren't empty
//				 ****************************************************/
//			} else if(filename[6]=='.' && filename[7]=='G' && filename[8]=='P' && filename[9]=='S'){
//				/*pSD->power_down();
//				delay(20);
//				pSD->power_up();*/
//				//				Serial.put_ln(USART,"Save Point start");
//				// buffer for dir name /GPS/2013/06/130612.GPS
//				unsigned char full_filename[24+1]; // 24 CHARS for filename + 3 for compatiblity
//				// create filename
//				full_filename[0]='/';
//				full_filename[1]='G';
//				full_filename[2]='P';
//				full_filename[3]='S';
//				full_filename[4]='/';
//				full_filename[5]='2';
//				full_filename[6]='0';
//				full_filename[7]=filename[0];
//				full_filename[8]=filename[1];
//				full_filename[9]='/';
//				full_filename[10]=filename[2];
//				full_filename[11]=filename[3];
//				full_filename[12]='/';
//				for (int i=0;i<=9; i++){
//					full_filename[i+13]=filename[i];
//				}
//				full_filename[23]='\0';
//
//#ifdef SD_DEBUG
//				Serial.put(USART,"*** Openning: ");
//				Serial.put_ln(USART,(char*)full_filename);
//#endif
//				// now open it up
//				if(!pFilemanager_v2->get_file_handle(full_filename,&file,O_WRITE|O_CREAT|O_APPEND)<0){
//					Serial.puts_ln(USART1,("open subdir /config failed"));
//#ifdef SD_DEBUG
//					Serial.put(USART,"*** Could not open: ");
//					Serial.put_ln(USART,(char*)full_filename);
//#endif
//					return -1;
//				} else {
//					Sensors.mGPS.gps_write_status=7;
//					// get some buffer
//					char buffer[63];
//					memset(buffer,'\0',63);
//
//					// get the info from the gps class
//#ifdef SD_DEBUG
//					Serial.put(USART,"*** vor get_logged_points ist im puffer: ");
//					Serial.put_ln(USART,buffer);
//#endif
//					int i=0;
//					int nbytes=0;
//					while(i<100 && Sensors.mGPS.get_logged_points(buffer,i,&nbytes)>=0){
//#ifdef SD_DEBUG
//						Serial.put(USART,"*** get_logged_points liefert: ");
//						Serial.put_ln(USART,buffer);
//						int result=file.write((uint8_t *)buffer, nbytes);
//						if(result<0){
//							Serial.put(USART,"*** writeString error: ");
//							Serial.put_ln(USART,result);
//						}
//#else
//						file.write((uint8_t *)buffer, nbytes);
//#endif
//						i++;
//					}
//					// free buffer and close file
//					file.close();
//					//Serial.put_ln(USART,"Save Point done");
//					storage_outdated=false;
//				}
//				/*************** BASE.TXT **************************
//				 * save the basic stuff like tire outline aka meters
//				 * per tick, flasher warning distance and so on
//				 ****************************************************/
//			} else if(strncmp("BASE.TXT",filename,8)==0){
//				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {
//					Serial.puts_ln(USART1,("open subdir /config failed"));
//					return -1;
//				};
//				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
//					//Serial.put(USART,"platzhalter4");
//					return -2;
//				} else {
//					// get some buffer
//					char buffer[45];
//					memset(buffer,'\0',45);
//
//					// write heading to file
//					strcpy_P(buffer, ("#This is an auto-generated file from (MonDay) "));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.mClock.getdate());
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("startup="));
//					pSD->writeString(file, buffer);
//					pSD->writeString(file, (char *)TFT.startup);
//					pSD->writeString(file, (char *)";\n");
//
//					strcpy_P(buffer, ("blinker="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_blinker->dist_to_warn%10000);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("blinker_high="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_blinker->high_speed_add%10000);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("fuel_blink_start="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_fuel->blink_start%1000);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("fuel_blink_freq="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_fuel->blink_freq%10000);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("reifen_umfang="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i.%i\n",int(floor(Sensors.m_speed->reifen_umfang)),int(floor(Sensors.m_speed->reifen_umfang*100))%100);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("gps_takeover="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_speed->gps_takeover%1000);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("refresh_cyle="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Speedo.refresh_cycle);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("active_navi_file="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.mGPS.active_file%10); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("navi_active="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.mGPS.navi_active);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("navi_point="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(Sensors.mGPS.navi_point));
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("disp_phase="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(TFT.phase));
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("disp_ref="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(TFT.ref));
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("mode_trip="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Speedo.m_trip_mode%10); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("storage_trip="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Speedo.m_trip_storage%10); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("skin_file="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pConfig->skin_file%10); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("oil_dist="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->m_oiler->grenze%10000); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("dz_flash="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_dz->blitz_dz); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("dz_flash_en="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_dz->blitz_en);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("dz_min="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->dz_min_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("dz_max="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->dz_max_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("oil_min="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->oil_min_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("oil_max="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->oil_max_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("water_min="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->water_min_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("water_max="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->water_max_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("kmh_min="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->kmh_min_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("kmh_max="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->kmh_max_value); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					////////////// static color /////////////
//					strcpy_P(buffer, ("rgb_out_static_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->static_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_static_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->static_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_static_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->static_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					////////////// dz color /////////////
//					strcpy_P(buffer, ("rgb_out_dz_start_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_start_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_dz_start_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_start_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_dz_start_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_start_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_dz_end_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_end_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_dz_end_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_end_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_dz_end_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_end_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//
//					////////////// kmh color /////////////
//					strcpy_P(buffer, ("rgb_out_kmh_start_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->kmh_start_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_kmh_start_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->kmh_start_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_kmh_start_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->kmh_start_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_kmh_end_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->kmh_end_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_kmh_end_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->kmh_end_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_kmh_end_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->kmh_end_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					////////////// oil color /////////////
//					strcpy_P(buffer, ("rgb_out_oil_start_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->oil_start_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_oil_start_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->oil_start_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_oil_start_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->oil_start_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_oil_end_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->oil_end_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_oil_end_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->oil_end_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_oil_end_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->oil_end_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					////////////// water color /////////////
//					strcpy_P(buffer, ("rgb_out_water_start_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->water_start_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_water_start_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->water_start_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_water_start_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->water_start_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_water_end_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->water_end_color.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_water_end_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->water_end_color.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_out_water_end_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->water_end_color.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("led_mode="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->led_mode)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					//////// flash //////////
//					strcpy_P(buffer, ("rgb_flash_r="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.r)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_flash_g="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.g)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("rgb_flash_b="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.b)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//					// RGB LEDs
//
//					strcpy_P(buffer, ("water_warning="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(Sensors.m_temperature->water_warning_temp)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("oil_warning="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",int(Sensors.m_temperature->oil_warning_temp)); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("bt_pin="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%04i\n",pAktors->bt_pin); // 12 chars max: max_=1=300\n\0
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("sensor_source="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.sensor_source); // SENSOR_AUTO 0x01, SENSOR_FORCE_CAN 0x02, SENSOR_NO_CAN --- everything else
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("CAN_type="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.m_CAN->get_active_can_type()); // triumph 0x01, OBD2 0x02
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("GPS_format="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",Sensors.mGPS.use_compressed_log_format);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("LT_realtime="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pLapTimer->use_realtime_not_calculated);
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("POI_active="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pSpeedCams->get_active());
//					pSD->writeString(file, buffer);
//
//					strcpy_P(buffer, ("P_highl="));
//					pSD->writeString(file, buffer);
//					sprintf(buffer,"%i\n",pAktors->pointer_highlight_mode);
//					pSD->writeString(file, buffer);
//
//					// ==============================================================================================================//
//
//
//					file.close();
//					storage_outdated=false;
//				};
//			};
//			subdir.close();
//			root.close();
//			return 0;
//		}; // storeage outdated
//	}; // sd_failed
	return -3;
};


/********** prozedure zum lesen des skinfiles **************
 * mit der absicherung das nur files im richtigen bereich
 * oder notfalls das defaultfile geladen wird
 */
int configuration::read_skin(){
//	if(skin_file>-1 && skin_file<9){
//		char filename[10];
//		sprintf(filename,"SKIN%i.SSF",skin_file);
//		return read(CONFIG_FOLDER,filename,READ_MODE_CONFIGFILE,"");
//	} else {
//		return read(CONFIG_FOLDER,"SKIN0.SSF",READ_MODE_CONFIGFILE,""); // load default
//	}
}


/******* configurations datei einlesen *************
 * filename = Dateiname im config.fOLDER verzeichniss, kein Unterverzeichniss
 * Liest zeichenweise (max 200B) ein und ruft  mit jeder Zeile parse_config auf
 ***************************************************/
int configuration::read( const char* folder, const char* filename, int read_mode, char* search_string){
//	SdFile root;
//	SdFile file;
//	SdFile subdir;
//	Serial.puts(USART1,("Reading: "));
//	Serial.put(USART,filename);
//	Serial.puts(USART1,(" ... "));
//
//	root.openRoot(&pSD->volume);
//	if(!subdir.open(&root, folder, O_READ)) {  Serial.puts_ln(USART1,("open subdir /config failed")); return -1; };
//	// datei im lese Modus oeffnen da config hier nicht geschrieben wird
//	if (file.open(&subdir, filename, O_READ)) {
//		int n,i=0;
//		boolean skip_row=false;
//		// store string
//		char *buf;
//		buf = (char*) malloc (200);
//		if (buf==NULL) Serial.puts_ln(USART1,("Malloc failed"));
//		else memset(buf,'\0',200);
//
//		uint8_t* buffer; // buffer fuer jedes Zeichen
//		buffer = (uint8_t*) malloc (2);
//		if (buffer==NULL) Serial.puts(USART1,("Malloc failed"));
//		else memset(buffer,'\0',2);
//
//		while ((n = file.read(buffer, 1)) > 0) { // n=1/0=wieviele byte gelesen wurden
//			if(char(buffer[0])=='\n' || char(buffer[0])=='\r') {  // auswerten
//				buf[i]='\0'; // eigentlich unnoetig da das ganze array mit den dinger voll ist .. oder ?
//				if(i>0){
//					// wir haben mehr als kein zeichen gelesen, und einen Zeilenumbruch gefunden => attake
//					if(read_mode==READ_MODE_CONFIGFILE){
//						int return_value=parse(&buf[0]);
//						if(return_value<0) {
//							Serial.puts(USART1,("parse_config erzeugte Fehlercode "));
//							Serial.put(USART,return_value);
//							Serial.puts(USART1,(".\r\nEingabe war:"));
//							Serial.put_ln(USART,buf);
//						};
//					} else if(read_mode==READ_MODE_TEXTREPLACEMENT){
//						int return_value=parse_textreplacement(buf,search_string);
//						if(return_value<0) {
//							Serial.puts(USART1,("parse_textreplacement erzeugte Fehlercode "));
//							Serial.put(USART,return_value);
//							Serial.puts(USART1,(". Eingabe war:"));
//							Serial.put_ln(USART,buf);
//						};
//					};
//				};
//				i=0; // buffer leeren
//				memset(buf,'\0',200); // warum geht hier nicht memset(buf,'1',200) ?
//				skip_row=false; // grundstzlich geh ich mal davon aus das keine Kommentarzeile kommt
//			} else if(char(buffer[0])=='#' && i==0){ // wenn wir ein # drin finden und das ganz vorne dann skippen wir die row
//				skip_row=true;
//			} else if(skip_row) {
//				i=0;
//			} else { // einlesen
//				// copy it to buffer
//				buf[i]=(char) buffer[0];
//				i++;
//				if(i>=200){ i=0; };
//			};
//			memset(buffer,'\0',2);
//		};
//		free(buf);
//	} else {
//		Serial.puts_ln(USART1,("!! File open failed !!"));
//		Serial.put_ln(USART,filename);
//		Serial.puts_ln(USART1,("!! File open failed !!"));
//		return -1;
//	};
//	file.close();
//	subdir.close();
//	root.close();
//	Serial.puts_ln(USART1,("Done"));
//	return 0;
}

/******* configurationszeile auswerten *************
 * bekommt eine Zeile als char-array und sucht nach dem "=" und dem \0
 * ruft dann entsprechend dessen was vor dem "=" steht den entsprechenden parser auf
 * schreibt dann den wert in config.f,config.i,config.a
 ***************************************************/
int configuration::parse(char* buffer){
//	char *name; // store string
//	name = (char*)malloc (50);
//	if (name==NULL) Serial.puts_ln(USART1,("Malloc failed"));
//
//	int seperator=0;
//	while(1){ // der name soll maximal 50 zeichen lang sein
//		if(char(buffer[seperator])!='='){
//			name[seperator]=buffer[seperator];
//		} else {
//			name[seperator]='\0';
//			break;
//		};
//		seperator++;
//		if(seperator>49){ free(name); return -1; }; // programmabbruch
//	};
//	Debug.parse(0,name);
//#ifdef PARSE_SHORT
//	Serial.put(USART,name);
//	Serial.put(USART,"=");
//#endif
//
//	int temp=0;
//	int return_value=0;
//
//	// hier wissen wir wie der name ist
//	if(strcmp(name,("startup"))==0){
//		parse_a(buffer,seperator,(char *)TFT.startup,sizeof(TFT.startup)/sizeof(TFT.startup[0]));
//	} else if(strcmp(name,("active_navi_file"))==0){ // welche datei ist die aktive ?
//		parse_int(buffer,seperator,&Sensors.mGPS.active_file);
//	} else if(strcmp(name,("navi_active"))==0){
//		parse_bool(buffer,seperator,&Sensors.mGPS.navi_active);
//	} else if(strcmp(name,("navi_point"))==0){
//		parse_int(buffer,seperator,&Sensors.mGPS.navi_point);
////	} else if(strcmp(name,("disp_phase"))==0){
////		parse_uint8_t(buffer,seperator,&TFT.phase);
////	} else if(strcmp(name,("disp_ref"))==0){
////		parse_uint8_t(buffer,seperator,&TFT.ref);
//	} else if(strcmp(name,("skin_file"))==0){ // welche datei ist der aktive skin
//		parse_int(buffer,seperator,&pConfig->skin_file);
//	} else if(strcmp(name,("oil_dist"))==0){ // distanz in meter nachder ge...lt wird
//		parse_int(buffer,seperator,&pAktors->m_oiler->grenze);
//	} else if(strncmp("oil_temp_r_",name,11)==0){ // ganzen Block auslesen, alle temp_rXXX gehen hier rein
//		char var_name[14]; // watch me i am IMPORTANT
//		for(int j=0;j<19;j++){ // alle mglichen strings von temp_r_0 bis temp_r_18 erzeugen
//			sprintf(var_name,"oil_temp_r_%i",j);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_int(buffer,seperator,&Sensors.m_temperature->oil_r_werte[j]);
//			};
//		};
//	} else if(strncmp("oil_temp_t_",name,11)==0){// ganzen Block auslesen, alle temp_tXXX gehen hier rein
//		char var_name[14];// watch me i am IMPORTANT
//		for(int j=0;j<19;j++){ // alle mglichen strings erzeugen
//			sprintf(var_name,"oil_temp_t_%i",j);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_uint8_t(buffer,seperator,(uint8_t*)&Sensors.m_temperature->oil_t_werte[j]);
//			};
//		};
//	} else if(strncmp("water_temp_r_",name,13)==0){ // ganzen Block auslesen, alle temp_rXXX gehen hier rein
//		char var_name[16];// watch me i am IMPORTANT
//		for(int j=0;j<19;j++){ // alle mglichen strings von temp_r_0 bis temp_r_18 erzeugen
//			sprintf(var_name,"water_temp_r_%i",j);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_int(buffer,seperator,&Sensors.m_temperature->water_r_werte[j]);
//			};
//		};
//	} else if(strncmp("water_temp_t_",name,13)==0){// ganzen Block auslesen, alle temp_tXXX gehen hier rein
//		char var_name[16];// watch me i am IMPORTANT
//		for(int j=0;j<19;j++){ // alle mglichen strings erzeugen
//			sprintf(var_name,"water_temp_t_%i",j);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_uint8_t(buffer,seperator,(uint8_t*)&Sensors.m_temperature->water_t_werte[j]);
//			};
//		};
//	} else if(strncmp("n_gang_",name,7)==0){// ganzen Block auslesen, alle n_gangXXX gehen hier rein
//		char var_name[9];
//		for(int j=1;j<=6;j++){ // alle mglichen strings erzeugen
//			sprintf(var_name,"n_gang_%i",j);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_int(buffer,seperator,&Sensors.m_gear->n_gang[j]);
//			};
//		};
//	} else if(strncmp("max_",name,4)==0){// ganzen Block auslesen, alle max_XXX gehen hier rein
//		char var_name[6];
//		for(int j=0;j<9;j++){ // alle mglichen strings erzeugen: max_1 ... max_8
//			sprintf(var_name,"max_%i",j+1);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_int(buffer,seperator,&Speedo.max_speed[j]);
//			};
//		};
//	} else if(strncmp("avg_time_",name,9)==0){// ganzen Block auslesen, alle avg_time_XXX gehen hier rein
//		char var_name[11];
//		for(int i=0;i<9;i++){ // alle mglichen strings erzeugen: avg_time_1 ... avg_time_8
//			sprintf(var_name,"avg_time_%i",i+1);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_ul(buffer,seperator,&Speedo.avg_timebase[i]);
//			};
//		};
//	} else if(strncmp("trip_",name,5)==0){// ganzen Block auslesen, alle trip_XXX gehen hier rein
//		char var_name[7];
//		for(int i=0;i<9;i++){ // alle mglichen strings erzeugen: trip_1 ... trip_8
//			sprintf(var_name,"trip_%i",i+1);
//			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
//				parse_ul(buffer,seperator,&Speedo.trip_dist[i]);
//			};
//		};
//	} else if(strcmp(name,("blinker"))==0){
//		parse_int(buffer,seperator,&Sensors.m_blinker->dist_to_warn);
//	} else if(strcmp(name,("blinker_high"))==0){
//		parse_int(buffer,seperator,&Sensors.m_blinker->high_speed_add);
//	} else if(strcmp(name,("fuel_blink_start"))==0){
//		parse_int(buffer,seperator,&Sensors.m_fuel->blink_start);
//	} else if(strcmp(name,("fuel_blink_freq"))==0){
//		parse_int(buffer,seperator,&Sensors.m_fuel->blink_freq);
//	} else if(strcmp(name,("reifen_umfang"))==0){
//		float save_me=Sensors.m_speed->reifen_umfang;
//		parse_float(buffer,seperator,&Sensors.m_speed->reifen_umfang);
//		if(Sensors.m_speed->reifen_umfang>4 || Sensors.m_speed->reifen_umfang<0) Sensors.m_speed->reifen_umfang=save_me;
//	} else if(strcmp(name,("gps_takeover"))==0){
//		parse_int(buffer,seperator,&Sensors.m_speed->gps_takeover);
//	} else if(strcmp(name,("refresh_cyle"))==0){
//		parse_int(buffer,seperator,&Speedo.refresh_cycle);
//	} else if(strcmp(name,("mode_trip"))==0){
//		parse_int(buffer,seperator,&Speedo.m_trip_mode);
//	} else if(strcmp(name,("storage_trip"))==0){
//		parse_int(buffer,seperator,&Speedo.m_trip_storage);
//	} else if(strcmp(name,("oil_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.oil_widget.x);
//	} else if(strcmp(name,("oil_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.oil_widget.y);
//	} else if(strcmp(name,("oil_widget.symbol"))==0){
//		parse_bool(buffer,seperator,&Speedo.oil_widget.symbol);
//	} else if(strcmp(name,("oil_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.oil_widget.font);
//	} else if(strcmp(name,("water_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.water_widget.x);
//	} else if(strcmp(name,("water_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.water_widget.y);
//	} else if(strcmp(name,("water_widget.symbol"))==0){
//		parse_bool(buffer,seperator,&Speedo.water_widget.symbol);
//	} else if(strcmp(name,("water_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.water_widget.font);
//	} else if(strcmp(name,("air_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.air_widget.x);
//	} else if(strcmp(name,("air_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.air_widget.y);
//	} else if(strcmp(name,("air_widget.symbol"))==0){
//		parse_bool(buffer,seperator,&Speedo.air_widget.symbol);
//	} else if(strcmp(name,("air_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.air_widget.font);
//	} else if(strcmp(name,("kmh_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmh_widget.x);
//	} else if(strcmp(name,("kmh_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmh_widget.y);
//	} else if(strcmp(name,("kmh_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmh_widget.font);
//	} else if(strcmp(name,("kmhchar_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmhchar_widget.x);
//	} else if(strcmp(name,("kmhchar_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmhchar_widget.y);
//	} else if(strcmp(name,("kmhchar_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmhchar_widget.font);
//	} else if(strcmp(name,("arrow_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.arrow_widget.x);
//	} else if(strcmp(name,("arrow_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.arrow_widget.y);
//	} else if(strcmp(name,("dz_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.dz_widget.x);
//	} else if(strcmp(name,("dz_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.dz_widget.y);
//	} else if(strcmp(name,("dz_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.dz_widget.font);
//	} else if(strcmp(name,("gps_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gps_widget.x);
//	} else if(strcmp(name,("gps_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gps_widget.y);
//	} else if(strcmp(name,("gps_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gps_widget.font);
//	} else if(strcmp(name,("gps_widget.symbol"))==0){
//		parse_bool(buffer,seperator,&Speedo.gps_widget.symbol);
//	} else if(strcmp(name,("addinfo_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo_widget.x);
//	} else if(strcmp(name,("addinfo_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo_widget.y);
//	} else if(strcmp(name,("addinfo_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo_widget.font);
//	} else if(strcmp(name,("addinfo2_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo2_widget.x);
//	} else if(strcmp(name,("addinfo2_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo2_widget.y);
//	} else if(strcmp(name,("addinfo2_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo2_widget.font);
//	} else if(strcmp(name,("clock_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.clock_widget.x);
//	} else if(strcmp(name,("clock_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.clock_widget.y);
//	} else if(strcmp(name,("clock_widget.symbol"))==0){
//		parse_bool(buffer,seperator,&Speedo.clock_widget.symbol);
//	} else if(strcmp(name,("clock_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.clock_widget.font);
//	} else if(strcmp(name,("gear_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gear_widget.x);
//	} else if(strcmp(name,("gear_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gear_widget.y);
//	} else if(strcmp(name,("gear_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gear_widget.font);
//	} else if(strcmp(name,("fuel_widget.x"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.fuel_widget.x);
//	} else if(strcmp(name,("fuel_widget.y"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.fuel_widget.y);
//	} else if(strcmp(name,("fuel_widget.symbol"))==0){
//		parse_bool(buffer,seperator,&Speedo.fuel_widget.symbol);
//	} else if(strcmp(name,("fuel_widget.font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.fuel_widget.font);
//	} else if(strcmp(name,("default_font"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.default_font);
//	} else if(strcmp(name,("dz_flash"))==0){
//		parse_int(buffer,seperator,&Sensors.m_dz->blitz_dz);
//	} else if(strcmp(name,("dz_flash_en"))==0){
//		parse_bool(buffer,seperator,&Sensors.m_dz->blitz_en);
//	} else if(strcmp(name,("water_warning"))==0){
//		parse_int(buffer,seperator,&Sensors.m_temperature->water_warning_temp);
//	} else if(strcmp(name,("oil_warning"))==0){
//		parse_int(buffer,seperator,&Sensors.m_temperature->oil_warning_temp);
//
//		// die grenzen fuer die farben
//	} else if(strcmp(name,("dz_min"))==0){
//		parse_int(buffer,seperator,&pAktors->dz_min_value);
//	} else if(strcmp(name,("dz_max"))==0){
//		parse_int(buffer,seperator,&pAktors->dz_max_value);
//	} else if(strcmp(name,("oil_min"))==0){
//		parse_int(buffer,seperator,&pAktors->oil_min_value);
//	} else if(strcmp(name,("oil_max"))==0){
//		parse_int(buffer,seperator,&pAktors->oil_max_value);
//	} else if(strcmp(name,("water_min"))==0){
//		parse_int(buffer,seperator,&pAktors->water_min_value);
//	} else if(strcmp(name,("water_max"))==0){
//		parse_int(buffer,seperator,&pAktors->water_max_value);
//	} else if(strcmp(name,("kmh_min"))==0){
//		parse_int(buffer,seperator,&pAktors->kmh_min_value);
//	} else if(strcmp(name,("kmh_max"))==0){
//		parse_int(buffer,seperator,&pAktors->kmh_max_value);
//
//		/////// RGB LEDs /////////
//	} else if(strcmp(name,("rgb_flash_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_flasher.r=temp;
//	} else if(strcmp(name,("rgb_flash_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_flasher.g=temp;
//	} else if(strcmp(name,("rgb_flash_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_flasher.b=temp;
//
//	} else if(strcmp(name,("rgb_out_static_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->static_color.r=temp;
//	} else if(strcmp(name,("rgb_out_static_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->static_color.g=temp;
//	} else if(strcmp(name,("rgb_out_static_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->static_color.b=temp;
//
//	} else if(strcmp(name,("rgb_out_oil_start_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->oil_start_color.r=temp;
//	} else if(strcmp(name,("rgb_out_oil_start_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->oil_start_color.g=temp;
//	} else if(strcmp(name,("rgb_out_oil_start_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->oil_start_color.b=temp;
//
//	} else if(strcmp(name,("rgb_out_oil_end_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->oil_end_color.r=temp;
//	} else if(strcmp(name,("rgb_out_oil_end_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->oil_end_color.g=temp;
//	} else if(strcmp(name,("rgb_out_oil_end_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->oil_end_color.b=temp;
//
//	} else if(strcmp(name,("rgb_out_water_start_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->water_start_color.r=temp;
//	} else if(strcmp(name,("rgb_out_water_start_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->water_start_color.g=temp;
//	} else if(strcmp(name,("rgb_out_water_start_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->water_start_color.b=temp;
//
//	} else if(strcmp(name,("rgb_out_water_end_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->water_end_color.r=temp;
//	} else if(strcmp(name,("rgb_out_water_end_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->water_end_color.g=temp;
//	} else if(strcmp(name,("rgb_out_water_end_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->water_end_color.b=temp;
//
//	} else if(strcmp(name,("rgb_out_kmh_start_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->kmh_start_color.r=temp;
//	} else if(strcmp(name,("rgb_out_kmh_start_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->kmh_start_color.g=temp;
//	} else if(strcmp(name,("rgb_out_kmh_start_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->kmh_start_color.b=temp;
//
//	} else if(strcmp(name,("rgb_out_kmh_end_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->kmh_end_color.r=temp;
//	} else if(strcmp(name,("rgb_out_kmh_end_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->kmh_end_color.g=temp;
//	} else if(strcmp(name,("rgb_out_kmh_end_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->kmh_end_color.b=temp;
//
//	} else if(strcmp(name,("rgb_out_dz_start_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_start_color.r=temp;
//	} else if(strcmp(name,("rgb_out_dz_start_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_start_color.g=temp;
//	} else if(strcmp(name,("rgb_out_dz_start_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_start_color.b=temp;
//	} else if(strcmp(name,("rgb_out_dz_end_r"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_end_color.r=temp;
//	} else if(strcmp(name,("rgb_out_dz_end_g"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_end_color.g=temp;
//	} else if(strcmp(name,("rgb_out_dz_end_b"))==0){
//		parse_int(buffer,seperator,&temp);
//		pAktors->dz_end_color.b=temp;
//	} else if(strcmp(name,("led_mode"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&pAktors->led_mode);
//		/////// RGB LEDs /////////
//	} else if(strcmp(name,("bt_pin"))==0){
//		parse_int(buffer,seperator,&pAktors->bt_pin);
//	} else if(strcmp(name,("sensor_source"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&Sensors.sensor_source);
//	} else if(strcmp(name,("CAN_type"))==0){
//		int temp=0x00;
//		parse_int(buffer,seperator,&temp);
//		Sensors.m_CAN->set_active_can_type((unsigned char)(temp&0xff));
//	} else if(strcmp(name,("GPS_format"))==0){
//		parse_bool(buffer,seperator,&Sensors.mGPS.use_compressed_log_format);
//	} else if(strcmp(name,("LT_realtime"))==0){
//		parse_bool(buffer,seperator,&pLapTimer->use_realtime_not_calculated);
//	} else if(strcmp(name,("POI_active"))==0){
//		bool temp;
//		parse_bool(buffer,seperator,&temp);
//		pSpeedCams->set_active(temp);
//	} else if(strcmp(name,("P_highl"))==0){
//		parse_uint8_t(buffer,seperator,(uint8_t*)&pAktors->pointer_highlight_mode);
//	} else {
//		return_value=-2; // ungltiger identifier
//	}
//	free(name);
//	return return_value; // alles bestens?
};



/******* float parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab dem gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * gleichzeitig wird berprft ob der zahlenbereich hinkommen kann
 ***************************************************/
int configuration::parse_float(char* buffer,int i,float* wert){
	Debug.parse_float(0,buffer,0,0,0);

	float t_wert=0;
	boolean decade_active=false; // false=vorkommabetrieb
	int decade_count=0; // anzahl an decadenstellen
	int vz=1; // um neg zahl zu parsen
	i++; // das "=" weglesen
	while(1){
		Debug.parse_float(1,buffer,i,0,0);

		if(char(buffer[i])=='\0' || char(buffer[i])=='\n' || char(buffer[i])==';'){
			break;
		}
		// unterscheiden zwischen punkten, minus und zahlen und ungueltigen
		if(int(buffer[i])==46){
			decade_active=true;
		} else if(int(buffer[i])==45){ // "-" TODO: das checken, scheint als obs nicht klappt
			Debug.parse_float(2,buffer,i,0,0);
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

	Debug.parse_float(3,buffer,t_wert,decade_count,vz);
	t_wert=t_wert/(pow(10,decade_count))*vz;
	Debug.parse_float(4,buffer,t_wert,decade_count,vz);

	*wert=t_wert;
#ifdef PARSE_SHORT
	Serial.put(USART,*wert);
	Serial.put_ln(USART,"<<--");
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
	Serial.put(USART,*wert);
	Serial.put_ln(USART,"<<--");
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
	//	Serial.put_ln(USART,temp);
	*wert=int(temp);

	Debug.parse_int();
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

	Debug.parse_a(0,wert);
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
	Serial.put(USART,*wert);
	Serial.put_ln(USART,"<<--");
#endif
	Debug.parse_ul(0,*wert);
	return 0;
};

// in diese routine werden die km hochgezaehlt und eventuell das auf die karte/eeprom speichern veranlasst
void configuration::km_save(){
	int speed_value=Sensors.get_speed(false);
	// debug
#ifdef STORAGE_DEBUG
	Serial.puts(USART1,("calling km_save"));
#endif
	// debug
	if(Sensors.get_RPM(RPM_TYPE_DIRECT)>0){ // if motor is running
		// debug
#ifdef STORAGE_DEBUG
		Serial.puts_ln(USART1,("speed>0 => storage outdated"));
#endif
		// debug
		storage_outdated=true;
		unsigned int strecke_m=round(speed_value/3.6*1); // km/h => m/1sec
		// debug
#ifdef STORAGE_DEBUG
		Serial.put(USART,"Prozentualer Unterschied zum letzen mal (muss zwischen 95 und 105 liegen): "); Serial.put_ln(USART,round(last_speed_value*100/speed_value));
		Serial.put(USART,"Speed value: "); Serial.put_ln(USART,speed_value);
#endif
		// debug
		for(unsigned int a=0;a<sizeof(Speedo.max_speed)/sizeof(Speedo.max_speed[0]);a++){
			// rescue after error
			if(Speedo.max_speed[a]>300){
				Speedo.max_speed[a]=speed_value;
			};
			// trips hochzaehlen, die sekunden basis +1 und die avg_speed + den aktuellen speed
			Speedo.trip_dist[a]+=strecke_m;
			Speedo.avg_timebase[a]++;
			// falls die geschwindigkeit zwischen 95% und 105% liegt
			// das dient dazu das ein peak gefiltert wird, wenn man tatsaechlich so schnell ist, wird man auch 1 sek lang aehnlich schnell bleiben
			if((last_speed_value*100/speed_value)>=95 && (last_speed_value*100/speed_value)<=105){
				// nur uebernehmen wenn unter 300 aber ueber der letzten max
				if(speed_value>Speedo.max_speed[a] && speed_value<256){
					Speedo.max_speed[a]=speed_value;
					// debug
#ifdef STORAGE_DEBUG
					Serial.put(USART,"====> Max updated: ");
					Serial.put(USART,Speedo.max_speed[a]);
					Serial.put_ln(USART," <==========");
#endif
					// debug
				};
			};
		};
		// save as backup
		last_speed_value=speed_value;

	};
	//save it
	if(Sensors.mClock.get_ss()==59){
		write("speedo.txt");
	};
};


void configuration::day_trip_check(){
//	//load date_of_today
//	int temp =eeprom_read_byte((const uint8_t *)3);
//	int temp2=eeprom_read_byte((const uint8_t *)4);
//	int date_of_today=temp+100*temp2;
//	// reset day trip at next day
//	// wenn keine RTC verbunden, dann gibt clock_getdate() 0 zurueck.
//	// wenn nun noch gps verbunden ist, dann wird ein richtiges datum
//	// abgespeichert und das hier bei jedem start durchlaufen.
//	// aber da clock_getdate()==0 ist, wird keine datei geloescht werden.
//#ifdef STORAGE_DEBUG
//	Serial.put(USART,"Laut speicher war das Datum zuletzt der ");
//	Serial.put(USART,date_of_today);
//	Serial.put(USART," und heute ist der ");
//	Serial.put_ln(USART,Sensors.mClock.getdate());
//#endif
//	if(Sensors.mClock.getdate()!=date_of_today){
//		Speedo.trip_dist[2]=0;
//		Speedo.max_speed[2]=0;
//		Speedo.avg_timebase[2]=0;
//
//		if(Sensors.mClock.getdate()!=0){ // wenn wir heute einen anderen tag haben als im "heute" vom eeprom steht dann schreiben wir das heute in den eeprom
//			// write "today"
//			byte tempByte = ((int)floor(Sensors.mClock.getdate()/100) & 0xFF);
//			eeprom_write_byte((uint8_t *)4,tempByte);
//			tempByte = ((int)floor(Sensors.mClock.getdate()%100) & 0xFF);
//			eeprom_write_byte((uint8_t *)3,tempByte);
//
//			storage_outdated=true; // zum speichern zwingen
//			write("speedo.txt"); // und ab dafuer
//		};
//	};
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
