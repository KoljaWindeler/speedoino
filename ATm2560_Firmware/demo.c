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
/*
 * demo.cpp
 *
 *  Created on: 17.06.2013
 *      Author: kolja
 */

speedo_demo::speedo_demo(){

}

speedo_demo::~speedo_demo(){

}

void speedo_demo::get_dz(const char *data){
	while(pgm_read_byte(data) != 0x00)
		Serial.print(pgm_read_byte(data++));
}
