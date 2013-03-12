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
	every_second_timer=millis();   // Zeitstempel vom letzten ausführen der "every_sec" funktion
	every_qsecond_timer=millis();   // Zeitstempel vom letzten ausführen der "every_qsec" funktion
	every_custom_timer=millis();   // Zeitstempel vom letzten ausführen der "every_qsec" funktion
	blitzer_warning_visible=false;
	pinMode(3,INPUT);
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
		every_second_timer=millis();
		//pConfig->ram_info(); // nur zum testen 19.12. 2900 free
		pAktors->m_oiler->check_value(); // gucken ob wir ölen müssten
		pConfig->km_save();    // avg,max,trips hochzählen, immer wenn ss==59 ist store to sd card

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
	}
};



// Muss öfter sein,da die Blink Geschichte sonst nicht klappt
void speedo_timer::every_qsec() {
	if((millis()-every_qsecond_timer)>=250){
		every_qsecond_timer=millis();
		// see if its a clock startup or a regular startup
		if(pSpeedo->regular_startup){ // TODO: warum so häufig?
			pAktors->update_outer_leds(false,false);
		}
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
