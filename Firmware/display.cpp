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
}

speedo_disp::~speedo_disp(){
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
	if((pSensors->m_speed->getSpeed()>=3) && (arrow>-1)){ // nur den Winkel "goto" einzeichnen wenn die Geschwindigkeit >=3 kmh ist
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

// aufruf parameter:

void speedo_disp::show_storry(char storry[],unsigned int storry_length,char title[],unsigned int title_length){
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
}


void speedo_disp::show_animation(const char command[]){
	pSensors->m_reset->set_deactive(false,false); // just deaktivate ist by now, dont save it, nowhere. this makes it possible to restore the IO state by var
	int spacer[4];
	int pointer_to_spacer=0;
	// alles nach den kommata durchsuchen
	for(int i=0;i<40;i++){
		if(char(command[i])==','){
			spacer[pointer_to_spacer]=i;
			pointer_to_spacer++;
		};
		if(char(command[i])=='\0'){
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
		pMenu->button_rechts_valid=false;
		pMenu->button_unten_valid=true;
		boolean killed=false;

		// animation starten
		char *char_buffer;
		char_buffer = (char*) malloc (22);
		if (char_buffer==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
		else memset(char_buffer,'\0',22);

		for(int i=start; i<=ende; i++){
			if(sd2ssd(filename,i)>0){
				i=ende;
				killed=true;
				clear_screen();
				string(pSpeedo->default_font,"Open file failed",3,2,0,DISP_BRIGHTNESS,0);
				string(pSpeedo->default_font,char_buffer,4,3,0,DISP_BRIGHTNESS,0);
			}
			delay(warte); // check ob das hier viel aendert
			if(pMenu->button_test(true,true))  {  i=ende; killed=true; pMenu->display(); } // muss man dann nicht vorher den state hochsetzen?!
		};

		free(char_buffer);

	}
	pSensors->m_reset->restore();
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

void speedo_disp::animation(int a){
	switch(a){
	case 1: //////////////////// simpsons ///////////////////
		show_animation("SIMP.TXT,0,19,15"); // 200 -> 15
		break;
	case 2: //////////////////// girl ///////////////////
		show_animation("BLO.TXT,0,37,15");
		break;
	case 3:
		show_animation("DJ.TXT,0,0,0");
		break;
	case 4:
		show_animation("JTM.TXT,0,37,15");
		break;
	case 5:
		show_animation("LG2.TXT,0,1,400");
		break;
	case 6: //////////////////// nur "honda" "hornet" ///////////////////
		show_animation("SIMP.TXT,0,19,15");
		break;
	case 7: //////////////////// hornet - biene ///////////////////
		show_animation("TM.TXT,0,6,15");
		show_animation("TM.TXT,0,6,15");
		show_animation("TM.TXT,0,6,15");
		break;
	case 8: //////////////////// Titten-Maus ///////////////////
		show_animation("SIMP.TXT,0,19,15");
		break;
	case 9: //////////////////// hornet-logo ///////////////////
		//
		break;
	};
};


void speedo_disp::init_speedo(){
	pinMode(29,INPUT); // interessiert keine sau, aber da der pin jetzt extern auf masse gezogen wird sollte der hier nicht besser kein Pegel treiben
	init(phase,ref);
	clear_screen();
	if(pSD->sd_failed){ // ausgabe wenns schief geht die Karte zu laden
		clear_screen();
		string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,0,0,DISP_BRIGHTNESS,0);
		string_P(pSpeedo->default_font,PSTR("SD access failed."),1,2,0,DISP_BRIGHTNESS,0);
		string_P(pSpeedo->default_font,PSTR("running on backup"),0,3,0,DISP_BRIGHTNESS,0);
		string_P(pSpeedo->default_font,PSTR("values"),6,4,0,DISP_BRIGHTNESS,0);
		string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,7,0,DISP_BRIGHTNESS,0);

		pDebug->sprintlnp(PSTR("!! WARNING !!"));
		pDebug->sprintlnp(PSTR("SD access failed"));
		pDebug->sprintlnp(PSTR("!! WARNING !!"));
		delay(3000);
		clear_screen();
	} else if(WELCOME){ // wenn die Karte ok ist können wir startup zeigen, wenn wir wollen
		show_animation(startup);
		Serial.println("welcome done");
	} else {
		clear_screen();
	};
};
