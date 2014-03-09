/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2013 Kolja Windeler
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
		4.4: Clear Sectors
		4.5: Evaluate Race
		4.6: Race Settings

 1.2. 	"Set Sector marks"  means that a new file with virtual Sector Goals will be recorded.
		Therefore the driver start at any place on the track BEHIND the START/GOAL line.
		As soon as he reaches the END of the first section he pushes the "right" button and the
		position will be saved to the SD Card. The END of sector N is in the same time the START
		of sector (N+1).
		The END of the very last sector is the END of the LAP and should therefor be the START/GOAL line.
		Is is set by leaving the "set new sectors" menu by pushing the "left" button

 1.3. 	"Race Mode" shows the Laptimer display
		On entering the "Race Mode" the file should be checked if the track was new recoded or has been driven already
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
	uint8_t		lap;
	uint32_t	sector_end_latitude;
	uint32_t 	sector_end_longitude;
	uint32_t 	best_sector_time_ms;
	uint32_t 	best_lap_time_ms;
	uint32_t	sector_start_timestamp_ms;
	uint32_t	lap_start_timestamp_ms;
	uint32_t	starting_standing_timestamp_ms;
	int32_t		delay_ms;
	bool		used_realtime_not_calculated;
	bool	 	delay_calc_active;
	bool 		last_gps_valid;
	unsigned char filename[20]; // "/NAVI/HOCKENHE.SST"

	==> 87 Byte (based on ram_info)
 */
// this will be callen on and on

#define UPDATE_BEST_LAP 0
#define UPDATE_DELAY 1
#define UPDATE_LAP_TIME 2
#define UPDATE_SECTOR 3
#define LAPTIMER_TARGET_RADIUS 25
#define LAPTIMER_SHOW_LAPTIME_DELAY 5000
#define LT_BLINK_FREQ 500

#define LAPTIMER_TEMP_FILE "NAVI/TEMP.SST"

#undef LAPTIMER_DEBUG_OUTPUT  //uses: 1252=2418-1166

LapTimer::~LapTimer(){
};

LapTimer::LapTimer(){
	lap=0;											// current lap
	sector_count=0;									// Number of Sectors ( 7 for 7 setors )
	current_sector=0;								// Sector we are currently in (0..sector_count-1)
	last_dist_to_target=LAPTIMER_TARGET_RADIUS+1;	// temporary storage to save the last distance
	sector_end_latitude=0;							// coordinates of the end of this sector
	sector_end_longitude=0;
	best_sector_time_ms=0;							// best time for the current sector, stored on the sd card
	best_lap_time_ms=0;								// sum of all sector best times
	sector_start_timestamp_ms=0;					// timestamp from GPS, saved when entering the sector
	total_lap_time_blink_ms=0;						// time of the lap is stored here to "blink"
	lap_start_timestamp_ms=0;						// timestamp from GPS, saved when entering the lap
	starting_standing_timestamp_ms=0;				// millis() timestamp from first measured speed=0
	delay_ms=0;										// race delay
	delay_calc_active=false;						// if delay should be calced or not
	last_gps_valid=true;							// validity of the last gps state
	use_realtime_not_calculated=true;				// used real besttime and not the calculated besttime <- sum of sector besttimes
	delay_reseted=true;
	strcpy_P((char *)filename,PSTR("NAVI/HANNOVER.SST")); // static file for the moment
}

/* prepare_race_loop() is the entry point to the laptimer <- by menu 411 OR after recording GPS points
 * first we check the selected file, read sector count, lap records and so on
 *
 * if an error occures while reading the file, calc_best_lap_time() kicks us back
 * to the menu on its own.
 *
 * if no error occures, the current speed and the gps connection is checked:
 * GPS OK + Speed > 0 mean that a) the driver started this menu late or b) we are coming from the "record gps points" menu
 * -> anyway, quick reset the race vars (by the true argument in init_race_screen) and show the screen
 *
 * if Speed = 0 or no GPS, so show some nice screen to the user
 */
void LapTimer::prepare_race_loop(){
	// prepare calculations
	if(calc_best_lap_time()>=0){ // if any kind of read error happens, don't go on
		/* calc_best_lap_time will:
		 * set delay_calc_active
		 * set best_lap_time_ms (theoretical or real, depending what is in the file)
		 * set sector_count
		 *
		 * get_sector_data() will:
		 * set sector end coordinates
		 */
		if(use_realtime_not_calculated){												// prepare the temp file, by clearing it and copying as many sectors to it as in the race file
			unsigned char temp_file[20];
			strcpy_P((char *)temp_file,PSTR(LAPTIMER_TEMP_FILE)); 						// static temp file

			clear_file(temp_file);														// will reset the temp file
			for(int i=0; i<sector_count; i++){											// loop through sectors and add sectors with useless data to the temp file, just to have the right amount of data in that file
				if(add_sector(0,0,temp_file)<0){										// add coordiantes 0,0
					pOLED->show_storry(PSTR("Sector read from file failed"),PSTR("Error"),DIALOG_SHOW_500MS);
				}
			}
		}

		current_sector=0;
		lap=0;
		if(get_sector_data(current_sector,&sector_end_latitude,&sector_end_longitude,&best_sector_time_ms,filename)<0){
			pOLED->show_storry(PSTR("Sector read from file failed"),PSTR("Error"),DIALOG_SHOW_500MS);
		}
		starting_standing_timestamp_ms=0;
		if(pSensors->m_gps->get_info(9)>3){ 	// we DONT have GPS signal
#ifdef LAPTIMER_DEBUG_OUTPUT
			Serial.println("Setze last_gps_valid=false, da GPS nicht valid ist.");
#endif
			last_gps_valid=false; 			// this will help us, jump to the "if(!last_gps_valid){" part of "waiting_on_speed_up()"
			pMenu->state=M_LAP_T*100+11; 	// main loop will call waiting_on_speed_up() in this state
			pOLED->clear_screen(); 			// draw some fancy screen while we are waiting
			pOLED->string_P_centered(PSTR("No GPS"),3);
			pOLED->string_P_centered(PSTR("please wait"),4);
		} else {								// we have GPS signal
			if(pSensors->get_speed(false)<=0) { // we are standing, but we are ready to race
				pMenu->state=M_LAP_T*100+11; 	// main loop will call waiting_on_speed_up() in this state
#ifdef LAPTIMER_DEBUG_OUTPUT
				Serial.println("Setze last_gps_valid=false, da geschwindigkeit<=0 ist.");
#endif
				last_gps_valid=false; 		// shortcut: jump to the "if(!last_gps_valid){" part of "waiting_on_speed_up()" eventhough we have gps, show "speed up" on screen
			} else { 						// we are moving, quick show screen and skip "waiting_on_speed_up()"
				last_gps_valid=true; 		// this will help us, jump to the "} else if(pSensors->get_speed(false)>0){" part of "waiting_on_speed_up()"
				pMenu->state=M_LAP_T*1000+111;
				init_race_screen(true); 	//draw border elements, reset vars
				update_race_screen(0xff); 	// draw display values
			}
		}
	} // else not needed, calc_best_theoretical_lap_time will show a error message and do the menu_state stuff
};

/* waiting_on_speed_up() is used to controll the state-in-the-middle 411 <- callen by the main loop
 * as soon as all preconditions are fullfilled we will be shifted to state 4111
 * and call init_race_screen(true); + race_loop();
 */
void LapTimer::waiting_on_speed_up(){
	if(!last_gps_valid){ // we are in this state because by entering the prepare loop we had no gps signal
		if(pSensors->m_gps->get_info(9)<3){ 	// gps signal?
			last_gps_valid=true;				// yep
			pOLED->clear_screen(); 				// draw some fancy screen while we are waiting
			pOLED->string_P_centered(PSTR("READY TO RACE"),3);
			pOLED->string_P_centered(PSTR("speed up"),4);
			pMenu->set_buttons(true,false,false,false);
		}
	} else if(pSensors->get_speed(false)>0){ // we are in this state because we were standing
		pMenu->state=pMenu->state*10+1; // move deeper
		init_race_screen(true); //draw border elements, reset times
		update_race_screen(0xff); // draw display values
	}
};


/* init_race_screen(bool reset_vars) will draw the race screen, all those fixed strings on the screen
 * bool reset_vars: if set, the time measuring vars will be setted to the actual timestamp
 * basicly this is a "on-time-call" function, race_loop() calls update_screen() to refresh needed values
 */
void LapTimer::init_race_screen(bool reset_vars){
	pOLED->clear_screen();

	pOLED->filled_rect(70,0,56,16,15);		// yellow rectangle for delay
	pOLED->string_P(pSpeedo->default_font,PSTR("Delay"),13,0,15,0,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("+--:--.--"),12,1,15,0,0);

	if(use_realtime_not_calculated){
		pOLED->string_P(pSpeedo->default_font,PSTR("Best Lap"),12,3);
	} else {
		pOLED->string_P(pSpeedo->default_font,PSTR("Calc.Best"),12,3);
	}
	pOLED->string_P(pSpeedo->default_font,PSTR("--:--.--"),13,4);

	pOLED->string_P(pSpeedo->default_font,PSTR("Sec 01"),0,4);
	pOLED->string_P(pSpeedo->default_font+1,PSTR("00:00.00"),2,6); // lower laptime
	pOLED->string_P(pSpeedo->default_font+1,PSTR("-"),5,0); // speed
	pOLED->string_P(pSpeedo->default_font,PSTR("KMH"),6,2); // label
	pOLED->string_P(pSpeedo->default_font+2,PSTR("N"),0,0); // gear
	pOLED->draw_oil(0,40);
	char char_buffer[8];
	sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->get_oil_temperature()/10))%1000,pSensors->get_oil_temperature()%10); // _32.3°C  7 stellen
	pOLED->string(pSpeedo->default_font,char_buffer,4,5,0,DISP_BRIGHTNESS,-4);
	pSpeedo->reset_bak();

	if(reset_vars){ // reset times
		sector_start_timestamp_ms=pSensors->m_gps->get_info(11);
		lap_start_timestamp_ms=pSensors->m_gps->get_info(11);
		delay_ms=0;
	}
}

/* race_loop() is THE main race loop, gets called by main() in state 4111
 * here all handling of distances, times etc will be done
 * First we have to check if we reached a MARKER,
 * if not just update the race time
 * but if we reached a marker we have to separate: Using theoretical mode , or real time mode
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * x               x             Real time mode                x                Theoretical time mode     x
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * x               x (1)save the time, consumed in this sector x (2)compare the time, consumped in this   x
 * x  reach end    x to the temp file /NAVI/TEMP.SST           x sector, with the sector time in the race x
 * x  of sector    x                                           x file, if smaller: update race file and   x
 * x               x                                           x recalculate best lap time                x
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * x               x (3)compare laptime with bestlap (stored   x                                          x
 * x               x in the race file) if we were faster all   x                                          x
 * x  reach end    x times from the temp file have to be       x                                          x
 * x  of lap       x copied to the race file and best_lap_time x                                          x
 * x               x has to be updated                         x                                          x
 * x               x                                                                                      x
 * x               x (4)in both cases: if delay calc was not active: it has to be activate now, therefore x
 * x               x    the best laptime has to be calculated                                             x
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *
 */
void LapTimer::race_loop(){
	if(pSpeedo->disp_zeile_bak[0]!=(int)pSensors->m_gps->mod(pSensors->m_gps->get_info(11),10000)){		// check if we have new gps data by comparing the current gps timestamp to the stored one
		pSpeedo->disp_zeile_bak[0]=(int)pSensors->m_gps->mod(pSensors->m_gps->get_info(11),10000); 		// jep new GPS data available, save timestamp to storage
		uint8_t update_level=(1<<UPDATE_LAP_TIME); 														// helps us to update only needed areas just update lap time by now
		if((pSensors->m_gps->get_info(11) - lap_start_timestamp_ms) < LAPTIMER_SHOW_LAPTIME_DELAY && lap>0){	// to show this blinking delay we have to submit the update request
			update_level|=(1<<UPDATE_DELAY);
		} else if(!delay_reseted && lap>0 && current_sector==0){										// once if LAPTIMER_SHOW_LAPTIME_DELAY is over in the first sector of any loop != 1st
			delay_reseted=true;																			// avoid entering the loop twice
			delay_ms=0;																					// reset delay
			update_level|=(1<<UPDATE_DELAY);															// and draw that
		}
		uint32_t dist=pSensors->m_gps->calc_dist(sector_end_longitude,sector_end_latitude);				// calc dist between actual location and the end of this sector

		//		///////////////////////////////////////////////////////////////////
		//		menu line 2070
		//		sensors 189
		//		dist=0;
		//		while(Serial.available()){
		//			Serial.read();
		//			dist++;
		//		}
		//
		//		///////////////////////////////////////////////////////////////////

		if((dist<=LAPTIMER_TARGET_RADIUS || last_dist_to_target<LAPTIMER_TARGET_RADIUS)){ 				// eighter the actual distance is short or the distance has been short before
			if(dist<=last_dist_to_target){ 																// ********** we are moving NEARER to the target ... lets wait a bit more **********
				last_dist_to_target=dist;																// remember this distance to compare it to the distance of the next gps sample
			} else { 																					// ********** waaah we are moving AWAY again! start calculations! **********
				last_dist_to_target=255; 																// we are calculating now, reset var to avoid entering this "if" on and on
				uint32_t cur_sector_time_ms=pSensors->m_gps->get_info(11) - sector_start_timestamp_ms; 	// time we needed in this sector
				sector_start_timestamp_ms=pSensors->m_gps->get_info(11);								// quick, save "now" as start of the new sector
				update_level|=(1<<UPDATE_SECTOR);														// as soone as we finish a sector update_screen should draw the current sector

#ifdef LAPTIMER_DEBUG_OUTPUT
				Serial.print("We have reached the end of sector nr ");
				Serial.println(current_sector+1); // +1 for humans :D
				Serial.println("We consumed for this sector ");
				Serial.print(cur_sector_time_ms);
				Serial.print("ms, best was ");
				Serial.println(best_sector_time_ms);
#endif
				////////////////////////////////////////////////////////////////////////////////// UPDATE SECTOR TIMES //////////////////////////////////////////////////////////////////////////////////
				if(use_realtime_not_calculated){														// See (1): IF we would like to see the real time, we have to save EVERY sector to our temp file
					unsigned char temp_file[20];														// and copy it to the race file IF the time of the complete lap is a new record time, not earlier
					strcpy_P((char *)temp_file,PSTR(LAPTIMER_TEMP_FILE)); 								// static temp file
					int error=update_sector_time(current_sector,cur_sector_time_ms,temp_file);			// save it to the temp file, every time we cross a sector border
					if(error<0){
						pOLED->show_storry(PSTR("File could not be written"),PSTR("Error"),DIALOG_SHOW_500MS);	// this might drop some gps marks, but its a real problem
					};
				} else {																				// See (2): but if we are in "theortical mode" we just have to update it, if it is better
					if(cur_sector_time_ms<best_sector_time_ms){											// ********** update sector time and theoretical best lap time ?? **********
#ifdef LAPTIMER_DEBUG_OUTPUT
						Serial.println("Thus we are updating the file.");
#endif
						int error=update_sector_time(current_sector,cur_sector_time_ms,filename);		// obviously we have a new record for this sector, write it to the SD file
						if(error<0){
							pOLED->show_storry(PSTR("File could not be written"),PSTR("Error"),DIALOG_SHOW_500MS);	// this might drop some gps marks, but its a real problem
						};

						if(delay_calc_active){															// as soon as we have times for all sectors, we can calculate a theoretical best lap, have we filled all slots?
							best_lap_time_ms+=(cur_sector_time_ms-best_sector_time_ms);					// yes, so calc NEW theoretical best lap time!! (new sector record -> new best lap)
							update_level|=(1<<UPDATE_BEST_LAP);											// tell update_screen to draw that
#ifdef LAPTIMER_DEBUG_OUTPUT
							Serial.print("Based on the new sector time, we have a theoretical best-lap-time of ");
							Serial.print(best_lap_time_ms);
							Serial.println("ms");
#endif
						};
					} // cur_sector_time_ms<best_sector_time_ms
				} // if NOT (use_realtime_not_calculated)
				////////////////////////////////////////////////////////////////////////////////// UPDATE SECTOR TIMES //////////////////////////////////////////////////////////////////////////////////

				////////////////////////////////////////////////////////////////////////////////// FINISH LINE HANDLING //////////////////////////////////////////////////////////////////////////////////
				if(current_sector+1>=sector_count){														// ********** start/goal line or "just" sector end? **********
#ifdef LAPTIMER_DEBUG_OUTPUT
					Serial.println("Attention, we reached the Finish line.");
#endif
					pSensors->m_gps->set_gps_mark(LAP_END_MARK);										// jep Start/goal Passed
					current_sector=0;																	// reset sector ID
					lap++;																				// count up to 255 ... should be enough
					if(use_realtime_not_calculated){													// IF we are using real time, we have to compare if this lap is a new record, otherwise it will be done in the end of each sector
#ifdef LAPTIMER_DEBUG_OUTPUT
						Serial.print("Our old LapRecord was:");
						Serial.print(best_lap_time_ms);
						Serial.print(" this laptime is ");
						Serial.println(sector_start_timestamp_ms-lap_start_timestamp_ms);
#endif
						if((sector_start_timestamp_ms-lap_start_timestamp_ms)<best_lap_time_ms || best_lap_time_ms==0){		// See (3): check if THIS lap was better than best_lap. sector_start_timestamp_ms holds accurate timestamp of "now"
#ifdef LAPTIMER_DEBUG_OUTPUT
							Serial.print("Starting to copy sector times from tempfile to racefile at:");
							Serial.println(millis());
#endif
							unsigned char temp_file[20];                                                // now copy all sector times from the temp file in the race file
							strcpy_P((char *)temp_file,PSTR(LAPTIMER_TEMP_FILE));                       // static temp file
							uint32_t sector_time_this_lap;
							for(int i=0; i<sector_count; i++){                                          // loop through sectors
#ifdef LAPTIMER_DEBUG_OUTPUT																			// DEBUG
								Serial.print("Loading Sector ");										// DEBUG
								Serial.print(i);														// DEBUG
								Serial.print("/");														// DEBUG
								Serial.print(sector_count-1);											// DEBUG
#endif																									// DEBUG
								if(get_sector_data(i,&sector_time_this_lap,&sector_time_this_lap,&sector_time_this_lap,temp_file)<0){    // read time from temp
#ifdef LAPTIMER_DEBUG_OUTPUT																			// DEBUG
									Serial.println(" FAILURE while read");								// DEBUG
#endif																									// DEBUG
									pOLED->show_storry(PSTR("Get Sectors from file failed"),PSTR("Error"),DIALOG_GO_LEFT_1000MS); // <-- pretty cool
								} else {
#ifdef LAPTIMER_DEBUG_OUTPUT																			// DEBUG
									Serial.print("Loaded time:");										// DEBUG
									Serial.print(sector_time_this_lap);									// DEBUG
									Serial.println(", writing now.");										// DEBUG
#endif																									// DEBUG
									int error=update_sector_time(i,sector_time_this_lap,filename);      // save it to the race file
									if(error<0){
#ifdef LAPTIMER_DEBUG_OUTPUT																			// DEBUG
										Serial.println("FAILURE while write");							// DEBUG
#endif																									// DEBUG
										pOLED->show_storry(PSTR("Race File could not be written"),PSTR("Error"),DIALOG_GO_LEFT_1000MS);
									};
								}
							}
#ifdef LAPTIMER_DEBUG_OUTPUT
							Serial.print("Finished at:");
							Serial.println(millis());

#endif
							best_lap_time_ms=(sector_start_timestamp_ms-lap_start_timestamp_ms);		// save new best time and
							update_level|=(1<<UPDATE_BEST_LAP);											// show that on the screen
							update_race_screen(update_level);											// call it once in between to update current race time, otherwise its 200ms behind
						} // new best lap
					} // if(use_realtime_not_calculated){
					// Activate delay calculation
					if(!delay_calc_active){																// have we already passed one round? delay_calc_active=false if not
#ifdef LAPTIMER_DEBUG_OUTPUT
						Serial.println("Delay calculation was not active until now.");
#endif
						calc_best_lap_time();															// not yet: okay, we have to calculate the theoretical best lap time, delay_calc_active is set to "true" by calc_best_lap_time() as well
						update_level|=(1<<UPDATE_BEST_LAP);												// show that on the screen
					}
					total_lap_time_blink_ms=(sector_start_timestamp_ms-lap_start_timestamp_ms);			// This is our time for the last lap!!
					lap_start_timestamp_ms=sector_start_timestamp_ms; 									// reset lap time. sector_start_timestamp_ms holds accurate timestamp of "now"
					if(lap>0){																			// avoid updating the displayed laptime
						update_level&=~(1<<UPDATE_LAP_TIME);											// right after crossing the finish line.
					}																					// after a time of 5 sec it will be updated <- Phils idea to realize the laptime
				} else {
					pSensors->m_gps->set_gps_mark(SECTOR_END_MARK);										// ********** nope just sector border **********
					current_sector++;																	// just entering the next sector of this lap
				}
				////////////////////////////////////////////////////////////////////////////////// FINISH LINE HANDLING //////////////////////////////////////////////////////////////////////////////////

				////////////////////////////////////////////////////////////////////////////////// DELAY CALCULATION //////////////////////////////////////////////////////////////////////////////////
				if(delay_calc_active){																	// ********** we finished a sector, lets check how good we are, calc delay **********
#ifdef LAPTIMER_DEBUG_OUTPUT
					Serial.print("Before reaching the sector end, our delay was ");
					Serial.print(delay_ms);
					Serial.println("ms");
#endif
					if(current_sector==0){																// reset delay when crossing the finish line
						delay_reseted=false;															// to have this 0:00.000 feeling
					} else {
						if(current_sector==1){ 															// we finished the first sector of this lap, reset delay calc
							delay_ms=0;																	// so we have allways the delay of the current lap displayed
							// resetting delay_ms is not soo important, is done in the top if structure
#ifdef LAPTIMER_DEBUG_OUTPUT
							Serial.println("This was the first sector, resetting delay.");
#endif
						};
						if(!delay_reseted){																// if the first sector is very very fast
							delay_ms=0;																	// its possible that we are still
							delay_reseted=true;															// blinking, reset delay faster!
						}
						delay_ms+=cur_sector_time_ms-best_sector_time_ms; 								// if you were faster, cur_sector_time_ms is smaller than best_sector_time_ms and your delay becomes "negative"
					};
#ifdef LAPTIMER_DEBUG_OUTPUT
					Serial.print("It is now ");
					Serial.print(delay_ms);
					Serial.println("ms");
#endif
					update_level|=(1<<UPDATE_DELAY);													// tell update_screen the write the new delay
				};
				////////////////////////////////////////////////////////////////////////////////// DELAY CALCULATION //////////////////////////////////////////////////////////////////////////////////

				////////////////////////////////////////////////////////////////////////////////// LOAD NEXT SECTOR //////////////////////////////////////////////////////////////////////////////////
				if(get_sector_data(current_sector,&sector_end_latitude,&sector_end_longitude,&best_sector_time_ms,filename)<0){
					pOLED->show_storry(PSTR("Sector read from file failed"),PSTR("Error"));
				};
				////////////////////////////////////////////////////////////////////////////////// LOAD NEXT SECTOR //////////////////////////////////////////////////////////////////////////////////

#ifdef LAPTIMER_DEBUG_OUTPUT
				Serial.print("New sector end coordinates are ");
				Serial.print(sector_end_latitude);
				Serial.print(" / ");
				Serial.print(sector_end_longitude);
				Serial.print(" current record: ");
				Serial.print(best_sector_time_ms);
				Serial.println("ms");
#endif
			} 																							// Finished all that calculation of records and so on
		} else { 																						// ********** we are on the way to the next marken, but haven't reacht him **********
			// Phils idea ... beeing ready in the box, switching on Race mode, but have to move up ... leads to start stop start stop start....
			if(pSensors->get_speed(false)==0 && current_sector==0){										// Hands off improvment: Auto reset the Laptimer if we stop for 2 sec in sector 0 (e.G. Box or start setup)
				if(starting_standing_timestamp_ms==0){ 													// timestamp_s==0 means counter not yet started
					starting_standing_timestamp_ms=millis();												// start it now
				} else if((millis()-starting_standing_timestamp_ms)>2000){ 								// counter was running for more than 2sec
					prepare_race_loop(); 								  								// kick us back to the welcome screen
				};																						// evaluation of prepare_race_loop() is not needed, calc_best_theoretical_lap_time will show a error message and do the menu_state stuff on its own
				// GPS Handling: last_gps_valid=true -> the last time we checked it, we had gps connection
			} else if(pSensors->m_gps->get_info(9)>3){ 													// 3 sec no valid GPS signal .. baaad
				if(last_gps_valid){																		// but last time GPS was OK, so obvious we just lost it
#ifdef LAPTIMER_DEBUG_OUTPUT
					Serial.println("Setze last_gps_valid=false, da gps nicht valid ist.");
#endif
					last_gps_valid=false;																// remember that
					pOLED->filled_rect(0,24,128,24,0x00);												// show some nice message
					pOLED->string_P_centered(PSTR("no GPS"),4,true);
				}
			} else if(!last_gps_valid){ 																// we have now a valid gps signal but we were offline before
#ifdef LAPTIMER_DEBUG_OUTPUT
				Serial.println("Init mit false, in der loop");
#endif
				init_race_screen(false); 																// redraw the race screen because we painted that fancy "no gps" message but dont reset vars
				last_gps_valid=true;																	// remember that we are reconnected
				update_level=0xff; 																		// update all on update_race_screen()
			} else {
				starting_standing_timestamp_ms=0; 														// reset standing counter, because we are moving // faster than if(starting_standing_timestamp_ms>0) { starting_standing_timestamp_ms=0; } ?!
			};

		} 																								// we are on the way to the next marken, but haven't reacht him
		update_race_screen(update_level);
	} 																									// new GPS point // else { this is nonsense, we have no new data to draw..
}

/* update display - is used to refresh all the drawn values on the screen.
 *	race_loop calls update_race_screen as soon as new GPS data are available.
 *	[in] level - is a bitwise OR connected field of:
 *		#define UPDATE_BEST_LAP 0
 *		#define UPDATE_DELAY 1
 *		#define UPDATE_LAP_TIME 2
 *		#define UPDATE_SECTOR 3
 *		helping us to update only needed fields
 */
void LapTimer::update_race_screen(uint8_t level){
	char char_buffer[20];
	uint16_t min,sec,f_sec;

	if(level&(1<<UPDATE_DELAY)){
		bool skip_displaying=false;
		if(lap>0 && current_sector==0 && (pSensors->m_gps->get_info(11) - lap_start_timestamp_ms) < LAPTIMER_SHOW_LAPTIME_DELAY){
			unsigned long blink_timer=pSensors->m_gps->mod((unsigned long)(pSensors->m_gps->get_info(11) - lap_start_timestamp_ms),2*LT_BLINK_FREQ);
			if(blink_timer<LT_BLINK_FREQ && pSpeedo->disp_zeile_bak[10]!=99){
				pSpeedo->disp_zeile_bak[10]=99;
				sprintf(char_buffer,"         ");
				pOLED->string(pSpeedo->default_font,char_buffer,12,1,15,0,0);
				skip_displaying=true;
			} else if(blink_timer>=LT_BLINK_FREQ && pSpeedo->disp_zeile_bak[10]!=(delay_ms&0xFF)){
				pSpeedo->disp_zeile_bak[10]=delay_ms&0xFF;
			} else {
				skip_displaying=true;
			}
		}

		if(!skip_displaying && delay_ms<5940000){ // 1*99*60*1000=5940000
			unsigned char sign='+';
			int32_t internal_delay=delay_ms;
			if(internal_delay<0){
				internal_delay*=-1;
				sign='-';
			}
			min=int(floor(internal_delay/60000));
			sec=int(floor(internal_delay/1000))%60;
			f_sec=int(floor((internal_delay%1000)/10));
			sprintf(char_buffer,"%c%02i:%02i.%02i",sign,min,sec,f_sec);
			pOLED->string(pSpeedo->default_font,char_buffer,12,1,15,0,0);
		}
	}

	if(level&(1<<UPDATE_SECTOR)){
		if(current_sector<99){
			if(sector_count>9){
				sprintf(char_buffer,"%2i/%2i",(current_sector+1)%100,sector_count%100);
			} else {
				sprintf(char_buffer,"%i/%i",(current_sector+1)%100,sector_count%100);
			}
			pOLED->string(pSpeedo->default_font,char_buffer,4,4);
		}
	}

	if(level&(1<<UPDATE_LAP_TIME)){
		bool skip_displaying=false;
		unsigned long differ;

		if(lap>0 && current_sector==0 && (pSensors->m_gps->get_info(11) - lap_start_timestamp_ms) < LAPTIMER_SHOW_LAPTIME_DELAY){
			unsigned long blink_timer=pSensors->m_gps->mod((unsigned long)(pSensors->m_gps->get_info(11) - lap_start_timestamp_ms),2*LT_BLINK_FREQ);
			if(blink_timer<LT_BLINK_FREQ && pSpeedo->disp_zeile_bak[1]!=99){
				pSpeedo->disp_zeile_bak[1]=99;
				sprintf(char_buffer,"        ");
				pOLED->string(pSpeedo->default_font+1,char_buffer,2,6);
				skip_displaying=true;
			} else if(blink_timer>=LT_BLINK_FREQ && pSpeedo->disp_zeile_bak[1]!=(signed)(total_lap_time_blink_ms&0xFF)){
				pSpeedo->disp_zeile_bak[1]=(signed)(total_lap_time_blink_ms&0xFF);
				differ=total_lap_time_blink_ms;
			} else {
				skip_displaying=true;
			}
		} else {
			differ=pSensors->m_gps->get_info(11)-lap_start_timestamp_ms;
		}

		if(!skip_displaying){
			if(differ<5940000){ // 1*99*60*1000=5940000
				min=floor(differ/60000);
				sec=int(floor(differ/1000))%60;
				f_sec=floor((differ%1000)/10);
				sprintf(char_buffer,"%02i:%02i.%02i",min,sec,f_sec);
				pOLED->string(pSpeedo->default_font+1,char_buffer,2,6);
			}
		}
	}

	if(level&(1<<UPDATE_BEST_LAP)){
		if(best_lap_time_ms>0){
			min=floor(best_lap_time_ms/60000);
			sec=int(floor(best_lap_time_ms/1000))%60;
			f_sec=floor((best_lap_time_ms%1000)/10);
			sprintf(char_buffer,"%02i:%02i.%02i",min,sec,f_sec);
		} else {
			sprintf(char_buffer,"--:--.--");
		}
		pOLED->string(pSpeedo->default_font,char_buffer,13,4);
	}

	// speed
	if(pSpeedo->disp_zeile_bak[7]!=(int)pSensors->get_speed(false)){
		pSpeedo->disp_zeile_bak[7]=pSensors->get_speed(false);
		sprintf(char_buffer,"%3i",pSensors->get_speed(false));
		pOLED->string(pSpeedo->default_font+1,char_buffer,4,0);
	}

	// gear
	if(pSpeedo->disp_zeile_bak[8]!=pSensors->m_gear->get()){
		pSpeedo->disp_zeile_bak[8]=pSensors->m_gear->get();
		if(pSensors->m_gear->get()>0){
			sprintf(char_buffer,"%i",pSensors->m_gear->get());
		} else {
			sprintf(char_buffer,"N");
		}
		pOLED->string(pSpeedo->default_font+2,char_buffer,0,0);
	}

	// oil
	if(pSpeedo->disp_zeile_bak[9]!=pSensors->get_oil_temperature()){
		pSpeedo->disp_zeile_bak[9]=pSensors->get_oil_temperature();
		sprintf(char_buffer,"%3i.%i{C",int(floor(pSensors->get_oil_temperature()/10))%1000,pSensors->get_oil_temperature()%10); // _32.3°C  7 stellen
		pOLED->string(pSpeedo->default_font,char_buffer,4,5,0,DISP_BRIGHTNESS,-4);
	}
}

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
	sector_start_timestamp_ms=pSensors->m_gps->get_info(11);
};

/* will be called from main() */
void LapTimer::gps_capture_loop(){
	// check if we have a new gps timestamp
	if(pSpeedo->disp_zeile_bak[0]!=(int)(pSensors->m_gps->get_info(11)&0xffff)){
		// jep new GPS data available
		pSpeedo->disp_zeile_bak[0]=(int)(pSensors->m_gps->get_info(11)&0xffff);
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
			strcpy_P(char_buffer,PSTR("    -     /    -     "));
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

/* clear file will open a file in truncate mode and close it*/
int LapTimer::clear_file(unsigned char* filename){
	SdFile folder;
	SdFile file;
	unsigned char temp[20];
	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_TRUNC|O_CREAT|O_RDWR)<0){
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*clear_file: get_file_handle ");
		Serial.print((char*)filename);
		Serial.println(" failed");
#endif
		return -1;
	};
	file.remove();
	file.close();
	folder.close();
	current_sector=0;
	return 0;
};

/*reset times uses update_sector_times to reset every sector to 99999999 ms */
int LapTimer::reset_times(unsigned char* filename){
	calc_best_lap_time();
	for(uint8_t i=0;i<sector_count; i++){
		int result=update_sector_time(i,99999999,filename);
		if(result<0){
			return result;
		}
	}
	return 0;
}

/* add_sector is used to append a NEW sector to a SST file
	[in] latitude
	[in] longitude = Coordiantes of the END of the sector!
	[in] 8.3 filename (12 chars!)

	returns 0=ok, -2 write error, -3 open error
 */
int LapTimer::add_sector(uint32_t latitude, uint32_t longitude, unsigned char* filename){
	if(latitude>90000000){ // max +/-90.000000? ^
		latitude=90000000;
	};
	if(longitude>180000000){ // max +/-180.000000? <->
		longitude=180000000;
	};

	latitude=pSensors->m_gps->nmea_to_dec(latitude);		// !! Saveing in dec deg format
	longitude=pSensors->m_gps->nmea_to_dec(longitude);

	unsigned char temp[31]; // reused to write time
	SdFile folder;
	SdFile file;
	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_CREAT| O_WRITE | O_APPEND)<0){
		file.close();
		folder.close();
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*add_sector: get_file_handle ");
		Serial.print((char*)filename);
		Serial.println(" failed");
#endif
		return -3;
	};

	unsigned long time_needed=pSensors->m_gps->get_info(11)-sector_start_timestamp_ms;
	sector_start_timestamp_ms=pSensors->m_gps->get_info(11);
	if(time_needed>99999999){
		time_needed=99999999;
	}

	sprintf((char *)temp,"%09lu,%09lu,%08lu\r\n",latitude,longitude,time_needed); // init with max time (27h)
	file.writeError=false;
	file.write((char *)temp);
	if(file.writeError){
		file.close();
		folder.close();
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*add_sector: adding sector to ");
		Serial.print((char*)filename);
		Serial.println(" failed");
#endif
		return -2; // write to file failed
	}
	file.close();
	folder.close();

	sector_end_longitude=longitude;
	sector_end_latitude=latitude;
	current_sector++; // we captured one ..goto next
	return 0;
};

/* update_sector time is used to write new, better times to the log file
	[in] sector_id = id of the sector
	[in] setor_time = new time, that should be written, max(uint32_t) = 4294967295 ==> 4294967.295 up to 50 Days :D
	[in] 8.3 filename (12 chars!)

	[out] 0 = ok, -1 = seek error, -2 write error, any other <0 => get file handle error

	File (SpeedoSectorTimes) "sectors.sst" syntax: latitude,longitude,time
	%09lu,%09lu,%08lu\r\n ==> 051123423,009235342,00324577\r\n  (length per line = 30 byte)
	means the sector ends at @51.123423?N, 9.235342?E and the best time for this sector was 324.577sec (wow 5 min) */
int LapTimer::update_sector_time(uint8_t sector_id, uint32_t sector_time, unsigned char *filename){
	unsigned char temp[25];
	SdFile folder;
	SdFile file;

	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_CREAT| O_WRITE)<0){
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*update_sector_time: get_file_handle ");
		Serial.print((char*)filename);
		Serial.println(" failed");
#endif
		return -3;
	};
	if(!file.seekSet(sector_id*30+20)){
		file.close();
		folder.close();
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*update_sector_time: seek to error on ");
		Serial.print((char*)filename);
		Serial.print(" pos: ");
		Serial.println(int(sector_id*30+20));
#endif
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
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*update_sector_time: write error on ");
		Serial.println((char*)filename);
#endif
		return -2; // write to file failed
	}
	file.close();
	folder.close();
	return 0;
};

/* calc_best_lap_time() - is used to parse the SD Card and find all best sector times,
	add them up and save it in best_lap_time_ms
	This should be the starting call!
 */
int LapTimer::calc_best_lap_time(){
	unsigned char temp[20];
	SdFile folder;
	SdFile file;
	if(pFilemanager_v2->get_file_handle(filename,temp,&file,&folder,O_CREAT| O_RDWR)<0){
		pOLED->show_storry(PSTR("File could not be read"),PSTR("Error"),DIALOG_GO_LEFT_1000MS);
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*calc_best_theoretical_lap_time: get_file_handle ");
		Serial.print((char*)filename);
		Serial.println(" failed");
#endif
		return -1;
	};
	delay_calc_active=true; // assume that this track has been driven
	best_lap_time_ms=0;
	sector_count=(int)floor(file.fileSize()/30);
	if(sector_count==0){
		pOLED->show_storry(PSTR("The current selected file is empty, record some sectors first"),PSTR("Empty file"),DIALOG_GO_LEFT_2000MS);
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*calc_best_theoretical_lap_time: file ");
		Serial.print((char*)filename);
		Serial.println(" has a sector_count=0");
#endif
		return -1;
	} else {
		for(uint8_t i=0;i<sector_count; i++){
			uint32_t temp;
			if(get_sector_data(i,&temp,&temp,&temp,filename)<0){
				pOLED->show_storry(PSTR("Get Sectors from file failed"),PSTR("Error"),DIALOG_GO_LEFT_1000MS); // <-- pretty cool
				return -1;
			}
			if(temp==999999){ // initial time for sector
				delay_calc_active=false;
				best_lap_time_ms=0; // invalid
				return 0; // no error but done, at least this sector has no "best" time
			} else {
				best_lap_time_ms+=temp;
			}
		};
		// max 99 min
		if(best_lap_time_ms>5940000){ // 1*99*60*1000=5940000
			best_lap_time_ms=0;
			delay_calc_active=false;
		}
	}
	return 0;
};


/* simply returns the pointer to the filename */
unsigned char* LapTimer::get_active_filename(){ // overload for the menu.cpp
	strcpy_P((char *)filename,PSTR("NAVI/HANNOVER.SST")); // static file for the moment TODO TODO TODO
	return filename;
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
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*get_sector_data: opening of file ");
		Serial.print((char*)filename);
		Serial.println(" failed");
#endif
		return -3;
	};

	// move cursor to sector line
	if(!file.seekSet(sector_id*30)){
		file.close();
		folder.close();
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*get_sector_data: seeking of file to ");
		Serial.print((int)sector_id);
		Serial.println(" failed");
#endif
		return -1;	// file seek failed
	};

	// read line
	int n=file.read(temp, 30);
	if(n<30){
		file.close();
		folder.close();
#ifdef LAPTIMER_DEBUG_OUTPUT
		Serial.print("*get_sector_data: read chars failed, got ");
		Serial.print(n);
		Serial.println("/30");
		Serial.println((char*)temp);
#endif
		return -2;
	}

	// get latitude
	uint32_t temp_value=0;
	for(int i=0;i<9;i++){ // 8-0 = 9 Chars
		if(temp[i]>='0' && temp[i]<='9'){
			temp_value=temp_value*10+(temp[i]-'0');
		}
	}
	*latitude=temp_value;

	// get longitude
	temp_value=0;
	for(int i=10;i<19;i++){ // 21-11 = 10 Chars
		if(temp[i]>='0' && temp[i]<='9'){
			temp_value=temp_value*10+(temp[i]-'0');
		}
	}
	*longitude=temp_value;

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
	*sector_time=temp_value;

	file.close();
	folder.close();
	return 0;
}
