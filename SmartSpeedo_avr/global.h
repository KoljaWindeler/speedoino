#ifndef GLOBAL_H_
#define GLOBAL_H_
#include "stdint.h"

typedef struct {
	uint32_t longitude;
	uint32_t latitude;
} simple_coordinate;

// sensoren
#include "dz.h"
#include "temperature.h"
#include "speed.h"
#include "reset.h"
#include "oiler.h"
#include "voltage.h"
#include "CAN.h"
#include "KKL.h"


#include "config.h" 	// vor timer
#include "timer.h" 	//nach config
#include "speedo.h"
#include "menu.h"
#include "sensors.h" 	// als vorletztes
#include "aktors.h" 	// als letztes
#include "debug.h"
#include "file_manager_v2.h"
#include "version.h"
/**********************************  working settings ********************************/
// development settings //
#define       GPS_SPEED_ONLY  false   // to ignore the magnetic value  -> hmm das ist doof, aber muss erstmal alleine arbeiten, mal sehen ob der gang angezeigt wird -> wenn ja => mag rennt
#define       BUTTONS_OFF     false   // disable all buttons
#define       WELCOME         true    // die frau am start
/**********************************  working settings ********************************/

#define TARGET_UART_SPEED 115200
#define BMP(a,b,c,d,e,f,g) (a*1000000L+b*100000L+c*10000L+d*1000+e*100+f*10+g)
//Useful macros for accessing single bytes of int and long variables
#define BYTE1(var) *((unsigned char *) &var+1)
#define BYTE0(var) *((unsigned char *) &var)

//#include <WProgram.h>
#include "wiring.h"
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h> // timing
#include <stdlib.h>
#include <stdint.h>
#include "inc/wiring.h"
#include "inc/I2C.h"
#include "inc/HardwareSerial.h"
#include "inc/wiring.h"
#include "inc/mcp2515_defs.h"
#include <string.h>
void setup();
void init_speedo();



#endif /* FKT_H_ */
