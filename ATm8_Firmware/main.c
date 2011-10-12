/*
 * main.c
 *
 *  Created on: 12.06.2011
 *      Author: kolja
 *
 *  Aufgaben des ATMega:
 *  1. Sobald eine Bluetoothverbindung aufgebaut wird einen Reset auslösen
 *  2. Wenn der große Prozessor nichts mehr sagt diesen Resetten können ( Watchdog Funktion )
 *  3. Den Reset global unterdrücken können
 *  4. Per UART antworten können warum resettet wurde
 *  5. Im Falle eines BT Reset den Bootloader des großen daran hindern schneller durchzulaufen als Daten gesendet werden können
 *
 *  Konzept:
 *  Wir haben 2 Counter, counter_bt und counter_avr, die werden hochgezählt,
 *  momentan etwas(!) langsamer als 1kHz ( 1ms warten + if's )
 *  Wenn einer von beiden den Schwellwert aus CYCLES_FOR_* erreicht dann wird ein RESET
 *  ausgelöst. Das heisst eine HIGH - LOW - HIGH Kombination am Reset Pin.
 *  Wenn es ein BT Reset ist dann sollte die ganze geschichte erst dann wieder freigegeben
 *  werden, wenn eine toggelnde Flanke am Connect Pin gefunden wird.
 *
 *  FALLS der AVR überhaupt abschmiert, soll hier ergänzt werden:
 *  Der AVR sollte in einem Takt ähnlich dem des Bluetooth "togglen",
 *  sobald er mal nicht togglet läuft sein zähler auch über und resettet wie bei BT
 *  eigentlicht das gleiche.
 *
 *  zusätzlich sollte der avr bestimmen können das die ganze geschichte deaktiviert wird,
 *  z.B. der Filemanager verweilt sehr lang ohne loop
 *  Daher ist der pin PinB0 vom AVR auf LOW zu ziehen um den Reset zu deaktivieren,
 *  wir ziehen den dann unsererseits intern per pullup auf high und somit können wir resetten
 *  wenn der AVR nix sagt, also z.B. kein Programm hat.
 *
 *  Startup: zum testen einmal mit allen augen zwinkern. Alle LED's an, nach 1000ms programmstart
 *  und damit wieder aus.
 *
 *  Wenn wir einen Bluetooth reset haben, schalten wir den PinB0 als Ausgang und ziehen ihn auf Masse
 *  da der Bootloader des großen einen Input mit Pullup schaltet. In der Zeit die der Pin auf Masse liegt
 *  wird der Count der des Bootloader deaktiviert.
 */
// includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

// PIN def
#define RST_PIN 5 // port D
#define RST_LED 6 // rst led=rot, port B
#define BT_LED 7  // bt=blau, port B
#define AVR_LED 5 // avr=orange, port B

// beide RESET_ARTEN global an/aus
#define AVR_RESET 1
#define BT_RESET 1

// überlaufwerte
#define CYLCES_FOR_AVR 10000 // steht auf 1000 ms, vielleicht mit speicher etc etwas mehr sagen wir 2000*1,1 => 2,2sec
#define CYLCES_FOR_BT 1000 // ist seeeehr zuverlässig, weils sonst nix macht 720=1,1*~640 war die kondensator lösung

//////////////// uart ////////////////
#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun nachgeholt mit 1Mhz"
#define F_CPU 1000000UL  // Systemtakt in Hz - Definition als unsigned long beachten
#endif

#define BAUD 4800UL      // Baudrate
#define UART_MAXSTRLEN 10

// Berechnungen
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
#endif

int uart_putc(unsigned char c);
void uart_puts (char *s);
volatile uint8_t uart_str_complete=0;
volatile uint8_t uart_str_counter=0;
volatile char uart_string[UART_MAXSTRLEN +1]="";
//////////////// uart ////////////////

// alle Var als volatile damit wir die im interrupt nutzen können
volatile int counter_bt=0;
volatile int counter_avr=0;
volatile int reset_led=0; // damit die am anfang etwas leuchtet, show ...
volatile short int reset_bt_running=0; // 1=bt reset am laufen, 0=nix
volatile short int reset_avr_running=0;// 1=avr reset am laufen, 0=nix
volatile short int last_avr_state=0; // 1=letzte flanke war steigend,0=fallend
volatile short int last_bt_state=0;  // 1=letzte flanke war steigend,0=fallend

/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////
// uart interrupt empfangen
ISR(USART_RXC_vect){
	unsigned char nextChar;
	nextChar = UDR;
	if(uart_str_complete==0){
		if(nextChar!= '\n' && nextChar != '\r' && uart_str_counter < UART_MAXSTRLEN -1){
			uart_string[uart_str_counter]=nextChar;
			uart_str_counter++;
		} else {
			uart_string[uart_str_counter]='\0';
			uart_str_counter=0;
			uart_str_complete=1;
		};
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
/////////////////////////// INTERRUPT ROUTINEN /////////////////////////////

// hätten wir doch noch einen interrupt über, dann müsste das hier nicht per polling abgefragt werden,
// ist die leitung auf LOW -> kein reset. pullup sollte den pin hier hochhalten wenn der atmega (der große) leer ist
void check_active(){
	if(bit_is_clear(PINB,0)){//LOW=reset des counders,pullup dran
		counter_avr=0;
		counter_bt=0;
	}
}

/* Szenario
 * 1. ATmega mit Programm, zieht pegel auf LOW, alles super
 * 2. ATmega nach verbocktem upload, was tut der pegel wenn er
 * 		a) vorher High war -> pegel wird gehalten
 * 		b) vorher low war -> pegel wird gehalten
 */


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

//////// uart //////////////
void uart_init(void)
{
	UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE); // rx,tx,rx interrupt
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // Asynchron 8N1

	UBRRH = UBRR_VAL >> 8;
	UBRRL = UBRR_VAL & 0xFF;
}

int uart_putc(unsigned char c)
{
	while (!(UCSRA & (1<<UDRE)))  /* warten bis Senden moeglich */
	{
	}

	UDR = c;                      /* sende Zeichen */
	return 0;
}

/* puts ist unabhaengig vom Controllertyp */
void uart_puts (char *s)
{
	while (*s)
	{   /* so lange *s != '\0' also ungleich dem "String-Endezeichen" */
		uart_putc(*s);
		s++;
	}
}
//////// uart //////////////

int main(){
	// IO konfigurieren
	DDRB = 0x00;  // b hat nur iscp header und einen enable eingang, b0
	PORTB = 0x01; // an den bauen wir einen pull up, und machen den lowactive!

	DDRC = 0x00; // alles eingänge  ( reset (pc5) schalten wir später )
	PORTC = 0xff; // alle auf high auch reset

	DDRD = 0x00 |(1<<PD5) |(1<<PD6) |(1<<PD7); // 3x led AUSGANG
	PORTD = 0x5f; // alle LEDs an, alle eingänge auf pullup, 20 - 50 kR => 0.1 - 0.25 mA

	MCUCR |= (1<<ISC00) | (1<<ISC10);           //jede Flanke von INT0 oder INT1 als auslöser
	MCUCR &= ~((1<<ISC01) | (1<<ISC11));        //jede Flanke von INT0 oder INT1 als auslöser
	GICR  |= (1<<INT0) | (1<<INT1);				//Global Interrupt Flag für INT0
	sei();
	uart_init();

	int last_rst=0;
	_delay_ms(3000); // verzögerung beim ersten Start

	for(;;){ // infinitiy loop
		// checken ob ich überhaupt arbeiten soll
		check_active(); // dafür war leider kein interrupt mehr über

		// uart abfragen und gegebenenfalls antworten
		if(uart_str_complete==1){
			if(uart_string[0]=='y'){ // das ist unsere "why" abfrage, da fragt der große warum resettet wurde
				uart_putc(last_rst+'0');
				uart_putc('!');
				last_rst=0; // setze den status zurück damit wir immer einen frischen abfragen, wenn der große jetzt neustartet aber es steht da power, dann wissen wir, das war nicht der kleine, solange wie nicht wirklich einen powerlost hatten
			}
			uart_str_complete=0; // damit der interrupt wieder aktiv wird
		}


		// wenn wir mit dem AVRPIN höher gezählt haben als wir sollten
		if((counter_avr>CYLCES_FOR_AVR) && AVR_RESET){
			reset_avr_running=1;
			reset(0); // run reset ohne langen bootloader quatsch
			last_rst=1;
		};

		// anzahl an iterationen für bt erreicht
		if((counter_bt>CYLCES_FOR_BT) && BT_RESET){
			reset_bt_running=1;
			reset(1);
			last_rst=2;
		};

		// nur hochzählen wenn auch wirklich kein reset gerade am laufen ist, wir wollen ja kein reset beim flashen
		if(reset_bt_running==0 && reset_avr_running==0){
			counter_bt++;
			counter_avr++;
		};

		// show and shine
		if(reset_led<1000){ // nach 0.5 sek reset led aus;
			reset_led++;
			if(reset_led%400 > 200){
				PORTD &= ~(1 << RST_LED); // led aus
			} else {
				PORTD |= (1 << RST_LED); // led an
			}
		} else {
			PORTD &= ~(1 << RST_LED); // led aus
		}

		_delay_ms(1); // damit wir nicht zu schnell sind
	}
}
