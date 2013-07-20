


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


/*  ============ LAPTIMER ============
 1. Basic Steps

 1.1. 	Calling menu 4
		4.1: Race Mode
		4.2: Load Sectors
		4.3: Set new Sectors
		4.4:
		4.5: Evaluate Race

 1.2. 	"Set Sector marks"  means that a new file with virtual Sector Goals will be recorded.
		Therefore the driver start at any place on the track BEHIND the START/GOAL line.
		As soon as he reaches the END of the first section he pushes the "right" button and the
		position will be saved to the SD Card. The END of sector N is in the same time the START
		of sector (N+1).
		The END of the very last sector is the END of the LAP and should therefor be the START/GOAL line.
		Is is set by leaving the "set new sectors" menu by pushing the "left" button

 1.3. 	"Race Mode" shows the Laptimer display (not jet defined)
		On entering the "Race Mode" the file should be checkt if the track was new recoded or has been driven already
		See check_vars();

		After that race_loop() will check if the distance to the next endpoint and calc new values

		Start race mode by calling:
		1. prepare_race_loop()


 2. pMenu->states

 2.1. Single States:
		4: Main Menu

		4x: Race Menu

		411: Menu calls prepare_race_loop()
				[ensures that state is 411]
				IF old_state was 4111 (so we are leaving) update_rate_1Hz() will be executed
				ELSE if old_state was 41 (so we are coming from main menu) update_rate_10Hz() will be executed

				IF we are already moving -> quickstart: menu state=4111 and draw screen
				ELSE we are standing and waiting -> print some nice "waiting on start" to the screen

		411: main loop will call waiting_on_speed_up() in this state,
				IF we are moving: waiting_on_speed_up() sets state=4111

		4111: main loop will call race_loop(); IF we are NOT moving for 2 sec, race_loop() kicks us back to prepare_race_loop();
		421: TODO
		431: state in the middle
				menu->old_state=43 	-> show "sure to clear file"
		4311: state in the middle
				menu->old_state=431 -> clear_file() + update_rate_10Hz() + kick state to 43111 + draw capture screen
				menu->old_state=43111 -> save coordinates as finish line + kick state to 411, start race mode by call prepare_race_loop()
				else -> kick state to 43111 + draw capture screen
		43111: capture mode, update place + gps signal + interspace
		431111: save coordiante as waypoint, show message + kick back to 4311 (will kick us to 43111 after redrawing to interface)

 2.2. State Flow:
		FIRST DAY
		On reaching the racetrack the user opens "main menu" -> "4. Race menu" -> "3. Set new Sectors" [11->X->4X->431]
		The Speedo will ask some sort of "Really clear all Sectors and record new", user will confirm by "right" [431->4311->43111]
		The Speedo shows the "record gps points"-screen. User will start driving on the track, on reaching the first sector end, he will push the "right" button [43111->431111]
		Speedo will save that point (and show a message as feedback) and return to the "record gps points"-screen. [431111->4311->43111]
		After setting all points in between the driver will reach the finish line. After crossing the finish line the first "real" lap begins.
		By pushing the "left" button the speedo will go by "flying start" race mode [43111->4311->411->4111]
		After Race is over, the user will push the "left" button to leave the race mode [4111->411->41]

		SECOND DAY
		On the second day the user will opens "main menu" -> "4. Race menu" -> "2. Load Sector" ... HERE SOME MAGIC HAPPENS AND A TRACK GOT LOADED, [11->X->4X->421-> ??? ->411]
		Speedo will show some fancy "waiting on start" to the screen
		The Driver starts and the Speedo will flip over to the race screen [411->4111]
		After Race is over, the user will push the "left" button to leave the race mode [4111->411->41]

 3. Needed RAM
	uint8_t 	sector_count;
	uint8_t 	current_sector;
	uint8_t		last_dist_to_target;
	uint32_t	sector_end_latitude;
	uint32_t 	sector_end_longitude;
	uint32_t 	best_sector_time_ms;
	uint32_t 	best_theoretical_lap_time_ms;
	uint32_t	sector_start_timestamp_ms;
	uint32_t	lap_start_timestamp_ms;
	uint32_t	starting_standing_timestamp_s;
	int32_t		delay_ms;
	bool	 	delay_calc_active;
	unsigned char filename[20]; // "/NAVI/HOCKENHE.SST"
	#define UPDATE_BEST_LAP 0
	#define UPDATE_DELAY 1
	#define UPDATE_LAP_TIME 2
	#define LAPTIMER_TARGET_RADIUS 15

	==> 86 Byte (based on ram_info)
 */
// this will be callen on and on

#define UPDATE_BEST_LAP 0
#define UPDATE_DELAY 1
#define UPDATE_LAP_TIME 2
#define LAPTIMER_TARGET_RADIUS 15

LapTimer::~LapTimer(){
};

LapTimer::LapTimer(){
	sector_count=0;
	current_sector=0;
	last_dist_to_target=0; // TODO init?
	sector_end_latitude=0;
	sector_end_longitude=0;
	best_sector_time_ms=0;
	best_theoretical_lap_time_ms=0;
	sector_start_timestamp_ms=0;
	lap_start_timestamp_ms=0;
	starting_standing_timestamp_s=0;
	delay_ms=0;
	delay_calc_active=false;
	sprintf((char *)filename,"NAVI/HANNOVER.SST");
}

void LapTimer::race_loop(){
	// check if we have a new gps timestamp
	if(pSpeedo->disp_zeile_bak[0]!=(int)(pSensors->m_gps->get_info(10)&0xffff)){
		// jep new GPS data available
		pSpeedo->disp_zeile_bak[0]=(int)(pSensors->m_gps->get_info(10)&0xffff);
		uint8_t update_level = 0; 	// helps us to update only needed areas

		// calc dist
		uint32_t dist=pSensors->m_gps->calc_dist(sector_end_longitude,sector_end_latitude);
		if(dist<=LAPTIMER_TARGET_RADIUS){
			// we are close to target
			if(dist<last_dist_to_target){ // we are moving nearer to the target ... lets wait a bit more
				update_level|=(1<<UPDATE_LAP_TIME); // just update time
				last_dist_to_target=dist;
			} else { // waaah we are moving away again!
				// start calculations!
				last_dist_to_target=255; // set to high value
				uint32_t cur_sector_time_ms=pSensors->m_gps->get_info(10) - sector_start_timestamp_ms; 	// time we needed in this sector
				sector_start_timestamp_ms=pSensors->m_gps->get_info(10);									// fast save new time!

				// update sector time and theoretical best lap time ??
				if(cur_sector_time_ms<best_sector_time_ms){
					// jep, new record, save to SD!!
					if(update_sector_time(current_sector,cur_sector_time_ms,filename)<0){
						//pOLED->show_storry("File could not be written", 25,"Error",5);
					};

					if(delay_calc_active){
						// calc new theoretical best lap time!!
						best_theoretical_lap_time_ms+=(cur_sector_time_ms-best_sector_time_ms);
						update_level|=(1<<UPDATE_BEST_LAP);
					}
				};

				// calc new delay? ... independend from new best time or not
				if(delay_calc_active){
					if(current_sector==0){ // this was the first sector, reset delay calc
						delay_ms=0;
					};
					delay_ms+=cur_sector_time_ms-best_sector_time_ms; // "negativ" if you were faster
					update_level|=(1<<UPDATE_DELAY);
				};

				// start/goal line or "just" sector end?
				if(current_sector+1>=sector_count){
					// jep Start/goal Passed
					pSensors->m_gps->set_gps_mark(LAP_END_MARK);
					current_sector=0;

					// have we already passed one round?
					if(!delay_calc_active){
						// not yet: okay, we have to calculate the theoretical best lap time
						// delay_calc_active we be activated by the function below as well
						calc_best_theoretical_lap_time();
						update_level|=(1<<UPDATE_BEST_LAP);
					}

					// reset lap time
					lap_start_timestamp_ms=sector_start_timestamp_ms;
					update_level|=(1<<UPDATE_LAP_TIME);

				} else {
					// nope just sector border
					pSensors->m_gps->set_gps_mark(SECTOR_END_MARK);
					current_sector++;
				}

				// load new values
				if(get_sector_data(current_sector,&sector_end_latitude,&sector_end_longitude,&best_sector_time_ms,filename)<0){
					//pOLED->show_storry("File could not be read", 22,"Error",5);
				};
			} // moving TO or AWAY FROM target
		} else { // we are on the way to the next marken, but haven't reacht him
			update_level|=(1<<UPDATE_LAP_TIME);
			// Phils idea ... beeing ready in the box, switching on Race mode, but have to stuff up ... leads to start stop start stop start.... Hands off improvment
			if(pSensors->get_speed(false)==0){
				if(starting_standing_timestamp_s==0){
					starting_standing_timestamp_s=millis();
				} else if((millis()-starting_standing_timestamp_s)>2000){
					prepare_race_loop(); // kick back
				};
			}

		} // we are on the way to the next marken, but haven't reacht him
		update_race_screen(update_level);
	} // new GPS point // else { this is nonsense, we have no new data to draw..
}


void LapTimer::prepare_race_loop(){
	// prepare calculations
	if(calc_best_theoretical_lap_time()>=0){ // if any kind of read error happens, don't go on
		starting_standing_timestamp_s=0;

		if(pSensors->get_speed(false)>0){ // we are moving, quick show screen
			pMenu->state=4111;
			init_race_screen(); //draw border elements
			update_race_screen(0xff); // draw display values
		} else { // we are standing, but we are ready to race
			pMenu->state=411; // main loop will call waiting_on_speed_up() in this state
			pOLED->clear_screen(); // draw some fancy screen while we are waiting

			char buffer[21];
			strcpy_P(buffer,PSTR("READY TO RACE"));
			pMenu->center_me(buffer,20);
			pOLED->string(pSpeedo->default_font,buffer,0,3);
			strcpy_P(buffer,PSTR("speed up"));
			pMenu->center_me(buffer,20);
			pOLED->string(pSpeedo->default_font,buffer,0,4);
			pMenu->set_buttons(true,false,false,false);
		}
	}
};


void LapTimer::waiting_on_speed_up(){
	if(pSensors->get_speed(false)>0){ // we are moving, quick show scren
		pMenu->state=pMenu->state*10+1; // move deeper
		init_race_screen(); //draw border elements
		race_loop(); // draw display values
	}
};


void LapTimer::init_race_screen(){
	pOLED->clear_screen();
	pOLED->filled_rect(70,0,56,16,15);
	pOLED->string_P(pSpeedo->default_font,PSTR("Delay"),13,0,15,0,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("+00:00.00"),12,1,15,0,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Calc.Best"),12,3);
	pOLED->string_P(pSpeedo->default_font,PSTR("00:00.00"),13,4);

	//pOLED->string_P(pSpeedo->default_font,PSTR("Current"),0,5); // lower laptime
	pOLED->string_P(pSpeedo->default_font+1,PSTR("00:00.00"),2,6); // lower laptime
	pOLED->string_P(pSpeedo->default_font+1,PSTR("122"),4,0); // speed
	pOLED->string_P(pSpeedo->default_font,PSTR("KMH"),6,2); // label
	pOLED->string_P(pSpeedo->default_font+2,PSTR("N"),0,0); // gear
	pOLED->draw_oil(0,40);
	char char_buffer[8];
	sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->get_oil_temperature()/10))%1000,pSensors->get_oil_temperature()%10); // _32.3Â°C  7 stellen
	pOLED->string(pSpeedo->default_font,char_buffer,4,5,0,DISP_BRIGHTNESS,-4);
}

/* will be called from main() */
void LapTimer::gps_capture_loop(){
	// check if we have a new gps timestamp
	if(pSpeedo->disp_zeile_bak[0]!=(int)(pSensors->m_gps->get_info(10)&0xffff)){
		// jep new GPS data available
		pSpeedo->disp_zeile_bak[0]=(int)(pSensors->m_gps->get_info(10)&0xffff);
		char char_buffer[21];

		/*
		Sector:_02__________
		____________________
		GPS Signal:_Good_(12)
		052.342232/09.211343
		Interspace:_186m____
		____________________
		\x7F_to_set_sector_end__
		\x7E_to_set_finish_line
		 */

		// sector
		sprintf(char_buffer,"%02i",current_sector+1);
		pOLED->string(pSpeedo->default_font,char_buffer,8,0);

		// gps
		if(pSensors->m_gps->get_info(6)<3){
			pOLED->string_P(pSpeedo->default_font,PSTR("-    "),12,2);
		} else if(pSensors->m_gps->get_info(6)<5){
			pOLED->string_P(pSpeedo->default_font,PSTR("bad  "),12,2);
		} else if(pSensors->m_gps->get_info(6)<7){
			pOLED->string_P(pSpeedo->default_font,PSTR("ok   "),12,2);
		} else {
			pOLED->string_P(pSpeedo->default_font,PSTR("good "),12,2);
		}
		sprintf(char_buffer,"(%02i)",(int)pSensors->m_gps->get_info(6));
		pOLED->string(pSpeedo->default_font,char_buffer,17,2);


		// coordinates
		if(pSensors->m_gps->get_info(6)<3){
			sprintf(char_buffer,"    -     /    -     ");
		} else {
			sprintf(char_buffer,"%09lu / %09lu",pSensors->m_gps->mod(pSensors->m_gps->get_info(2),1000000000),pSensors->m_gps->mod(pSensors->m_gps->get_info(3),1000000000));
		};
		pOLED->string(pSpeedo->default_font,char_buffer,0,3);

		// interspace = distance to last point
		if(current_sector>0){
			sprintf(char_buffer,"%4im",int(pSensors->m_gps->calc_dist(sector_end_longitude,sector_end_latitude))%1000);
			pOLED->string(pSpeedo->default_font,char_buffer,12,4);
		} else {
			pOLED->string_P(pSpeedo->default_font,PSTR("-    "),12,4);
		}

		// arrows
		if(pSensors->m_gps->get_info(6)<3){
			if(pSpeedo->disp_zeile_bak[1]!=1){
				pSpeedo->disp_zeile_bak[1]=1;
				pOLED->filled_rect(0,48,128,16,0);
				pOLED->string_P_centered(PSTR("no GPS"),6,true);
				pOLED->string_P(pSpeedo->default_font,PSTR("\x7E back"),0,7);
				pMenu->set_buttons(true,false,false,false); // only back
			}
		} else {
			if(pSpeedo->disp_zeile_bak[1]!=2){
				pSpeedo->disp_zeile_bak[1]=2;
				pOLED->filled_rect(0,48,128,16,0);
				pOLED->string_P(pSpeedo->default_font,PSTR("\x7F to set sector end"),0,6);
				pOLED->string_P(pSpeedo->default_font,PSTR("\x7E to set finish line"),0,7);
				pMenu->set_buttons(true,false,false,true); // only left and right
			}
		}
	}
};

/* will be called from menu */
void LapTimer::initial_draw_gps_capture_screen(){
	pOLED->clear_screen();

	/*
	Sector:_02__________
	____________________
	GPS Signal:_Good_(12)
	052.342232/09.211343
	Interspace:_186m____
	____________________
	\x7F_to_set_sector_end__
	\x7E_to_set_finish_line
	 */

	// sector
	pOLED->string_P(pSpeedo->default_font,PSTR("Sector:"),0,0);

	// gps
	pOLED->string_P(pSpeedo->default_font,PSTR("GPS Signal:"),0,2);

	// interspace = distance to last point
	pOLED->string_P(pSpeedo->default_font,PSTR("Interspace:"),0,4);

	pSpeedo->reset_bak(); // alle disp_zeile_bak auf -99 setzen
};


/* update display - is used to refresh all the drawn values on the screen.
	race_loop calls update_race_screen as soon as new GPS data are available.
	[in] level - is a bitwise OR connected field of:
		#define UPDATE_BEST_LAP 0
		#define UPDATE_DELAY 1
		#define UPDATE_LAP_TIME 2
		helping us to update only needed fields
 */
void LapTimer::update_race_screen(uint8_t level){
	char char_buffer[20];
	uint16_t min,sec,f_sec;

	if(level&(1<<UPDATE_DELAY)){
		min=floor(delay_ms/60000);
		sec=int(floor(delay_ms/1000))%60;
		f_sec=floor((delay_ms%1000)/10);
		sprintf(char_buffer,"%+02i:%02i.%02i",min,sec,f_sec);
		pOLED->string(pSpeedo->default_font+1,char_buffer,2,6);
	}

	if(level&(1<<UPDATE_LAP_TIME)){
		min=floor(best_theoretical_lap_time_ms/60000);
		sec=int(floor(best_theoretical_lap_time_ms/1000))%60;
		f_sec=floor((best_theoretical_lap_time_ms%1000)/10);
		sprintf(char_buffer,"%02i:%02i.%02i",min,sec,f_sec);
		pOLED->string(pSpeedo->default_font+1,char_buffer,2,6);
	}

	if(level&(1<<UPDATE_BEST_LAP)){
		min=floor((pSensors->m_gps->get_info(10)-lap_start_timestamp_ms)/60000);
		sec=int(floor((pSensors->m_gps->get_info(10)-lap_start_timestamp_ms)/1000))%60;
		f_sec=floor(((pSensors->m_gps->get_info(10)-lap_start_timestamp_ms)%1000)/10);
		sprintf(char_buffer,"%02i:%02i.%02i",min,sec,f_sec);
		pOLED->string(pSpeedo->default_font,char_buffer,13,4);
	}
}


/* calc_best_theoretical_lap_time() - is used to parse the SD Card and find all best sector times,
	add them up and save it in best_theoretical_lap_time_ms
	This should be the starting call!
 */
int LapTimer::calc_best_theoretical_lap_time(){
	unsigned char temp[20];
	SdFile folder;
	SdFile file;

	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_CREAT| O_WRITE)<0){
		pOLED->show_storry(PSTR("File could not be read"),PSTR("Error"),DIALOG_GO_LEFT_1000MS);
		return -1;
	};

	delay_calc_active=true; // assume that this track has been driven
	best_theoretical_lap_time_ms=0;
	sector_count=(int)floor(file.fileSize()/30);
	if(sector_count==0){
		pOLED->show_storry(PSTR("The current selected file is empty, record some sectors first"),PSTR("Empty file"),DIALOG_GO_LEFT_2000MS);
		return -1;
	} else {
		for(uint8_t i=0;i<sector_count; i++){
			uint32_t temp;
			if(get_sector_data(i,&temp,&temp,&temp,filename)<0){
				pOLED->show_storry(PSTR("Get Sectors from file failed"),PSTR("Error"),DIALOG_GO_LEFT_1000MS); // <-- pretty cool
				return -1;
			}
			if(temp==99999999){ // initial time for sector
				delay_calc_active=false;
				best_theoretical_lap_time_ms=0; // invalid
				return -1;
			} else {
				best_theoretical_lap_time_ms+=temp;
			}
		};
		current_sector=0; // is that wise?
	}
	return 0;
};


/* update_sector time is used to write new, better times to the log file
	[in] sector_id = id of the sector
	[in] setor_time = new time, that should be written, max(uint32_t) = 4294967295 ==> 4294967.295 up to 50 Days :D
	[in] 8.3 filename (12 chars!)

	[out] 0 = ok, -1 = seek error, -2 write error, any other <0 => get file handle error

	File (SpeedoSectorTimes) "sectors.sst" syntax: latitude,longitude,time
	%09lu,%09lu,%08lu\r\n ==> 051123423,009235342,00324577\r\n  (length per line = 30 byte)
	means the sector ends at @51.123423°N, 9.235342°E and the best time for this sector was 324.577sec (wow 5 min) */
int LapTimer::update_sector_time(uint8_t sector_id, uint32_t sector_time, unsigned char *filename){
	unsigned char temp[12];
	SdFile folder;
	SdFile file;

	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_CREAT| O_WRITE)<0){
		return -3;
	};
	if(!file.seekSet(sector_id*30+20)){
		file.close();
		folder.close();
		return -1;	// file seek failed
	};

	if(sector_time>99999999){ // avoid length blasting (27h)
		sector_time=99999999;
	}

	sprintf((char *)temp,"%08lu",sector_time);
	file.writeError=false;
	file.write((char *)temp);
	if(file.writeError){
		file.close();
		folder.close();
		return -2; // write to file failed
	}
	file.close();
	folder.close();
	return 0;
};

/* simply returns the pointer to the filename */
unsigned char* LapTimer::get_active_filename(){ // overload for the menu.cpp
	return filename;
};

/* clear file will open a file in truncate mode and close it*/
int LapTimer::clear_file(unsigned char* filename){
	SdFile folder;
	SdFile file;
	unsigned char temp[20];
	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_TRUNC|O_CREAT)<0){
		return -1;
	};

	file.close();
	folder.close();
	current_sector=0;
	return 0;
};

/* add_sector is used to append a NEW sector to a SST file
	[in] latitude
	[in] longitude = Coordiantes of the END of the sector!
	[in] 8.3 filename (12 chars!)

	returns 0=ok, -2 write error, -3 open error
 */
int LapTimer::add_sector(uint32_t latitude, uint32_t longitude, unsigned char* filename){
	if(latitude>90000000){ // max +/-90.000000° ^
		latitude=90000000;
	};
	if(longitude>180000000){ // max +/-180.000000° <->
		longitude=180000000;
	};

	unsigned char temp[31]; // reused to write time
	SdFile folder;
	SdFile file;
	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_CREAT| O_WRITE | O_APPEND)<0){
		file.close();
		folder.close();
		return -3;
	};
	sprintf((char *)temp,"%09lu,%09lu,%08lu\r\n",latitude,longitude,99999999); // init with max time (27h)
	file.writeError=false;
	file.write((char *)temp);
	if(file.writeError){
		file.close();
		folder.close();
		return -2; // write to file failed
	}
	file.close();
	folder.close();

	sector_end_longitude=longitude;
	sector_end_latitude=latitude;
	current_sector++; // we captured one ..goto next
	return 0;
};


/* get_sector_data is used to get the data of a sector from a SST file
	[in] sector id
	[in] pointer to latitude
	[in] pointer to longitude = Coordiantes of the END of the sector!
	[in] pointer to best time of this sector
	[in] 8.3 filename (12 chars!)

	returns 0=ok,-1 seek error, -2 read error, -3 open error
 */
int LapTimer::get_sector_data(uint8_t sector_id, uint32_t* latitude,uint32_t* longitude,uint32_t* sector_time, unsigned char* filename){
	unsigned char temp[31];
	SdFile folder;
	SdFile file;

	// open file
	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_READ)<0){
		file.close();
		folder.close();
		return -3;
	};

	// move cursor to sector line
	if(!file.seekSet(sector_id*30)){
		file.close();
		folder.close();
		return -1;	// file seek failed
	};

	// read line
	int n=file.read(temp, 30);
	if(n<30){
		file.close();
		folder.close();
		return -2;
	}

	// get latitude
	uint32_t temp_value=0;
	for(int i=0;i<10;i++){ // 10-0 = 10 Chars
		if(temp[i]>='0' && temp[i]<='9'){
			temp_value=temp_value*10+(temp[i]-'0');
		}
	}
	*latitude=temp_value;

	// get longitude
	temp_value=0;
	for(int i=11;i<21;i++){ // 21-11 = 10 Chars
		if(temp[i]>='0' && temp[i]<='9'){
			temp_value=temp_value*10+(temp[i]-'0');
		}
	}
	*longitude=temp_value;

	// get sector_time
	temp_value=0;
	for(int i=22;i<30;i++){ // 30-22 = 8 Chars
		if(temp[i]>='0' && temp[i]<='9'){
			temp_value=temp_value*10+(temp[i]-'0');
		}
	}
	*sector_time=temp_value;

	file.close();
	folder.close();
	return 0;
}
