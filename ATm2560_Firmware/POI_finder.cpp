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

// init & clean buffer
speedo_poi_finder::speedo_poi_finder(){}

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

int speedo_poi_finder::calc(int file_id){
	// (1)
	SdFile dir_handle;
	SdFile file_handle;
	char filename[22];
	strcpy_P((char*)filename,PSTR("/POI/"));

	if(pFilemanager_v2->get_file_handle((unsigned char*)filename,(unsigned char*)filename,&file_handle,&dir_handle,O_READ|O_CREAT)<0){	// O_CREATE to create dir, if not existing <- TODO check if that is working
		pOLED->show_storry(PSTR("Open POI dir failed"),PSTR("Error"),DIALOG_GO_LEFT_1000MS);
	} else {
		unsigned long size;
		if(!dir_handle.lsJKWNext((unsigned char*)filename,file_id,&size)){
			pOLED->show_storry(PSTR("Open POI file failed"),PSTR("Error"),DIALOG_GO_LEFT_1000MS);
		}
	}

	return 0;
}







