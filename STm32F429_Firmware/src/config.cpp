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
	return return_value;
}

void configuration::ram_info() {
	Serial.puts(USART1,"Ram:");
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
#ifdef SD_DEBUG
	Serial.puts(USART1,"Writing:");
	Serial.puts_ln(USART1,filename);
#endif
	Sensors.mGPS.gps_write_status=4;
	if(!SD.sd_failed){
		Sensors.mGPS.gps_write_status=5;
		if(storage_outdated){ // jede Minute
#ifdef SD_DEBUG
			Serial.puts_ln(USART1,"sd is not in failed state");
#endif
			Sensors.mGPS.gps_write_status=6;
			FIL write_file;
			/*************** SPEEDO.TXT **************************
			 * Write those fast changing things like
			 * avg, dist, time for each of the 10 storages
			 ****************************************************/
			//if(strncmp_P(filename, ("speedo.txt"),10) && storage_outdated){
			if(strncmp("speedo.txt",filename,10)==0 && storage_outdated){
				unsigned char complete_path[35];
				sprintf((char*)complete_path,"%s/%s",CONFIG_FOLDER,filename);
				if(SD.get_file_handle(complete_path,&write_file,FA_READ|FA_WRITE|FA_CREATE_ALWAYS)<0){ // create always == truncate if existing
					//Serial.puts(USART1,"platzhalter1");
					return -2;
				} else {
					// get some buffer
					char buffer[35];
					memset(buffer,'\0',35);

					// write heading to file
					strcpy(buffer, ("#This is an auto-generated file from (MonDay) "));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mClock.getdate());
					SD.writeString(&write_file, buffer);
					for(unsigned int i=0;i<sizeof(Speedo.max_speed)/sizeof(Speedo.max_speed[0]);i++){
						if(Speedo.max_speed[i]>350) Speedo.max_speed[i]=200;
						sprintf(buffer,"max_%i=%i;\n",i+1,Speedo.max_speed[i]%1000); // 12 chars max: max_1=300\n\0
						SD.writeString(&write_file, buffer);
						sprintf(buffer,"avg_time_%i=%lu;\n",i+1,Speedo.avg_timebase[i]); // 23 chars max: avg_time_1=4294967295\n\0
						SD.writeString(&write_file, buffer);
						sprintf(buffer,"trip_%i=%lu;\n\n",i+1,Speedo.trip_dist[i]); // 19 chars max: trip_1=4294967295\n\0
						SD.writeString(&write_file, buffer);
					};

					f_close(&write_file);
					storage_outdated=false;
				}
				/*************** GANG.TXT **************************
				 * as soon as the user has calibrated at least one gear
				 ****************************************************/
			} else if(strncmp("GANG.TXT",filename,8)==0){
				unsigned char complete_path[35];
				sprintf((char*)complete_path,"%s/%s",CONFIG_FOLDER,filename);
				if(SD.get_file_handle(complete_path,&write_file,FA_READ|FA_WRITE|FA_CREATE_ALWAYS)<0){
					//Serial.puts(USART1,"platzhalter2");
					return -2;
				} else {
					// get some buffer
					char buffer[20];
					memset(buffer,'\0',20);

					// write heading to file
					strcpy(buffer, ("#This is an auto-generated file from (MonDay) "));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mClock.getdate());
					SD.writeString(&write_file, buffer);


					for(unsigned int i=1;i<=6;i++){
						sprintf(buffer,"n_gang_%i=%i;\n",i,Sensors.mGear.n_gang[i]); // 12 chars max: max_1=300\n\0
						SD.writeString(&write_file, buffer);
					};

					f_close(&write_file);

					storage_outdated=false;
				}
				/*************** 311211.GPS **************************
				 * this function is triggered by the m_gps Sensor, every 30 sec
				 * we will the other way round ask the m_gps for information
				 * about the logged points as long as they aren't empty
				 ****************************************************/
			} else if(filename[6]=='.' && filename[7]=='G' && filename[8]=='P' && filename[9]=='S'){
				/*SD.power_down();
					delay(20);
					SD.power_up();*/
				//				Serial.puts_ln(USART1,"Save Point start");
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
				Serial.puts(USART1,"*** Openning: ");
				Serial.puts_ln(USART1,(char*)full_filename);
#endif
				// now open it up
				if(SD.get_file_handle(full_filename,&write_file,FA_READ|FA_WRITE|FA_OPEN_ALWAYS)<0){ // open always will not truncate file
					Serial.puts_ln(USART1,("open subdir /config failed"));
#ifdef SD_DEBUG
					Serial.puts(USART1,"*** Could not open: ");
					Serial.puts_ln(USART1,(char*)full_filename);
#endif
					return -1;
				} else {
					Sensors.mGPS.gps_write_status=7;
					// move file pointer to the end to append
					f_lseek(&write_file,write_file.fsize);
					// get some buffer
					char buffer[63];
					memset(buffer,'\0',63);

					// get the info from the gps class
#ifdef SD_DEBUG
					Serial.puts(USART1,"*** vor get_logged_points ist im puffer: ");
					Serial.puts_ln(USART1,buffer);
#endif
					int i=0;
					int nbytes=0;
					while(i<100 && Sensors.mGPS.get_logged_points((uint8_t*)buffer,i,&nbytes)>=0){
#ifdef SD_DEBUG
						Serial.puts(USART1,"*** get_logged_points liefert: ");
						Serial.puts_ln(USART1,buffer);
						int result=file.write((uint8_t *)buffer, nbytes);
						if(result<0){
							Serial.puts(USART1,"*** writeString error: ");
							Serial.puts_ln(USART1,result);
						}
#else
						UINT byte_count;
						f_write(&write_file,(uint8_t*)buffer,nbytes,&byte_count);
#endif
						i++;
					}
					// free buffer and close file
					f_close(&write_file);
					//Serial.puts_ln(USART1,"Save Point done");
					storage_outdated=false;
				}
				/*************** BASE.TXT **************************
				 * save the basic stuff like tire outline aka meters
				 * per tick, flasher warning distance and so on
				 ****************************************************/
			} else if(strncmp("BASE.TXT",filename,8)==0){
				unsigned char complete_path[35];
				sprintf((char*)complete_path,"%s/%s",CONFIG_FOLDER,filename);
				if(SD.get_file_handle(complete_path,&write_file,FA_READ|FA_WRITE|FA_CREATE_ALWAYS)<0){ // create always -> truncate
					//Serial.puts(USART1,"platzhalter4");
					return -2;
				} else {
					// get some buffer
					char buffer[45];
					memset(buffer,'\0',45);

					// write heading to file
					strcpy(buffer, ("#This is an auto-generated file from (MonDay) "));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mClock.getdate());
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("startup="));
					SD.writeString(&write_file, buffer);
					SD.writeString(&write_file, (char *)TFT.startup);
					SD.writeString(&write_file, (char *)";\n");

					strcpy(buffer, ("blinker="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mFlasher.dist_to_warn%10000);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("blinker_high="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mFlasher.high_speed_add%10000);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("fuel_blink_start="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mFuel.blink_start%1000);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("fuel_blink_freq="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mFuel.blink_freq%10000);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("reifen_umfang="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i.%i\n",int(floor(Sensors.mSpeed.reifen_umfang)),int(floor(Sensors.mSpeed.reifen_umfang*100))%100);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("gps_takeover="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mSpeed.gps_takeover%1000);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("refresh_cyle="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Speedo.refresh_cycle);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("active_navi_file="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mGPS.active_file%10); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("navi_active="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mGPS.navi_active);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("navi_point="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Sensors.mGPS.navi_point));
					SD.writeString(&write_file, buffer);

					//					strcpy(buffer, ("disp_phase="));
					//					SD.writeString(&write_file, buffer);
					//					sprintf(buffer,"%i\n",int(TFT.phase));
					//					SD.writeString(&write_file, buffer);
					//
					//					strcpy(buffer, ("disp_ref="));
					//					SD.writeString(&write_file, buffer);
					//					sprintf(buffer,"%i\n",int(TFT.ref));
					//					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("mode_trip="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Speedo.m_trip_mode%10); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("storage_trip="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Speedo.m_trip_storage%10); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("skin_file="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Config.skin_file%10); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("oil_dist="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.mOiler.grenze%10000); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("dz_flash="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mRpm.blitz_dz); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("dz_flash_en="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mRpm.blitz_en);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("dz_min="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.dz_min_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("dz_max="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.dz_max_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("oil_min="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.oil_min_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("oil_max="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.oil_max_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("water_min="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.water_min_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("water_max="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.water_max_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("kmh_min="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.kmh_min_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("kmh_max="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.kmh_max_value); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					////////////// static color /////////////
					strcpy(buffer, ("rgb_out_static_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.static_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_static_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.static_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_static_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.static_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					////////////// dz color /////////////
					strcpy(buffer, ("rgb_out_dz_start_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_start_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_dz_start_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_start_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_dz_start_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_start_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_dz_end_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_end_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_dz_end_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_end_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_dz_end_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_end_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);


					////////////// kmh color /////////////
					strcpy(buffer, ("rgb_out_kmh_start_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.kmh_start_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_kmh_start_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.kmh_start_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_kmh_start_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.kmh_start_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_kmh_end_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.kmh_end_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_kmh_end_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.kmh_end_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_kmh_end_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.kmh_end_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					////////////// oil color /////////////
					strcpy(buffer, ("rgb_out_oil_start_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.oil_start_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_oil_start_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.oil_start_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_oil_start_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.oil_start_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_oil_end_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.oil_end_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_oil_end_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.oil_end_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_oil_end_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.oil_end_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					////////////// water color /////////////
					strcpy(buffer, ("rgb_out_water_start_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.water_start_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_water_start_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.water_start_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_water_start_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.water_start_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_water_end_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.water_end_color.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_water_end_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.water_end_color.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_out_water_end_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.water_end_color.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("led_mode="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.led_mode)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					//////// flash //////////
					strcpy(buffer, ("rgb_flash_r="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_flasher.r)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_flash_g="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_flasher.g)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("rgb_flash_b="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Aktors.dz_flasher.b)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);
					// RGB LEDs

					strcpy(buffer, ("water_warning="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Sensors.mTemperature.water_warning_temp)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("oil_warning="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",int(Sensors.mTemperature.oil_warning_temp)); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("bt_pin="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%04i\n",Aktors.bt_pin); // 12 chars max: max_=1=300\n\0
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("sensor_source="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.sensor_source); // SENSOR_AUTO 0x01, SENSOR_FORCE_CAN 0x02, SENSOR_NO_CAN --- everything else
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("CAN_type="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mCAN.get_active_can_type()); // triumph 0x01, OBD2 0x02
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("GPS_format="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Sensors.mGPS.use_compressed_log_format);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("LT_realtime="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",LapTimer.use_realtime_not_calculated);
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("POI_active="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",SpeedCams.get_active());
					SD.writeString(&write_file, buffer);

					strcpy(buffer, ("P_highl="));
					SD.writeString(&write_file, buffer);
					sprintf(buffer,"%i\n",Aktors.pointer_highlight_mode);
					SD.writeString(&write_file, buffer);

					// ==============================================================================================================//


					f_close(&write_file);
					storage_outdated=false;
				};
			};
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
		char filename[18];
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
	FIL file;
	char c_filename[23];
	sprintf(c_filename,"/%s/%s",folder,filename);

	Serial.puts(USART1,("Reading: "));
	Serial.puts(USART1,(char*)c_filename);
	Serial.puts(USART1,(" ... "));

	if(SD.get_file_handle((unsigned char*)c_filename,&file,FA_READ|FA_OPEN_EXISTING)>=0){
//	if(!f_open(&file, (const TCHAR *)filename, FA_READ)){

		int i=0;
		boolean skip_row=false;
		// store string
		char buf[200];

		uint8_t* buffer; // buffer fuer jedes Zeichen
		buffer = (uint8_t*) malloc (2);
		if (buffer==NULL) Serial.puts(USART1,("Malloc failed"));
		else memset(buffer,'\0',2);


		UINT n_byte_read=1;
		while (n_byte_read > 0) { // n=1/0=wieviele byte gelesen wurden
			f_read(&file, buffer, 1, &n_byte_read);
			if(n_byte_read>0){
				if(char(buffer[0])=='\n' || char(buffer[0])=='\r') {  // auswerten
					buf[i]='\0'; // eigentlich unnoetig da das ganze array mit den dinger voll ist .. oder ?
					if(i>0){
						// wir haben mehr als kein zeichen gelesen, und einen Zeilenumbruch gefunden => attake
						if(read_mode==READ_MODE_CONFIGFILE){
							int return_value=parse(&buf[0]);
							if(return_value<0) {
								Serial.puts(USART1,("parse_config erzeugte Fehlercode "));
								Serial.puts(USART1,return_value);
								Serial.puts(USART1,(".\r\nEingabe war:"));
								Serial.puts_ln(USART1,buf);
							};
						} else if(read_mode==READ_MODE_TEXTREPLACEMENT){
							int return_value=parse_textreplacement(buf,search_string);
							if(return_value<0) {
								Serial.puts(USART1,("parse_textreplacement erzeugte Fehlercode "));
								Serial.puts(USART1,return_value);
								Serial.puts(USART1,(". Eingabe war:"));
								Serial.puts_ln(USART1,buf);
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
		}
	} else {
		Serial.puts_ln(USART1,("!! File open failed !!"));
		Serial.puts_ln(USART1,(char*)filename);
		Serial.puts_ln(USART1,("!! File open failed !!"));
		return -1;
	};
	f_close(&file);
	Serial.puts_ln(USART1,("Done"));
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
	if (name==NULL) Serial.puts_ln(USART1,("Malloc failed"));

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
	Debug.parse(0,name);
#ifdef PARSE_SHORT
	Serial.puts(USART1,name);
	Serial.puts(USART1,"=");
#endif

	int16_t temp=0;
	int return_value=0;

	// hier wissen wir wie der name ist
	if(strcmp(name,("startup"))==0){
		parse_a(buffer,seperator,(char *)TFT.startup,sizeof(TFT.startup)/sizeof(TFT.startup[0]));
	} else if(strcmp(name,("active_navi_file"))==0){ // welche datei ist die aktive ?
		parse_int16_t(buffer,seperator,&Sensors.mGPS.active_file);
	} else if(strcmp(name,("navi_active"))==0){
		parse_bool(buffer,seperator,&Sensors.mGPS.navi_active);
	} else if(strcmp(name,("navi_point"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mGPS.navi_point);
		//	} else if(strcmp(name,("disp_phase"))==0){
		//		parse_uint8_t(buffer,seperator,&TFT.phase);
		//	} else if(strcmp(name,("disp_ref"))==0){
		//		parse_uint8_t(buffer,seperator,&TFT.ref);
	} else if(strcmp(name,("skin_file"))==0){ // welche datei ist der aktive skin
		parse_int16_t(buffer,seperator,&Config.skin_file);
	} else if(strcmp(name,("oil_dist"))==0){ // distanz in meter nachder ge...lt wird
		parse_int16_t(buffer,seperator,&Aktors.mOiler.grenze);
	} else if(strncmp("oil_temp_r_",name,11)==0){ // ganzen Block auslesen, alle temp_rXXX gehen hier rein
		char var_name[14]; // watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings von temp_r_0 bis temp_r_18 erzeugen
			sprintf(var_name,"oil_temp_r_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int16_t(buffer,seperator,&Sensors.mTemperature.oil_r_werte[j]);
			};
		};
	} else if(strncmp("oil_temp_t_",name,11)==0){// ganzen Block auslesen, alle temp_tXXX gehen hier rein
		char var_name[14];// watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"oil_temp_t_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_uint8_t(buffer,seperator,(uint8_t*)&Sensors.mTemperature.oil_t_werte[j]);
			};
		};
	} else if(strncmp("water_temp_r_",name,13)==0){ // ganzen Block auslesen, alle temp_rXXX gehen hier rein
		char var_name[16];// watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings von temp_r_0 bis temp_r_18 erzeugen
			sprintf(var_name,"water_temp_r_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int16_t(buffer,seperator,&Sensors.mTemperature.water_r_werte[j]);
			};
		};
	} else if(strncmp("water_temp_t_",name,13)==0){// ganzen Block auslesen, alle temp_tXXX gehen hier rein
		char var_name[16];// watch me i am IMPORTANT
		for(int j=0;j<19;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"water_temp_t_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_uint8_t(buffer,seperator,(uint8_t*)&Sensors.mTemperature.water_t_werte[j]);
			};
		};
	} else if(strncmp("n_gang_",name,7)==0){// ganzen Block auslesen, alle n_gangXXX gehen hier rein
		char var_name[9];
		for(int j=1;j<=6;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"n_gang_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int16_t(buffer,seperator,&Sensors.mGear.n_gang[j]);
			};
		};
	} else if(strncmp("max_",name,4)==0){// ganzen Block auslesen, alle max_XXX gehen hier rein
		char var_name[6];
		for(int j=0;j<9;j++){ // alle mglichen strings erzeugen: max_1 ... max_8
			sprintf(var_name,"max_%i",j+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int16_t(buffer,seperator,&Speedo.max_speed[j]);
			};
		};
	} else if(strncmp("avg_time_",name,9)==0){// ganzen Block auslesen, alle avg_time_XXX gehen hier rein
		char var_name[11];
		for(int i=0;i<9;i++){ // alle mglichen strings erzeugen: avg_time_1 ... avg_time_8
			sprintf(var_name,"avg_time_%i",i+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_ul(buffer,seperator,&Speedo.avg_timebase[i]);
			};
		};
	} else if(strncmp("trip_",name,5)==0){// ganzen Block auslesen, alle trip_XXX gehen hier rein
		char var_name[7];
		for(int i=0;i<9;i++){ // alle mglichen strings erzeugen: trip_1 ... trip_8
			sprintf(var_name,"trip_%i",i+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_ul(buffer,seperator,&Speedo.trip_dist[i]);
			};
		};
	} else if(strcmp(name,("blinker"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mFlasher.dist_to_warn);
	} else if(strcmp(name,("blinker_high"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mFlasher.high_speed_add);
	} else if(strcmp(name,("fuel_blink_start"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mFuel.blink_start);
	} else if(strcmp(name,("fuel_blink_freq"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mFuel.blink_freq);
	} else if(strcmp(name,("reifen_umfang"))==0){
		float save_me=Sensors.mSpeed.reifen_umfang;
		parse_float(buffer,seperator,&Sensors.mSpeed.reifen_umfang);
		if(Sensors.mSpeed.reifen_umfang>4 || Sensors.mSpeed.reifen_umfang<0) Sensors.mSpeed.reifen_umfang=save_me;
	} else if(strcmp(name,("gps_takeover"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mSpeed.gps_takeover);
	} else if(strcmp(name,("refresh_cyle"))==0){
		parse_int16_t(buffer,seperator,&Speedo.refresh_cycle);
	} else if(strcmp(name,("mode_trip"))==0){
		parse_int16_t(buffer,seperator,&Speedo.m_trip_mode);
	} else if(strcmp(name,("storage_trip"))==0){
		parse_int16_t(buffer,seperator,&Speedo.m_trip_storage);
	} else if(strcmp(name,("oil_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.oil_widget.x);
	} else if(strcmp(name,("oil_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.oil_widget.y);
	} else if(strcmp(name,("oil_widget.symbol"))==0){
		parse_bool(buffer,seperator,&Speedo.oil_widget.symbol);
	} else if(strcmp(name,("oil_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.oil_widget.font);
	} else if(strcmp(name,("water_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.water_widget.x);
	} else if(strcmp(name,("water_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.water_widget.y);
	} else if(strcmp(name,("water_widget.symbol"))==0){
		parse_bool(buffer,seperator,&Speedo.water_widget.symbol);
	} else if(strcmp(name,("water_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.water_widget.font);
	} else if(strcmp(name,("air_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.air_widget.x);
	} else if(strcmp(name,("air_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.air_widget.y);
	} else if(strcmp(name,("air_widget.symbol"))==0){
		parse_bool(buffer,seperator,&Speedo.air_widget.symbol);
	} else if(strcmp(name,("air_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.air_widget.font);
	} else if(strcmp(name,("kmh_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmh_widget.x);
	} else if(strcmp(name,("kmh_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmh_widget.y);
	} else if(strcmp(name,("kmh_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmh_widget.font);
	} else if(strcmp(name,("kmhchar_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmhchar_widget.x);
	} else if(strcmp(name,("kmhchar_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmhchar_widget.y);
	} else if(strcmp(name,("kmhchar_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.kmhchar_widget.font);
	} else if(strcmp(name,("arrow_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.arrow_widget.x);
	} else if(strcmp(name,("arrow_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.arrow_widget.y);
	} else if(strcmp(name,("dz_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.dz_widget.x);
	} else if(strcmp(name,("dz_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.dz_widget.y);
	} else if(strcmp(name,("dz_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.dz_widget.font);
	} else if(strcmp(name,("gps_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gps_widget.x);
	} else if(strcmp(name,("gps_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gps_widget.y);
	} else if(strcmp(name,("gps_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gps_widget.font);
	} else if(strcmp(name,("gps_widget.symbol"))==0){
		parse_bool(buffer,seperator,&Speedo.gps_widget.symbol);
	} else if(strcmp(name,("addinfo_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo_widget.x);
	} else if(strcmp(name,("addinfo_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo_widget.y);
	} else if(strcmp(name,("addinfo_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo_widget.font);
	} else if(strcmp(name,("addinfo2_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo2_widget.x);
	} else if(strcmp(name,("addinfo2_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo2_widget.y);
	} else if(strcmp(name,("addinfo2_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.addinfo2_widget.font);
	} else if(strcmp(name,("clock_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.clock_widget.x);
	} else if(strcmp(name,("clock_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.clock_widget.y);
	} else if(strcmp(name,("clock_widget.symbol"))==0){
		parse_bool(buffer,seperator,&Speedo.clock_widget.symbol);
	} else if(strcmp(name,("clock_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.clock_widget.font);
	} else if(strcmp(name,("gear_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gear_widget.x);
	} else if(strcmp(name,("gear_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gear_widget.y);
	} else if(strcmp(name,("gear_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.gear_widget.font);
	} else if(strcmp(name,("fuel_widget.x"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.fuel_widget.x);
	} else if(strcmp(name,("fuel_widget.y"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.fuel_widget.y);
	} else if(strcmp(name,("fuel_widget.symbol"))==0){
		parse_bool(buffer,seperator,&Speedo.fuel_widget.symbol);
	} else if(strcmp(name,("fuel_widget.font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.fuel_widget.font);
	} else if(strcmp(name,("default_font"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Speedo.default_font);
	} else if(strcmp(name,("dz_flash"))==0){
		parse_int16_t(buffer,seperator,(int16_t*)&Sensors.mRpm.blitz_dz);
	} else if(strcmp(name,("dz_flash_en"))==0){
		parse_bool(buffer,seperator,&Sensors.mRpm.blitz_en);
	} else if(strcmp(name,("water_warning"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mTemperature.water_warning_temp);
	} else if(strcmp(name,("oil_warning"))==0){
		parse_int16_t(buffer,seperator,&Sensors.mTemperature.oil_warning_temp);

		// die grenzen fuer die farben
	} else if(strcmp(name,("dz_min"))==0){
		parse_int16_t(buffer,seperator,&Aktors.dz_min_value);
	} else if(strcmp(name,("dz_max"))==0){
		parse_int16_t(buffer,seperator,&Aktors.dz_max_value);
	} else if(strcmp(name,("oil_min"))==0){
		parse_int16_t(buffer,seperator,&Aktors.oil_min_value);
	} else if(strcmp(name,("oil_max"))==0){
		parse_int16_t(buffer,seperator,&Aktors.oil_max_value);
	} else if(strcmp(name,("water_min"))==0){
		parse_int16_t(buffer,seperator,&Aktors.water_min_value);
	} else if(strcmp(name,("water_max"))==0){
		parse_int16_t(buffer,seperator,&Aktors.water_max_value);
	} else if(strcmp(name,("kmh_min"))==0){
		parse_int16_t(buffer,seperator,&Aktors.kmh_min_value);
	} else if(strcmp(name,("kmh_max"))==0){
		parse_int16_t(buffer,seperator,&Aktors.kmh_max_value);

		/////// RGB LEDs /////////
	} else if(strcmp(name,("rgb_flash_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_flasher.r=temp;
	} else if(strcmp(name,("rgb_flash_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_flasher.g=temp;
	} else if(strcmp(name,("rgb_flash_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_flasher.b=temp;

	} else if(strcmp(name,("rgb_out_static_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.static_color.r=temp;
	} else if(strcmp(name,("rgb_out_static_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.static_color.g=temp;
	} else if(strcmp(name,("rgb_out_static_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.static_color.b=temp;

	} else if(strcmp(name,("rgb_out_oil_start_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.oil_start_color.r=temp;
	} else if(strcmp(name,("rgb_out_oil_start_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.oil_start_color.g=temp;
	} else if(strcmp(name,("rgb_out_oil_start_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.oil_start_color.b=temp;

	} else if(strcmp(name,("rgb_out_oil_end_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.oil_end_color.r=temp;
	} else if(strcmp(name,("rgb_out_oil_end_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.oil_end_color.g=temp;
	} else if(strcmp(name,("rgb_out_oil_end_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.oil_end_color.b=temp;

	} else if(strcmp(name,("rgb_out_water_start_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.water_start_color.r=temp;
	} else if(strcmp(name,("rgb_out_water_start_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.water_start_color.g=temp;
	} else if(strcmp(name,("rgb_out_water_start_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.water_start_color.b=temp;

	} else if(strcmp(name,("rgb_out_water_end_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.water_end_color.r=temp;
	} else if(strcmp(name,("rgb_out_water_end_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.water_end_color.g=temp;
	} else if(strcmp(name,("rgb_out_water_end_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.water_end_color.b=temp;

	} else if(strcmp(name,("rgb_out_kmh_start_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.kmh_start_color.r=temp;
	} else if(strcmp(name,("rgb_out_kmh_start_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.kmh_start_color.g=temp;
	} else if(strcmp(name,("rgb_out_kmh_start_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.kmh_start_color.b=temp;

	} else if(strcmp(name,("rgb_out_kmh_end_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.kmh_end_color.r=temp;
	} else if(strcmp(name,("rgb_out_kmh_end_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.kmh_end_color.g=temp;
	} else if(strcmp(name,("rgb_out_kmh_end_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.kmh_end_color.b=temp;

	} else if(strcmp(name,("rgb_out_dz_start_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_start_color.r=temp;
	} else if(strcmp(name,("rgb_out_dz_start_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_start_color.g=temp;
	} else if(strcmp(name,("rgb_out_dz_start_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_start_color.b=temp;
	} else if(strcmp(name,("rgb_out_dz_end_r"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_end_color.r=temp;
	} else if(strcmp(name,("rgb_out_dz_end_g"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_end_color.g=temp;
	} else if(strcmp(name,("rgb_out_dz_end_b"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Aktors.dz_end_color.b=temp;
	} else if(strcmp(name,("led_mode"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Aktors.led_mode);
		/////// RGB LEDs /////////
	} else if(strcmp(name,("bt_pin"))==0){
		parse_int16_t(buffer,seperator,&Aktors.bt_pin);
	} else if(strcmp(name,("sensor_source"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Sensors.sensor_source);
	} else if(strcmp(name,("CAN_type"))==0){
		parse_int16_t(buffer,seperator,&temp);
		Sensors.mCAN.set_active_can_type((unsigned char)(temp&0xff));
	} else if(strcmp(name,("GPS_format"))==0){
		parse_bool(buffer,seperator,&Sensors.mGPS.use_compressed_log_format);
	} else if(strcmp(name,("LT_realtime"))==0){
		parse_bool(buffer,seperator,&LapTimer.use_realtime_not_calculated);
	} else if(strcmp(name,("POI_active"))==0){
		bool temp;
		parse_bool(buffer,seperator,&temp);
		SpeedCams.set_active(temp);
	} else if(strcmp(name,("P_highl"))==0){
		parse_uint8_t(buffer,seperator,(uint8_t*)&Aktors.pointer_highlight_mode);
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
	Serial.puts(USART1,*wert);
	Serial.puts_ln(USART1,"<<--");
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
	Serial.puts(USART1,*wert);
	Serial.puts_ln(USART1,"<<--");
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
int configuration::parse_int16_t(char* buffer,int i,int16_t* wert){
	float temp;
	int return_value=parse_float(buffer,i,&temp);
	//	Serial.puts_ln(USART1,temp);
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
	Serial.puts(USART1,*wert);
	Serial.puts_ln(USART1,"<<--");
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
		Serial.puts(USART1,"Prozentualer Unterschied zum letzen mal (muss zwischen 95 und 105 liegen): "); Serial.puts_ln(USART1,round(last_speed_value*100/speed_value));
		Serial.puts(USART1,"Speed value: "); Serial.puts_ln(USART1,speed_value);
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
					Serial.puts(USART1,"====> Max updated: ");
					Serial.puts(USART1,Speedo.max_speed[a]);
					Serial.puts_ln(USART1," <==========");
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
	//	Serial.puts(USART1,"Laut speicher war das Datum zuletzt der ");
	//	Serial.puts(USART1,date_of_today);
	//	Serial.puts(USART1," und heute ist der ");
	//	Serial.puts_ln(USART1,Sensors.mClock.getdate());
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
	};
	return return_value;
}
