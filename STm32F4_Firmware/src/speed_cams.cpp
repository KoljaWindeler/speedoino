#undef DEBUG_EXECUTION_TIME
#undef DEBUG_POINT_FOUND
#undef DEBUG_HEAVY_CHANGES
#undef DEBUG_EVERY_POINT_DANGER

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
#include "limits.h"

// init & clean buffer
speedcams::speedcams(){
	b2s_status.dest_file_seek=0;			// this is the position in the write file, as indicator if we have written to the file
	b2s_status.state=SPEEDCAM_STATE_INIT; 	// state= init -> read_file_open -> read_error/read_start
	b2s_status.dest_file_open=false;		// var to remember if we have opened the write file, important to remember closing it
	b2s_status.running=false;				// process is currently running
	b2s_status.in_longitude_section=false;	// if we have an file with sorted longitude and we have reached "our" region
	b2s_status.POIs_parsed=0;				// number of POI, read from the big db

	simple_coordinate temp;					// clear coordinates
	temp.latitude=0;
	temp.longitude=0;
	top_three[0]=temp;
	top_three[1]=temp;
	top_three[2]=temp;
	bestOfThree_last_calc=temp;				// coordinaten of the place, where we last calculated the best three of the small db
	db_last_calc=temp;						// coordinaten of the place, where we last calculated small db from the big db

	gps_outdated=false;						// var that will be triggered by GPS
	POI_near=false;							// this is the "result"
	POI_near_dist=9999;						// distance to the POI
	active=false;							// assume the state is "inactive"

	active=true; // DEBUG

	POI_near_id=-1;							// which of the "top" three is the nearest
	bestOfThree_retrigger_distance=0;		// how far must the distance between our current coordinates and "bestOfThree_last_calc" be until we retrigger it
}

bool speedcams::get_active(){
	return active;
}

uint16_t speedcams::get_dist_to_next_point(){
	return POI_near_dist;
}

void speedcams::set_active(bool outer_active){
	active=outer_active;
}

void speedcams::set_gps_outdated(){
	gps_outdated=true;
}

void speedcams::override_start(){
	b2s_status.running=true;				// trigger db read process
	b2s_status.state=SPEEDCAM_STATE_START; 	// this will trigger all file open processings
}

void speedcams::interface(){
	char buffer[22];
	if(!active){
		TFT.string_centered(("inactive"),3);
		return;
	}

	// what are you doing
	if(Speedo.disp_zeile_bak[6]!=Sensors.mClock.get_ss() || b2s_status.state!=Speedo.disp_zeile_bak[0]){
		Speedo.disp_zeile_bak[0]=b2s_status.state;
		Speedo.disp_zeile_bak[6]=Sensors.mClock.get_ss();

		if(b2s_status.state==SPEEDCAM_STATE_INIT){
			if(Sensors.mGPS.get_info(9)>3){
				TFT.string(Speedo.default_font,(" Wait for GPS Signal "),0,0);
			} else {
				sprintf(buffer,("%2i sec to run 1st b2s"),(60-Sensors.mClock.get_ss())%60);
				TFT.string(Speedo.default_font,buffer,0,0);
			}
		} else if(b2s_status.state==SPEEDCAM_STATE_READFILE_OPEN){
			TFT.string_centered(("b2s running"),0);
		} else if(b2s_status.state==SPEEDCAM_STATE_START){
			TFT.string_centered(("b2s idle"),0);
		} else if(b2s_status.state==SPEEDCAM_STATE_ERROR_OPEN_READFILE){
			TFT.string_centered(("error open db"),0);
		} else if(b2s_status.state==SPEEDCAM_STATE_ERROR_OPEN_WRITEFILE){
			TFT.string_centered(("error open s_db"),0);
		} else if(b2s_status.state==SPEEDCAM_STATE_ERROR_WRITEFILE_SEEK || b2s_status.state==SPEEDCAM_STATE_ERROR_WRITE_WRITEFILE){
			TFT.string_centered(("error write to s_db"),0);
		}
	}

	// dont show b2s and s2r if we have nothing calculated
	// init == no gps, start + top_three[0].lat=0 == not yet calced
	if(b2s_status.state==SPEEDCAM_STATE_INIT || (b2s_status.state==SPEEDCAM_STATE_START && top_three[0].longitude==0)){
		return;
	}


	// how many points have we parsed
	if(Speedo.disp_zeile_bak[1]!=b2s_status.POIs_parsed){
		Speedo.disp_zeile_bak[1]=b2s_status.POIs_parsed;
		sprintf(buffer,("parsed   %7i POIs"),b2s_status.POIs_parsed);
		TFT.string(Speedo.default_font,buffer,0,2);
	}

	// how many points have you parsed to the small db
	if(Speedo.disp_zeile_bak[2]!=(signed)b2s_status.dest_file_seek){
		Speedo.disp_zeile_bak[2]=(signed)b2s_status.dest_file_seek;
		sprintf(buffer,("s_db has   %5lu POIs"),b2s_status.dest_file_seek);
		TFT.string(Speedo.default_font,buffer,0,3);
	}


	// dist to retrigger b2s
	int temp=Sensors.mGPS.calc_dist_supported(db_last_calc);
	if(Speedo.disp_zeile_bak[4]!=temp){
		Speedo.disp_zeile_bak[4]=temp;
		sprintf(buffer,("b2s  %5i m/ ca10 km"),temp);
		TFT.string(Speedo.default_font,buffer,0,4);
	}

	// dist to rebuild s2r
	temp=Sensors.mGPS.calc_dist_supported(bestOfThree_last_calc);
	if(Speedo.disp_zeile_bak[5]!=temp){
		Speedo.disp_zeile_bak[5]=temp;
		sprintf(buffer,("s2r  %5i m/%6i m"),temp,bestOfThree_retrigger_distance);
		TFT.string(Speedo.default_font,buffer,0,5);
	}

	// distance to nearest poi
	if(Speedo.disp_zeile_bak[3]!=POI_near_dist){
		Speedo.disp_zeile_bak[3]=POI_near_dist;
		sprintf(buffer,("Nearest POI %7i m"),POI_near_dist);
		TFT.string(Speedo.default_font,buffer,0,7);
	}
}

bool speedcams::calc(){
	//////////////////////////////////////// DEBUG ///////////////////////////////////////////////////
#if defined(DEBUG_EXECUTION_TIME)					// DEBUG TIME
	uint32_t time=Millis.get();							// DEBUG TIME
	uint16_t state=0;								// DEBUG TIME
#endif												// DEBUG TIME
	//////////////////////////////////////// DEBUG ///////////////////////////////////////////////////

	///////////////////////////////// break here if we are not active /////////////////////////////////
	if(!active){
		return false;
	}
	///////////////////////////////// break here if we are not active /////////////////////////////////

	//////////////////////// bigdatabase 2 smalldatabase is currently running ////////////////////////
	if(b2s_status.running){							// let him finish first (fast)
#if defined(DEBUG_EXECUTION_TIME)					// DEBUG TIME
		state=1;									// DEBUG TIME
#endif												// DEBUG TIME
		parse_complete_db();						// calc big database to small database
	}
	//////////////////////// bigdatabase 2 smalldatabase is currently running ////////////////////////


	///////////////////////////////// b2s is not running, trigger b2s/s2r/cn3 /////////////////////////////////
	else { 											// big2small not running
		if(gps_outdated){							// updating is only valid if the last gps_sample is outdated
			if(Sensors.mGPS.calc_gps_goodies()>=0){ // we have 4 Points at all, so its worth calc the goodies
				gps_outdated=false;					// GPS is not longer outdated..
				bool calc_top_three=true;			// assume that we should recalc the top three


				/////////////////////////////////////////////// trigger b2s ///////////////////////////////////////////////
				// DB rebuild process is offline, now check if the seconds of the clock equal 00, if so: check if have to recreate our big db
				if(Sensors.mClock.get_ss()==0){// every 60 seconds: check if we have to retrigger db calculation
					// is the distance between to point of last calculation and our actual coordinates bigger than 10km? if so: recalc db
					if(Sensors.mGPS.calc_dist_supported(db_last_calc)>10000UL){
						b2s_status.running=true;				// trigger db read process
						b2s_status.state=SPEEDCAM_STATE_START; 	// this will trigger all file open processings
						parse_complete_db();					// aaand go!

#if defined(DEBUG_EXECUTION_TIME)								// DEBUG
						state=1;								// DEBUG
#endif															// DEBUG
						// GarbageCleaning
						// good results: SPEEDCAM_STATE_READFILE_OPEN,        SPEEDCAM_STATE_START
						// bad results:  SPEEDCAM_STATE_ERROR_OPEN_READFILE,  SPEEDCAM_STATE_ERROR_OPEN_WRITEFILE
						// bad results:  SPEEDCAM_STATE_ERROR_WRITEFILE_SEEK, SPEEDCAM_STATE_ERROR_WRITE_WRITEFILE
						if(b2s_status.state<0){ // error
							calc_top_three=false; // problem while creation of db -> shutdown
						} // GC
					} // neccessary to update complete db
				} // get_ss==0
				/////////////////////////////////////////////// trigger b2s ///////////////////////////////////////////////


				/////////////////////////////////////////////// trigger s2r ///////////////////////////////////////////////
				// next: check if we have to copy from small db to ram
				// therefore check first that b2s is not running, otherwise the file is corrupted
				if(!b2s_status.running && (b2s_status.state==SPEEDCAM_STATE_READFILE_OPEN || b2s_status.state==SPEEDCAM_STATE_START)){
					// alright, we can access the small file, now check if a update of our RAM is needed
					if(Sensors.mGPS.calc_dist_supported(bestOfThree_last_calc)>bestOfThree_retrigger_distance){
						// our distance to the last point of calc is bigger than our retrigger distance -> rebuild top3 from small db
						parse_small_db(); 			// onestep operation
						calc_top_three=false;	 	// to avoid heavy calculation in one step: only calc dist to top three if nothing other happened
						// GC?
#if defined(DEBUG_EXECUTION_TIME)
						state=2;
#endif
					}

				} // !b2s_status.running
				/////////////////////////////////////////////// trigger s2r ///////////////////////////////////////////////


				/////////////////////////////////////////////// trigger cn3 ///////////////////////////////////////////////
				// CalcNext3, coordinates
				// this should run at any time,exepct in the same timeslot as parse_small_db()
				// (to save time) and before the small_db has been created once (nonsense)
				if(calc_top_three && top_three[0].longitude!=0){ // !=0 means they at least one point is next to us and has been calced at any time
					// now calc distances and find out who is next
					uint16_t distances[3];
					POI_near_dist=9999; // 64km...faaar away .. will be recalculated in the next loop
					for(int i=0;i<3;i++){
						distances[i]=Sensors.mGPS.calc_dist_supported(top_three[i]);
#if defined(DEBUG_POINT_FOUND)
						Serial.print("Top ");
						Serial.print(i);
						Serial.print(" latitude:");
						Serial.print(top_three[i].latitude);
						Serial.print(" longitude:");
						Serial.print(top_three[i].longitude);
						Serial.print(" abs:");
						Serial.println(distances[i]);
#endif
						if(distances[i]<POI_near_dist){
							POI_near_id=i;
							POI_near_dist=distances[POI_near_id];
						}
					}

					// is it worth turning on the warning?

					if(Sensors.get_speed(false)>10 && ((Sensors.get_speed(false)<70 && POI_near_dist<(WARNING_DIST>>1)) || (Sensors.get_speed(false)>=70 && POI_near_dist<WARNING_DIST))){
						POI_near=true;
					} else {
						POI_near=false;
					}
#if defined(DEBUG_EXECUTION_TIME)				// DEBUG
					state+=3;					// DEBUG
#endif											// DEBUG
				} else { // if calc_top_three
					POI_near=false;
				}
				/////////////////////////////////////////////// trigger cn3 ///////////////////////////////////////////////

			} // gps_goodies ok
		}// gps outdated
	} // b2s running
	///////////////////////////////// b2s is not running, trigger b2s/s2r/cn3 /////////////////////////////////


	//////////////////////////////////////// DEBUG ///////////////////////////////////////////////////
#if defined(DEBUG_EXECUTION_TIME)
	if(state>0){
		time=Millis.get()-time;
		Serial.print("calc() took ");
		Serial.print(time);
		Serial.print(" ms");
		if(state==1){
			Serial.println(" to run one step of full->small db");
		} else if(state==2){
			Serial.println(" to parse the complete small db");
		} else if(state==3){
			Serial.print(" to check the nearest 3, closest: ");
			Serial.print(POI_near_dist);
			Serial.print(" dist to calc_point: ");
			Serial.print(Sensors.mGPS.calc_dist_supported(bestOfThree_last_calc));
			Serial.print(" / ");
			Serial.println(bestOfThree_retrigger_distance);
		} else if(state==4){
			Serial.println("to parse the db + to check the nearest three");
		}
	}
#endif
	//////////////////////////////////////// DEBUG ///////////////////////////////////////////////////
	return POI_near;
}


int8_t speedcams::parse_complete_db(){
	unsigned char temp[25];
	uint32_t loaded_latitude;
	uint32_t loaded_longitude;
	uint32_t lati_diff;
	uint32_t long_diff;

//	////////////////// open database ////////////////////////////
//	if(b2s_status.state<=SPEEDCAM_STATE_START){ // nothing has been done yet or an error happend
//#if defined(DEBUG_HEAVY_CHANGES)
//		Serial.println("opening sourcefile");
//#endif
//		unsigned char source_filename[20]; //
//		strcpy((char *)source_filename,("CONFIG/POI.TXT")); // static file
//		b2s_status.POIs_parsed=0;
//		source_file.close();	// should fail very often because the file should already be close, but thats not interessting
//		if(Filemanager_v2.get_file_handle(source_filename,&source_file,O_READ)<0){	// open it now
//			source_file.close();
//#if defined(DEBUG_HEAVY_CHANGES)
//			Serial.println("Damn, opening failed!");
//#endif
//			b2s_status.state=SPEEDCAM_STATE_ERROR_OPEN_READFILE;	// fail
//			b2s_status.running=false;
//			return -3;
//		} else {
//#if defined(DEBUG_HEAVY_CHANGES)
//			Serial.println("ok, file open!");
//#endif
//			b2s_status.state=SPEEDCAM_STATE_READFILE_OPEN;			// ok
//			b2s_status.dest_file_seek=0;							// if we had to reopen our readfile we should reopen our write file
//			b2s_status.dest_file_open=false;
//		}
//	}
//	////////////////// open database ////////////////////////////
//
//	////////////////// parse database ////////////////////////////
//	bool read_on=true;
//	while(read_on){
//		// read line
//		if(source_file.read(temp, 20)<20){
//			////////////////// EOF  ////////////////////////////
//			source_file.close();				// EOF reached
//			dest_file.close();					// EOF reached
//			b2s_status.dest_file_open=false;
//			b2s_status.state=SPEEDCAM_STATE_START;	// reached EOF, return to start
//			b2s_status.running=false;				// we are done
//			read_on=false;						// break the while loop
//
//			db_last_calc=Sensors.mGPS.gps_goody;	 // save location
//
//#if defined(DEBUG_HEAVY_CHANGES)
//			Serial.println("Read return less than 20byte, assuming end of file, done :D");
//#endif
//			////////////////// EOF  ////////////////////////////
//		} else {
//#if defined(DEBUG_EVERY_POINT_DANGER)
//			Serial.print("r:");
//			Serial.println((char*)temp);
//#endif
//			// parse longitude first (1st column)
//			loaded_longitude=0;
//			for(int i=0;i<9;i++){ // 8-0 = 9 Chars
//				if(temp[i]>='0' && temp[i]<='9'){
//					loaded_longitude=loaded_longitude*10+(temp[i]-'0');
//				}
//			}
//
//			// check if it is next to us
//			if(Sensors.mGPS.gps_goody.longitude>loaded_longitude){
//				long_diff=Sensors.mGPS.gps_goody.longitude-loaded_longitude;
//			} else {
//				long_diff=loaded_longitude-Sensors.mGPS.gps_goody.longitude;
//			}
//
//			////////////////// POI has small longitude diff  ////////////////////////////
//			if(long_diff<300000){ // ca 20km in E<->W and N<->S
//#if defined(DEBUG_EVERY_POINT_DANGER)
//				Serial.println("long diff < 300000");
//#endif
//				b2s_status.in_longitude_section=true;
//				// get latitude only if longitude is in range
//				loaded_latitude=0;
//				for(int i=10;i<19;i++){ // 21-11 = 10 Chars
//					if(temp[i]>='0' && temp[i]<='9'){
//						loaded_latitude=loaded_latitude*10+(temp[i]-'0');
//					}
//				}
//
//				if(Sensors.mGPS.gps_goody.latitude>loaded_latitude){
//					lati_diff=Sensors.mGPS.gps_goody.latitude-loaded_latitude;
//				} else {
//					lati_diff=loaded_latitude-Sensors.mGPS.gps_goody.latitude;
//				}
//				////////////////// POI has small longitude+latitude diff  ////////////////////////////
//				if(lati_diff<200000){
//#if defined(DEBUG_POINT_FOUND)
//					Serial.print("New newby Point found at line ");
//					Serial.print(b2s_status.POIs_parsed);
//					Serial.print(". Coordinates:");
//					Serial.print(loaded_longitude);
//					Serial.print("/");
//					Serial.println(loaded_latitude);
//#endif
//					if(!b2s_status.dest_file_open || b2s_status.state==SPEEDCAM_STATE_ERROR_OPEN_WRITEFILE){					// we are NOT ready to write to the dest_file
//#if defined(DEBUG_HEAVY_CHANGES)
//						Serial.println("Dest file has never been opened, doing it now");
//						if(b2s_status.state==SPEEDCAM_STATE_ERROR_OPEN_WRITEFILE){
//							Serial.println("write error?");
//						}
//#endif
//						////////////////// OPEN WRITE FILE  ////////////////////////////
//						dest_file.close(); 																				// just to be sure
//						unsigned char dest_filename[20]; //
//						strcpy((char *)dest_filename,("CONFIG/POI_N.TXT")); // static file
//						if(b2s_status.dest_file_seek>0){																	// but we HAVE written to it in the past
//#if defined(DEBUG_HEAVY_CHANGES)
//							Serial.println("Dest file should have content, open in append mode");
//#endif
//							if(Filemanager_v2.get_file_handle(dest_filename,&dest_file,O_RDWR|O_CREAT|O_APPEND)<0){	// so open it to append further lines
//								dest_file.close();
//								b2s_status.dest_file_open=false;
//								b2s_status.state=SPEEDCAM_STATE_ERROR_OPEN_WRITEFILE;
//#if defined(DEBUG_HEAVY_CHANGES)
//								Serial.println("DAMN file open failed");
//#endif
//								read_on=false;
//							}																							// end of reopen file
//						} else {																						// this is the first time we open this file
//							if(Filemanager_v2.get_file_handle(dest_filename,&dest_file,O_RDWR|O_CREAT|O_TRUNC)<0){	// reset it
//								dest_file.close();
//								b2s_status.dest_file_open=false;
//								b2s_status.state=SPEEDCAM_STATE_ERROR_OPEN_WRITEFILE;
//#if defined(DEBUG_HEAVY_CHANGES)
//								Serial.println("DAMN file open failed");
//#endif
//								read_on=false;
//							}
//						} // restart file
//						////////////////// OPEN WRITE FILE  ////////////////////////////
//					}
//					if(read_on){ // still everything allright
//						b2s_status.dest_file_open=true;
//						if(dest_file.write(temp,20)>=0){
//							b2s_status.dest_file_seek++;
//#if defined(DEBUG_POINT_FOUND)
//							Serial.print("Point nr ");
//							Serial.print(b2s_status.dest_file_seek);
//							Serial.println(" written");
//#endif
//						} else {
//							b2s_status.state=SPEEDCAM_STATE_ERROR_WRITE_WRITEFILE;
//							dest_file.close();
//							b2s_status.dest_file_open=false;
//							b2s_status.running=false;
//							read_on=false;
//						}
//					}
//				};
//#if defined(USE_SORTED_FILE)
//			} else if(b2s_status.in_longitude_section){	// file is sorted by longitude and as soon as we enter "our" region in_longitude_section becomes true, as soon as we leave, we can stop
//#if defined(DEBUG_HEAVY_CHANGES)
//				Serial.println("Point is out of scope from now on, thank you - we are done");
//#endif
//				read_on=false;						// breaks while condition
//				b2s_status.in_longitude_section=false;	// reset for next read
//				dest_file.close();					// done
//				source_file.close();				// done
//				b2s_status.dest_file_open=false;
//				b2s_status.state=SPEEDCAM_STATE_START;	// EOF reached
//				b2s_status.running=false;
//				db_last_calc=Sensors.mGPS.gps_goody;	 // save location
//#endif
//			}
//
//			b2s_status.POIs_parsed++;
//			if(b2s_status.POIs_parsed%50==0){
//				// 50 points == 13ms
//				read_on=false;						// time for a break, we resume this later
//				if(b2s_status.dest_file_open){			// close write file if open to avoid file corruption
//					dest_file.close();
//					b2s_status.dest_file_open=false;
//				}
//#if defined(DEBUG_HEAVY_CHANGES)
//				Serial.println("We parsed 50 POIs, time for a Coke, see you soon");
//#endif
//			}
//		}
//	}
	////////////////// parse database ////////////////////////////
	return b2s_status.state;
};


int8_t speedcams::parse_small_db(){
//	unsigned char temp[25];
//	simple_coordinate loaded_coordinates;
//	uint32_t distances[3]={LONG_MAX,LONG_MAX,LONG_MAX}; // use 2000... sqrt(200...)=44km out of small db scope
//	uint32_t temp_distance=0;
//	uint8_t points_parsed=0;
//	SdFile poi_n_file;
//	strcpy((char *)temp,("CONFIG/POI_N.TXT")); // static file
//
//	if(Filemanager_v2.get_file_handle(temp,&poi_n_file,O_READ)<0){    // open it now
//		poi_n_file.close();
//#if defined(DEBUG_HEAVY_CHANGES)
//		Serial.println("Damn, opening failed!");
//#endif
//		return -3;
//	} else {
//		bool read_on=true;
//		top_three[0].latitude=0;    // clear points
//		top_three[0].longitude=0;
//		top_three[1].latitude=0;
//		top_three[1].longitude=0;
//		top_three[2].latitude=0;
//		top_three[2].longitude=0;
//
//		while(read_on){
//			if(poi_n_file.read(temp, 20)<20){
//				read_on=false;
//			} else {
//				// longitude
//				loaded_coordinates.latitude=0;
//				for(int i=10;i<19;i++){ // 21-11 = 10 Chars
//					if(temp[i]>='0' && temp[i]<='9'){
//						loaded_coordinates.latitude=loaded_coordinates.latitude*10+(temp[i]-'0');
//					}
//				}
//
//				// longitude
//				loaded_coordinates.longitude=0;
//				for(int i=0;i<10;i++){ // 21-11 = 10 Chars
//					if(temp[i]>='0' && temp[i]<='9'){
//						loaded_coordinates.longitude=loaded_coordinates.longitude*10+(temp[i]-'0');
//					}
//				}
//
//				temp_distance=Sensors.mGPS.calc_dist_supported(loaded_coordinates,true); // warning this will NOT return the correct distance still the sqrt is missing but its way faster!
//#if DEBUG_POINT_FOUND
//				Serial.print("Check punkt: ");
//				Serial.print(loaded_coordinates.latitude);
//				Serial.print(" / ");
//				Serial.print(loaded_coordinates.longitude);
//				Serial.print(" = ");
//				Serial.println(temp_distance);
//#endif
//
//				if(temp_distance<distances[0]){ //nearest
//					top_three[2]=top_three[1];
//					top_three[1]=top_three[0];
//					top_three[0]=loaded_coordinates;
//					distances[2]=distances[1];
//					distances[1]=distances[0];
//					distances[0]=temp_distance;
//				} else if(temp_distance<distances[1]){
//					top_three[2]=top_three[1];
//					top_three[1]=loaded_coordinates;
//					distances[2]=distances[1];
//					distances[1]=temp_distance;
//				} else if(temp_distance<distances[2]){ // farest
//					top_three[2]=loaded_coordinates;
//					distances[2]=temp_distance;
//				}
//
//				if(points_parsed<3){
//					points_parsed++;
//				}
//			} // read 30 okay
//		} // while read on
//	} // if open file ok
//	poi_n_file.close();
//	//could lead to problems if only one speed-cam is there and we are going straight to it?
//	bestOfThree_retrigger_distance=(3*Sensors.mGPS.calc_dist_supported(top_three[points_parsed-1]))>>2; // calc real distance now
//	bestOfThree_last_calc=Sensors.mGPS.gps_goody;     // save current location

	return 0;
};
