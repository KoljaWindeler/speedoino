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

extern "C" {
#include <inttypes.h> // eben für int
#include <string.h> // für strlen
#include <math.h> // für round/floor
#include <avr/io.h> // gute frage
#include <avr/delay.h> // gute frage
#include <avr/pgmspace.h> // für die schrift im progmem
#include <wiring.h> // für sowas wie high low auf dem kabel
}

#include "ssd0323.h" 
#include "schrift.h" // die progmem schrift

/////////////////////////////// basis funktion ///////////////////////////////
void ssd0323::senden_spi(unsigned char zeichen)
{
	unsigned char a=0, b=0b10000000;
	fastWriteLow(SPI_CS); // cs auf low => aktivieren
	fastWriteHigh(SPI_CLK); // clock auf high, zu fallender flanke werden daten übernommen

	for(a=0;a<8;a++){      /* a = 0..7, wegen 8 pixel zeichen breite */
		if(zeichen&b)	fastWriteHigh(SPI_DATA);  	/* wenn zeichen&b dann porta bit 0 auf high serial data line */
		else 		fastWriteLow(SPI_DATA); 		/* PORTA bit 0 auf low zwingen serial data line */
		fastWriteLow(SPI_CLK); // clock auf high, zu fallender flanke werden daten übernommen
		//_delay_ms(1);	// technisch korrekt: hier wartezeit einfügen
		fastWriteHigh(SPI_CLK); // clock auf high, zu fallender flanke werden daten übernommen
		//_delay_ms(1);	// technisch korrekt: hier wartezeit einfügen
		b=b>>1;				/* bitmaske einen nach recht schieben => msb first ?! */
	}
	fastWriteHigh(SPI_CS); // cs auf high => deaktivieren
}

/////////////////////////////// kommando funktion ///////////////////////////////
void ssd0323::send_command(unsigned char theCommand)
{
	fastWriteLow(SPI_CD); //turn to command mode
	senden_spi(theCommand);
	fastWriteHigh(SPI_CD); //turn to data mode
}

/////////////////////////////// data funktion ///////////////////////////////
void ssd0323::send_char(unsigned char zeichen)
{
	fastWriteHigh(SPI_CD); //turn to data mode
	senden_spi(zeichen);
}


/////////////////////////////// kommando funktion ///////////////////////////////
void ssd0323::set2pixels(unsigned char x, unsigned char y,unsigned char first,unsigned char second){
	send_command(0x15);
	send_command(x/2);
	send_command((x+1)/2);
	send_command(0x75);
	send_command(y);
	send_command(y+1);
	char ba=(second&0x0f)+((first&0x0f)<<4);
	send_char(ba);
}
/////////////////////////////// line funktion ///////////////////////////////
void ssd0323::line(unsigned char x,unsigned char y,unsigned char x2,unsigned char y2,unsigned char color){
	if(abs(x-x2)>=abs(y-y2)){ // für flache kurven viele x für wenig y werte
		int dir=2;    	int klein=x;	int gr=x2;
		if(x2<x){ 	dir=-1; 	klein=x2;	gr=x; };
		int m=100*(y2-y)/(x2-x);
		for(int i=x; i>=klein && i<=gr; i=i+dir) {
			send_command(0x15);
			send_command(floor(i/2));
			send_command(floor(i/2));		//send_command(floor((1+i)/2));
			send_command(0x75);

			/* so und nun die y-koodirnate:
			 * wir reservieren uns erstmal diese fläche
			 * xx
			 * 0x
			 * xx
			 * weil wir wissen das wir bei 0 auf jedenfall etwas hinsetzen wollen,
			 * die frage ist nur, wohin kommt der rechte, ist er eventuell höher oder tiefer ?
			 */

			send_command(round(m*(i)/100+(y-(x)*m/100))-1);
			send_command(round(m*(i)/100+(y-(x)*m/100))+1);

			// ist der pixel rechts vom aktiven ( der ungerade 1,3,5,... ) größer und müsste somit nach oben rechts oder
			if(round(m*(i+1)/100+(y-(x)*m/100))<round(m*(i)/100+(y-(x)*m/100))){ // dann haben wir unten links und oben rechts
				send_char(color&0x0f);
				send_char((color&0x0f)<<4);
				send_char(0);
			} else if(round(m*(i+1)/100+(y-(x)*m/100))>round(m*(i)/100+(y-(x)*m/100))){
				send_char(0);
				send_char((color&0x0f)<<4);
				send_char(color&0x0f);
			} else {
				send_char(0);
				send_char(((color&0x0f)<<4)+(color&0x0f));
				send_char(0);
			}
		};
	} else {
		int dir=1;    	int klein=y;	int gr=y2;
		if(y2<y){ 	dir=-1; 	klein=y2;	gr=y; };
		int m=100*(x2-x)/(y2-y);
		for(int i=y; i>=klein && i<=gr; i=i+dir) {
			send_command(0x15);
			send_command(floor((x2-(y2-i)*m/100)/2));
			send_command(floor((x2-(y2-i)*m/100)/2));		//send_command(floor((1+i)/2));
			send_command(0x75);
			send_command(i);
			send_command(i);
			if(round((x2-(y2-i)*m/100))%2==0){ // links
				send_char((color&0x0f)<<4);
			} else { // rechts
				send_char(color&0x0f);
			}
		};

	}
}

void ssd0323::zeichen_big_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset){
	unsigned char a,b;
	unsigned int stelle;

	if( z<0x30 || z>0x39 ){
		send_command(0x15);
		send_command(3*spalte+floor(offset/2)); // 30 pixel
		send_command(3*spalte+11+floor(offset/2)); // buchstaben sind 24 px breit
		send_command(0x75);
		send_command(8*zeile);
		send_command(8*zeile+34); // und 35 zeilen hoch, 32 voll, 3 leere zeilen
		for(int i=0; i<11*35;i++){
			send_char(0x00);
		}
		return;
	}
	stelle = 96*(z-0x30);
	// send commands
	send_command(0x15);
	send_command(3*spalte+floor(offset/2)); // 30 pixel
	send_command((3*spalte)+11+floor(offset/2)); // buchstaben sind 24 px breit
	send_command(0x75);
	send_command(8*zeile);
	send_command((8*zeile)+34); // und 35 zeilen hoch, 32 voll, 3 leere zeilen

	for(int c=0;c<32;c++){
		for(int d=0;d<3;d++){
			b=(back&0x0f)+((back&0x0f)<<4); /* default sende 2 pixel mit backlight */
			a = pgm_read_byte(&(font[stelle]));/* 8 px */

			if(a&0x80) b = (b & 0x0f) + ((color&0x0f)<<4); /*funktion für [1]XXX XXXX  */
			if(a&0x40) b = (b & 0xf0) + (color&0x0f); /*funktion für X[1]XX XXXX  */
			send_char(b);

			b=(back&0x0f)+((back&0x0f)<<4); /* default sende 2 pixel mit backlight */
			if(a&0x20) b = (b & 0x0f) + ((color&0x0f)<<4); /*funktion für XX[1]X XXXX  */
			if(a&0x10) b = (b & 0xf0) + (color&0x0f); /*funktion für XXX[1] XXXX  */
			send_char(b);

			b=(back&0x0f)+((back&0x0f)<<4); /* default sende 2 pixel mit backlight */
			if(a&0x08) b = (b & 0x0f) + ((color&0x0f)<<4); /*funktion für XXXX [1]XXX  */
			if(a&0x04) b = (b & 0xf0) + (color&0x0f); /*funktion für XXXX X[1]XX  */
			send_char(b);

			b=(back&0x0f)+((back&0x0f)<<4); /* default sende 2 pixel mit backlight */
			if(a&0x02) b = (b & 0x0f) + ((color&0x0f)<<4); /*funktion für XXXX XX[1]X  */
			if(a&0x01) b = (b & 0xf0) + (color&0x0f); /*funktion für XXXX XXX[1]  */
			send_char(b);
			stelle++;
		}
		for(int d=0;d<0;d++){ // 6 leer pixel nach dem buchstaben
			send_char(0);
		};
	};

	for(int d=0;d<2*12;d++){ // 6 leer pixel nach dem buchstaben
		send_char(0);
	};
}


/////////////////////////////// string to display 5x8 ///////////////////////////////
void ssd0323::zeichen_small_1x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset){
	unsigned char a ,b ,c;
	unsigned int stelle;

	if((z<0x20)||(z>0x7f))z=0x20;
	stelle = 8*(z-0x20);

	send_command(0x15);
	send_command(3*spalte+floor(offset/2)); // die sind 6 breit => immer 2 auf einmal
	send_command((3*spalte)+2+floor(offset/2)); // (6*spalte)+5 => 0,1,2,3,4 das sind 5
	send_command(0x75);
	send_command(8*zeile);
	send_command((8*zeile)+7);

	for(c=0;c<8;c++){
		a = pgm_read_byte(&(font[stelle]));

		b=(back&0x0f)+((back&0x0f)<<4); /* default sende 2 pixel mit backlight */
		if(a&0x80)b = (b & 0x0f) + ((color&0x0f)<<4); /*alibi funktion für XX[1]X XXXX ... kommt nicht vor daher leerstelle */
		if(a&0x40)b = (b & 0xf0) + (color&0x0f); /*wenn XXX[1] XXXX dann sende 4 bit backlight/char, 4 bit char*/
		send_char(b);

		b=(back&0x0f)+((back&0x0f)<<4); /* default sende 2 pixel mit backlight */
		if(a&0x20)b = (b & 0x0f) + ((color&0x0f)<<4); /* wenn XXXX [1]XXX dann sende 4 bit char und dann 4 bit backlight */
		if(a&0x10)b = (b & 0xf0) + (color&0x0f); /* wenn XXXX X[1]XX dann sende 4 bit backlight und 4 bit back/char */
		send_char(b);

		b=(back&0x0f)+((back&0x0f)<<4);
		if(a&0x08)b = (b & 0x0f) + ((color&0x0f)<<4);/* wenn XXXX XX[1]X dann sende 4 bit char und dann 4 bit backlight */
		if(a&0x04)b = (b & 0xf0) + (color&0x0f);/* wenn XXXX XXX[1] dann sende 4 bit char und dann 4 bit back/char */
		send_char(b);

		stelle++;
	}
}

/////////////////////////////// string to display 5x8 ///////////////////////////////
void ssd0323::zeichen_small_2x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset){
	unsigned char a ,b ,c,d, ba,co;
	unsigned int stelle;

	if((z<0x20)||(z>0x7f))z=0x20;
	stelle = 8*(z-0x20);
	ba=(back&0x0f)+((back&0x0f)<<4);
	co=(color&0x0f)+((color&0x0f)<<4);

	send_command(0x15);
	send_command(3*spalte+floor(offset/2)); // die sind 6 breit => immer 2 auf einmal
	send_command((3*spalte)+5+floor(offset/2)); // (6*spalte)+5 => 0,1,2,3,4 das sind 5
	send_command(0x75);
	send_command(8*zeile);
	send_command((8*zeile)+15);

	for(c=0;c<8;c++)
	{
		a = pgm_read_byte(&(font[stelle]));
		for(d=0;d<2;d++)
		{
			b=ba;
			if(a&0x80)b = co;
			send_char(b);
			b=ba;
			if(a&0x40)b = co;
			send_char(b);
			b=ba;
			if(a&0x20)b = co;
			send_char(b);
			b=ba;
			if(a&0x10)b = co;
			send_char(b);
			b=ba;
			if(a&0x08)b = co;
			send_char(b);
			b=ba;
			if(a&0x04)b = co;
			send_char(b);
		}
		stelle++;
	}
}

/////////////////////////////// string to display 5x8 ///////////////////////////////
void ssd0323::zeichen_small_3x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset){
	unsigned char a ,b ,c,d, ba,co;
	unsigned int stelle;

	if((z<0x20)||(z>0x7f))z=0x20;
	stelle = 8*(z-0x20);
	ba=(back&0x0f)+((back&0x0f)<<4);
	co=(color&0x0f)+((color&0x0f)<<4);

	send_command(0x15);
	send_command(3*spalte+floor(offset/2)); // die sind 6 breit => immer 2 auf einmal
	send_command((3*spalte)+8+floor(offset/2)); // (6*spalte)+5 => 0,1,2,3,4 das sind 5
	send_command(0x75);
	send_command(8*zeile);
	send_command((8*zeile)+23);

	for(c=0;c<8;c++){
		a = pgm_read_byte(&(font[stelle]));
		for(d=0;d<3;d++){
			b=ba;
			if(a&0x80)b = co;
			send_char(b);

			b=ba;
			if(a&0x80 && a&0x40) b=co;
			else if(a&0x80) b=((color&0x0f)<<4)+(back&0x0f);
			else if(a&0x40) b=((back&0x0f)<<4)+((color&0x0f));
			send_char(b);

			b=ba;
			if(a&0x40)b = co;
			send_char(b);

			b=ba;
			if(a&0x20)b = co;
			send_char(b);

			b=ba;
			if(a&0x20 && a&0x10) b=co;
			else if(a&0x20) b=((color&0x0f)<<4)+(back&0x0f);
			else if(a&0x10) b=((back&0x0f)<<4)+((color&0x0f));
			send_char(b);

			b=ba;
			if(a&0x10)b = co;
			send_char(b);

			b=ba;
			if(a&0x08)b = co;
			send_char(b);

			b=ba;
			if(a&0x08 && a&0x04) b=co;
			else if(a&0x08) b=((color&0x0f)<<4)+(back&0x0f);
			else if(a&0x04) b=((back&0x0f)<<4)+((color&0x0f));
			send_char(b);

			b=ba;
			if(a&0x04)b = co;
			send_char(b);
		}
		stelle++;
	}
}
/////////////////////////////// string to display 5x8 ///////////////////////////////
void ssd0323::zeichen_small_4x(const uint8_t *font,unsigned char z, int spalte, int zeile, int back, int color, unsigned char offset){
	unsigned char a ,b ,c,d, ba,co;
	unsigned int stelle;

	if((z<0x20)||(z>0x7f))z=0x20;
	stelle = 8*(z-0x20);
	ba=(back&0x0f)+((back&0x0f)<<4);
	co=(color&0x0f)+((color&0x0f)<<4);

	send_command(0x15);
	send_command(3*spalte+floor(offset/2)); // die sind 6 breit => immer 2 auf einmal
	send_command((3*spalte)+11+floor(offset/2)); // (6*spalte)+5 => 0,1,2,3,4 das sind 5
	send_command(0x75);
	send_command(8*zeile);
	send_command((8*zeile)+31);

	for(c=0;c<8;c++)
	{
		a = pgm_read_byte(&(font[stelle]));
		for(d=0;d<4;d++)
		{
			b=ba;
			if(a&0x80)b = co;
			send_char(b);
			send_char(b);
			b=ba;
			if(a&0x40)b = co;
			send_char(b);
			send_char(b);
			b=ba;
			if(a&0x20)b = co;
			send_char(b);
			send_char(b);
			b=ba;
			if(a&0x10)b = co;
			send_char(b);
			send_char(b);
			b=ba;
			if(a&0x08)b = co;
			send_char(b);
			send_char(b);
			b=ba;
			if(a&0x04)b = co;
			send_char(b);
			send_char(b);
		}
		stelle++;
	}
}
/////////////////////////////// progmem string to display 5x8 ///////////////////////////////
void ssd0323::string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset){
	char *char_buffer;
	char_buffer = (char*) malloc (23);
	if (!(char_buffer==NULL)) memset(char_buffer,'\0',sizeof(char_buffer)/sizeof(char_buffer[0]));

	int i=0;
	while(i<22){
		char_buffer[i]=pgm_read_byte(p_string++);
		if(char_buffer[i]=='\0')
			break;
		i++;
	}
	string(font,char_buffer,spalte,zeile,back,color,offset);
	free(char_buffer);
}

/////////////////////////////// progmem string to display 5x8 ///////////////////////////////
void ssd0323::string_P(short font,const char *p_string,unsigned char spalte, unsigned char zeile){
	string_P(font,p_string,spalte,zeile,0,15,0);
}

/////////////////////////////// string to display 5x8 ///////////////////////////////
void ssd0323::string(short font,const char str[],unsigned char spalte, unsigned char zeile){
	string(font,str,spalte,zeile,0,15,0);
}


/////////////////////////////// string to display 5x8 ///////////////////////////////
void ssd0323::string(short font,const char str[],unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset){
	switch(font){
	case SANS_SMALL_4X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_4x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i*4),zeile,back,color,offset);
		};
		break;
	case SANS_SMALL_3X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_3x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i*3),zeile,back,color,offset);
		};
		break;
	case SANS_SMALL_2X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_2x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i*2),zeile,back,color,offset);
		};
		break;
	case SANS_SMALL_1X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_1x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i),zeile,back,color,offset);
		};
		break;
	case VISITOR_SMALL_4X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_4x(&visitor_code[0],str[i],(spalte+i*4),zeile,back,color,offset);
		};
		break;
	case VISITOR_SMALL_3X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_3x(&visitor_code[0],str[i],(spalte+i*3),zeile,back,color,offset);
		};
		break;
	case VISITOR_SMALL_2X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_2x(&visitor_code[0],str[i],(spalte+i*2),zeile,back,color,offset);
		};
		break;
	case VISITOR_SMALL_1X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_1x(&visitor_code[0],str[i],(spalte+i),zeile,back,color,offset);
		};
		break;
	case SANS_BIG_1X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_big_1x(&sans_big[0],str[i],(spalte+i*4),zeile,back,color,offset);
		};
		break;
	case STD_SMALL_2X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_2x(&std_small[0],str[i],(spalte+i*2),zeile,back,color,offset);
		};
		break;
	case STD_SMALL_3X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_3x(&std_small[0],str[i],(spalte+i*3),zeile,back,color,offset);
		};
		break;
	case STD_SMALL_4X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_4x(&std_small[0],str[i],(spalte+i*4),zeile,back,color,offset);
		};
		break;
	default: // das hier ist eigentlich STD_SMALL_1X_FONT
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_1x(&std_small[0],str[i],(spalte+i),zeile,back,color,offset);
		};
		break;
	};
}


/////////////////////////////// filled rectangle funktion ///////////////////////////////
void ssd0323::filled_rect(unsigned char x,unsigned char y,unsigned char width,unsigned char height,unsigned char color){
	send_command(0x15);
	send_command(floor(x/2));
	send_command(floor((width-1+x)/2));
	send_command(0x75);
	send_command(y);
	send_command(y+height);
	unsigned char b=((color&0x0f)<<4)+(color&0x0f);
	for(int a=0;a<(width*height/2);a++){
		send_char(b);
	};
}

///////////////////////// bar zum highlighten //////////////////////////////////
void ssd0323::highlight_bar(unsigned char x,unsigned char y,unsigned char width,unsigned char height){
	x=floor(x/2)*2;
	width=floor(width/2)*2;

	send_command(0x15);
	send_command(floor(x/2));
	send_command(floor((width-1+x)/2));
	send_command(0x75);
	send_command(y);
	send_command(height+y);

	for(int j=0;j<height;j++){
		unsigned char b=0x34;
		for(int i=0;i<6;i++){
			send_char(b);
			b+=34;
		}
		b=0xff;
		for(int a=0;a<(width/2)-12;a++){
			send_char(b);
		};
		b=0xED;
		for(int i=0;i<6;i++){
			send_char(b);
			b-=34;
		}
	};
};


/////////////////////////////// init sequenz ///////////////////////////////
void ssd0323::init(unsigned char phase,unsigned char ref) {  

	pinMode(SPI_CD, OUTPUT); // D/C low = command
	pinMode(SPI_RESET, OUTPUT); // reset 
	pinMode(SPI_DATA, OUTPUT);
	pinMode(SPI_CLK,OUTPUT);
	pinMode(SPI_CS,OUTPUT);

	_delay_ms(10);

	fastWriteHigh(SPI_CS);
	fastWriteLow(SPI_CD);
	fastWriteLow(SPI_DATA);
	fastWriteLow(SPI_CLK);

	// reset
	fastWriteLow(SPI_RESET);
	_delay_ms(10);
	fastWriteHigh(SPI_RESET);
	_delay_ms(1);

	// init sequenze
	/////////////////////////////
	// XXXXXX | Normale | 180° //
	// ----------------------- //
	// Re-Map |  0x41   | 0x52 //
	// Offset |  0x44   | 0x4C //
	/////////////////////////////

	// Column Address
	send_command(0x15);	send_command(0x00);	send_command(0x3F); 
	// Row Address
	send_command(0x75);	send_command(0x00);	send_command(0x3F);
	// Contrast Control
	send_command(0x81);	send_command(0x66);
	// Current Range
	send_command(0x86);
	// Re-map 
	send_command(0xA0);	send_command(0x52);
	// Display Start Line
	send_command(0xA1);	send_command(0x00);
	// Display Offset
	send_command(0xA2);	send_command(0x4C);
	// Display Mode
	send_command(0xA4);
	// Multiplex Ratio
	send_command(0xA8);	send_command(0x3F);
	// set prechange
	// Phase Length
	send_command(0xB1);	send_command(phase);
	// Row Period
	send_command(0xB2);	send_command(0x46);
	// Display Clock Divide
	send_command(0xB3);	send_command(0xF1); // war f1
	/* POR = 0000 0001*/
	// VSL
	send_command(0xBF);	send_command(0x0D);
	// VCOMH
	send_command(0xBE);	send_command(0x02);	send_command(0xBC);	send_command(ref);
	// Gamma
	send_command(0xB8);	send_command(0x01);	send_command(0x11);	send_command(0x22);	send_command(0x32);	
	send_command(0x43);	send_command(0x54);	send_command(0x65);	send_command(0x76);
	/* LB[6:4], LA[2:0] 0101 0101 */
	/* LD[6:4], LC[2:0] 0110 0110 */
	/* LF[6:4], LE[2:0] 1000 0111 */
	// Set DC-DC
	send_command(0xAD); /* Set DC-DC */
	send_command(0x02); /* 03=ON, 02=Off */
	// Display ON/OFF
	send_command(0xAF);
	/* AF=ON, AE=Sleep Mode */
}

/////////////////////////////// create object funktion ///////////////////////////////
ssd0323::ssd0323(){
}


