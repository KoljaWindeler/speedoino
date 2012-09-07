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


void Speedo_aktors::run_reset(){
	pOLED->clear_screen();
	pOLED->string_P(pSpeedo->default_font,PSTR("Running Update"),3,3);
	// pin as output
	//DDRD |= (1<<ATM328RESETPIN);
	DDRL |= (1<<ATM328RESETPIN);
	// set low -> low active
	//PORTD &= ~(1<<ATM328RESETPIN);
	PORTL &= ~(1<<ATM328RESETPIN);
	_delay_ms(50);
	//PORTD |= (1<<ATM328RESETPIN);
	PORTL |= (1<<ATM328RESETPIN);
	// set high, as pull up
	//DDRD |= (1<<ATM328RESETPIN);
	DDRL |= (1<<ATM328RESETPIN);
	//PORTD |= (1<<ATM328RESETPIN);
	PORTL |= (1<<ATM328RESETPIN);

	// tunnel mode
	unsigned long timeout=millis();
	while(millis()-timeout>5000){
		timeout=millis();
		while(Serial.available()>0){
			Serial3.print(Serial.read(),BYTE);
		}
		while(Serial3.available()>0){
			Serial.print(Serial3.read(),BYTE);
		}
	}
}

void Speedo_aktors::clear_vars(){
	kmh_min_value=0;
	kmh_max_value=0;

	dz_min_value=0;
	dz_max_value=0;

	oil_min_value=0;
	oil_max_value=0;

	water_min_value=0;
	water_max_value=0;

	led_mode=1;

	bt_pin=1234;
};

bool Speedo_aktors::check_vars(){
	if(kmh_min_value+kmh_max_value+dz_min_value+dz_max_value+oil_min_value+oil_max_value+water_min_value+water_max_value==0){
		static_color.r=0;
		static_color.g=0;
		static_color.b=255;

		dz_flasher.r=255;
		dz_flasher.g=0;
		dz_flasher.b=0;

		kmh_start_color.r=0;
		kmh_start_color.g=0;
		kmh_start_color.b=255;

		kmh_end_color.r=255;
		kmh_end_color.g=255;
		kmh_end_color.b=0;

		dz_start_color.r=0;
		dz_start_color.g=0;
		dz_start_color.b=255;

		dz_end_color.r=255;
		dz_end_color.g=0;
		dz_end_color.b=255;

		oil_start_color.r=255;
		oil_start_color.g=255;
		oil_start_color.b=0;

		oil_end_color.r=0;
		oil_end_color.g=0;
		oil_end_color.b=255;

		water_start_color.r=255;
		water_start_color.g=255;
		water_start_color.b=0;

		water_end_color.r=0;
		water_end_color.g=0;
		water_end_color.b=255;

		kmh_min_value=20;
		kmh_max_value=50;

		dz_min_value=7000;
		dz_max_value=14000;

		oil_min_value=200;
		oil_max_value=500;

		water_min_value=200;
		water_max_value=500;
		pDebug->sprintlnp(PSTR("Var check failed"));
		return true;
	}
	pDebug->sprintlnp(PSTR("Checking Var: Passed"));
	return false;
};

void Speedo_aktors::init(){
	pDebug->sprintp(PSTR("Aktoren init ..."));
	/* vier werte paare:
	 * 1. Außen -> ändert sich öfters mal, wird mit std werten geladen
	 * 2. Innen -> ändert sich eigentlich nie
	 * 3. Flasher -> Farbe zu der übergeblendet wird wenn die schaltdrehzahl erreicht wird
	 * 4. out_base -> Farbkopie von Außen
	 */

	dimm_step=0;
	dimm_steps=0;
	colorfade_active=true;

	// innen
	pinMode(RGB_IN_R,OUTPUT);
	pinMode(RGB_IN_G,OUTPUT);
	pinMode(RGB_IN_B,OUTPUT);

	// see if its a clock startup or a regular startup
	if(pSpeedo->regular_startup){
		// beleuchtung
		analogWrite(RGB_IN_R,0);
		analogWrite(RGB_IN_G,0);
		analogWrite(RGB_IN_B,0);
		// beleuchtung
	}

	// außen
	pinMode(RGB_OUT_R,OUTPUT);
	pinMode(RGB_OUT_G,OUTPUT);
	pinMode(RGB_OUT_B,OUTPUT);

	set_rgb_out(0,0,0); // dimm ich in main ein .. hmm

	// see if its a clock startup or a regular startup
	if(pSpeedo->regular_startup){
		update_outer_leds(true,true);
	};

	// stepper drehen
	m_stepper->init();
	pDebug->sprintlnp(PSTR(" Done"));
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
		RGB.r.actual=r;
		RGB.g.actual=g;
		RGB.b.actual=b;
	};
};
/* 10ms pro schritt -> 0 to 256 = 2,56 sec */
void Speedo_aktors::dimm_rgb_to(int r,int g,int b,int max_dimm_steps){

	if(r>255) r=255;
	if(g>255) g=255;
	if(b>255) b=255;

	RGB.r.to=r;
	RGB.g.to=g;
	RGB.b.to=b;
	RGB.r.from=RGB.r.actual;
	RGB.g.from=RGB.g.actual;
	RGB.b.from=RGB.b.actual;

	dimm_steps=max_dimm_steps;
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

ISR(TIMER3_OVF_vect){
	pAktors->timer_overflow();
}
void Speedo_aktors::timer_overflow(){
	int r=int(round((float(int(RGB.r.to)-int(RGB.r.from))*dimm_step)/dimm_steps))+int(RGB.r.from);
	int g=int(round((float(int(RGB.g.to)-int(RGB.g.from))*dimm_step)/dimm_steps))+int(RGB.g.from);
	int b=int(round((float(int(RGB.b.to)-int(RGB.b.from))*dimm_step)/dimm_steps))+int(RGB.b.from);
	set_rgb_out(r,g,b);


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
		RGB.r.from=RGB.r.to;
		RGB.g.from=RGB.g.to;
		RGB.b.from=RGB.b.to;
	};
};

void Speedo_aktors::stop_dimmer(){
	// timer aus
	TCCR3B &= ~((1<<CS32) | (1<<CS31) | (1<<CS30));
}

bool Speedo_aktors::dimm_available(){
	if(dimm_step==dimm_steps)
		return true;
	else
		return false;
};



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

void Speedo_aktors::set_active_dimmer(bool state){
	colorfade_active=state;
}

int Speedo_aktors::update_outer_leds(bool dimm,bool overwrite){
	if(!colorfade_active) return 0;
	if(overwrite) dimm_state=999;

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
				pAktors->dimm_rgb_to(pAktors->dz_flasher.r,pAktors->dz_flasher.g,pAktors->dz_flasher.b,15); // 25*10ms = 250 ms
				dimm_state=DIMM_TO_FLASH_COLOR;
			}
		} else {
			if(pAktors->dimm_available()){
				pAktors->dimm_rgb_to(5,5,5,15); // 25*10ms = 250 ms
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
			pAktors->dimm_rgb_to(pAktors->static_color.r,pAktors->static_color.g,pAktors->static_color.b,350); // 25*10ms = 250 ms
			dimm_state=DIMM_TO_STATIC_COLOR;
		};
		return 0;

	} else if(led_mode>0){
		///////// color fader //////////
		int max_value=0,min_value=0,actual_value=0;
		led_simple from_color, to_color;

		///////// select values  ///////
		///////// kmh ////////////////
		switch(led_mode){
		case 1:
			max_value=kmh_max_value;
			min_value=kmh_min_value;
			actual_value=pSensors->m_speed->getSpeed();
			from_color=kmh_start_color;
			to_color=kmh_end_color;
			break;
		case 2:
			max_value=dz_max_value*100;
			min_value=dz_min_value*100;
			actual_value=pSensors->m_dz->exact;
			from_color=dz_start_color;
			to_color=dz_end_color;
			break;
		case 3:
			max_value=oil_max_value*10;
			min_value=oil_min_value*10;
			actual_value=pSensors->m_temperature->get_oil_temp();
			from_color=oil_start_color;
			to_color=oil_end_color;
			break;
		case 4:
			max_value=water_max_value*10;
			min_value=water_min_value*10;
			actual_value=pSensors->m_temperature->get_water_temp();
			from_color=water_start_color;
			to_color=water_end_color;
			break;
		}
		///////// dimm now ///////////
		if(actual_value>=max_value){
			if(dimm_state!=MAX_REACHED){
				if(dimm){
					dimm_rgb_to(int(to_color.r),int(to_color.g),int(to_color.b),350);
				} else {
					pAktors->set_rgb_out(int(to_color.r),int(to_color.g),int(to_color.b));
				};
			};
			dimm_state=MAX_REACHED;
		} else if(actual_value<=min_value){
			if(dimm_state!=MIN_REACHED){
				if(dimm)
					dimm_rgb_to(from_color.r,from_color.g,from_color.b,350);
				else {
					pAktors->set_rgb_out(from_color.r,from_color.g,from_color.b);
					dimm_state=MIN_REACHED;
				};
			};

		} else {
			int temp_r,temp_g,temp_b,differ;
			differ=max_value-min_value;
			temp_r = float(to_color.r-from_color.r)/float(differ)*(actual_value-min_value)+from_color.r;
			temp_g = float(to_color.g-from_color.g)/float(differ)*(actual_value-min_value)+from_color.g;
			temp_b = float(to_color.b-from_color.b)/float(differ)*(actual_value-min_value)+from_color.b;
			if(dimm)
				dimm_rgb_to(temp_r,temp_g,temp_b,350);
			else
				pAktors->set_rgb_out(temp_r,temp_g,temp_b);
		};
		///////// dimm now end ///////////
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
					_delay_ms(2000);
					return 0;
				}
			}
		}
	} else {
		ask_bt(&at_commands[0]);
		pOLED->string_P(pSpeedo->default_font,PSTR("failed, hmmm"),0,1);
		_delay_ms(5000);
	}
	_delay_ms(2000);
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

