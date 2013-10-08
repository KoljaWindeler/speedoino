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
/************************* timer *********************
 * ausführender timer, den einen jede sec
 * den anderen vier mal pro sekunde
 * den letzten nur nach bedarf per config, war 15 sec
 ************************* timer *********************/
speedo_timer::speedo_timer(){
	every_second_timer=millis()+50;   // Zeitstempel vom letzten ausführen der "every_sec" funktion
	every_qsecond_timer=millis()+125;   // Zeitstempel vom letzten ausführen der "every_qsec" funktion
	every_custom_timer=millis();   // Zeitstempel vom letzten ausführen der "every_qsec" funktion
}

speedo_timer::~speedo_timer(){

}

void ram_info() {
	int size = 8192; // Use 2048 with ATmega328
	byte *buf;
	while ((buf = (byte *) malloc(--size)) == NULL);
	free(buf);
	Serial.println(size);
}

void speedo_timer::every_sec(configuration* pConfig) {
	if((millis()-every_second_timer)>=998){
#ifdef TACHO_SMALLDEBUG
		pDebug->sprintp(PSTR("-t1"));
#endif
		every_second_timer=millis();
		//pConfig->ram_info(); // nur zum testen 19.12. 2900 free
		pAktors->m_oiler->check_value(); // gucken ob wir ölen müssten
		pConfig->km_save();    // avg,max,trips hochzählen, immer wenn ss==59 ist store to sd card
		pSensors->check_inputs(); // check once a second the state of the PINs

		if(pSD->sd_failed && (millis()/1000)%30==0 && pSensors->m_reset->reboots_caused_by_sd_problems<2){ // just two ties .. after that: die SD!
			Serial.print(millis());
			Serial.print("-");
			pSD->power_up(1);
			Serial.println(millis());
			if(!pSD->sd_failed){
				pSensors->m_reset->reboots_caused_by_sd_problems++;
				pDebug->sprintlnp(PSTR("SD reboot successful, rebooting speedo!"));
				init_speedo();
			}
		}

		// check if MIL should be active
		if((millis()/1000)%30==0 && pSensors->CAN_active && pSensors->m_CAN->get_active_can_type()==CAN_TYPE_OBD2){
			pSensors->m_CAN->request(CAN_CURRENT_INFO,CAN_MIL_STATUS);
		}

		// check gps
		if((millis()/1000)%10==0 && pSensors->m_gps->wait_on_gps()){
			pSensors->m_gps->reconfigure(); // default 9600
			pSensors->m_gps->update_rate_1Hz();
		}

		if((millis()/1000)==60){
			uint8_t result=0xff;
			bool comm_check;
			if(!pAktors->check_mac_key(&result,&comm_check)){
				pOLED->clear_screen();
				pOLED->string_P_centered(PSTR("Please contact"),2,true);
				pOLED->string_P_centered(PSTR("KKoolljjaa@gmail.com"),3,true);
				pOLED->string_P_centered(PSTR("FAQ #1"),4,true);
			}
		}
#ifdef TACHO_SMALLDEBUG
		pDebug->sprintlnp(PSTR("."));
#endif
	}
};



// Muss öfter sein,da die Blink Geschichte sonst nicht klappt
void speedo_timer::every_qsec() {
	if((millis()-every_qsecond_timer)>=250){
#ifdef TACHO_SMALLDEBUG
		pDebug->sprintp(PSTR("-t2"));
#endif
		every_qsecond_timer=millis();

		// TODO, testen mit meinem krams dran!
		if(pSensors->last_int_state!=(CAN_INTERRUPT_PIN_PORT_V7&0x07)){
			pSensors->check_inputs();
			pSensors->last_int_state=(CAN_INTERRUPT_PIN_PORT_V7&0x07);
		}

		// see if its a clock startup or a regular startup
		if(pSpeedo->startup_by_ignition){
			// TODO: warum so häufig?
			pAktors->update_outer_leds(false,false);

			// könntem mit 250ms update rate fast etwas lahm sein
			pAktors->m_stepper->startup();
		}
#ifdef TACHO_SMALLDEBUG
		pDebug->sprintlnp(PSTR("."));
#endif
	};
};

void speedo_timer::every_custom() {
	if((signed(millis()-every_custom_timer)>=pSpeedo->refresh_cycle) && (pSpeedo->refresh_cycle>1)){
		if((pMenu->state/10)==1 || pMenu->state==8511111)  {
			pOLED->clear_screen(); // Brauchen wir noch ein "clean" ?  <- für den Moment, ja!
			// einfach mal alle zeichen übermalen
			if(pSpeedo->oil_widget.symbol==1)
				pOLED->draw_oil(0,0);
			if(pSpeedo->fuel_widget.symbol==1)
				pOLED->draw_fuel(44,56);
			if(pSpeedo->air_widget.symbol==1)
				pOLED->draw_air(41,0);
			if(pSpeedo->clock_widget.symbol==1)
				pOLED->draw_clock(0,56);
			// und hier alle kontrollregister auf -99 zu setzen, zack werden alle Sachen erstellt
			pSpeedo->reset_bak();
		};
		every_custom_timer=millis();
	}
}
