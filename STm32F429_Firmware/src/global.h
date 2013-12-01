/*
 * global.h
 *
 *  Created on: 20.10.2013
 *      Author: jkw
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define ARM_MATH_CM4

extern "C" {
#include <arm_math.h>
#include <stm32f4xx.h>
#include "delay.h"
#include "stdlib.h"
#include <misc.h>			 // I recommend you have a look at these in the ST firmware folder
//#include "stm32_ub_fatfs.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_spi.h"
//#include "stm32f4_discovery_sdio_sd.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_sdram.h"
#include "stm32f4xx_fmc.h"
//#include "stm32_ub_usb_msc_host.h"
}



#include <stddef.h>
#include "stdlib.h"
#include "stdio.h"
#include "version.h"

typedef struct {
	uint32_t longitude;
	uint32_t latitude;
} simple_coordinate;

#include "AsciiLib.h"
#include "schrift.h"
#include "ILI9325.h"
#include "ILI9341.h"
#include "ssd0323.h"
#include "debug.h"
#include "menu.h"

#include "uart.h"
#include "demo.h"
#include "timer.h"
#include "millis.h"
#include "Rpm.h"
#include "speedo.h"
#include "flasher.h"
#include "temperature.h"
#include "gps.h"
#include "clock.h"
#include "stepper.h"
#include "CAN.h"
#include "reset.h"
#include "speed.h"
#include "gear.h"
#include "oiler.h"
#include "fuel.h"
#include "voltage.h"
#include "oiler.h"
#include "sd.h"


#include "config.h"
#include "Sensors.h"
#include "lap_timer.h"
#include "aktors.h"
#include "file_manager_v2.h"
#include "POI_finder.h"
#include "menu.h"

#include "speed_cams.h"
#include "sprint.h"
#include "tetris.h"
#include "POI_finder.h"


/**********************************  working settings ********************************/
// development settings //
#define       GPS_SPEED_ONLY  false   // to ignore the magnetic value  -> hmm das ist doof, aber muss erstmal alleine arbeiten, mal sehen ob der gang angezeigt wird -> wenn ja => mag rennt
#define       BUTTONS_OFF     false   // disable all buttons
#define       WELCOME         true    // die frau am start
/**********************************  working settings ********************************/

#define BMP(a,b,c,d,e,f,g) (a*1000000L+b*100000L+c*10000L+d*1000+e*100+f*10+g)

#endif /* GLOBAL_H_ */
