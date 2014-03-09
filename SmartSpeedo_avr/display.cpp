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
#include <ssd0323.h>

speedo_disp::speedo_disp(void){
	phase=0;
	ref=0;
}

speedo_disp::~speedo_disp(){
};


void speedo_disp::draw_gps(unsigned char x,unsigned char y, unsigned char sats){
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
		xy[0][0]=0x0F;		//	 X X
		xy[1][0]=0x0F;		//	  X
		//xy[0][1]=0x00;	//	 X X
		xy[1][1]=0xF0;		//
		xy[0][2]=0x0F;		//
		xy[1][2]=0x0F;		//
		//xy[0][3]=0x00;	//
		//xy[1][3]=0x00;	//
	} else if(sats==4){
		//xy[0][0]=0x00;	//
		//xy[1][0]=0x00;	//
		//xy[0][1]=0x00;	//	X
		//xy[1][1]=0x00;	//	 X
		xy[0][2]=0xF0;		//
		//xy[1][2]=0x00;	//
		xy[0][3]=0x0F;		//
		//xy[1][3]=0x00;	//
	} else if(sats>4){
		xy[0][0]=0xFF;		//	XX
		//xy[1][0]=0x00;	//	  X
		//xy[0][1]=0x00;	//	X  X
		xy[1][1]=0xF0;		//	 X X
		xy[0][2]=0xF0;		//
		xy[1][2]=0x0F;		//
		xy[0][3]=0x0F;		//
		xy[1][3]=0x0F;		//
	};


	send_char(0x00);  send_char(0x00);  send_char(xy[0][0]);  send_char(xy[1][0]);	//
	send_char(0x00);  send_char(0x00);  send_char(xy[0][1]);  send_char(xy[1][1]);	//
	send_char(0x0F);  send_char(0x00);  send_char(xy[0][2]);  send_char(xy[1][2]);	// x
	send_char(0xF0);  send_char(0xF0);  send_char(xy[0][3]);  send_char(xy[1][3]);	//x x
	send_char(0xF0);  send_char(0x0F);  send_char(0x00);  	  send_char(0x00);		//x  x
	send_char(0xF0);  send_char(0x00);  send_char(0xF0);	  send_char(0x00);		//x   x
	send_char(0x0F);  send_char(0x00);  send_char(0x0F);	  send_char(0x00);		// x   x
	send_char(0x00);  send_char(0xFF);  send_char(0xF0);	  send_char(0x00);		//  xxx
};

void speedo_disp::draw_oil(unsigned char x,unsigned char y){
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

void speedo_disp::draw_water(unsigned char x,unsigned char y){
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

void speedo_disp::draw_air(unsigned char x,unsigned char y){
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

void speedo_disp::draw_fuel(unsigned char x,unsigned char y){
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

void speedo_disp::draw_clock(unsigned char x,unsigned char y){
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

void speedo_disp::draw_blitzer(unsigned char x,unsigned char y){
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

void speedo_disp::draw_arrow(int arrow, int spalte, int zeile){
	pOLED->string(pSpeedo->default_font,"   ",spalte,zeile,0,0,0); // 3x3 Buchstaben, 6x8
	pOLED->string(pSpeedo->default_font,"   ",spalte,zeile+1,0,0,0); // 18x24
	pOLED->string(pSpeedo->default_font,"   ",spalte,zeile+2,0,0,0);
	if((pSensors->get_speed(false)>=3) && (arrow>-1)){ // nur den Winkel "goto" einzeichnen wenn die Geschwindigkeit >=3 kmh ist
		// TODO: hier einen schickeren Winkel zeichnen...
		// ein Strich im winkel pSensors->m_gps->winkel mit der länge l nach oben und unten
		int m_x=(spalte+1)*6+3,m_y=(zeile+1)*8+4,l=8; // m_x=spalte*8+2, m_y=zeile*8+4 // spalte=1, zeile=2
		int sp_x=round(m_x+sin(arrow*2*M_PI/360)*l);
		int sp_y=round(m_y-cos(arrow*2*M_PI/360)*l);
		int end_x=round(m_x-sin(arrow*2*M_PI/360)*l);
		int end_y=round(m_y+cos(arrow*2*M_PI/360)*l);
		pOLED->line(sp_x,sp_y,end_x,end_y,10); // neuen Winkel malen

		// spitze "einfärben" Spitze: round(m_x+sin(pSensors->m_gps->winkel)*l),round(m_y-cos(pSensors->m_gps->winkel)*l)
		int pf_length=8;
		int temp_winkel=(arrow+30)%360;
		end_x=round(sp_x-sin(temp_winkel*2*M_PI/360)*pf_length);
		end_y=round(sp_y+cos(temp_winkel*2*M_PI/360)*pf_length);
		pOLED->line(sp_x,sp_y,end_x,end_y,15); // neuen Winkel malen

		temp_winkel=(arrow+330)%360; // was das gleiche ist wie -30° und check obs unter null ist
		end_x=round(sp_x-sin(temp_winkel*2*M_PI/360)*pf_length);
		end_y=round(sp_y+cos(temp_winkel*2*M_PI/360)*pf_length);
		pOLED->line(sp_x,sp_y,end_x,end_y,15); // neuen Winkel malen

		//l++;
		//pOLED->set2pixels(round(m_x+sin(pSensors->m_gps->winkel*2*M_PI/360)*l),round(m_y-cos(pSensors->m_gps->winkel*2*M_PI/360)*l),15,15);
		//pOLED->set2pixels(round(m_x+sin(pSensors->m_gps->winkel*2*M_PI/360)*l),round(m_y-cos(pSensors->m_gps->winkel*2*M_PI/360)*l+1),15,15);
	};
	// schreibe winkel
}

// grafix has to be stored at /gfx/
int speedo_disp::sd2ssd(char filename[10],int frame){
	send_command(0x15);
	send_command(0x00);
	send_command(0x7F);
	send_command(0x75);
	send_command(0x00);
	send_command(0x3F);

	SdFile root;
	SdFile file;
	SdFile subdir;
	root.openRoot(&pSD->volume);

	if(!subdir.open(&root, "gfx", O_READ))    {  return 1; };
	if(!file.open(&subdir, filename, O_READ)) {  return 2; };
	unsigned long frame_seeker=(unsigned long)frame*64*64;
	if(!file.seekSet(frame_seeker))			  {  return 3; }; // ein bild ist 64*64 Byte groß, da wir 64 lines zu je 64*2*4 Bit Breite haben

	uint8_t buf[65];
	//int n;
	//while ((n = file.read(buf, sizeof(byte)*64)) > 0) {
	for (int zeile=0;	(file.read(buf, sizeof(byte)*64)>0)	&& zeile<64;	zeile++ ) {
		for(int j=0;j<64;j++){
			send_char(buf[j]);
		};
	};
	file.close();
	subdir.close();
	root.close();
	return 0;
};

void speedo_disp::string_P_centered(const char* text, uint8_t line){
	string_P_centered(text,line,false);
}

void speedo_disp::string_P_centered(const char* text, uint8_t line, bool inverted){
	if(strlen_P(text)>20){
		return;
	};

	uint16_t front_color=0x0f;
	uint16_t back_color=0x00;
	uint16_t start_pos=0;
	uint16_t length_of_char=22;

	if(inverted){
		front_color=0x00;
		back_color=0x0f;
		if(strlen_P(text)<=17){ // 6*2pixel == 2 chars. 21 Chars - 2*2 = 17chars max
			length_of_char=17;
			start_pos=2;
			pOLED->highlight_bar(0,line*8,128,8);
		} else {
			pOLED->filled_rect(0,line*8,128,8,0x0f);
		}
	}
	char text_char[length_of_char]; // full display width +1
	strcpy_P(text_char,text);
	pMenu->center_me(text_char,length_of_char); // full display width
	pOLED->string(pSpeedo->default_font,text_char,start_pos,line,back_color,front_color,0);
}

// aufruf parameter:
void speedo_disp::show_storry(const char* storry,const char* title){
	show_storry(storry,title,0x00);
}

void speedo_disp::show_storry(const char* storry,const char* title, uint8_t type){
	char storry_char[strlen_P(storry)+1];
	char title_char[strlen_P(title)+1];

	strcpy_P(storry_char,storry);
	strcpy_P(title_char,title);

	show_storry(storry_char,strlen_P(storry),title_char,strlen_P(title),type);
}

void speedo_disp::show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length){
	show_storry(storry,storry_length,title, title_length, 0x00);
}

void speedo_disp::show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length, uint8_t type){
	// show title
	pOLED->clear_screen();
	pOLED->highlight_bar(0,0,128,8); // mit hintergrundfarbe nen kasten malen
	pOLED->string(pSpeedo->default_font,title,2,0,DISP_BRIGHTNESS,0,0);

	// Generate borders
	unsigned int fill_line=0; // actual line
	unsigned int char_in_line=0; // count char in this line
	int von[4]={0,0,0,0}; // nutzen um zu bestimmten wo wir den "\0" setzen
	int bis[4]={0,0,0,0}; // nutzen um zu bestimmten wo wir den "\0" setzen

	for(unsigned int i=0; i<storry_length;i++){
		if(char_in_line>10){ // mindestesn mal 10 zeichen aufnehmen
			if((storry[i]==' ' || char_in_line==21) && (fill_line+1<(sizeof(bis)/sizeof(bis[0])))){
				bis[fill_line]=i; // damit haben wir das ende dieser Zeile gefunden
				fill_line++;
				von[fill_line]=i;// und den anfang der nächsten, wobei das noch nicht save ost
				if(storry[i]==' '){ // wir haben hier ein freizeichen, hätten wir sinnvoll weitergucken können?
					von[fill_line]++; // das freizeichen brauchen wie eh nicht mehr
					//haben ein volles wort, mal sehen ob ncoh was geht
					//aktuell sind char_in_line chars im puffer
					int onemoreword=0; // wieviele chars gehts denn weiter, falls sinnvoll
					for(unsigned int k=char_in_line; k<21; k++){
						if(storry[i+k-char_in_line]==' ') onemoreword=k-char_in_line;
					}
					if(onemoreword>0){
						// es scheint sinnig noch onemoreword buchstaben zu nutzen
						i+=onemoreword;
						von[fill_line]+=onemoreword;
						bis[fill_line-1]+=onemoreword;
					}
				}
				char_in_line=-1; // wird gleich inc -> dann sind wir fertig und der counter bei 0
			}
		}
		char_in_line++;
	};
	if(bis[fill_line]==0){ // letztes array ding
		bis[fill_line]=storry_length;
	};
	// we got the borders

	// draw to display
	for(unsigned int i=0; i<4; i++){ // nur 4 zeilen
		if(von[i]!=bis[i]){
			// reserve buffer
			char *buffer2;
			buffer2 = (char*) malloc (22);
			if (buffer2==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
			else memset(buffer2,'\0',sizeof(buffer2)/sizeof(buffer2[0]));

			int k=0;
			for(int j=von[i]; j<bis[i] && k<22; j++){
				if(!(i==0 && storry[j]=='#')){ // in der ersten zeile, das erste "#" an stelle 0 überlesen
					buffer2[k]=storry[j];
					k++;
				};
			};
			buffer2[k]='\0';
			pOLED->string(pSpeedo->default_font,buffer2,0,i+2,0,DISP_BRIGHTNESS,0);

			//delete buffer
			free(buffer2);
		};
	}

	//
	unsigned int current_state=pMenu->state;
	unsigned long current_timestamp=millis();

	// set buttons if needed
	if(type>=DIALOG_NO_YES){
		pMenu->set_buttons(true,false,false,true);
	}

	if(type==DIALOG_NO_YES){
		pOLED->string_P(pSpeedo->default_font,PSTR("\x7E back        next \x7F"),0,7);
	}
	else if(type==DIALOG_GO_RIGHT_200MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<200){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_200MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<200){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_500MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<500){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_500MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<500){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_1000MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<1000){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_1000MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<1000){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_2000MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<2000){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_2000MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<2000){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_5000MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<5000){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_5000MS){
		while(current_state==pMenu->state && (millis()-current_timestamp)<5000){
			_delay_ms(1);
		}
		if(current_state==pMenu->state){
			pMenu->go_left(true);
		}
	} else if(type==DIALOG_SHOW_500MS){
		_delay_ms(500);
	}
}


void speedo_disp::show_animation(unsigned char *command){
	//pSensors->m_reset->set_deactive(false,false); // just deaktivate ist by now, dont save it, nowhere. this makes it possible to restore the IO state by var
	int spacer[4];
	int pointer_to_spacer=0;
	// alles nach den kommata durchsuchen
	for(int i=0;i<40;i++){
		if(command[i]==','){
			spacer[pointer_to_spacer]=i;
			pointer_to_spacer++;
		};
		if(command[i]=='\0'){
			spacer[pointer_to_spacer]=i;
			i=99;
		};
		// overrun protection
		if(pointer_to_spacer>3){
			pointer_to_spacer=3;
			i=99;
		};
	};

	// prüfen obs passt
	if(pointer_to_spacer!=3){
		pDebug->sprintp(PSTR("falsche anzahl an kommata"));
	} else {
		// start zahl suchen
		int start=0;
		for(int i=spacer[0]+1;i<spacer[1];i++){
			start=start*10+(command[i]-48);
		};

		// endzahl
		int ende=0;
		for(int i=spacer[1]+1;i<spacer[2];i++){
			ende=ende*10+(command[i]-48);
		};

		// wartezeit
		int warte=0;
		for(int i=spacer[2]+1;i<spacer[3];i++){
			warte=warte*10+(command[i]-48);
		};

		// den dateinamen
		char filename[spacer[0]]; // spacer[0]=16
		for(int i=0; i<spacer[0]; i++){ // 0 .. 15
			filename[i]=command[i];
		};
		filename[spacer[0]]='\0';

		//    Serial.print("start: ");
		//    Serial.print(start);
		//    Serial.print(" ende ");
		//    Serial.print(ende);
		//    Serial.print(" wartezeit dazwischen ");
		//    Serial.print(warte);

		// tasten bis auf "nachlinks" aus, menu_state eine eben nach rechts schieben
		pMenu->button_oben_valid=true;
		pMenu->button_links_valid=true;
		pMenu->button_rechts_valid=true;
		pMenu->button_unten_valid=true;
		unsigned int state_before=pMenu->state%100;

		// switch to image mode and remeber that
		init(0x38,0x38);
		reinit_display=true;


		// animation starten
		unsigned long timestamp=millis();
		for(int i=start; i<ende; i++){ // check i<ende OR i<=ende (like it was) TODO
			// try to show the image, and print every error
			if(sd2ssd(filename,i)>0){
				i=ende;
				clear_screen();
				string(pSpeedo->default_font,"Open file failed",3,2,0,DISP_BRIGHTNESS,0);
			}

			// wait, the better way: First remember actual timestamp, then check connection and reset, than check button,
			// if everything is fine, check if there is additional time to wait

			// timestamp
			timestamp=millis();
			// serial
			if(Serial.available()>0){ // an sonsten gern
				if(Serial.read()==MESSAGE_START){
					pFilemanager_v2->parse_command();
				};
			};
			// hardware buttons
			if((pMenu->state%100)!=state_before) { i=ende; } // indicates that a hardware key was pressed // TODO not(!) working

			// pReset toggle
			pSensors->m_reset->toggle();

			// additional wait?
			while(timestamp+warte>millis()){
				_delay_ms(1); // check ob das hier viel aendert
			}; //wait
		}; // for frames
	}; // enough spacer (',')
};


void speedo_disp::disp_waiting(int position,unsigned char spalte,unsigned char zeile){
	switch (position%4){
	case 0:
		string(pSpeedo->default_font," |",spalte,zeile,0,15,0);
		break;
	case 1:
		string(pSpeedo->default_font," /",spalte,zeile,0,15,0);
		break;
	case 2:
		string(pSpeedo->default_font," -",spalte,zeile,0,15,0);
		break;
	case 3:
		string(pSpeedo->default_font," \\",spalte,zeile,0,15,0);
		break;
	};
};

int speedo_disp::animation(int ani_nr){
	SdFile dir_handle;
	SdFile file_handle;
	int status=0;
	int item=0; // filecounter
	int ani_found=0;
	unsigned long size=0;
	bool sgf_file_found=false;
	unsigned char filename[24]; // reused for show_animation string
	// prepare handle (reuse of filename) for dir /GFX/ (length:5)
	filename[0]='/';
	filename[1]='G';
	filename[2]='F';
	filename[3]='X';
	filename[4]='/';
	filename[5]='\0';

	if(pFilemanager_v2->get_file_handle(filename,filename,&file_handle,&dir_handle,O_READ)<0){
		status=-1; // DIR problem
	} else {
		filename[0]='\0';// remove from cache to avoid reuse
		while(!sgf_file_found && status>=0){ // file not found AND no problem
			// status: 0=EOF, 1=FILE, 2=FOLDER
			if(dir_handle.lsJKWNext(filename,item,&size)){ // <- returns the filename of the file nr "item"
				// get length
				unsigned char filename_length=0;
				while(filename[filename_length]!='\0' && filename_length<8+3+1+1){ // Filename is 8+3 (+1='.' +1='\0')
					filename_length++;
				}
				// e.g. deyes.sgf: filename_length is 9
				if(filename[filename_length-3]=='S' && filename[filename_length-2]=='G' && filename[filename_length-1]=='F'){ // correct suffix
					if(ani_found==ani_nr){ // check if this is the animiation nr we were looking for
						// file info
						char msgBuffer[22];
						clear_screen();
						sprintf(msgBuffer, "File #%i",ani_nr);
						pMenu->center_me(msgBuffer,21);
						string(pSpeedo->default_font,msgBuffer,0,2,0,DISP_BRIGHTNESS,0);
						sprintf(msgBuffer, "Showing %s",filename);
						pMenu->center_me(msgBuffer,21);
						string(pSpeedo->default_font,msgBuffer,0,3,0,DISP_BRIGHTNESS,0);
						sprintf(msgBuffer, "%i Frames",(int)(size>>12));
						pMenu->center_me(msgBuffer,21);
						string(pSpeedo->default_font,msgBuffer,0,4,0,DISP_BRIGHTNESS,0);
						_delay_ms(2000);
						// go show it
						int delay=20;
						if((size>>12)<10) { delay = 100; }
						sprintf((char *)filename,"%s,%i,%i,%i",(char *)filename,0,(int)(size>>12),delay); // filename, start at frame 0, end at last frame (size/4096), 20ms interframespacing
						show_animation(filename);
						sgf_file_found=true; // ACK
					} else {
						item++; // yes, its a animation but not the one we are looking for
						ani_found++;
					}
				} else { // not a SGF file
					item++; // look for next file
				}
			} else {
				status=-2; // EOF
			}
		}
		file_handle.close();
		dir_handle.close();
	}
	return status;
}


void speedo_disp::init_speedo(){
	pDebug->sprintp(PSTR("Display init ... "));
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
	//	if(pConfig->get_hw_version()>7 && pSensors->m_voltage->bat_empty && false){
	//		pOLED->string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,0,0,DISP_BRIGHTNESS,0);
	//		pOLED->string_P(pSpeedo->default_font,PSTR("GPS Bat empty"),3,2,0,DISP_BRIGHTNESS,0);
	//		pOLED->string_P(pSpeedo->default_font,PSTR("cold fixing now"),2,3,0,DISP_BRIGHTNESS,0);
	//		pOLED->string_P(pSpeedo->default_font,PSTR("this will take a min"),0,4,0,DISP_BRIGHTNESS,0);
	//		pOLED->string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,7,0,DISP_BRIGHTNESS,0);
	//		_delay_ms(10000);
	//	}

	// if storage init failed notify as well
	if(pSD->sd_failed){
		pOLED->string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("SD access failed"),1,2,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("check the Card!"),0,3,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,7,0,DISP_BRIGHTNESS,0);
		_delay_ms(5000);
	}

	// wenn die Karte ok ist können wir startup zeigen, wenn wir wollen
	// see if its a clock startup or a regular startup
	if(pSpeedo->startup_by_ignition){
		show_animation(startup);
	} else {
		clear_screen();
	}
	pDebug->sprintlnp(PSTR("Done"));
};

/////////////////////////////// leeren ///////////////////////////////
void speedo_disp::clear_screen(){
	// if a animation was started, reinit_display will be set to true.
	// after that, we have to reinit it
	if(reinit_display){
		init(phase,ref);
		reinit_display=false;
	}

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

