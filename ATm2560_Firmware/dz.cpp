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

speedo_dz::speedo_dz(){
	previous_time=millis();
	previous_dz=0;
	rounded=0;                 // to show on display, rounded by 50
	exact=0;                 // real rotation speed
	exact_disp=0;
	peak_count=0;
	blitz_dz=0;
	blitz_en=false;
	dz_faktor_counter=0;
	dz_disp_faktor_counter=0;
	previous_peaks=0;
	dz_flat=0;
}

void speedo_dz::counter(){
#ifdef DZ_DEBUG
	Serial.print("DZ peak@");
	Serial.print(millis());
	Serial.print(" ");
	Serial.println(peak_count);
#endif
	peak_count++; // läuft bis 65.536 dann auf 0
};

unsigned int speedo_dz::get_dz(bool exact_dz){
	return exact;
}

bool speedo_dz::calc(bool force_calc) { // called by "pull_values" with 10Hz // new 3.7.2013, previous 5Hz
    ///// DZ BERECHNUNG ////////
    unsigned long now=millis();                                             // aktuelle zeit
    unsigned long differ=now-previous_time;                                 // zeit seit dem letzte mal abholen der daten
    unsigned int  now_peaks=peak_count;                                     // aktueller dz zähler stand, separate var damit der peakcount weiter verndert werden koennte
    if(now_peaks>4 && (differ>25 || force_calc)){                 // max mit 40Hz, bei niedriger drehzahl noch seltener, 1400 rpm => 680 ms
        // am 17.8. von 50 auf 25 geändert ... das sind jetzt 40 Hz, mal sehen ob das noch klappt
        // am 3.8. von 100 auf 50 geändert
        // bei 4.000 rpm => 66 pps => 15ms Pro Peak, 4 Peaks in 60 ms
        // somit kann man mit 50 statt 100 bis 4800 doppelt so schnell reagieren,
        // mit 100ms war das sehr träge, mal sehen ob das die COM schnittstelle hergibt
        //now_peaks=now_peaks>>anzahl_shift;                                // könnte ja sein das man weniger umdrehungen als funken hat, hornet hat 2 Funken je Umdrehun
        // die maximale übertragungsrate zwischen ATm8 und ATm2560 sollte nicht überschritten werden
        // pro Übertragung werden benötigt: 19200 Baud, 2400 Byte/sek, 7 Byte, 2,916667 ms, machen wir mal 10 ms draus.
        // bei 15.000 U/min => 250 U/sec
        // da bei der Hornet 2 Zündungen pro Umdrehung vorkommen
        // 500 Pulse / sec => 2ms zwischen 2 Pulsen, 5 Pulse in 10 ms
        //
        // bei 1.300 U/min => 21,6 U/min
        // 43 Pulse / sec => 23ms zwischen 2 Pulsen, 5 Pulse in 116,27ms

        unsigned int dz=60000/differ*now_peaks>>1; // Drehzahl berechnet (60.000 weil ms => min)
        if(dz>15000){ // wenn man über 15000 U/min => Abstand von 2 Zündungen = 60000[ms/min]/15000[U/min] = 4 [ms/U]
            dz=previous_dz; // alten Wert halten, kann nicht sein
        };

        /* Timing */
        peak_count=0;
        previous_time=now; // speichere wann ich zuletzt nachgesehen habe
        previous_dz=dz; // speichere die aktuelle dz
        /* Timing */

        /* values */
        //exact=dz;
        exact=pSensors->flatIt_shift(dz,&dz_faktor_counter,2,exact);                        // IIR mit Rückführungsfaktor 1 für DZmotor
        /* values */
		return true;
        // Stop
    } else if(now-previous_time>500){
        rounded=0;
        exact=0;
        previous_time=now;
		return true;
    };
	return false;
};

ISR(INT4_vect){
	pSensors->m_dz->counter();
}


void speedo_dz::init() {
	EIMSK |= (1<<INT4); // Enable Interrupt
	EICRB |= (1<<ISC40) | (1<<ISC41); // rising edge on INT4
	peak_count=0; // set to zero

	pDebug->sprintlnp(PSTR("DZ init done"));
	blitz_en=false;
	Serial3.flush();
};

void speedo_dz::shutdown(){
	EIMSK &= ~(1<<INT4); // DISABLE Interrupt
	EICRB &= ~(1<<ISC40) | (1<<ISC41); // no edge on INT4
};


int speedo_dz::check_vars(){
	if(blitz_dz==0){
		pDebug->sprintp(PSTR("DZ failed"));
		blitz_dz=12500; // hornet maessig
		blitz_en=true; // gehen wir mal von "an" aus
		return 1;
	}
	return 0;
};

