#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "global.h"
#include "reset.h"

void reset_init(){
	// IO konfigurieren
	DDRB = 0x00;  // b hat nur iscp header und einen enable eingang, b0
	PORTB = 0x01; // an den bauen wir einen pull up, und machen den lowactive!

	// LED's
	DDRD = 0x00 |(1<<PD5) |(1<<PD6) |(1<<PD7); // 3x led AUSGANG
	PORTD = 0x1F; // alle LEDs an, alle eingänge auf pullup, 20 - 50 kR => 0.1 - 0.25 mA

	// interrupts aktivieren
	MCUCR |= (1<<ISC00) | (1<<ISC10);           //jede Flanke von INT0 oder INT1 als auslöser
	MCUCR &= ~((1<<ISC01) | (1<<ISC11));        //jede Flanke von INT0 oder INT1 als auslöser
	GICR  |= (1<<INT0) | (1<<INT1);				//Global Interrupt Flag für INT0 und INT1

	counter_bt=0;
	counter_avr=0;
	reset_led=0; // damit die am anfang etwas leuchtet, show ...
	reset_bt_running=0; // 1=bt reset am laufen, 0=nix
	reset_avr_running=0;// 1=avr reset am laufen, 0=nix
	last_avr_state=0; // 1=letzte flanke war steigend,0=fallend
	last_bt_state=0;  // 1=letzte flanke war steigend,0=fallend
	last_rst=0;
}

// funktion zum rücksetzen und eventuellen verweilen (spezial_down=1) im bootloader
void reset(int spezial_down){
	if(bit_is_set(PINB,0)){//HIGH active, oder mal sehen was ein leere pin so macht, pullup ziehts hoch aktives runterziehen zum deaktiveren
		/* Vorgehen:
		 * 1. Interrupts aus
		 * 2. Reset starten
		 * 3. Spezialleitung auf LOW
		 * 4. LED AN
		 * 5. Warten
		 * 6. Reset wieder zurückschalten
		 * 7. LED wider aus
		 * 8. Spezialleitung wieder zurück als input
		 */
		// 1. interrupts aus
		cli();

		// 2. Pin c5 auf Ausgang und auf LOW ziehen
		DDRC   = 0x00 | (1<<PC5); // alles eingänge bis auf pc5 ( reset )
		PORTC &= ~(1 << RST_PIN); // reset auf low

		// 3. spezialleitung auf masse ziehen
		if(spezial_down==1){
			DDRB = 0x00 | (1<<PB0); // alles eingänge bis auf pb0
			PORTB |= (1); //<< PB0 bt-flash-spezial-pin auf HIGH debug
			_delay_ms(100);
			PORTB &= ~(1); //<< PB0 bt-flash-spezial-pin auf low
		}

		// 4. LED an
		PORTD |= (1 << RST_LED);// led an

		// 5. Warten
		_delay_ms(200);

		// 6.
		DDRC   = 0x00; // alles eingänge
		PORTC  = 0xff; // alle auf pullup

		// 7. LED aus
		PORTD &= ~(1 << RST_LED); // led aus

		// 8. eventuell mehr warten, dann aber zurück als input
		if(spezial_down==1){
			_delay_ms(1000); // extrazeit im bootloader
			PORTB |= (1); //<< PB0 bt-flash-spezial-pin auf HIGH debug
			DDRB = 0x00; // b hat nur iscp header und einen enable eingang, b0
			PORTB = 0x01; // an den bauen wir einen pull up
		}

		sei(); // aktiviere interrupts
	}
	counter_avr=0;
	counter_bt=0;
};

void config_timer0(){
	// Timer/Counter 0 prescaler 8 => grob 125khz
	TCCR0 = (1<<CS01);
	// Timer/Counter 0 Overflog timer0 interrupt
	TIMSK |= (1<<TOIE0) ;
}

/* overflow vom timer 0 ..
 * 256*8/1.000.000 = 2,048ms
 * 489 * 2,048 = 1001,472 ms zum auslösen
*/
ISR(TIMER0_OVF_vect){
	if(bit_is_set(PINB,0) && !reset_bt_running && !reset_avr_running){//wenn es high ist soll resetet werden
		if(counter_bt>=489){ // 1000 * 1/1000khz => 1sec
			reset_bt_running=1;
			reset(1);
			last_rst=2;
		}
		
		if(counter_avr>=489){ // 1000 * 1/1000khz => 1sec
			reset_avr_running=1;
			reset(0); // run reset ohne langen bootloader quatsch
			last_rst=1;
		}

		// nur hochzählen wenn auch wirklich kein reset gerade am laufen ist, wir wollen ja kein reset beim flashen
		if(reset_bt_running==0 && reset_avr_running==0){
			counter_bt++;
			counter_avr++;
		};
		
	} else {
		counter_avr=0;
		counter_bt=0;
	};
};
	



// interrupt handle für pin-pd2, hier hängt der BT empfänger dran
// reagiert auf jede Flanke
ISR(INT0_vect){
	if(bit_is_clear(PIND,2) && last_bt_state){ //wenn der pin low und die var high
		// hier könnte man die Zeit ausgeben
		PORTD &= ~(1 << BT_LED); // led aus
		last_bt_state=0;
		counter_bt=0;
		if(reset_bt_running==1){ // if there was an reset, it is done by now
			reset_bt_running=0;
		}
	} else if(bit_is_set(PIND,2) && !last_bt_state) { // wenn der pin high und wir vorher low waren
		PORTD |= (1 << BT_LED);// led an
		last_bt_state=1;
		counter_bt=0;
		if(reset_bt_running==1){ // if there was an reset, it is done by now
			reset_bt_running=0;
		}
	}
}



// interrupt handle für pin-pd2, hier hängt der AVR dran
// reagiert auf fallende Flanke, hier die LED AN machen und den zähler resetten
ISR(INT1_vect){
	/* das ist total geil mit dem last_avr_state: wenn der avr sich programmbedingt aufhängt
	 * ist sagen wir mal der pin low, dann wird das hier ausgeführt, danach läuft der puffer über,
	 * der AVR startet aber nicht durch, last_avr_state=0 && bit_stays_clear zumindest wird der hängende
	 * avr nicht zyklisch an dem Pin wackeln und dadurch wird der reset nicht freigegeben und ein
	 * langsam startendes device bleibt nicht in einer resetschleife hängen, TOP ;)
	 */
	if(bit_is_clear(PIND,3) && last_avr_state){ // fallende flanke, sollte also low sein
		PORTD |= (1 << AVR_LED);// led an
		last_avr_state=0;
		counter_avr=0;
		if(reset_avr_running==1){	reset_avr_running=0;		}
	} else if(bit_is_set(PIND,3) && !last_avr_state) { // wenn der pin high und wir vorher low waren
		PORTD &= ~(1 << AVR_LED); // led aus
		last_avr_state=1;
		counter_avr=0;
		if(reset_avr_running==1){	reset_avr_running=0;		}
	}
}

