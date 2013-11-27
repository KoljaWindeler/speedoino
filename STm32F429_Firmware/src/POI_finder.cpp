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
#undef DEBUG_POI_FINDER
#undef DEBUG_POI_FINDER_DETAILED
// init & clean buffer
poi_finder::poi_finder(){}

/* steps:
 * (1) open folder /POI/
 * (2) open file nr "file_id" for the folder opened above
 * (3) parse char by char, check fields to find coordinates + descriptions
 * (4) check if the coordinates are within a range (simple +/- of coordinates)
 * (5) calc distance of those "possibly near" coordinates, note distance + seek line start-point in an [3] array
 * (6) parse all lines to receive after all an array with the nearest seek positions
 * (7) seek the file to this positions, parse the coordinates, write the coordinates in a navi_file
 * (8) activate the navi + set file + set pos = 0
 */

int poi_finder::calc(int file_id){
//	// handle GPS
//	Sensors.mReset.toggle(); // might take some time
//	if(Sensors.mGPS.calc_gps_goodies()!=0){ // returns false if we are not conneted
//		TFT.clear_screen();
//		TFT.string_centered(("Wait on GPS"),3,false);
//		TFT.string(("\x7E cancel"),0,7);
//		uint32_t state_before=Menu.state;
//		while(Sensors.mGPS.calc_gps_goodies()!=0){
//			if(state_before!=Menu.state){ // button push?
//				Sensors.mReset.restore();
//				return -1;
//			}
//			Sensors.mGPS.check_flag(); // otherwise calc_gps_goodies will remain in that state
//		}
//	}
//	TFT.clear_screen();
//	TFT.string_centered(("Scanning file"),0,false);
//	TFT.string(("\x7E cancel"),0,7);
//
//	// (1)
//	SdFile dir_handle;
//	SdFile read_file;
//	char filename[22];
//	strcpy((char*)filename,("/POI/"));
//	if(Filemanager_v2.get_file_handle((uint8_t*)filename,(uint8_t*)filename,&read_file,&dir_handle,O_READ|O_CREAT)<0){	// O_CREATE to create dir if not existing, works like charm
//		TFT.show_storry(("Open POI dir failed"),("Error"),DIALOG_GO_LEFT_1000MS);
//		return -2;
//	} else { // dir is open
//		// (2)
//		unsigned long size;
//		if(!dir_handle.lsJKWNext((uint8_t*)filename,file_id-1,&size)){ // get the filename, cause we have only get passed the "id" of it
//			TFT.show_storry(("Get POI filename failed"),("Error"),DIALOG_GO_LEFT_1000MS);
//			return -3;
//		} else {
//			read_file.close();
//			if(!read_file.open(dir_handle, filename,O_READ)){ // sure its possible to use "get_file_handle", but that leads to a lot of parsing action for nothing... doing it on our own
//				TFT.show_storry(("Open POI file failed"),("Error"),DIALOG_GO_LEFT_1000MS);
//				return -4;
//			} else { // file is open
//				// (3)
//				int8_t state=POI_FINDER_PRECOMMA_LONGITUDE; // see below
//
//				int8_t n=1; // num of chars read
//				uint8_t post_comma=0; // we want exact 6 digits behind the "."
//				uint32_t loaded_latitude=0;	// parsed coordinates
//				uint32_t loaded_longitude=0;
//				uint32_t long_diff;			// differ of parsed coordinates to our
//				uint32_t lati_diff;
//				uint8_t buffer[2];			// file read buffer
//				uint8_t text_buffer[13]; 		// text to display: 10 chars + \x00
//				int8_t pointer_of_text_buffer=0;
//				simple_coordinate loaded_pos;		// dummy to calc distance
//				uint32_t best_dist[3]={999999,999999,999999};
//				ffpos_t best_dist_seek[3];
//				ffpos_t seekstart_of_this_line; // remember to which position we have to jump to get this coordinates
//				seekstart_of_this_line.cluster=0;
//				seekstart_of_this_line.position=0;
//				bool in_copy_mode=false;			// copy mode => copy data to navi file, !copy_mode => parse coordinates to find our who is near us
//				int8_t points_to_copy=0;				// number of points to copy (max 3)
//				bool append_to_write_file=false;	// shall we truncate the content of that file or not?
//
//				while(n>0){
//					n=read_file.read(buffer, 1); // byte by byte
//					int8_t percentage=round(read_file.curPosition()*100/read_file.fileSize());
//					if(percentage!=Speedo.disp_zeile_bak[0]){
//						Speedo.disp_zeile_bak[0]=percentage;
//						char disp_buffer[21];
//						sprintf(disp_buffer,("read %3i%%"),percentage%101);
//						Menu.center_me(disp_buffer,21);
//						TFT.string(disp_buffer,0,1);
//						Sensors.mReset.toggle(); // might take some time
//					}
//
//					if(n>0){ // read ok
//						if(state==POI_FINDER_PRECOMMA_LONGITUDE || state==POI_FINDER_POSTCOMMA_LONGITUDE || state==POI_FINDER_PRECOMMA_LATITUDE || state==POI_FINDER_POSTCOMMA_LATITUDE){
//							// ascii parse content to the loaded_* vars
//							if(buffer[0]>='0' && buffer[0]<='9'){
//								if(state==POI_FINDER_PRECOMMA_LONGITUDE || state==POI_FINDER_POSTCOMMA_LONGITUDE){
//									loaded_longitude=loaded_longitude*10+(buffer[0]-'0');
//								}else if(state==POI_FINDER_PRECOMMA_LATITUDE || state==POI_FINDER_POSTCOMMA_LATITUDE){
//									loaded_latitude=loaded_latitude*10+(buffer[0]-'0');
//								}
//							}
//							if(state==POI_FINDER_PRECOMMA_LONGITUDE || state==POI_FINDER_PRECOMMA_LATITUDE){
//								if(buffer[0]=='.'){// move pre comma to post comma mode
//									state++;
//									post_comma=0; // reset for next coordiante half
//#ifdef DEBUG_POI_FINDER_DETAILED
//									Serial.print("'.' found -> state:");
//									Serial.println(state);
//									Serial.print("Lati/Long so far:");
//									Serial.print(loaded_latitude);
//									Serial.print(" / ");
//									Serial.println(loaded_longitude);
//#endif
//								}
//							} else if(state==POI_FINDER_POSTCOMMA_LONGITUDE || state==POI_FINDER_POSTCOMMA_LATITUDE){ // if we are in "postcomma" mode we have to skip after 6 digits
//								post_comma++;
//								if(post_comma>=6){ // yep we are full
//									state++; // goto unused comma mode
//									post_comma=0; // reset for next coordiante half
//#ifdef DEBUG_POI_FINDER_DETAILED
//									Serial.print("Enough postcomma digits -> state:");
//									Serial.println(state);
//									Serial.print("Lati/Long so far:");
//									Serial.print(loaded_latitude);
//									Serial.print(" / ");
//									Serial.println(loaded_longitude);
//#endif
//								} else if(buffer[0]==','){ // less post_comma than expected but no more will come .. so fill up
//									while(post_comma<6){ // filling
//										if(state==POI_FINDER_POSTCOMMA_LONGITUDE){
//											loaded_longitude=loaded_longitude*10;
//										} else if(state==POI_FINDER_POSTCOMMA_LATITUDE){
//											loaded_latitude=loaded_latitude*10;
//										}
//										post_comma++;
//									}
//									state+=2; // skill unused postcomma section cause there are none
//#ifdef DEBUG_POI_FINDER_DETAILED
//									Serial.print("Less comma found than expected, filled up -> state:");
//									Serial.println(state);
//									if(state==POI_FINDER_POSTCOMMA_LONGITUDE){
//										Serial.print("longitude:");
//										Serial.println(loaded_longitude);
//									} else {
//										Serial.print("latitude:");
//										Serial.println(loaded_latitude);
//									}
//#endif
//								}
//							} // post comma state
//						} else if(state==POI_FINDER_UNUSED_POSTCOMMA_LONGITUDE || state==POI_FINDER_UNUSED_POSTCOMMA_LATITUDE){
//							if(buffer[0]==','){
//								state++; // goto precomma_latitude or LineBreak Mode
//#ifdef DEBUG_POI_FINDER_DETAILED
//								Serial.print("end of unused postcomma digits -> state:");
//								Serial.println(state);
//								if(state==POI_FINDER_WAIT_LINE_BREAK){
//									Serial.print("longitude:");
//									Serial.print(loaded_longitude);
//									Serial.print(" / latitude:");
//									Serial.println(loaded_latitude);
//								}
//#endif
//
//								if(state==POI_FINDER_WAIT_LINE_BREAK && !in_copy_mode){ // this is the last time we are in this state: coordinates parsed, check point (4)
//									// check if it is next to us
//									if(Sensors.mGPS.gps_goody.longitude > loaded_longitude){
//										long_diff=Sensors.mGPS.gps_goody.longitude - loaded_longitude;
//									} else {
//										long_diff=loaded_longitude - Sensors.mGPS.gps_goody.longitude;
//									}
//
//									if(long_diff<300000){ // ca 19,3km in E<->W
//										if(Sensors.mGPS.gps_goody.latitude > loaded_latitude){
//											lati_diff=Sensors.mGPS.gps_goody.latitude - loaded_latitude;
//										} else {
//											lati_diff=loaded_latitude - Sensors.mGPS.gps_goody.latitude;
//										}
//										if(lati_diff<200000){// ca 22km in N<->S
//											loaded_pos.latitude=loaded_latitude;
//											loaded_pos.longitude=loaded_longitude;
//											uint32_t this_dist=Sensors.mGPS.calc_dist_supported(loaded_pos);	// (5) more or less "real distance"
//#ifdef DEBUG_POI_FINDER_DETAILED
//											Serial.print("Point near us -> longitude:");
//											Serial.print(loaded_longitude);
//											Serial.print(" / latitude:");
//											Serial.print(loaded_latitude);
//											Serial.print(" dist:");
//											Serial.print(this_dist);
//											Serial.print(" at cluster ");
//											Serial.print(seekstart_of_this_line.cluster);
//											Serial.print(" and pos ");
//											Serial.println(seekstart_of_this_line.position);
//#endif
//											// check if near and if so, sort in
//											if(this_dist<best_dist[2]){ // nearest, ATTENTION the other way round
//												best_dist[0]=best_dist[1];
//												best_dist[1]=best_dist[2];
//												best_dist[2]=this_dist;
//												best_dist_seek[2]=seekstart_of_this_line;
//											} else if(this_dist<best_dist[1]){
//												best_dist[0]=best_dist[1];
//												best_dist[1]=this_dist;
//												best_dist_seek[1]=seekstart_of_this_line;
//											} else if(this_dist<best_dist[0]){
//												best_dist[0]=this_dist;
//												best_dist_seek[0]=seekstart_of_this_line;
//											}
//											// some nice output
//											if(points_to_copy<3){
//												points_to_copy++;
//
//												sprintf((char*)text_buffer,("At least %i/3"),points_to_copy); // reusing text_buffer to output
//												Menu.center_me((char*)text_buffer,12);
//												TFT.string((char*)text_buffer,5,3);
//												TFT.string_centered(("nearby point found"),4,false);
//											}
//										} // lati_diff<200000
//									} // long_diff<300000
//								} //state==POI_FINDER_WAIT_LINE_BREAK && !in_copy_mode
//							} // wait on "," ... while waiting stay in UNUSED mode
//						} else if(state==POI_FINDER_WAIT_LINE_BREAK){
//							if(in_copy_mode){ // get the Text to copy it
//								if(buffer[0]=='"' && pointer_of_text_buffer==0){ // if string starts with ->"<-
//									pointer_of_text_buffer=0;
//								} else if(buffer[0]==']' && text_buffer[0]=='['){ // if there is something like [ ] on the starting
//									pointer_of_text_buffer=0;
//								} else if(buffer[0]==' ' && pointer_of_text_buffer==0){ // is string starts with -> <-
//									pointer_of_text_buffer=0;
//								} else {
//									bool write_now=false;
//									if(buffer[0]==0x0a){ // uhuh end of line
//										write_now=true;
//									} else if(buffer[0]>=0x20){ // copy everything above controll chars
//										text_buffer[pointer_of_text_buffer]=buffer[0]; // fill 0..9
//										text_buffer[pointer_of_text_buffer+1]=0x00; // move End-of-string
//										pointer_of_text_buffer++; // 9+1->10
//										if(pointer_of_text_buffer>=10){ // 10 == full
//											write_now=true;
//											text_buffer[9]='.'; // add '.' to show that the string is cutted
//										};
//									}
//
//									if(write_now){ // call write_navi_file routine
//										int16_t return_value=write_navi_file(&pointer_of_text_buffer, text_buffer, &loaded_latitude, &loaded_longitude, &append_to_write_file, &points_to_copy, &state, &n, &read_file,best_dist_seek);
//										if(return_value<=0){ // not good, file error? or FINISHED? Anyway: Break processing now!
//											return return_value;
//										}
//									} // write to file
//								} // regular text parsing
//							} // in_copy_mode
//
//							if(buffer[0]==0x0A){ // windows 0x0d 0x0a, linux 0x0a ... everyone happy?
//								state=POI_FINDER_PRECOMMA_LONGITUDE; // rewind
//								read_file.getpos(&seekstart_of_this_line); // +1? prepare for the next coordinate
//								loaded_latitude=0;	// reset coordinates for next round
//								loaded_longitude=0;
//							};
//						}
//					} else { // n==0
//						if(!in_copy_mode){ // regual EOF ... alright
//							if(points_to_copy==0){	// no points found at all
//								TFT.string_centered(("No nearby point found"),4,false);
//								return -8;
//							}
//							read_file.setpos(&best_dist_seek[points_to_copy-1]);
//							TFT.string_centered(("Creating Navi file"),6,false);
//							in_copy_mode=true;						// rerun in copy mode
//							state=POI_FINDER_PRECOMMA_LONGITUDE; 	// reset state
//							n=1; 									// rejoin while loop
//						} else { // what happens if you try to parse the last line and that one is very short? untypical emergency!
//							int16_t return_value=write_navi_file(&pointer_of_text_buffer, text_buffer, &loaded_latitude, &loaded_longitude, &append_to_write_file, &points_to_copy, &state, &n, &read_file,best_dist_seek); // mumble -2 ?
//							if(return_value<=0){ // not good, break
//								return return_value;
//							}
//						}
//					} // n==0
//				} // if(n>0)
//			} // while(n>0){
//		} // get filename
//	} // get dir handle
//	Sensors.mReset.restore();
	return 1; // all ok
}

int poi_finder::write_navi_file(int8_t* pointer_of_text_buffer,uint8_t* text_buffer,uint32_t* loaded_latitude,uint32_t* loaded_longitude,bool* append_to_write_file,int8_t* points_to_copy, int8_t* state, int8_t* n, FIL* read_file,int32_t* best_dist_seek){
//	Sensors.mReset.toggle(); // might take some time
//#ifdef DEBUG_POI_FINDER
//	Serial.println("= write_navi_file =");
//	Serial.println("arguments:");
//	Serial.print("pointer_of_text_buffer:");
//	Serial.println(*pointer_of_text_buffer);
//	Serial.print("text_buffer:");
//	Serial.println((char*)text_buffer);
//	Serial.print("loaded_latitude:");
//	Serial.println(*loaded_latitude);
//	Serial.print("loaded_longitude:");
//	Serial.println(*loaded_longitude);
//#endif
//	// ok: first prepare string
//	while(*pointer_of_text_buffer<10){
//		text_buffer[*pointer_of_text_buffer]=' ';
//		(*pointer_of_text_buffer)++;
//	}
//	text_buffer[*pointer_of_text_buffer]=0x00;
//
//	// failsave
//	if(*loaded_latitude>999999999){
//		*loaded_latitude=999999999;
//	}
//	if(*loaded_longitude>999999999){
//		*loaded_longitude=999999999;
//	}
//
//	// now copy it all together
//	char file_write_buffer[35]; // 2 chars + 0x0D + 0x0A + 0x00
//	sprintf(file_write_buffer,("%09lu,%09lu,0,%s%c"),*loaded_latitude,*loaded_longitude,text_buffer,0x0A);
//
//	// open file
//	SdFile write_file;
//	unsigned char write_filename[25];
//	strcpy((char*)write_filename,("/NAVI/NAVI9.SMF"));
//	uint8_t flags;
//	if(!*append_to_write_file){
//		flags=O_WRITE|O_CREAT|O_TRUNC;
//#ifdef DEBUG_POI_FINDER
//		Serial.println("write mode truncate");
//#endif
//	} else {
//		flags=O_WRITE|O_APPEND; // don't delte or create it
//#ifdef DEBUG_POI_FINDER
//		Serial.println("write mode append");
//#endif
//	}
//
//	// open file
//	if(Filemanager_v2.get_file_handle((uint8_t*)write_filename,&write_file,flags)<0){
//		TFT.show_storry(("Open write-file failed"),("Error"),DIALOG_GO_LEFT_1000MS);
//		return -5;
//	}
//
//	// write description first
//	if(!*append_to_write_file){
//		char temp_chars[22];
//		strcpy(temp_chars,("#dAutogenerated POIs\x0a"));
//		if(!write_file.write(temp_chars,21)){
//			TFT.show_storry(("Write write-file failed"),("Error"),DIALOG_GO_LEFT_1000MS);
//			return -6;
//		}
//	}
//
//	// write stuff to file
//	if(!write_file.write(file_write_buffer,33)){
//		TFT.show_storry(("Write write-file failed"),("Error"),DIALOG_GO_LEFT_1000MS);
//		return -6;
//	}
//
//	// close it
//	write_file.close();
//
//	// stuff we have to do in here, cause this might be the emergency catch
//	*points_to_copy=*points_to_copy-1;		// reduces the number of points that should be written "3->2->1"->0 (end)
//	*append_to_write_file=true; 			// first writing should be truncate file, every following should append
//	*pointer_of_text_buffer=0;				// reset buffer
//	*loaded_latitude=0;						// reset buffer
//	*loaded_longitude=0;					// reset buffer
//	text_buffer[0]='\0';					// reset buffer
//	*state=POI_FINDER_PRECOMMA_LONGITUDE; 	// reset state
//	*n=1; 									// rejoin while loop+
//
//	// finish file
//	if((*points_to_copy)<1){ // == FINISHED ==
//		read_file->close();
//		bool navi_was_active=false;
//		if(Sensors.mGPS.navi_active){	// inform user that we switched his navi
//			navi_was_active=true;
//		}
//		Sensors.mGPS.navi_active=true;
//		Sensors.mGPS.active_file=9;
//		Sensors.mGPS.navi_point=0;
//		Sensors.mGPS.generate_new_order();
//		if(navi_was_active){
//			write_navigation_switch_warning(Sensors.mGPS.active_file,Sensors.mGPS.navi_point);
//			Menu.state=1;
//		} else {
//			Menu.state=11;
//			Menu.update_display=true; // redraw screen (draw speedo)
//		}
//#ifdef DEBUG_POI_FINDER
//		Serial.println("FINISHED POI_FINDER");
//#endif
//		return 0;	// == FINISHED ==
//	};
//	read_file->setpos(&best_dist_seek[*points_to_copy-1]);
//#ifdef DEBUG_POI_FINDER
//	Serial.println("done");
//#endif
	return 1; // go on
}
/* write_navigation_switch_warning is just a kind of infobox
 * it is shown, if the user has had the navigation system active,
 * and we "reprogrammed" it as "POI navi" .. I see no possibility to reroute the navi
 * so we "at least" tell it to the user
 */
void poi_finder::write_navigation_switch_warning(int file_id, int point_id){
	TFT.clear_screen();
	TFT.string(("Very sorry to tell"),0,0);
	TFT.string(("you, but I have to "),0,1);
	TFT.string(("switch to POI mode."),0,2);

	TFT.string(("To resume your track"),0,3);
	TFT.string(("reopen Navi file"),0,4);
	TFT.string(("and select point"),0,5);
	char buffer[3];
	sprintf(buffer,("%2i"),file_id);
	TFT.string(buffer,17,4);
	sprintf(buffer,("%2i"),file_id);
	TFT.string(buffer,17,5);

	TFT.string(("\x7E OK"),0,7);
	Menu.set_buttons(true,false,false,false); // only left
}
