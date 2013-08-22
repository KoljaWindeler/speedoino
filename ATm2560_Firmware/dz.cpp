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
	calc_previous_time=millis();
	peak_last_reception_timestamp=millis();
	previous_dz=0;
	exact=0;                 // real rotation speed
	peak_count=0;
	blitz_dz=0;
	blitz_en=false;
	dz_faktor_counter=0;
}

void speedo_dz::counter(){
#ifdef DZ_DEBUG
	Serial.print("DZ peak@");
	Serial.print(millis());
	Serial.print(" ");
	Serial.println(peak_count);
#endif
	peak_count++; // lï¿½uft bis 65.536 dann auf 0
	peak_last_reception_timestamp=millis();
};

unsigned int speedo_dz::get_dz(bool exact_dz){
	return exact;
}

bool speedo_dz::calc(bool force_calc) { // called by "pull_values" with 10Hz // new 3.7.2013, previous 5Hz
    ///// DZ BERECHNUNG ////////
    unsigned int  _temp_peak_count=peak_count;                                     // aktueller dz zï¿½hler stand, separate var damit der peakcount weiter verndert werden koennte
	unsigned long _temp_peak_last_reception_timestamp=peak_last_reception_timestamp;

	unsigned long differ=_temp_peak_last_reception_timestamp-calc_previous_time;  // zeit seit dem letzte mal abholen der daten
    if(_temp_peak_count>4 && (differ>25 || force_calc)){  // max mit 40Hz, bei niedriger drehzahl noch seltener, 1400 rpm => 680 ms
		/* new concept at 22/08/2013
         * at 4.000 rpm => 4000/60*2(double ignition) => 133 pps => 7,5ms per Peak, 5 Peaks in 37 ms
		 * -> The Sensor class calls us ~every 25ms with force_calc=true
		 * AT t=000.0: 01st pulse -> calls "counter()" -> counter value = 0 | peak_last_reception_timestamp=0
		 * AT t=007.5: 02nd pulse -> calls "counter()" -> counter value = 1 | peak_last_reception_timestamp=7.5
		 * AT t=015.0: 03rd pulse -> calls "counter()" -> counter value = 2 | peak_last_reception_timestamp=15
		 * AT t=022.5: 04th pulse -> calls "counter()" -> counter value = 3 | peak_last_reception_timestamp=22.5
		 * AT t=025.0: sensor class calls us _temp_peak_count=4 -> no calculation
		 * AT t=030.0: 05th pulse -> calls "counter()" -> counter value = 4 | peak_last_reception_timestamp=30
		 * AT t=037.5: 06th pulse -> calls "counter()" -> counter value = 5 | peak_last_reception_timestamp=37.5
		 * AT t=045.0: 07th pulse -> calls "counter()" -> counter value = 6 | peak_last_reception_timestamp=45
		 * AT t=050.0: sensor class calls us _temp_peak_count=6 -> differ=45, counter=6 -> dz=((60000>>1)*_temp_peak_count/differ)=((30000)*6/45)=4000 setting counter value=0
		 * AT t=052.5: 08th pulse -> calls "counter()" -> counter value = 1 | peak_last_reception_timestamp=52.5
		 * AT t=060.0: 09th pulse -> calls "counter()" -> counter value = 2 | peak_last_reception_timestamp=60
		 * AT t=067.5: 10th pulse -> calls "counter()" -> counter value = 3 | peak_last_reception_timestamp=67.5
		 * AT t=075.0: sensor class calls us _temp_peak_count=3 -> no calculation
		 * AT t=075.0: 11th pulse -> calls "counter()" -> counter value = 4 | peak_last_reception_timestamp=75
		 * AT t=082.5: 12th pulse -> calls "counter()" -> counter value = 5 | peak_last_reception_timestamp=82.5
		 * AT t=090.0: 13th pulse -> calls "counter()" -> counter value = 6 | peak_last_reception_timestamp=90
		 * AT t=097.5: 14th pulse -> calls "counter()" -> counter value = 7 | peak_last_reception_timestamp=97.5
		 * AT t=100.0: sensor class calls us _temp_peak_count=7 -> differ=97.5-45.0 counter=7 -> dz=((60000>>1)*_temp_peak_count/differ)=((30000)*7/52.5)=4000 -> passt
		 */

		// am 17.8. von 50 auf 25 geï¿½ndert ... das sind jetzt 40 Hz, mal sehen ob das noch klappt
        // am 3.8. von 100 auf 50 geï¿½ndert

		peak_count=0;
		calc_previous_time=_temp_peak_last_reception_timestamp; // save timestamp (of calc() call (!))
        unsigned int dz=((60000>>1)*_temp_peak_count/differ); // Calc RPM (60.000 for ms => min) // >>1 for doule ignition
        if(dz>15000){ // wenn man ï¿½ber 15000 U/min => Abstand von 2 Zï¿½ndungen = 60000[ms/min]/15000[U/min] = 4 [ms/U]
            dz=previous_dz; // alten Wert halten, kann nicht sein
        } else {
			previous_dz=dz; // speichere die aktuelle dz
		}

        /* values */
        //exact=dz;
        exact=pSensors->flatIt_shift(dz,&dz_faktor_counter,2,exact);       // IIR mit Rï¿½ckfï¿½hrungsfaktor 4 fï¿½r DZmotor
        /* values */
		return true;
        // Stop
    } else if(_temp_peak_last_reception_timestamp-calc_previous_time>500){
        exact=0;
        calc_previous_time=_temp_peak_last_reception_timestamp;
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
