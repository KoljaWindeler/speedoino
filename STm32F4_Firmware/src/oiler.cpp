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

oiler::oiler(void){
	grenze=0;
}

oiler::~oiler(){
};

void oiler::init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; 			// the pins are configured as alternate function so the USART peripheral has access to them
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;		// this defines the IO speed and has nothing to do with the baudrate!
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;			// this defines the output type as push pull mode (as opposed to open drain)
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// this activates the pullup resistors on the IO pins
	//GPIO_Init(GPIOD, &GPIO_InitStruct); TODO pin ok?
	//GPIO_WriteBit(GPIOD,GPIO_Pin_11,Bit_RESET); TODO pin ok?
	Serial.puts_ln(USART1,("Oiler init done"));
};

bool oiler::check_vars(){
	if(grenze==0){
		grenze=4000;
		Serial.puts(USART1,("oiler failed"));
		return true;
	}
	return false;
};



int oiler::send_impulse(){
	if(Sensors.get_speed(false)>30){ // nur ölen wenn wir über 30 kmh sind
		//GPIO_WriteBit(GPIOD,GPIO_Pin_11,Bit_SET); TODO pin ok? // 150ms high
		_delay_ms(150);
		//GPIO_WriteBit(GPIOD,GPIO_Pin_11,Bit_RESET); TODO pin ok? // 150ms high
		Config.write("speedo.txt"); // store this
		return 0;
	} else {
		return -1;
	}
};

void oiler::check_value(){
	/* hier noch die geschwindigkeitsprogression einbauen
	 * sowas wie
	 * if(Speedo.trip_dist[6]/Speedo.avg_timebase[6]*3.6>140 && Speedo.trip_dist[6]>grenze*0.85){
	 * da kann man noch ne ganze Latte hinterlegen wenn man das umbedingt wollen würde
	 */

	if(Speedo.trip_dist[6]>unsigned(grenze)){
		if(send_impulse()>=0){ // nur ölen wenn wir über 30 kmh sind
			Speedo.trip_dist[6]=0;
			Speedo.max_speed[6]=0;
			Speedo.avg_timebase[6]=0;
		};
	};
}
