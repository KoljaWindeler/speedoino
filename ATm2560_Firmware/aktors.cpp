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
	m_oiler=new speedo_oiler();

	kmh_min_value=0;
	kmh_max_value=0;

	dz_min_value=0;
	dz_max_value=0;

	oil_min_value=0;
	oil_max_value=0;

	water_min_value=0;
	water_max_value=0;

	led_mode=1;
	pointer_highlight_mode=0;
	bt_pin=1234;
	dimm_state=999; //???
};

Speedo_aktors::~Speedo_aktors(){
};


// run reset could be called in three ways,
// convertional by the bluetooth command
// -> that should write "Running Update", reconfigure serial speed and reset the ATm328
// second from the menu
// -> thereby only the "running update" should be written and the serial speed should be reconfigured
// third: from the state (2) if the user pushed "right" button to trigger reset
// -> only reset ATm328 and return to last state
void Speedo_aktors::run_reset_on_ATm328(char mode){
	// serial setup and "klickibunti"-"show some msg"
	if(mode==RESET_COMPLETE || mode==RESET_PREPARE){
		pSensors->m_reset->set_deactive(false,false);
		_delay_ms(100);// give serial 3 some time to send the messeage
		Serial3.end();
		Serial3.begin(115200);
		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("Running Update"),3,2);
		pOLED->string_P(pSpeedo->default_font,PSTR("on AT328P"),5,3);
	};

	// show "_R_to_trigger_reset__" only if in "preparation" mode
	if(mode==RESET_PREPARE){
		char temp[2];
		sprintf(temp,"%c",127);
		pOLED->string(pSpeedo->default_font,temp,1,7);
		pOLED->string_P(pSpeedo->default_font,PSTR("to trigger reset"),3,7);
	}

	// run the reset
	if(mode==RESET_COMPLETE || mode==RESET_KICK_TO_RESET){
		// pin as output
		DDRD |= (1<<ATM328RESETPIN);
		// set low -> low active
		PORTD &= ~(1<<ATM328RESETPIN);
		_delay_ms(50);
		PORTD |= (1<<ATM328RESETPIN);
		// set high, as pull up
		DDRD |= (1<<ATM328RESETPIN);
		PORTD |= (1<<ATM328RESETPIN);
	}

	// tunnel connection
	if(mode==RESET_COMPLETE || mode==RESET_PREPARE){
		// tunnel mode
		unsigned long timeout=millis();
		unsigned long menu_state_on_enter=pMenu->state; // save the menu state, to detect button pushes
		unsigned int max_time=5000; // should be enough if in android mode
		if(mode==RESET_PREPARE){ // longer timeout if in "menu-mode"
			max_time=30000;
		};
		while(millis()-timeout<max_time){ // time out
			while(Serial3.available()>0){
				Serial.print(Serial3.read(),BYTE);
				timeout=millis();
			}
			while(Serial.available()>0){
				Serial3.print(Serial.read(),BYTE);
				timeout=millis();
			}
			if(mode==RESET_PREPARE){
				if(menu_state_on_enter==((pMenu->state/10))){ // button "right" was pushed
					run_reset_on_ATm328(RESET_KICK_TO_RESET); // to the reset
					pMenu->state=menu_state_on_enter; // reset menu state
				} else if(menu_state_on_enter==((pMenu->state*10)+1)){ // button "left" was pushed
					pMenu->state=menu_state_on_enter; // reset menu state, because menu->back will otherwise recaluclate two steps back
					max_time=0;
					break;
				}
			}
		}
		Serial3.end();
		Serial3.begin(19200);
		pSensors->m_reset->set_active(false,false);
		pMenu->back();
	};

}


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

	if(m_oiler->check_vars()){
		return true;
	};

	pDebug->sprintlnp(PSTR("Checking Var: Passed"));
	return false;
};

void Speedo_aktors::init(){
	/* vier werte paare:
	 * 1. Außen -> ändert sich öfters mal, wird mit std werten geladen
	 * 2. Innen -> ändert sich eigentlich nie
	 * 3. Flasher -> Farbe zu der übergeblendet wird wenn die schaltdrehzahl erreicht wird
	 * 4. out_base -> Farbkopie von Außen
	 */

	dimm_step=0;
	dimm_steps=0;
	dimm_state=999; //unknown
	current_sensor_value=9999; // a very unrealistic value
	colorfade_active=true;
	control_lights=0x00;
	led_area_controll=0x00;
	expander_outdated=true;

	// inner leds
	RGB_IN_W_DIR  |= (1<<RGB_IN_W_PIN);   //pinMode(RGB_IN_W,OUTPUT);
	// outer leds
	RGB_OUT_R_DIR |= (1<<RGB_OUT_R_PIN); //pinMode(RGB_OUT_R,OUTPUT);
	RGB_OUT_G_DIR |= (1<<RGB_OUT_G_PIN); //pinMode(RGB_OUT_G,OUTPUT);
	RGB_OUT_B_DIR |= (1<<RGB_OUT_B_PIN); //pinMode(RGB_OUT_B,OUTPUT);

	set_rgb_out(0,0,0); // dimm ich in main ein .. hmm

	// see if its a clock startup or a regular startup
	if(pSpeedo->startup_by_ignition){
		// beleuchtung
		TCCR4A|=1<<COM4B1; // RGB_OUT_R 7  OCR4B = val;
		TCCR4A|=1<<COM4C1; // RGB_OUT_G 8  OCR4C = val;
		TCCR2A|=1<<COM2B1; // RGB_OUT_B 9  OCR2B = val;

		RGB_IN_W_PORT |= (1<<RGB_IN_W_PIN); // inner LED active
		// beleuchtung

		set_rbg_active((int)0x0000,false);
		update_outer_leds(true,true);
	};

	// stepper drehen
	m_stepper->init();
	m_oiler->init();

	// init port rep
	//	  Wire.beginTransmission(0x20);
	//	  Wire.write(0x12);
	//	  Wire.write(gpio);
	//	  Wire.endTransmission();
	I2c.write(PORT_REP_ADDR,0x00,0x00);
	I2c.write(PORT_REP_ADDR,0x01,0x00);

	pDebug->sprintlnp(PSTR("Aktoren init ... Done"));
};

void Speedo_aktors::set_rgb_out(int r,int g,int b){
	set_rgb_out(r,g,b,1);
}

void Speedo_aktors::set_rgb_out(int r,int g,int b,int save){
	if(r>255) r=255; else if(r<0) r=0;
	if(g>255) g=255; else if(g<0) g=0;
	if(b>255) b=255; else if(b<0) b=0;

	OCR4B=r; //analogWrite(RGB_OUT_R,r);
	OCR4C=g; //analogWrite(RGB_OUT_G,g);
	OCR2B=b; //analogWrite(RGB_OUT_B,b);

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


void Speedo_aktors::set_active_dimmer(bool state){
	colorfade_active=state;
}

int Speedo_aktors::update_outer_leds(bool dimm,bool overwrite){ // 250ms
	if(!colorfade_active) return 0;
	if(overwrite) dimm_state=999;

	////////// SHIFT FLASH ////////////////
	////////// calc it //////////
	if(pSensors->get_RPM(RPM_TYPE_DIRECT)>unsigned(pSensors->m_dz->blitz_dz) && pSensors->m_dz->blitz_en){
		attention_required=true;
		set_rbg_active((int)0x0000,false); // activate all led's
	} else if(pSpeedCams->get_active() && pMenu->state==11){
		if(pSpeedCams->calc()){
			attention_required=true;
			set_rbg_active((int)0x0000,false); // activate all led's
		} else {
			attention_required=false;
		}
	} else {
		attention_required=false;
	}

	/////// now show it //////////
	if(attention_required){
		if(dimm_state==FLASH_COLOR_REACHED){
			return 0;
		} else if(dimm_state==DIMM_TO_FLASH_COLOR){
			if(pAktors->dimm_available()){
				dimm_state=FLASH_COLOR_REACHED;
			}
		} else if(dimm_state==DIMM_TO_DARK){
			if(pAktors->dimm_available()){
				pAktors->dimm_rgb_to(pAktors->dz_flasher.r,pAktors->dz_flasher.g,pAktors->dz_flasher.b,15); // 15*10ms = 150 ms
				dimm_state=DIMM_TO_FLASH_COLOR;
			}
		} else {
			if(pAktors->dimm_available()){
				pAktors->dimm_rgb_to(5,5,5,15); // 15*10ms = 150 ms
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
			pAktors->dimm_rgb_to(pAktors->static_color.r,pAktors->static_color.g,pAktors->static_color.b,25); // 25*10ms = 250 ms
			dimm_state=DIMM_TO_STATIC_COLOR;
		};
		return 0;

	} else if(led_mode>0){
		///////// color fader //////////
		int max_value=0,min_value=0;
		led_simple from_color, to_color;
		bool colorfade_required=false;


		///////// select values  ///////
		///////// kmh ////////////////
		switch(led_mode){
		case 1:
			if(current_sensor_value!=(signed)(pSensors->get_speed(false))){
				current_sensor_value=(signed)(pSensors->get_speed(false));

				max_value=kmh_max_value;
				min_value=kmh_min_value;
				from_color=kmh_start_color;
				to_color=kmh_end_color;

				colorfade_required=true;
			}
			break;
		case 2:
			if(current_sensor_value!=(signed)(pSensors->get_RPM(RPM_TYPE_DIRECT))){
				current_sensor_value=(signed)(pSensors->get_RPM(RPM_TYPE_DIRECT));

				max_value=dz_max_value*100;
				min_value=dz_min_value*100;
				from_color=dz_start_color;
				to_color=dz_end_color;

				colorfade_required=true;
			}
			break;
		case 3:
			if(current_sensor_value!=pSensors->get_oil_temperature()){
				current_sensor_value=pSensors->get_oil_temperature();

				max_value=oil_max_value*10;
				min_value=oil_min_value*10;
				from_color=oil_start_color;
				to_color=oil_end_color;

				colorfade_required=true;
			}
			break;
		case 4:
			if(current_sensor_value!=pSensors->get_water_temperature()){
				current_sensor_value=pSensors->get_water_temperature();

				max_value=water_max_value*10;
				min_value=water_min_value*10;
				from_color=water_start_color;
				to_color=water_end_color;

				colorfade_required=true;
			}
			break;
		}

		if(colorfade_required){
			///////// dimm now ///////////
			int temp_r,temp_g,temp_b,differ;
			if(current_sensor_value>=max_value){
				temp_r=to_color.r;
				temp_g=to_color.g;
				temp_b=to_color.b;
			} else if(current_sensor_value<=min_value){
				temp_r=from_color.r;
				temp_g=from_color.g;
				temp_b=from_color.b;
			} else {
				differ=max_value-min_value;
				temp_r = float(to_color.r-from_color.r)/float(differ)*(current_sensor_value-min_value)+from_color.r;
				temp_g = float(to_color.g-from_color.g)/float(differ)*(current_sensor_value-min_value)+from_color.g;
				temp_b = float(to_color.b-from_color.b)/float(differ)*(current_sensor_value-min_value)+from_color.b;
			};

			pAktors->dimm_rgb_to(temp_r,temp_g,temp_b,25); // 25*10ms = 250 ms
			dimm_state=DIMM_TO_STATIC_COLOR;
			///////// dimm now end ///////////
		}
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
			pOLED->string_P(pSpeedo->default_font,PSTR("FAILED"),14,3);
			pOLED->string_P(pSpeedo->default_font,PSTR("DAMN"),14,4); // add message to make sure no active bt connection disturbs TODO
			_delay_ms(4000);
			Serial.end();
			Serial.begin(115200);
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

bool Speedo_aktors::check_mac_key(uint8_t* result,bool* comm_error){
	char at_commands[22];
	sprintf_P(at_commands,PSTR("ATB?%c"),0x0D);
	uint8_t returns=0;
	if(ask_bt(at_commands,true,22,&returns)==0){
		if(returns>=17){
			*comm_error=false;
			// kick sondernzeichern
			int char_array_pointer=0;
			for(uint8_t n=0;n<returns; n++){
				if((at_commands[n]>='0' && at_commands[n]<='9') || (at_commands[n]>='a' && at_commands[n]<='f')){
					at_commands[char_array_pointer]=at_commands[n];
					char_array_pointer++;
				} else if(at_commands[n]==' ' || char_array_pointer>=12){
					at_commands[char_array_pointer]=0x00;
					n=returns;
					returns=char_array_pointer;
				}
			}

			// move it from ascii hex to hex
			for(uint8_t n=0;n<char_array_pointer; n++){
				if(at_commands[n]>='0' && at_commands[n]<='9'){
					at_commands[n]-='0';
				}
				if(at_commands[n]>='a' && at_commands[n]<='f'){
					at_commands[n]-='a'-10;
				}
				if(at_commands[n]>='A' && at_commands[n]<='F'){
					at_commands[n]-='A'-10;
				}
			}

			// calc checksum
			uint16_t temp=0x00;
			for(uint8_t n=0;n<6;n++){
				temp+=((at_commands[n*2]&0x0f)<<4)+(at_commands[n*2+1]&0x0f);
			}
			temp=temp&0xff; // cut it to 8-bit

			if(		temp==0xD2 || // Kolja			= 00+12+6F+28+3C+ED
					temp==0xC8 || // Thomas			= 00+12+6F+28+3C+E3
					temp==0xC7 || // Phil			= 00+12+6F+28+3C+E2
					temp==0xD3 || // Patrick		= 00+12+6F+28+3C+EE
					temp==0xA3 || // Devel-Speedo 	= 00+12+6f+22+45+bb
					temp==0xC6){  // Andrej			= 00+12+6F+28+3C+E1
				*result=temp;
				return true;
			}
		} else { // if no answere -> break
			*result=0x00;
			*comm_error=true;
			return true;
		}
	}
	return false;
}


int Speedo_aktors::ask_bt(char *command){
	uint8_t temp;
	return ask_bt(command,false,0,&temp); // by default no answere
}

int Speedo_aktors::ask_bt(char *buffer, bool answere_needed, int8_t max_length, uint8_t* char_rec){
	for(int looper=0;looper<30;looper++){
		Serial.print(buffer);
		// A T \r \n O K \r \n = 8
		//warte bis der Buffer nicht voller wird
		_delay_ms(200);

		uint8_t ok_state=0; // 0 teile von "o" "k"
		int8_t n=0;
		*char_rec=0;
		char answere[20];
		while(Serial.available()){
			char temp = Serial.read();
			if(answere_needed){
				*char_rec=*char_rec+1;
				if(n<(max_length-1)){
					answere[n]=temp;
					n++;
					answere[n]=0x00; // EOString, 0x00 will be MAX in (max-length-1)
				};
			};

			if(temp=='O'){
				ok_state=1; // 1. TEIL
			} else if(temp=='K' && ok_state==1){
				strcpy(buffer,answere);
				return 0;

			} else {
				ok_state=0;
			}
		};
	};
	return -1;
}

int Speedo_aktors::set_expander(){
	uint8_t data[2];
	data[0]=(uint8_t)control_lights;
	data[1]=(uint8_t)led_area_controll;
	//	char data2[20];
	//	Sprint(data2,"Setze auf %i:%i",data[0],data[1]);
	//	Serial.println(data2);
	I2c.write(PORT_REP_ADDR,PORT_REP_ADDR_GPIO_A,data,2);
	expander_outdated=false; // update done
	return 0;
}

int Speedo_aktors::set_controll_lights(uint8_t oil,uint8_t flasher_left,uint8_t n_gear,uint8_t flasher_right,uint8_t high_beam, bool now){
	// light, 0=off, 1==on, x=dontcar
	uint8_t input[5]={flasher_right,high_beam,n_gear,flasher_left,oil};
	for(int i=3; i<8; i++){ // pins of port extender
		if(input[i-3]==0){ // offset of 3 between first position (flasher_righ) in array and pin on port extender
			control_lights&=~(1<<i);
		} else if(input[i-3]==1){
			control_lights|=(1<<i);
		}
	};
	if(now){
		return set_expander();
	}
	expander_outdated=true;
	return 0;
}

void Speedo_aktors::check_flag(){
	if(expander_outdated){
		set_expander();
	};
};

int Speedo_aktors::set_rbg_active(int status,bool now){
	uint8_t shifter[11]={5,6,2,7,4,1,1,2,3,0,0};
	uint8_t port[11]={1,1,1,1,1,0,1,0,1,0,1};
	uint8_t control_lights_old=control_lights;
	uint8_t led_area_controll_old=led_area_controll;

	for(int i=0; i<11; i++){
		if(status&(1<<i)){
			if(port[i]==0){
				control_lights|=(1<<shifter[i]);
			} else {
				led_area_controll|=(1<<shifter[i]);
			}
		} else {
			if(port[i]==0){
				control_lights&=~(1<<shifter[i]);
			} else {
				led_area_controll&=~(1<<shifter[i]);
			}
		}
	}

	if(control_lights_old!=control_lights || led_area_controll_old!=led_area_controll){ // check if anything changed
		if(now){
			return set_expander();
		}
		expander_outdated=true;
	}
	return 0;
}

// at on day the mStepper should be part of the aktor class


void Speedo_aktors::rgb_action(int needle_pos){
	if(pointer_highlight_mode==RGB_ACTION_TYPE_FOLLOW && !attention_required){
		int8_t pos=((needle_pos/15)*11)/1000; // 15k rpm on 11 leds
		int8_t end=pos+1;
		int8_t start=pos-1;
		int16_t status=0; // switch all off

		while(start<0){ start++; };
		while(end>10) { end--; };
		for(int8_t i=start; i<=end; i++){ // activate only the interesting ones
			status|=(1<<i);
		};
		set_rbg_active(~status,false); // low active
	} else if(pointer_highlight_mode==RGB_ACTION_TYPE_STACK && !attention_required){
		int8_t pos=((needle_pos/15)*11)/1000; // 15k rpm on 11 leds
		int16_t status=0; // switch all off

		if(pos>10) { pos=10; };
		for(int8_t i=0; i<=pos; i++){ // activate only the interesting ones
			status|=(1<<i);
		};
		set_rbg_active(~status,false); // low active
	} else if(pointer_highlight_mode==RGB_ACTION_TYPE_RSTACK && !attention_required){
		int8_t pos=((needle_pos/15)*11)/1000; // 15k rpm on 11 leds
		int16_t status=0; // switch all off

		if(pos>10) { pos=10; };
		if(pos<0) { pos=0; };
		for(int8_t i=pos; i<=10; i++){ // activate only the interesting ones
			status|=(1<<i);
		};
		set_rbg_active(~status,false); // low active
	}
}
