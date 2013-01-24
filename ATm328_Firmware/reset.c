#include <avr/io.h>
//#include <avr/iom328p.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "global.h"
#include "reset.h"
int ledstate=0;

void reset_init(){
	// IO konfigurieren - LED's
	//DDRD 0=eingang, 1=ausgang
	//PORTD 1=pullup 0=kein pullup
	DDRD = 0x00 |(1<<PD5) |(1<<PD6) |(1<<PD7); // 3x led AUSGANG,
	PORTD = 0x00; // kein Pull up, zwei eingänge die getrieben werden, eine RX/TX Leitung und die LEDS werden selbst gesteuert
	//0xFF & ~((1<<PD7) | (1<<PD6) | (1<<PD5) | (1<<PD3)); // alle LEDs an, alle einguenge auf pullup bis auf D3, 20 - 50 kR => 0.1 - 0.25 mA

	// interrupts aktivieren
	EICRA |= (1<<ISC00) | (1<<ISC10);           //jede Flanke von INT0 oder INT1 als auslueser
	EICRA &= ~((1<<ISC01) | (1<<ISC11));        //jede Flanke von INT0 oder INT1 als auslueser
	EIMSK  |= (1<<INT0) | (1<<INT1);			//Global Interrupt Flag fuer INT0 und INT1

	counter_bt=0;
	counter_avr=0;
	reset_led=0; // damit die am anfang etwas leuchtet, show ...
	reset_bt_running=0; // 1=bt reset am laufen, 0=nix
	reset_avr_running=0;// 1=avr reset am laufen, 0=nix
	reset_global_active=1; // 1 = active, 0 = inactive
	last_avr_state=0; // 1=letzte flanke war steigend,0=fallend
	last_bt_state=0;  // 1=letzte flanke war steigend,0=fallend
	last_rst=0;
	counter_bt_init=0;
}

// funktion zum ruecksetzen und eventuellen verweilen (spezial_down=1) im bootloader
void reset(int spezial_down){
	if(reset_global_active){
		/* Vorgehen:
		 * 1. Interrupts aus
		 * 2. Reset starten
		 * 3. Spezialleitung auf LOW
		 * 4. LED AN
		 * 5. Warten
		 * 6. Reset wieder zurueckschalten
		 * 7. LED wider aus
		 * 8. Spezialleitung wieder zurueck als input
		 */
		// 1. interrupts aus
		cli();

		// 2. Pin c5 auf Ausgang und auf LOW ziehen
		DDRC  |=  (1<<PC5); // pc5 auf ausgang ( reset )
		PORTC &= ~(1 << RST_PIN); // reset auf low

		// 3. spezialleitung auf masse ziehen
		if(spezial_down==1){
			DDRD |= (1<<PD3); // pd3 auf ausgang
			PORTD &= ~(1<<PD3); // pd3 auf low
		}

		// 4. LED an
		if(DEBUG_LEDS){
			PORTD |= (1 << RST_LED);// led an
		}

		// 5. Warten
		_delay_ms(200);

		// 6.
		DDRC   &= ~(1<<PC5) ; // Reset ausgang wieder auf zuhoeren schalten
		PORTC  |= 1<<PC5; // und ihm ein pull up geben

		// 7. eventuell mehr warten, dann aber zurueck als input
		if(spezial_down==1){
			_delay_ms(10); // extrazeit im bootloader
			PORTD |= (1<<PD3); //<< PD3 bt-flash-spezial-pin auf HIGH ( wie pull up auch )
			DDRD &= ~(1<<PD3); // PD3 wieder auf eingang setzen
		}

		// 8. LED aus
		if(DEBUG_LEDS){
			PORTD &= ~(1 << RST_LED); // led aus
		}

		sei(); // aktiviere interrupts
	}
	counter_avr=0;
	counter_bt=0;
	reset_avr_running=0; // macht das hier sinn? bluetooth sollte hier nicht zurueck gesetzt werden
	// bluetooth macht hier den reset und erst wenn es blinkt dann gibt er ihn frei,
};

void config_timer0(){
	// Timer/Counter 0 prescaler 64 => 8Mhz 127khz
	TIMSK0 = (1<<TOIE0);   //Enable a timer overflow interrupt
	TCCR0A = 0;         //just a normal overflow
	TCCR0B = (1<<CS01) | (1<<CS00);   //count
}

/* overflow vom timer 0 ..
 * prescale 64, cpu_freq=16mhz => 64/16.000.000 = 0,000004sec each increment
 * Timer0 is a 8-Bit Timer => 2⁸=256 Counts to overflow = 0,000004sec*256=0,002048 sec= 1,024 ms
 * Bluetooth pin is: 312ms high,312ms low, we have an interrupt on pin change so we can reset after (312/1,024)*1,2 where 20% is safety
 * 364 Timer overruns is max
 *
 * AVR: Lets say 20sec, 20/0,002=10000 Overruns
 */

ISR(TIMER0_OVF_vect){
	TCNT0=0;   //reset the counter
	if(reset_global_active){//wenn es high ist soll resetet werden
		if(counter_bt>=364 && !reset_bt_running && counter_bt_init>10){ //
			reset_bt_running=1;
			reset(1);
			last_rst=2;
		}

		if(counter_avr>=10000 && !reset_avr_running){
			reset_avr_running=1;
			reset(1); // run reset ohne langen bootloader quatsch
			last_rst=1;
		}

		// nur hochzuehlen wenn auch wirklich kein reset gerade am laufen ist, wir wollen ja kein reset beim flashen
		if(reset_bt_running==0 && reset_avr_running==0){
			counter_bt++;
			counter_avr++;
		};

	};
};


// interrupt handle fuer pin-pd2, hier huengt der BT empfuenger dran
// reagiert auf jede Flanke
ISR(INT0_vect){
	if(counter_bt_init<=10) counter_bt_init++;
	if(bit_is_clear(PIND,2) && last_bt_state){ //wenn der pin low und die var high
		// hier kuennte man die Zeit ausgebena#
		if(DEBUG_LEDS){
			PORTD &= ~(1 << BT_LED); // led aus
		}
		last_bt_state=0;
		counter_bt=0;
		if(reset_bt_running==1){ // if there was an reset, it is done by now
			reset_bt_running=0;
		}
	} else if(bit_is_set(PIND,2) && !last_bt_state) { // wenn der pin high und wir vorher low waren
		if(DEBUG_LEDS){
			PORTD |= (1 << BT_LED);// led an
		}
		last_bt_state=1;
		counter_bt=0;
		if(reset_bt_running==1){ // if there was an reset, it is done by now
			reset_bt_running=0;
		}
	}
}



// interrupt handle fuer pin-pd2, hier huengt der AVR dran
// reagiert auf fallende Flanke, hier die LED AN machen und den zuehler resetten
ISR(INT1_vect){
	/* das ist total geil mit dem last_avr_state: wenn der avr sich programmbedingt aufhuengt
	 * ist sagen wir mal der pin low, dann wird das hier ausgefuehrt, danach lueuft der puffer ueber,
	 * der AVR startet aber nicht durch, last_avr_state=0 && bit_stays_clear zumindest wird der huengende
	 * avr nicht zyklisch an dem Pin wackeln und dadurch wird der reset nicht freigegeben und ein
	 * langsam startendes device bleibt nicht in einer resetschleife huengen, TOP ;)
	 */
	if(bit_is_clear(PIND,3) && last_avr_state){ // fallende flanke, sollte also low sein
		if(DEBUG_LEDS){
			PORTD |= (1 << AVR_LED);// led an
		}
		last_avr_state=0;
	} else if(bit_is_set(PIND,3) && !last_avr_state) { // wenn der pin high und wir vorher low waren
		if(DEBUG_LEDS){
			PORTD &= ~(1 << AVR_LED); // led aus
		}
		last_avr_state=1;
	}
	counter_avr=0;
	reset_avr_running=0;
}

