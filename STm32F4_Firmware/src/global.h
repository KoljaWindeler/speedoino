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
#include "stm32_ub_fatfs.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
}

#include <stddef.h>
#include "stdlib.h"
#include "stdio.h"

#include "AsciiLib.h"
#include "ILI9325.h"
#include "ssd0323.h"
#include "debug.h"
#include "menu.h"

#include "uart.h"
#include "demo.h"
#include "millis.h"
#include "Rpm.h"
#include "aktors.h"
#include "speedo.h"
#include "flasher.h"
#include "temperature.h"

#include "Sensors.h"

/**********************************  working settings ********************************/
// development settings //
#define       GPS_SPEED_ONLY  false   // to ignore the magnetic value  -> hmm das ist doof, aber muss erstmal alleine arbeiten, mal sehen ob der gang angezeigt wird -> wenn ja => mag rennt
#define       BUTTONS_OFF     false   // disable all buttons
#define       WELCOME         true    // die frau am start
/**********************************  working settings ********************************/

#define BMP(a,b,c,d,e,f,g) (a*1000000L+b*100000L+c*10000L+d*1000+e*100+f*10+g)


#endif /* GLOBAL_H_ */
