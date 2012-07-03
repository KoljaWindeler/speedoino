/*
 * reset.h
 *
 *  Created on: 26.10.2011
 *      Author: jkw
 */

#ifndef RESET_H_
#define RESET_H_

void reset(int spezial_down);
void reset_init();
void config_timer0();

// alle Var als volatile damit wir die im interrupt nutzen koennen
volatile int counter_bt;
volatile int counter_avr;
volatile int reset_led; // damit die am anfang etwas leuchtet, show ...
volatile short int counter_bt_init;
volatile short int reset_bt_running; // 1=bt reset am laufen, 0=nix
volatile short int reset_avr_running;// 1=avr reset am laufen, 0=nix
volatile short int last_avr_state; // 1=letzte flanke war steigend,0=fallend
volatile short int last_bt_state;  // 1=letzte flanke war steigend,0=fallend
volatile short int reset_global_active; //1=global aktiv, 0=global aus
volatile int last_rst;

#define RST_PIN 5 // port D
#define RST_LED 6 // rst led=rot, port B
#define BT_LED 7  // bt=blau, port B
#define AVR_LED 5 // avr=orange, port B

#endif /* RESET_H_ */
