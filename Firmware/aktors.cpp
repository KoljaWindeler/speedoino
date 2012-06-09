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

Speedo_aktors::Speedo_aktors(){
	m_stepper=new speedo_stepper();
};

Speedo_aktors::~Speedo_aktors(){
};

void Speedo_aktors::init(){
	/* vier werte paare:
	 * 1. Außen -> ändert sich öfters mal, wird mit std werten geladen
	 * 2. Innen -> ändert sich eigentlich nie
	 * 3. Flasher -> Farbe zu der übergeblendet wird wenn die schaltdrehzahl erreicht wird
	 * 4. out_base -> Farbkopie von Außen
	 */
	// innen
	pinMode(RGB_IN_R,OUTPUT);
	pinMode(RGB_IN_G,OUTPUT);
	pinMode(RGB_IN_B,OUTPUT);
	// außen
	pinMode(RGB_OUT_R,OUTPUT);
	pinMode(RGB_OUT_G,OUTPUT);
	pinMode(RGB_OUT_B,OUTPUT);


	dimm_step=0;
	dimm_steps=0;
	dimm_state=999;

	set_rgb_out(0,0,0); // dimm ich in main ein .. hmm
	//	if(led_mode==0){
	//		dimm_rgb_to(static_color.r,static_color.g,static_color.b,256,0);
	//	} else if(led_mode==1){
	//		dimm_rgb_to(kmh_start_color.r,kmh_start_color.g,kmh_start_color.b,256,0);
	//	} else if(led_mode==2){
	//		dimm_rgb_to(dz_start_color.r,dz_start_color.g,dz_start_color.b,256,0);
	//	} else if(led_mode==3){
	//		dimm_rgb_to(oil_start_color.r,oil_start_color.g,oil_start_color.b,256,0);
	//	}
	update_outer_leds(true);

	// stepper drehen
	m_stepper->init();
};

void Speedo_aktors::set_rgb_in(int r,int g,int b){
	set_rgb_in(r,g,b,1);
}

void Speedo_aktors::set_rgb_in(int r,int g,int b,int save){
	if(r>255) r=255; else if(r<0) r=0;
	if(g>255) g=255; else if(g<0) g=0;
	if(b>255) b=255; else if(b<0) b=0;

	analogWrite(RGB_IN_R,255-r);
	analogWrite(RGB_IN_G,255-g);
	analogWrite(RGB_IN_B,255-b);

	if(save){
		RGB.inner.r.actual=r;
		RGB.inner.g.actual=g;
		RGB.inner.b.actual=b;
	};
};

void Speedo_aktors::set_rgb_out(int r,int g,int b){
	set_rgb_out(r,g,b,1);
}

void Speedo_aktors::set_rgb_out(int r,int g,int b,int save){
	if(r>255) r=255; else if(r<0) r=0;
	if(g>255) g=255; else if(g<0) g=0;
	if(b>255) b=255; else if(b<0) b=0;

	analogWrite(RGB_OUT_R,r);
	analogWrite(RGB_OUT_G,g);
	analogWrite(RGB_OUT_B,b);

	if(save){
		RGB.outer.r.actual=r;
		RGB.outer.g.actual=g;
		RGB.outer.b.actual=b;
	};
};
/* 10ms pro schritt -> 0 to 256 = 2,56 sec */
void Speedo_aktors::dimm_rgb_to(int r,int g,int b,int max_dimm_steps, int set_in_out){
	if(r>255) r=255;
	if(g>255) g=255;
	if(b>255) b=255;

	if(set_in_out==1){
		RGB.inner.r.to=r;
		RGB.inner.g.to=g;
		RGB.inner.b.to=b;
		RGB.inner.r.from=RGB.inner.r.actual;
		RGB.inner.g.from=RGB.inner.g.actual;
		RGB.inner.b.from=RGB.inner.b.actual;
	} else {
		RGB.outer.r.to=r;
		RGB.outer.g.to=g;
		RGB.outer.b.to=b;
		RGB.outer.r.from=RGB.outer.r.actual;
		RGB.outer.g.from=RGB.outer.g.actual;
		RGB.outer.b.from=RGB.outer.b.actual;
	}
	dimm_steps=max_dimm_steps;
	in_out=set_in_out;
	dimm_step=0; // wir beginnen bei 0 bis max dimm_steps


	TCCR3A = 0x00;
	/* ich denke mal alles umzuschalten => 256 schritte in 2,56 sec
	 * wäre doch okay, also 256 Timer Schritte in 0,01 sec
	 * ein schritt demnach in 0,01/256=0,000039062
	 * das sind in der welt von 16 mhz => 1/16000000*x=0,000039062
	 * x=625 schritte .. das ist doof vorteiler kann 256 oder 1024 sein
	 * 16000000/256/256 = 244 hz -> nur ne gute sekunde
	 * 16000000/1024/256 = 61 hz -> mehr als 4 sekunde, dazu dann mit
	 * 256/2-1 vorladen = 127 -> zack 2 sek
	 */
	// prescale 1024
	TCCR3B |= (1<<CS32) | (1<<CS30);
	TCCR3B &= ~(1<<CS31);
	// 98 vorladen, 157 schritte
	TCNT3H = 0xFF;
	TCNT3L = 0x62;
	// interrupts aktivieren
	TIMSK3 |= (1<<TOIE3);
	TIFR3  |= (1<<TOV3);
};

void Speedo_aktors::timer_overflow(){
	if(in_out==1){
		int r=int(round((float(int(RGB.inner.r.to)-int(RGB.inner.r.from))*dimm_step)/dimm_steps))+int(RGB.inner.r.from);
		int g=int(round((float(int(RGB.inner.g.to)-int(RGB.inner.g.from))*dimm_step)/dimm_steps))+int(RGB.inner.g.from);
		int b=int(round((float(int(RGB.inner.b.to)-int(RGB.inner.b.from))*dimm_step)/dimm_steps))+int(RGB.inner.b.from);
		set_rgb_in(r,g,b);
	} else {
		int r=int(round((float(int(RGB.outer.r.to)-int(RGB.outer.r.from))*dimm_step)/dimm_steps))+int(RGB.outer.r.from);
		int g=int(round((float(int(RGB.outer.g.to)-int(RGB.outer.g.from))*dimm_step)/dimm_steps))+int(RGB.outer.g.from);
		int b=int(round((float(int(RGB.outer.b.to)-int(RGB.outer.b.from))*dimm_step)/dimm_steps))+int(RGB.outer.b.from);
		set_rgb_out(r,g,b);
	}

	// sind am ende ? wenn ja, timer aus und to werte in from speichern,
	// wenn nicht dimm_steps hochzählen und timer wieder vorladen damit er mit 10 ms
	// läuft
	if(dimm_step<dimm_steps){
		dimm_step++;
		// 127 vorladen
		TCNT3H = 0xFF;
		TCNT3L = 0x62;
	} else {
		// timer aus
		TCCR3B &= ~((1<<CS32) | (1<<CS31) | (1<<CS30));
		// aktueller und from wert sind damit der to wert
		if(in_out==1){
			RGB.inner.r.from=RGB.inner.r.to;
			RGB.inner.g.from=RGB.inner.g.to;
			RGB.inner.b.from=RGB.inner.b.to;
		} else {
			RGB.outer.r.from=RGB.outer.r.to;
			RGB.outer.g.from=RGB.outer.g.to;
			RGB.outer.b.from=RGB.outer.b.to;
		};
	};
};

bool Speedo_aktors::dimm_available(){
	if(dimm_step==dimm_steps)
		return true;
	else
		return false;
};

ISR(TIMER3_OVF_vect){
	pAktors->timer_overflow();
}

/* defines:
 *
 */
#define FLASH_COLOR_REACHED 0
#define DIMM_TO_FLASH_COLOR 1
#define STATIC_COLOR_REACHED 2
#define DIMM_TO_STATIC_COLOR 3
#define DIMM_TO_DARK 4
#define MIN_REACHED 5
#define MAX_REACHED 6



int Speedo_aktors::update_outer_leds(bool dimm){
	if(floor(pMenu->state/100)==65 || floor(pMenu->state/1000)==65){
		return -1;
	};

	if(floor(pMenu->state/100)==66 || floor(pMenu->state/1000)==66 || floor(pMenu->state/10000)==66){
		return -2;
	};

	////////// SHIFT FLASH ////////////////
	if(pSensors->m_dz->exact>unsigned(pSensors->m_dz->blitz_dz) && pSensors->m_dz->blitz_en){
		if(dimm_state==FLASH_COLOR_REACHED){
			return 0;
		} else if(dimm_state==DIMM_TO_FLASH_COLOR){
			if(pAktors->dimm_available()){
				dimm_state=FLASH_COLOR_REACHED;
			}
		} else if(dimm_state==DIMM_TO_DARK){
			if(pAktors->dimm_available()){
				pAktors->dimm_rgb_to(pAktors->dz_flasher.r,pAktors->dz_flasher.g,pAktors->dz_flasher.b,15,0); // 25*10ms = 250 ms
				dimm_state=DIMM_TO_FLASH_COLOR;
			}
		} else {
			if(pAktors->dimm_available()){
				pAktors->dimm_rgb_to(5,5,5,15,0); // 25*10ms = 250 ms
				dimm_state=DIMM_TO_DARK;
			}
		}
		///////// STATIC ////////////////
	} else if(led_mode==0){ // simple static color
		if(dimm_state==STATIC_COLOR_REACHED){
			return 0;
		} else if(dimm_state==DIMM_TO_STATIC_COLOR){
			if(pAktors->dimm_available()){
				dimm_state=STATIC_COLOR_REACHED;
			};
		} else {
			pAktors->dimm_rgb_to(pAktors->static_color.r,pAktors->static_color.g,pAktors->static_color.b,15,0); // 25*10ms = 250 ms
			dimm_state=DIMM_TO_STATIC_COLOR;
		};

		///////// kmh ////////////////
	} else if(led_mode==1){ //follow kmh
		if(pSensors->m_speed->getSpeed()>=kmh_max_value){
			if(dimm_state!=MAX_REACHED){
				pAktors->set_rgb_out(int(kmh_end_color.r),int(kmh_end_color.g),int(kmh_end_color.b));
			};
			dimm_state=MAX_REACHED;
		} else if(pSensors->m_speed->getSpeed()<=kmh_min_value){
			if(dimm_state!=MIN_REACHED){
				if(dimm)
					dimm_rgb_to(kmh_start_color.r,kmh_start_color.g,kmh_start_color.b,256,0);
				else {
					pAktors->set_rgb_out(kmh_start_color.r,kmh_start_color.g,kmh_start_color.b);
					dimm_state=MIN_REACHED;
				};
			};

		} else {
			int temp_r,temp_g,temp_b,kmh_differ;
			kmh_differ=kmh_max_value-kmh_min_value;
			temp_r = float(kmh_end_color.r-kmh_start_color.r)/float(kmh_differ)*(pSensors->m_speed->getSpeed()-kmh_min_value)+kmh_start_color.r;
			temp_g = float(kmh_end_color.g-kmh_start_color.g)/float(kmh_differ)*(pSensors->m_speed->getSpeed()-kmh_min_value)+kmh_start_color.g;
			temp_b = float(kmh_end_color.b-kmh_start_color.b)/float(kmh_differ)*(pSensors->m_speed->getSpeed()-kmh_min_value)+kmh_start_color.b;
			if(dimm)
				dimm_rgb_to(temp_r,temp_g,temp_b,256,0);
			else
				pAktors->set_rgb_out(temp_r,temp_g,temp_b);
		};

		///////// DZ ////////////////
	} else if(led_mode==2){ //follow dz
		if(pSensors->m_dz->exact>=unsigned(dz_max_value)){
			if(dimm_state!=MAX_REACHED){
				pAktors->set_rgb_out(int(dz_end_color.r),int(dz_end_color.g),int(dz_end_color.b));
			}
			dimm_state=MAX_REACHED;
		} else if(pSensors->m_dz->exact<=unsigned(dz_min_value)){
			if(dimm_state!=MIN_REACHED){
				if(dimm)
					dimm_rgb_to(dz_start_color.r,dz_start_color.g,dz_start_color.b,256,0);
				else {
					pAktors->set_rgb_out(dz_start_color.r,dz_start_color.g,dz_start_color.b);
					dimm_state=MIN_REACHED;
				};
			};
			dimm_state=MIN_REACHED;
		} else {
			int temp_r,temp_g,temp_b,dz_differ;
			dz_differ=dz_max_value-dz_min_value;
			temp_r = float(dz_end_color.r-dz_start_color.r)/float(dz_differ)*(pSensors->m_dz->exact-dz_min_value)+dz_start_color.r;
			temp_g = float(dz_end_color.g-dz_start_color.g)/float(dz_differ)*(pSensors->m_dz->exact-dz_min_value)+dz_start_color.g;
			temp_b = float(dz_end_color.b-dz_start_color.b)/float(dz_differ)*(pSensors->m_dz->exact-dz_min_value)+dz_start_color.b;
			if(dimm)
				dimm_rgb_to(temp_r,temp_g,temp_b,256,0);
			else
				pAktors->set_rgb_out(temp_r,temp_g,temp_b);
		};

	}

	///////// OIL ////////////////
	else if(led_mode==3){ //follow oil temp // werte sind in out_start_color->from
		if(pSensors->m_temperature->get_oil_temp()>=oil_max_value){
			if(dimm_state!=MAX_REACHED){
				pAktors->set_rgb_out(int(oil_end_color.r),int(oil_end_color.g),int(oil_end_color.b));
			};
			dimm_state=MAX_REACHED;
		} else if(pSensors->m_temperature->get_oil_temp()<=oil_min_value){
			if(dimm_state!=MIN_REACHED){
				if(dimm)
					dimm_rgb_to(oil_start_color.r,oil_start_color.g,oil_start_color.b,256,0);
				else {
					pAktors->set_rgb_out(oil_start_color.r,oil_start_color.g,oil_start_color.b);
					dimm_state=MIN_REACHED;
				};
			};
		} else {
			int temp_r,temp_g,temp_b,oil_differ;
			oil_differ=oil_max_value-oil_min_value;
			temp_r = float(oil_end_color.r-oil_start_color.r)/float(oil_differ)*(pSensors->m_temperature->get_oil_temp()-oil_min_value)+oil_start_color.r;
			temp_g = float(oil_end_color.g-oil_start_color.g)/float(oil_differ)*(pSensors->m_temperature->get_oil_temp()-oil_min_value)+oil_start_color.g;
			temp_b = float(oil_end_color.b-oil_start_color.b)/float(oil_differ)*(pSensors->m_temperature->get_oil_temp()-oil_min_value)+oil_start_color.b;
			if(dimm)
				dimm_rgb_to(temp_r,temp_g,temp_b,256,0);
			else
				pAktors->set_rgb_out(temp_r,temp_g,temp_b);
		};
		return 0;
	}

	///////// Water ////////////////
	else if(led_mode==4){ //follow water temp // werte sind in out_start_color->from
		if(pSensors->m_temperature->get_water_temp()>=water_max_value){
			if(dimm_state!=MAX_REACHED){
				pAktors->set_rgb_out(int(water_end_color.r),int(water_end_color.g),int(water_end_color.b));
			};
			dimm_state=MAX_REACHED;
		} else if(pSensors->m_temperature->get_water_temp()<=water_min_value){
			if(dimm_state!=MIN_REACHED){
				if(dimm)
					dimm_rgb_to(water_start_color.r,water_start_color.g,water_start_color.b,256,0);
				else {
					pAktors->set_rgb_out(water_start_color.r,water_start_color.g,water_start_color.b);
					dimm_state=MIN_REACHED;
				};
			};
		} else {
			int temp_r,temp_g,temp_b,water_differ;
			water_differ=water_max_value-water_min_value;
			temp_r = float(water_end_color.r-water_start_color.r)/float(water_differ)*(pSensors->m_temperature->get_water_temp()-water_min_value)+water_start_color.r;
			temp_g = float(water_end_color.g-water_start_color.g)/float(water_differ)*(pSensors->m_temperature->get_water_temp()-water_min_value)+water_start_color.g;
			temp_b = float(water_end_color.b-water_start_color.b)/float(water_differ)*(pSensors->m_temperature->get_water_temp()-water_min_value)+water_start_color.b;
			if(dimm)
				dimm_rgb_to(temp_r,temp_g,temp_b,256,0);
			else
				pAktors->set_rgb_out(temp_r,temp_g,temp_b);
		};
		return 0;
	}
	return 1;
};

int Speedo_aktors::set_bt_pin(){
	pOLED->clear_screen();
	char at_commands[22];
	bool connection_established=true;

	// check connection
	pOLED->string_P(pSpeedo->default_font,PSTR("Checking connection"),0,0);

	sprintf(at_commands,"ATQ0%c",0x0D);
	if(ask_bt(&at_commands[0])!=0){														// fehler aufgetreten
		connection_established=false;
		pOLED->string_P(pSpeedo->default_font,PSTR("FAILED"),14,1);								// hat nicht geklappt
		Serial.end();																	// setzte neue serielle Geschwindigkeit
		_delay_ms(500);
		pOLED->string_P(pSpeedo->default_font,PSTR("TRYING 19200 BAUD"),0,2);					// hat nicht geklappt
		Serial.begin(19200);
		_delay_ms(2000);
		if(ask_bt(&at_commands[0])==0){
			pOLED->string_P(pSpeedo->default_font,PSTR("OK"),14,3);
			_delay_ms(500);
			pOLED->string_P(pSpeedo->default_font,PSTR("BASIC SETUP"),0,4);
			sprintf(at_commands,"ATE0%c",0x0D);
			if(ask_bt(&at_commands[0])==0){
				sprintf(at_commands,"ATN=SPEEDOINO%c",0x0D);
				if(ask_bt(&at_commands[0])==0){
					sprintf(at_commands,"ATL5%c",0x0D);
					ask_bt(&at_commands[0]);											// fire && forget
					pOLED->string_P(pSpeedo->default_font,PSTR("OK"),14,5);				// wird schon auf anderer geschwindigkeit geantwortet, können wir hier nicht testen
					Serial.end();														// setzte neue serielle Geschwindigkeit
					pOLED->string_P(pSpeedo->default_font,PSTR("RETRYING"),0,6);		// hat nicht geklappt
					Serial.begin(115200);
					_delay_ms(2000);
					pOLED->clear_screen();
					pOLED->string_P(pSpeedo->default_font,PSTR("Checking connection"),0,0);
					sprintf(at_commands,"AT%c",0x0D);									// gleich neu testen
					if(ask_bt(&at_commands[0])==0){
						connection_established=true;
						pOLED->string_P(pSpeedo->default_font,PSTR("OK"),14,1);
					};
				}
			}
		} else {
			pOLED->string_P(pSpeedo->default_font,PSTR("FAILED"),14,1);
			_delay_ms(10000);
			return -9;
		}
	}

	if(connection_established){
		pOLED->filled_rect(0,8,128,56,0x00); // clear the lower lines
		pOLED->string_P(pSpeedo->default_font,PSTR("OK"),14,1);
		pOLED->string_P(pSpeedo->default_font,PSTR("Activating responses"),0,2);
		sprintf(at_commands,"ATQ0%c",0x0D); // schaltet result codes ein				// jetzt richtig
		if(ask_bt(&at_commands[0])==0){
			pOLED->string_P(pSpeedo->default_font,PSTR("OK"),14,3);
			pOLED->string_P(pSpeedo->default_font,PSTR("Setting PIN Code"),0,4);
			sprintf(at_commands,"ATP=%04i%c",bt_pin,13);

			if(ask_bt(&at_commands[0])==0){
				pOLED->string_P(pSpeedo->default_font,PSTR("OK"),14,5);
				pOLED->string_P(pSpeedo->default_font,PSTR("Deactivating response"),0,6);
				sprintf(at_commands,"ATQ0%c",13); // schaltet result codes ein

				if(ask_bt(&at_commands[0])==0){
					pOLED->string_P(pSpeedo->default_font,PSTR("OK"),14,7);
					_delay_ms(10000);
					return 0;
				}
			}
		}
	} else {
		ask_bt(&at_commands[0]);
		pOLED->string_P(pSpeedo->default_font,PSTR("failed, hmmm"),0,1);
		_delay_ms(5000);
	}
	_delay_ms(10000);
	return -2;
}

int Speedo_aktors::ask_bt(char *command){
	for(int looper=0;looper<3;looper++){
		Serial.flush();
		Serial.print(command);

		// A T \r \n O K \r \n = 8

		//warte bis der Buffer nicht voller wird
		int j=-99;
		while(j!=Serial.available()){
			j=Serial.available();
			_delay_ms(200);
		};

		if(j>=4){								// bei 8 hat man "O" "K" "0x1D" "0x1A"
			char answere[3]={0,0,0};
			for(int i=0;i<j; i++){
				if(i==j-4){						// I=0
					answere[0]=Serial.read();	// answere[0]=O
				} else if(i==j-3){				// I=1
					answere[1]=Serial.read();	// answere[1]=K
				} else {
					Serial.read();
				}
			};

			if(answere[0]=='O' && answere[1]=='K'){
				return 0;
			}
		}
	};
	return -1;
}
