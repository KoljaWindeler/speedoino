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
}

#include "global.h"
#include "ssd0323.h"
#include "schrift.h" // die progmem schrift


/////////////////////////////// basis funktion ///////////////////////////////
void ssd0323::senden_spi(unsigned char zeichen)
{
	unsigned char a=0, b=0b10000000;
	GPIO_WriteBit(GPIOE,GPIO_Pin_7,Bit_RESET);//fastWriteLow(SPI_CS); // cs auf low => aktivieren
	GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_SET);//fastWriteHigh(SPI_CLK); // clock auf high, zu fallender flanke werden daten übernommen

	for(a=0;a<8;a++){      /* a = 0..7, wegen 8 pixel zeichen breite */
		if(zeichen&b){
			GPIO_WriteBit(GPIOE,GPIO_Pin_11,Bit_SET);//fastWriteHigh(SPI_DATA);  	/* wenn zeichen&b dann porta bit 0 auf high serial data line */
		} else {
			GPIO_WriteBit(GPIOE,GPIO_Pin_11,Bit_RESET);//fastWriteLow(SPI_DATA); 		/* PORTA bit 0 auf low zwingen serial data line */
		}
		GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_RESET);//fastWriteLow(SPI_CLK); // clock auf high, zu fallender flanke werden daten übernommen
//		_delay_ms(1);	// technisch korrekt: hier wartezeit einfügen
		GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_SET);//fastWriteHigh(SPI_CLK); // clock auf high, zu fallender flanke werden daten übernommen
//		_delay_ms(1);	// technisch korrekt: hier wartezeit einfügen
		b=b>>1;				/* bitmaske einen nach recht schieben => msb first ?! */
	}
	GPIO_WriteBit(GPIOE,GPIO_Pin_7,Bit_SET);//fastWriteHigh(SPI_CS); // cs auf high => deaktivieren
}

/////////////////////////////// kommando funktion ///////////////////////////////
void ssd0323::send_command(unsigned char theCommand)
{
	GPIO_WriteBit(GPIOE,GPIO_Pin_15,Bit_RESET);//fastWriteLow(SPI_CD); //turn to command mode
	senden_spi(theCommand);
	GPIO_WriteBit(GPIOE,GPIO_Pin_15,Bit_SET);//fastWriteHigh(SPI_CD); //turn to data mode
}

/////////////////////////////// data funktion ///////////////////////////////
void ssd0323::send_char(unsigned char zeichen)
{
	GPIO_WriteBit(GPIOE,GPIO_Pin_15,Bit_SET);//fastWriteHigh(SPI_CD); //turn to data mode
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
			if(int(round((x2-(y2-i)*m/100)))%2==0){ // links
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
			a = font[stelle];/* 8 px */

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
		a = font[stelle];/* 8 px */

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
		a = font[stelle];/* 8 px */
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
		a = font[stelle];/* 8 px */
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
		a = font[stelle];/* 8 px */
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
//void ssd0323::string(short font,const char *p_string,unsigned char spalte, unsigned char zeile, unsigned char back, unsigned char color,unsigned char offset){
//	char *char_buffer;
//	char_buffer = (char*) malloc (23);
//	if (!(char_buffer==NULL)) memset(char_buffer,'\0',sizeof(char_buffer)/sizeof(char_buffer[0]));
//
//	int i=0;
//	while(i<22){
//		char_buffer[i]=pgm_read_byte(p_string++);
//		if(char_buffer[i]=='\0')
//			break;
//		i++;
//	}
//	string(font,char_buffer,spalte,zeile,back,color,offset);
//	free(char_buffer);
//}
//
///////////////////////////////// progmem string to display 5x8 ///////////////////////////////
//void ssd0323::string(short font,const char *p_string,unsigned char spalte, unsigned char zeile){
//	string(font,p_string,spalte,zeile,0,15,0);
//}

/////////////////////////////// string to display 5x8 ///////////////////////////////
void ssd0323::string(const char str[],unsigned char spalte, unsigned char zeile){
	//string(Speedo.default_font,str,spalte,zeile,0,15,0);
	string(VISITOR_SMALL_1X_FONT,str,spalte,zeile,0,15,0);

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


void ssd0323::clear_screen(){
    // if a animation was started, reinit_display will be set to true.
    // after that, we have to reinit it
//    if(reinit_display){
//        init(phase,ref);
//        reinit_display=false;
//    }

    send_command(0x15);
    send_command(0x00);
    send_command(0x7F);
    send_command(0x75);
    send_command(0x00);
    send_command(0x3F);
    for (int a=0;a<(128*32);a++){
        send_char(0x00);
    }
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

	//	pinMode(SPI_CD, OUTPUT); 	7
	//	pinMode(SPI_RESET, OUTPUT);	9
	//	pinMode(SPI_DATA, OUTPUT);	11
	//	pinMode(SPI_CLK,OUTPUT);	13
	//	pinMode(SPI_CS,OUTPUT);		15
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* Configure PD12, 13, 14 and PD15 in output pushpull mode */
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	_delay_ms(10); // TODO to long

	GPIO_WriteBit(GPIOE,GPIO_Pin_7,Bit_SET);//	fastWriteHigh(SPI_CS);
	GPIO_WriteBit(GPIOE,GPIO_Pin_15,Bit_RESET);//fastWriteLow(SPI_CD);
	GPIO_WriteBit(GPIOE,GPIO_Pin_11,Bit_RESET);//fastWriteLow(SPI_DATA);
	GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_RESET);//fastWriteLow(SPI_CLK);
	GPIO_WriteBit(GPIOE,GPIO_Pin_9,Bit_SET);//fastWriteHigh(SPI_RESET);
	_delay_ms(10);
	// reset
	GPIO_WriteBit(GPIOE,GPIO_Pin_9,Bit_RESET);//fastWriteLow(SPI_RESET);
	_delay_ms(10);
	GPIO_WriteBit(GPIOE,GPIO_Pin_9,Bit_SET);//fastWriteHigh(SPI_RESET);
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


void ssd0323::draw_gps(unsigned char x,unsigned char y, unsigned char sats){
    send_command(0x15);
    send_command(x);
    send_command(x+3);
    send_command(0x75);
    send_command(y);
    send_command(y+7);
    char xy[2][4];
    xy[0][0]=0x00;
    xy[1][0]=0x00;
    xy[0][1]=0x00;
    xy[1][1]=0x00;
    xy[0][2]=0x00;
    xy[1][2]=0x00;
    xy[0][3]=0x00;
    xy[1][3]=0x00;

    // 4 symbols:
    // 0 GPS => cross
    // 3 GPS => empty (indicating minimal signal)
    // 4 GPS => single bow (indicating poor signal)
    // 5-X GPS => dual bow (indicating strong signal)

    if(sats==0){
        xy[0][0]=0x0F;        //     X X
        xy[1][0]=0x0F;        //      X
        //xy[0][1]=0x00;    //     X X
        xy[1][1]=0xF0;        //
        xy[0][2]=0x0F;        //
        xy[1][2]=0x0F;        //
        //xy[0][3]=0x00;    //
        //xy[1][3]=0x00;    //
    } else if(sats==4){
        //xy[0][0]=0x00;    //
        //xy[1][0]=0x00;    //
        //xy[0][1]=0x00;    //    X
        //xy[1][1]=0x00;    //     X
        xy[0][2]=0xF0;        //
        //xy[1][2]=0x00;    //
        xy[0][3]=0x0F;        //
        //xy[1][3]=0x00;    //
    } else if(sats>4){
        xy[0][0]=0xFF;        //    XX
        //xy[1][0]=0x00;    //      X
        //xy[0][1]=0x00;    //    X  X
        xy[1][1]=0xF0;        //     X X
        xy[0][2]=0xF0;        //
        xy[1][2]=0x0F;        //
        xy[0][3]=0x0F;        //
        xy[1][3]=0x0F;        //
    };


    send_char(0x00);  send_char(0x00);  send_char(xy[0][0]);  send_char(xy[1][0]);    //
    send_char(0x00);  send_char(0x00);  send_char(xy[0][1]);  send_char(xy[1][1]);    //
    send_char(0x0F);  send_char(0x00);  send_char(xy[0][2]);  send_char(xy[1][2]);    // x
    send_char(0xF0);  send_char(0xF0);  send_char(xy[0][3]);  send_char(xy[1][3]);    //x x
    send_char(0xF0);  send_char(0x0F);  send_char(0x00);        send_char(0x00);        //x  x
    send_char(0xF0);  send_char(0x00);  send_char(0xF0);      send_char(0x00);        //x   x
    send_char(0x0F);  send_char(0x00);  send_char(0x0F);      send_char(0x00);        // x   x
    send_char(0x00);  send_char(0xFF);  send_char(0xF0);      send_char(0x00);        //  xxx
};

void ssd0323::draw_oil(unsigned char x,unsigned char y){
    send_command(0x15);
    send_command(x);
    send_command(x+9);
    send_command(0x75);
    send_command(y);
    send_command(y+7);
    send_char(0xFF); send_char(0xF0); send_char(0x0F); send_char(0xFF); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00);
    send_char(0xF0); send_char(0x0F); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0xFF); send_char(0xF0); send_char(0x00);
    send_char(0xF0); send_char(0x0F); send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0x0F); send_char(0xF0); send_char(0xF0); send_char(0x00); send_char(0x00);
    send_char(0x0F); send_char(0x0F); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x0F); send_char(0x00); send_char(0xF0); send_char(0x00);
    send_char(0x00); send_char(0xFF); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x0F); send_char(0xFF); send_char(0x00);
    send_char(0x00); send_char(0x0F); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0x00); send_char(0xFF); send_char(0x70); send_char(0xF0);
    send_char(0x00); send_char(0x00); send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0xF0); send_char(0x00); send_char(0xFF); send_char(0x77); send_char(0xF0);
    send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0xFF); send_char(0x00);
};

void ssd0323::draw_water(unsigned char x,unsigned char y){
    send_command(0x15);
    send_command(x);
    send_command(x+9);
    send_command(0x75);
    send_command(y);
    send_command(y+7);
    send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);  send_char(0x00);  send_char(0x00);
    send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0x00);  send_char(0x0F);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);
    send_char(0x00);  send_char(0x77);  send_char(0xF0);  send_char(0x0F);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);
    send_char(0x00);  send_char(0x77);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);  send_char(0x00);
    send_char(0x00);  send_char(0x77);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
    send_char(0x00);  send_char(0x77);  send_char(0xF0);  send_char(0x0F);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);  send_char(0x0F);  send_char(0xF0);  send_char(0x00);
    send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x0F);  send_char(0xF0);  send_char(0x00);
    send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);
};

void ssd0323::draw_air(unsigned char x,unsigned char y){
    send_command(0x15);
    send_command(x);
    send_command(x+2);
    send_command(0x75);
    send_command(y);
    send_command(y+7);
    send_char(0x00);  send_char(0xF0);  send_char(0x00);
    send_char(0x00);  send_char(0xFF);  send_char(0xF0);
    send_char(0x00);  send_char(0xF0);  send_char(0x00);
    send_char(0x00);  send_char(0xFF);  send_char(0xF0);
    send_char(0x00);  send_char(0xF0);  send_char(0x00);
    send_char(0x0F);  send_char(0xFF);  send_char(0x00);
    send_char(0xFF);  send_char(0xFF);  send_char(0xF0);
    send_char(0x0F);  send_char(0xFF);  send_char(0x00);
};

void ssd0323::draw_fuel(unsigned char x,unsigned char y){
    send_command(0x15);
    send_command(x);
    send_command(x+3);
    send_command(0x75);
    send_command(y);
    send_command(y+6);
    send_char(0x0F);  send_char(0xFF);  send_char(0xF0);  send_char(0x0F);
    send_char(0x0F);  send_char(0x00);  send_char(0xF0);  send_char(0xF0);
    send_char(0x0F);  send_char(0x00);  send_char(0xF0);  send_char(0xF0);
    send_char(0x0F);  send_char(0xFF);  send_char(0xF0);  send_char(0xF0);
    send_char(0x0F);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);
    send_char(0x0F);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
    send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);
};

void ssd0323::draw_clock(unsigned char x,unsigned char y){
    send_command(0x15);
    send_command(x);
    send_command(x+3);
    send_command(0x75);
    send_command(y);
    send_command(y+7);
    send_char(0x00);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
    send_char(0x0F);  send_char(0x0F);  send_char(0x0F);  send_char(0x00);
    send_char(0xF0);  send_char(0x0F);  send_char(0x00);  send_char(0xF0);
    send_char(0xF0);  send_char(0x0F);  send_char(0xF0);  send_char(0xF0);
    send_char(0xF0);  send_char(0x00);  send_char(0x00);  send_char(0xF0);
    send_char(0x0F);  send_char(0x00);  send_char(0x0F);  send_char(0x00);
    send_char(0x00);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
};

void ssd0323::draw_blitzer(unsigned char x,unsigned char y){
    send_command(0x15);
    send_command(x);
    send_command(x+4);
    send_command(0x75);
    send_command(y);
    send_command(y+14);
    send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0xF0);
    send_char(0xFF); send_char(0xF0); send_char(0x00); send_char(0xFF); send_char(0xF0);
    send_char(0xFF); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0xF0);
    send_char(0xFF); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0xF0);
    send_char(0xFF); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0xF0);
    send_char(0xFF); send_char(0xF0); send_char(0x00); send_char(0xFF); send_char(0xF0);
    send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0xF0);
    send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00);
    send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00);
    send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00);
    send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00);
    send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00);
    send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00);
    send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00);
}

//void ssd0323::draw_arrow(int arrow, int spalte, int zeile){
//    OLED.string(pSpeedo->default_font,"   ",spalte,zeile,0,0,0); // 3x3 Buchstaben, 6x8
//    OLED.string(pSpeedo->default_font,"   ",spalte,zeile+1,0,0,0); // 18x24
//    OLED.string(pSpeedo->default_font,"   ",spalte,zeile+2,0,0,0);
//    if((pSensors->get_speed(false)>=3) && (arrow>-1)){ // nur den Winkel "goto" einzeichnen wenn die Geschwindigkeit >=3 kmh ist
//        // TODO: hier einen schickeren Winkel zeichnen...
//        // ein Strich im winkel pSensors->m_gps->winkel mit der länge l nach oben und unten
//        int m_x=(spalte+1)*6+3,m_y=(zeile+1)*8+4,l=8; // m_x=spalte*8+2, m_y=zeile*8+4 // spalte=1, zeile=2
//        int sp_x=round(m_x+sin(arrow*2*M_PI/360)*l);
//        int sp_y=round(m_y-cos(arrow*2*M_PI/360)*l);
//        int end_x=round(m_x-sin(arrow*2*M_PI/360)*l);
//        int end_y=round(m_y+cos(arrow*2*M_PI/360)*l);
//        OLED.line(sp_x,sp_y,end_x,end_y,10); // neuen Winkel malen
//
//        // spitze "einfärben" Spitze: round(m_x+sin(pSensors->m_gps->winkel)*l),round(m_y-cos(pSensors->m_gps->winkel)*l)
//        int pf_length=8;
//        int temp_winkel=(arrow+30)%360;
//        end_x=round(sp_x-sin(temp_winkel*2*M_PI/360)*pf_length);
//        end_y=round(sp_y+cos(temp_winkel*2*M_PI/360)*pf_length);
//        OLED.line(sp_x,sp_y,end_x,end_y,15); // neuen Winkel malen
//
//        temp_winkel=(arrow+330)%360; // was das gleiche ist wie -30° und check obs unter null ist
//        end_x=round(sp_x-sin(temp_winkel*2*M_PI/360)*pf_length);
//        end_y=round(sp_y+cos(temp_winkel*2*M_PI/360)*pf_length);
//        OLED.line(sp_x,sp_y,end_x,end_y,15); // neuen Winkel malen
//
//        //l++;
//        //OLED.set2pixels(round(m_x+sin(pSensors->m_gps->winkel*2*M_PI/360)*l),round(m_y-cos(pSensors->m_gps->winkel*2*M_PI/360)*l),15,15);
//        //OLED.set2pixels(round(m_x+sin(pSensors->m_gps->winkel*2*M_PI/360)*l),round(m_y-cos(pSensors->m_gps->winkel*2*M_PI/360)*l+1),15,15);
//    };
//    // schreibe winkel
//}

// grafix has to be stored at /gfx/
//int ssd0323::sd2ssd(char filename[10],int frame){
//    send_command(0x15);
//    send_command(0x00);
//    send_command(0x7F);
//    send_command(0x75);
//    send_command(0x00);
//    send_command(0x3F);
//
//    SdFile root;
//    SdFile file;
//    SdFile subdir;
//    root.openRoot(&pSD->volume);
//
//    if(!subdir.open(&root, "gfx", O_READ))    {  return 1; };
//    if(!file.open(&subdir, filename, O_READ)) {  return 2; };
//    unsigned long frame_seeker=(unsigned long)frame*64*64;
//    if(!file.seekSet(frame_seeker))              {  return 3; }; // ein bild ist 64*64 Byte groß, da wir 64 lines zu je 64*2*4 Bit Breite haben
//
//    uint8_t buf[65];
//    //int n;
//    //while ((n = file.read(buf, sizeof(byte)*64)) > 0) {
//    for (int zeile=0;    (file.read(buf, sizeof(byte)*64)>0)    && zeile<64;    zeile++ ) {
//        for(int j=0;j<64;j++){
//            send_char(buf[j]);
//        };
//    };
//    file.close();
//    subdir.close();
//    root.close();
//    return 0;
//};

void ssd0323::string_centered(const char* text, uint8_t line){
    string_centered(text,line,false);
}

//void ssd0323::string_centered(const char* text, uint8_t line, bool inverted){
//    if(strlen(text)>20){
//        return;
//    };
//
//    uint16_t front_color=0x0f;
//    uint16_t back_color=0x00;
//    uint16_t start_pos=0;
//    uint16_t length_of_char=22;
//
//    if(inverted){
//        front_color=0x00;
//        back_color=0x0f;
//        if(strlen(text)<=17){ // 6*2pixel == 2 chars. 21 Chars - 2*2 = 17chars max
//            length_of_char=17;
//            start_pos=2;
//            OLED.highlight_bar(0,line*8,128,8);
//        } else {
//            OLED.filled_rect(0,line*8,128,8,0x0f);
//        }
//    }
//    char text_char[length_of_char]; // full display width +1
//    strcpy(text_char,text);
//    Menu.center_me(text_char,length_of_char); // full display width
//    OLED.string(pSpeedo->default_font,text_char,start_pos,line,back_color,front_color,0);
//}

// aufruf parameter:
void ssd0323::show_storry(const char* storry,const char* title){
    show_storry(storry,title,0x00);
}

void ssd0323::show_storry(const char* storry,const char* title, uint8_t type){
    char storry_char[strlen(storry)+1];
    char title_char[strlen(title)+1];

    strcpy(storry_char,storry);
    strcpy(title_char,title);

    show_storry(storry_char,strlen(storry),title_char,strlen(title),type);
}

void ssd0323::show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length){
    show_storry(storry,storry_length,title, title_length, 0x00);
}

//void ssd0323::show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length, uint8_t type){
//    // show title
//    OLED.clear_screen();
//    OLED.highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
//    OLED.string(pSpeedo->default_font,title,2,0,DISP_BRIGHTNESS,0,0);
//
//    // Generate borders
//    unsigned int fill_line=0; // actual line
//    unsigned int char_in_line=0; // count char in this line
//    int von[4]={0,0,0,0}; // nutzen um zu bestimmten wo wir den "\0" setzen
//    int bis[4]={0,0,0,0}; // nutzen um zu bestimmten wo wir den "\0" setzen
//
//    for(unsigned int i=0; i<storry_length;i++){
//        if(char_in_line>10){ // mindestesn mal 10 zeichen aufnehmen
//            if((storry[i]==' ' || char_in_line==21) && (fill_line+1<(sizeof(bis)/sizeof(bis[0])))){
//                bis[fill_line]=i; // damit haben wir das ende dieser Zeile gefunden
//                fill_line++;
//                von[fill_line]=i;// und den anfang der nächsten, wobei das noch nicht save ost
//                if(storry[i]==' '){ // wir haben hier ein freizeichen, hätten wir sinnvoll weitergucken können?
//                    von[fill_line]++; // das freizeichen brauchen wie eh nicht mehr
//                    //haben ein volles wort, mal sehen ob ncoh was geht
//                    //aktuell sind char_in_line chars im puffer
//                    int onemoreword=0; // wieviele chars gehts denn weiter, falls sinnvoll
//                    for(unsigned int k=char_in_line; k<21; k++){
//                        if(storry[i+k-char_in_line]==' ') onemoreword=k-char_in_line;
//                    }
//                    if(onemoreword>0){
//                        // es scheint sinnig noch onemoreword buchstaben zu nutzen
//                        i+=onemoreword;
//                        von[fill_line]+=onemoreword;
//                        bis[fill_line-1]+=onemoreword;
//                    }
//                }
//                char_in_line=-1; // wird gleich inc -> dann sind wir fertig und der counter bei 0
//            }
//        }
//        char_in_line++;
//    };
//    if(bis[fill_line]==0){ // letztes array ding
//        bis[fill_line]=storry_length;
//    };
//    // we got the borders
//
//    // draw to display
//    for(unsigned int i=0; i<4; i++){ // nur 4 zeilen
//        if(von[i]!=bis[i]){
//            // reserve buffer
//            char *buffer2;
//            buffer2 = (char*) malloc (22);
//            if (buffer2==NULL) Serial.puts_ln(USART1,("Malloc failed"));
//            else memset(buffer2,'\0',sizeof(buffer2)/sizeof(buffer2[0]));
//
//            int k=0;
//            for(int j=von[i]; j<bis[i] && k<22; j++){
//                if(!(i==0 && storry[j]=='#')){ // in der ersten zeile, das erste "#" an stelle 0 überlesen
//                    buffer2[k]=storry[j];
//                    k++;
//                };
//            };
//            buffer2[k]='\0';
//            OLED.string(pSpeedo->default_font,buffer2,0,i+2,0,DISP_BRIGHTNESS,0);
//
//            //delete buffer
//            free(buffer2);
//        };
//    }
//
//    //
//    unsigned int current_state=pMenu->state;
//    unsigned long current_timestamp=Millis.get();
//
//    // set buttons if needed
//    if(type>=DIALOG_NO_YES){
//        pMenu->set_buttons(true,false,false,true);
//    }
//
//    if(type==DIALOG_NO_YES){
//        OLED.string(pSpeedo->default_font,("\x7E back        next \x7F"),0,7);
//    }
//    else if(type==DIALOG_GO_RIGHT_200MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<200){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_right(true);
//        }
//    } else if(type==DIALOG_GO_LEFT_200MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<200){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_left(true);
//        }
//    } else if(type==DIALOG_GO_RIGHT_500MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<500){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_right(true);
//        }
//    } else if(type==DIALOG_GO_LEFT_500MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<500){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_left(true);
//        }
//    } else if(type==DIALOG_GO_RIGHT_1000MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<1000){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_right(true);
//        }
//    } else if(type==DIALOG_GO_LEFT_1000MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<1000){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_left(true);
//        }
//    } else if(type==DIALOG_GO_RIGHT_2000MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<2000){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_right(true);
//        }
//    } else if(type==DIALOG_GO_LEFT_2000MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<2000){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_left(true);
//        }
//    } else if(type==DIALOG_GO_RIGHT_5000MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<5000){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_right(true);
//        }
//    } else if(type==DIALOG_GO_LEFT_5000MS){
//        while(current_state==pMenu->state && (Millis.get()-current_timestamp)<5000){
//            _delay_ms(1);
//        }
//        if(current_state==pMenu->state){
//            pMenu->go_left(true);
//        }
//    } else if(type==DIALOG_SHOW_500MS){
//        _delay_ms(500);
//    }
//}


//void ssd0323::show_animation(unsigned char *command){
//    //pSensors->m_reset->set_deactive(false,false); // just deaktivate ist by now, dont save it, nowhere. this makes it possible to restore the IO state by var
//    int spacer[4];
//    int pointer_to_spacer=0;
//    // alles nach den kommata durchsuchen
//    for(int i=0;i<40;i++){
//        if(command[i]==','){
//            spacer[pointer_to_spacer]=i;
//            pointer_to_spacer++;
//        };
//        if(command[i]=='\0'){
//            spacer[pointer_to_spacer]=i;
//            i=99;
//        };
//        // overrun protection
//        if(pointer_to_spacer>3){
//            pointer_to_spacer=3;
//            i=99;
//        };
//    };
//
//    // prüfen obs passt
//    if(pointer_to_spacer!=3){
//        Serial.puts(USART1,("falsche anzahl an kommata"));
//    } else {
//        // start zahl suchen
//        int start=0;
//        for(int i=spacer[0]+1;i<spacer[1];i++){
//            start=start*10+(command[i]-48);
//        };
//
//        // endzahl
//        int ende=0;
//        for(int i=spacer[1]+1;i<spacer[2];i++){
//            ende=ende*10+(command[i]-48);
//        };
//
//        // wartezeit
//        int warte=0;
//        for(int i=spacer[2]+1;i<spacer[3];i++){
//            warte=warte*10+(command[i]-48);
//        };
//
//        // den dateinamen
//        char filename[spacer[0]]; // spacer[0]=16
//        for(int i=0; i<spacer[0]; i++){ // 0 .. 15
//            filename[i]=command[i];
//        };
//        filename[spacer[0]]='\0';
//
//        //    Serial.print("start: ");
//        //    Serial.print(start);
//        //    Serial.print(" ende ");
//        //    Serial.print(ende);
//        //    Serial.print(" wartezeit dazwischen ");
//        //    Serial.print(warte);
//
//        // tasten bis auf "nachlinks" aus, menu_state eine eben nach rechts schieben
//        pMenu->button_oben_valid=true;
//        pMenu->button_links_valid=true;
//        pMenu->button_rechts_valid=true;
//        pMenu->button_unten_valid=true;
//        unsigned int state_before=pMenu->state%100;
//
//        // switch to image mode and remeber that
//        init(0x38,0x38);
//        reinit_display=true;
//
//
//        // animation starten
//        unsigned long timestamp=Millis.get();
//        for(int i=start; i<ende; i++){ // check i<ende OR i<=ende (like it was) TODO
//            // try to show the image, and print every error
//            if(sd2ssd(filename,i)>0){
//                i=ende;
//                clear_screen();
//                string(pSpeedo->default_font,"Open file failed",3,2,0,DISP_BRIGHTNESS,0);
//            }
//
//            // wait, the better way: First remember actual timestamp, then check connection and reset, than check button,
//            // if everything is fine, check if there is additional time to wait
//
//            // timestamp
//            timestamp=Millis.get();
//            // serial
//            if(Serial.available()>0){ // an sonsten gern
//                if(Serial.read()==MESSAGE_START){
//                    pFilemanager_v2->parse_command();
//                };
//            };
//            // hardware buttons
//            if((pMenu->state%100)!=state_before) { i=ende; } // indicates that a hardware key was pressed // TODO not(!) working
//
//            // pReset toggle
//            pSensors->m_reset->toggle();
//
//            // additional wait?
//            while(timestamp+warte>Millis.get()){
//                _delay_ms(1); // check ob das hier viel aendert
//            }; //wait
//        }; // for frames
//    }; // enough spacer (',')
//};


//void ssd0323::disp_waiting(int position,unsigned char spalte,unsigned char zeile){
//    switch (position%4){
//    case 0:
//        string(pSpeedo->default_font," |",spalte,zeile,0,15,0);
//        break;
//    case 1:
//        string(pSpeedo->default_font," /",spalte,zeile,0,15,0);
//        break;
//    case 2:
//        string(pSpeedo->default_font," -",spalte,zeile,0,15,0);
//        break;
//    case 3:
//        string(pSpeedo->default_font," \\",spalte,zeile,0,15,0);
//        break;
//    };
//};

//int ssd0323::animation(int ani_nr){
//    SdFile dir_handle;
//    SdFile file_handle;
//    int status=0;
//    int item=0; // filecounter
//    int ani_found=0;
//    unsigned long size=0;
//    bool sgf_file_found=false;
//    unsigned char filename[24]; // reused for show_animation string
//    // prepare handle (reuse of filename) for dir /GFX/ (length:5)
//    filename[0]='/';
//    filename[1]='G';
//    filename[2]='F';
//    filename[3]='X';
//    filename[4]='/';
//    filename[5]='\0';
//
//    if(pFilemanager_v2->get_file_handle(filename,filename,&file_handle,&dir_handle,O_READ)<0){
//        status=-1; // DIR problem
//    } else {
//        filename[0]='\0';// remove from cache to avoid reuse
//        while(!sgf_file_found && status>=0){ // file not found AND no problem
//            // status: 0=EOF, 1=FILE, 2=FOLDER
//            if(dir_handle.lsJKWNext(filename,item,&size)){ // <- returns the filename of the file nr "item"
//                // get length
//                unsigned char filename_length=0;
//                while(filename[filename_length]!='\0' && filename_length<8+3+1+1){ // Filename is 8+3 (+1='.' +1='\0')
//                    filename_length++;
//                }
//                // e.g. deyes.sgf: filename_length is 9
//                if(filename[filename_length-3]=='S' && filename[filename_length-2]=='G' && filename[filename_length-1]=='F'){ // correct suffix
//                    if(ani_found==ani_nr){ // check if this is the animiation nr we were looking for
//                        // file info
//                        char msgBuffer[22];
//                        clear_screen();
//                        sprintf(msgBuffer, "File #%i",ani_nr);
//                        pMenu->center_me(msgBuffer,21);
//                        string(pSpeedo->default_font,msgBuffer,0,2,0,DISP_BRIGHTNESS,0);
//                        sprintf(msgBuffer, "Showing %s",filename);
//                        pMenu->center_me(msgBuffer,21);
//                        string(pSpeedo->default_font,msgBuffer,0,3,0,DISP_BRIGHTNESS,0);
//                        sprintf(msgBuffer, "%i Frames",(int)(size>>12));
//                        pMenu->center_me(msgBuffer,21);
//                        string(pSpeedo->default_font,msgBuffer,0,4,0,DISP_BRIGHTNESS,0);
//                        _delay_ms(2000);
//                        // go show it
//                        int delay=20;
//                        if((size>>12)<10) { delay = 100; }
//                        sprintf((char *)filename,"%s,%i,%i,%i",(char *)filename,0,(int)(size>>12),delay); // filename, start at frame 0, end at last frame (size/4096), 20ms interframespacing
//                        show_animation(filename);
//                        sgf_file_found=true; // ACK
//                    } else {
//                        item++; // yes, its a animation but not the one we are looking for
//                        ani_found++;
//                    }
//                } else { // not a SGF file
//                    item++; // look for next file
//                }
//            } else {
//                status=-2; // EOF
//            }
//        }
//        file_handle.close();
//        dir_handle.close();
//    }
//    return status;
//}


void ssd0323::start_up(){
    Serial.puts(USART1,("Display init ... "));
    if(phase==0 && ref==0){
        phase=0xA8;
        ref=0x28;
    }
    init(phase,ref);
    reinit_display=false;
    clear_screen();

    // if hardware version is above 6, there is an emergency V_BACKUP_driver for the GPS
    // if bat is empty, inform user
    // DUE TO BAT MEASUREMENT BUG IN HARDWARE NOT AVAILABLE
    //    if(pConfig->get_hw_version()>7 && pSensors->m_voltage->bat_empty && false){
    //        OLED.string(pSpeedo->default_font,("!! WARNING !!"),4,0,0,DISP_BRIGHTNESS,0);
    //        OLED.string(pSpeedo->default_font,("GPS Bat empty"),3,2,0,DISP_BRIGHTNESS,0);
    //        OLED.string(pSpeedo->default_font,("cold fixing now"),2,3,0,DISP_BRIGHTNESS,0);
    //        OLED.string(pSpeedo->default_font,("this will take a min"),0,4,0,DISP_BRIGHTNESS,0);
    //        OLED.string(pSpeedo->default_font,("!! WARNING !!"),4,7,0,DISP_BRIGHTNESS,0);
    //        _delay_ms(10000);
    //    }

    // if storage init failed notify as well
//    if(pSD->sd_failed){
//        OLED.string(pSpeedo->default_font,("!! WARNING !!"),4,0,0,DISP_BRIGHTNESS,0);
//        OLED.string(pSpeedo->default_font,("SD access failed"),1,2,0,DISP_BRIGHTNESS,0);
//        OLED.string(pSpeedo->default_font,("check the Card!"),0,3,0,DISP_BRIGHTNESS,0);
//        OLED.string(pSpeedo->default_font,("!! WARNING !!"),4,7,0,DISP_BRIGHTNESS,0);
//        _delay_ms(5000);
//    }

    // wenn die Karte ok ist können wir startup zeigen, wenn wir wollen
    // see if its a clock startup or a regular startup
//    if(pSpeedo->startup_by_ignition){
//        show_animation(startup);
//    } else {
//        clear_screen();
//    }
    Serial.puts_ln(USART1,("Done"));
};


/////////////////////////////// create object funktion ///////////////////////////////
ssd0323::ssd0323(){
}


