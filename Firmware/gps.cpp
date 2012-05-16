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

// init & clean buffer
speedo_gps::speedo_gps(){
};

speedo_gps::~speedo_gps(){
};

// hässlich hier den interrupt eingefügt .. 
ISR(USART1_RX_vect ){
	pSensors->m_gps->recv_data();
};

void speedo_gps::init(){

	// interrupt + UART
#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun nachgeholt mit 16Mhz"
#define F_CPU 16000000UL // Systemtakt in Hz - Definition als unsigned long beachten
#endif

	// Berechnungen
#define BAUD 9600UL // Baudrate
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1) // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1))) // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#error Systematischer Fehler der Baudrate grsser 1% und damit zu hoch!
#endif
	UBRR1H = UBRR_VAL >> 8;
	UBRR1L = UBRR_VAL & 0xFF;

	UCSR1B |= (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1); // rx,tx,rx interrupt

	sei(); // global anschalten, falls sie es nicht ohnehin schon sind
	// interrupt
	// parsen
	memset(gps_buffer1,'*',SERIAL_BUFFER_SIZE); // leere internen buffer
	memset(gps_buffer2,'*',SERIAL_BUFFER_SIZE); // '*' ist endchar fuer gps signale, super "filler"

	ringbuf_counter=0;
	gps_ready1=false;
	gps_ready2=false;
	gps_state=0;
	written_gps_points=0;
	// parsen

	// navigation
	valid=99;
	entf=0;
	winkel=0; // 0-359
	inner_circle=false; // navigation
	navi_active=false; // navigation
	note_this_place=0;
	// navigation

	// speichern
	gps_count=-1;
	gps_count2=-1;
	// speichern

	// Debug
	gps_write_status=0;

	// hier das gps konfigurieren ..  yeah yeah yeah
	SendString("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");

	speed=0;

	pDebug->sprintlnp(PSTR("GPS init done"));
};

// wird als interrupt aufgerufen
// prüft ob daten da sind, die valid sind
// wenn ja dann wird get_GPS damit aufgerufen
void speedo_gps::recv_data(){
	char byteGPS = UDR1;
	//Serial.print(byteGPS);
	switch(gps_state){
	case 0:  // hier sitzen wir und warten auf das startzeichen
		if(byteGPS=='$'){ gps_state=1;	};
		break;
	case 1:  // das wird dann ein "G" sein
		gps_state=2;
		break;
	case 2: // ein "P"
		gps_state=3;
		break;
	case 3: // hier wirds interessant: isses ein ein R oder G ?
		if(byteGPS=='R')
			gps_state=4;
		else if(byteGPS=='G')
			gps_state=14; // hier mal einfach um 10 weiterspringen
		else
			gps_state=0;
		break;
	case 4: // auch noch interessant, hier müsste auf das R ein M folgen
		if(byteGPS=='M') // jaha
			gps_state=5;
		else			// schade, doch nüscht
			gps_state=0;
		break;
	case 5: // soweit gut $GPRM fehlt das C
		if(byteGPS=='C'){// jaha, ab in den "lese modus"
			ringbuf_counter=0; // position in die geschrieben werden soll
			gps_state=6;
		} else			// schade, doch nüscht
			gps_state=0;
		break;
	case 6:
		if(ringbuf_counter>=SERIAL_BUFFER_SIZE-1){ gps_state=0; break; }    // überlauf, kein gps wert ist länger als 75 Byte. Mist
		gps_buffer1[ringbuf_counter]=byteGPS;   // in den buffer schmeißen
		if(byteGPS=='*'){                // * das ist mein end signal dann is ende im gelände und abmarsch
			gps_ready1=true;
			gps_state=0;
		} else if(byteGPS=='$'){ // damn da haben wir was verpasst
			gps_state=1; // jump start
		} else { // es scheint also ein ganz normales zeichen zu sein, dann könnte nur noch der hier schief gehen
			ringbuf_counter++;
		}
		break;
	case 14: // auch noch interessant, hier müsste auf das GPG ein G und dann ein A folgen
		if(byteGPS=='G') // jaha
			gps_state=15;
		else			// schade, doch nüscht
			gps_state=0;
		break;
	case 15: // jetzt fehlt nur noch das A
		if(byteGPS=='A'){ // jaha
			ringbuf_counter=0; // position in die geschrieben werden soll
			gps_state=16;
		} else			// schade, doch nüscht
			gps_state=0;
		break;
	case 16:
		if(ringbuf_counter>=SERIAL_BUFFER_SIZE){ gps_state=0; break; }    // überlauf, kein gps wert ist länger als 75 Byte. Mist
		gps_buffer2[ringbuf_counter]=byteGPS;   // in den buffer schmeißen
		if(byteGPS=='*'){                // * das ist mein end signal dann is ende im gelände und abmarsch
			gps_ready2=true;
			gps_state=0;
		} else if(byteGPS=='$'){ // damn da haben wir was verpasst
			gps_state=1; // jump start
		} else { // es scheint also ein ganz normales zeichen zu sein, dann könnte nur noch der hier schief gehen
			ringbuf_counter++;
		}
		break;
		// end case 
	}
};

/* diese funktion wird in jeder for schleife abgegraßt und fall hier ein flag auf true steht
 * werden die daten geparst
 */
void speedo_gps::check_flag(){
	if(gps_ready1){
		// debug
		if(IGPS_DEBUG){
			pDebug->sprintlnp(PSTR("calling get_gps buffer1:"));
			Serial.println(gps_buffer1);
		};
		// debug
		parse(gps_buffer1,1);         // Daten übergeben
		gps_ready1=false;
	};
	if(gps_ready2){
		// debug
		if(IGPS_DEBUG){
			pDebug->sprintlnp(PSTR("calling get_gps buffer2:"));
			Serial.println(gps_buffer2);
		};
		// debug
		parse(gps_buffer2,2);         // Daten übergeben
		gps_ready2=false;
	};
};

/* parsen von nmea daten zu gps daten
 * im dümmsten fall haben wir die beiden Datensätze direkt hinter einander
 * dann haben wir durch die Statemashine 2 Byte für die Prüfsumme und 6 Byte für die anfangssequnz bis wir 
 * in den gleichen serial_buffer reinschreiben
 * bei 4800 Baud sind das 1,6 ms für den Spass hier unten .. nicht gerade üppig
 * datensatz=1 => $GPRMC
 */
void speedo_gps::parse(char linea[SERIAL_BUFFER_SIZE],int datensatz){
	int cont=0;
	char status;
	int indices[13]; // positionsmarker für die kommata

	//debug
	if(GPS_DEBUG){
		pDebug->sprintlnp(PSTR("get_gps hat bekommen: "));
		for(int i=0;i<SERIAL_BUFFER_SIZE;i++){
			Serial.print(linea[i]);
		};
		Serial.println("");
	};
	//debug
	// seperatorstellen suchen
	for (int i=0;i<SERIAL_BUFFER_SIZE;i++){
		if (linea[i]==','){    // check for the position of the  "," separator
			indices[cont]=i;
			cont=(cont+1)%13; // to prevent buffer overrun
		}
	}
	// modus 1, $gprmc empfangen
	if(datensatz==1){ // lat,long,fix,time,Course,date
		status=linea[indices[1]+1];

		// zeit in jedem fall berechnen, auch wenn das sample nicht valid ist, das ist der RTC
		long temp_gps_time=0;
		long temp_gps_date=0;
		for (int j=indices[0]+1;j<(indices[0]+7);j++){ // format 234500 für füddelvorzwölf
			temp_gps_time=temp_gps_time*10+(linea[j]-48);
		}
		for (int j=indices[8]+1;j<(indices[9]);j++){
			temp_gps_date=temp_gps_date*10+(linea[j]-48);    //181102 für 18.Nov 2002
		}
		// in date steht sowas wie  030411 für 3.4.2011
		// Uhr stellen
		int temp_mon=int(floor(temp_gps_date/100))%100;
		int temp_day=floor(temp_gps_date/10000);
		pSensors->m_clock->set_date_time(
				int(temp_gps_date-temp_mon*100-temp_day*10000)%100,
				temp_mon,
				temp_day,
				int(floor(temp_gps_time/10000))%24,
				int(floor(temp_gps_time/100))%100,
				temp_gps_time%100,
				-1,
				-1
		);

		if(status=='A'){
			valid=0;
			gps_count=(gps_count+1)%(sizeof(gps_time)/sizeof(gps_time[0]));
			// alle leeren
			gps_time[gps_count]=0;
			gps_date[gps_count]=0;
			gps_long[gps_count]=0;
			gps_lati[gps_count]=0;
			gps_speed_arr[gps_count]=0;
			gps_course[gps_count]=0;

			if(note_this_place!=0){ // kann man im tacho setzen, bewirkt dann das im logfile eine zahl statt 0 steht
				gps_special[gps_count]=note_this_place;
				note_this_place=0;
			}
			else {
				gps_special[gps_count]=0;
			};

			gps_time[gps_count]=temp_gps_time;
			gps_date[gps_count]=temp_gps_date;

			for (int j=indices[7]+1;j<(indices[8])-2;j++){ // keine Nachkommastelle mehr
				if(linea[j]!=46){
					gps_course[gps_count]=gps_course[gps_count]*10+(linea[j]-48);
				};
			}
			for (int j=indices[2]+1;j<(indices[3]);j++){ // keine Nachkommastelle mehr, punktkiller format 1-3 vorkomma und immer 6(!) nachkomma
				if(linea[j]!=46){
					gps_lati[gps_count]=gps_lati[gps_count]*10+(linea[j]-48);
				};
			}

			for (int j=indices[4]+1;j<(indices[5]);j++){ // keine Nachkommastelle mehr, punktkiller format 1-3 vorkomma und immer 6(!) nachkomma
				if(linea[j]!=46){
					gps_long[gps_count]=gps_long[gps_count]*10+(linea[j]-48);
				};
			}

			for (int j=indices[6]+1;j<(indices[7]);j++){
				if(linea[j]!=46){
					gps_speed_arr[gps_count]=gps_speed_arr[gps_count]*10+(linea[j]-48);
				};
			}
			gps_speed_arr[gps_count]=round(gps_speed_arr[gps_count]/54);//round((gps_speed_arr[gps_count]*1.852)/100); // auf kmh umbbiegen
			speed=gps_speed_arr[gps_count];


			// debug
			if(GPS_DEBUG){
				pDebug->sprintp(PSTR("Time in UTC (HhMmSs): "));
				Serial.println(gps_time[gps_count]);
				pDebug->sprintp(PSTR("Date UTC (DdMmAa) "));
				Serial.println(gps_date[gps_count]);
				pDebug->sprintp(PSTR("Heading in degrees:(*10)"));
				Serial.println(gps_course[gps_count]);
				pDebug->sprintp(PSTR("Latitude: "));
				Serial.println(gps_lati[gps_count]);
				pDebug->sprintp(PSTR("Longitude: "));
				Serial.println(gps_long[gps_count]);
				pDebug->sprintp(PSTR("Speed kmh: "));
				Serial.println(gps_speed_arr[gps_count]);
			}
			//Debug
			// uiuiuiuiuiuiui
			if(NAVI_DEBUG){
				pDebug->sprintp(PSTR("Zeit vorm gps_navi: "));
				Serial.println(millis());
			};
			if(navi_active){ // only calc if needed
				calc_navi();
			}
			if(NAVI_DEBUG){
				pDebug->sprintp(PSTR("Zeit nach gps_navi: "));
				Serial.println(millis());
			};
			// uiuiuiuiuiuiui
		}
	} // anderer modus, gpgga empfangen
	else if(datensatz==2){ // altitude, fix ok?,sats,
		gps_count2=(gps_count2+1)%(sizeof(gps_alt)/sizeof(gps_alt[0])); // %30 => 0..29
		gps_alt[gps_count2]=0;
		gps_sats[gps_count2]=0;
		for (int j=indices[5]+1;j<(indices[6]);j++){
			gps_fix[gps_count2]=(linea[j]-48);
		};
		for (int j=indices[6]+1;j<(indices[7]);j++){
			gps_sats[gps_count2]=gps_sats[gps_count2]*10+(linea[j]-48);
		}
		for (int j=indices[8]+1;j<(indices[9]);j++){
			if(linea[j]!=46){
				gps_alt[gps_count2]=gps_alt[gps_count2]*10+(linea[j]-48);
			};
		}
		//debug
		if(GPS_DEBUG){
			pDebug->sprintp(PSTR("alt: "));
			Serial.println(gps_alt[gps_count2]);
			pDebug->sprintp(PSTR("sats: "));
			Serial.println(gps_sats[gps_count2]);
			pDebug->sprintp(PSTR("fix? 1==ja: "));
			Serial.println(gps_fix[gps_count2]);
		};
		//debug
	};
	// klappt das ? eigentlich schon oder? wir dürfen nur keine Datensatz verpassen oder ?

	if(gps_count>=29 && gps_count2>=29){ // nur jede halbe minute speichern, burstmäßig auf die Karte schreiben
		if(SD_DEBUG){
			pDebug->sprintlnp(PSTR("Vor store_to_sd()"));
			Serial.println(millis());
		};
		gps_write_status=1;
		store_to_sd();
		if(SD_DEBUG){
			pDebug->sprintlnp(PSTR("nach store_to_sd()"));
			Serial.println(millis());
		};
	};
};

// return values of the gps_field, convertet as long
long speedo_gps::get_info(int select){
	if(gps_count==-1){
		return 0;
	};
	switch(select){
	case 0:
		return gps_time[gps_count];
		break;
	case 1:
		return gps_date[gps_count];
		break;
	case 2:
		return gps_long[gps_count];
		break;
	case 3:
		return gps_lati[gps_count];
		break;
	case 4:
		return gps_alt[gps_count2];
		break;
	case 5:
		return long(gps_speed_arr[gps_count]);
		break;
	case 6:
		if(long(gps_sats[gps_count2])>20) { return 20; }; //höhö
		if(long(gps_sats[gps_count2])<0) { return 0; };
		return long(gps_sats[gps_count2]);
		break;
	case 7:
		return long(gps_fix[gps_count2]);
		break;
	case 8:
		return long(gps_course[gps_count]);
		break;
	}
	return -1;
};

// alle 30 gültigen gps sample einmalauf der SD speichern bitte
// im kolja_gps format -> yeah bloß nix bekanntes, kompatibles verwenden, alles properitär machen :D
void speedo_gps::store_to_sd(){
	// generate filename && store
	char *filename;
	filename = (char*) malloc (11);
	if (filename==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
	else memset(filename,'\0',11);

	// 20.3.2011 => 110320
	sprintf(filename,"%06lu.GPS",pSensors->m_clock->get_long_date());

	if(SD_DEBUG){
		pDebug->sprintlnp(PSTR("filename is "));
		Serial.println(filename);
	};
	gps_write_status=2;
	// das kann auch ruhig alle 30 sec gemacht werden
	//pSensors->m_clock->store();
	pConfig->storage_outdated=true; // damit geschrieben werden darf
	gps_write_status=3;
	pConfig->write(filename); // schreiben

	free(filename);
	//done
};

/********* berechnung der distanz ***********
 * eingabe als ul in dezimalgrad*10^6 => 9473208.0;
 * aktuelle Position ist im array gps_lati/gps_long
 * LATI ist immer !! 111km pro Grad und ist quasi die y koodinate
 * LONG ist x Richtung und im Abstand ist abhängig von der Latitude, bei 0 ist er auch 111km bei 90° = 0km
 * Ausgabe als meter! och dann können wir maximal ein paar mio km fahren, wirs nix mit zum mars ..
 ********************************************/
unsigned long speedo_gps::calc_dist(unsigned long longitude,unsigned long latitude){
	if(valid>5) return -1; // rollt über und ist dann unendlich riesig

	float dist_y,dist_x; // abstands vars
	unsigned long actual_long=nmea_to_dec(gps_long[gps_count]);
	unsigned long actual_lati=nmea_to_dec(gps_lati[gps_count]);

	// können wir nicht mit abs machen, weil das unsigned ist
	if( actual_lati > latitude){
		dist_y=(float)(actual_lati - latitude); // direkt
	} else {
		dist_y=(float)(latitude - actual_lati); // direkt
	}
	// können wir nicht mit abs machen, weil das unsigned ist
	if( actual_long > longitude){
		// hier kommen grad*10⁶ raus
		dist_x=(float)(actual_long - longitude); // erstmal ungewichtet
	} else {
		dist_x=(float)(longitude - actual_long); // erstmal ungewichtet
	}
	dist_x=round(dist_x*cos(floor((latitude+actual_lati)/2000000.0)*2*M_PI/360)); // gewichtet
	// berechne distanz
	return round((unsigned long)sqrt((dist_x*dist_x)+(dist_y*dist_y))*0.111);
};
// get distance between aktueller und ziel punkt


/*******************************************************************
 * kern Funktion des gps,
 * gobale winkel var für den zeiger berechnen und vor allem
 * feststellen ob wir die ziel koodinaten erreicht haben
 * falls ja werden die funktionen für "lad das nächste" angetippt
 *******************************************************************/
void speedo_gps::calc_navi(){
	// differenz der Breitengrade in grad*1.000.000
	entf=calc_dist(navi_ziel_long,navi_ziel_lati);
	if(entf==0) entf++; // if anything is crashed, don't divide by zero!! this will makes it even worse !

	// winkel berechnen, klappt so nur in europa ;)  // long ist nach rechts links,lati oben und unten wir wollen den winkel zwischen dem lati/long also gegen/hypo = sin
	winkel=round(asin(abs(navi_ziel_lati-nmea_to_dec(float(gps_lati[gps_count])))*0.111/entf)/(2*M_PI)*360); // zu grad

	if((nmea_to_dec(float(gps_lati[gps_count]))>navi_ziel_lati) && (nmea_to_dec(float(gps_long[gps_count]))<navi_ziel_long)){ // unter uns und rechts ist das ziel
		winkel+=90;
	} else if((nmea_to_dec(float(gps_lati[gps_count]))>navi_ziel_lati) && (nmea_to_dec(float(gps_long[gps_count]))>navi_ziel_long)){ // unter uns und links ist das ziel
		winkel=270-winkel;
	} else if((nmea_to_dec(float(gps_lati[gps_count]))<navi_ziel_lati) && (nmea_to_dec(float(gps_long[gps_count]))>navi_ziel_long)){ // über uns und links ist das ziel
		winkel+=270;
	} else if((nmea_to_dec(float(gps_lati[gps_count]))<navi_ziel_lati) && (nmea_to_dec(float(gps_long[gps_count]))<navi_ziel_long)){ // über uns und rechts ist das ziel
		winkel=90-winkel;
	};
	winkel=winkel%360; // falls zu groß geworden
	//Serial.print("Von meiner Position: "); Serial.print(nmea_to_dec(float(gps_lati[gps_count]))); Serial.print(nmea_to_dec(float(gps_long[gps_count])));
	//Serial.print(" befindet sich das Ziel "); Serial.print(navi_ziel_lati); Serial.print(navi_ziel_long);
	//Serial.print(" in einem 0-Winkel von "); Serial.println(gps_winkel);
	//Soll fahrt Winkel - Fahrt winkel ergibt winkel im vergleich zu gerade aus ...
	winkel-=gps_course[gps_count];
	if(winkel<0){
		winkel+=360;
	};


	// debug
	if(NAVI_DEBUG){
		pDebug->sprintp(PSTR("Aktuelle lati: "));
		Serial.print(nmea_to_dec(float(gps_lati[gps_count])));
		pDebug->sprintp(PSTR(" Ziel Lati "));
		Serial.println(navi_ziel_lati);
		pDebug->sprintp(PSTR("Aktuelle long: "));
		Serial.print(nmea_to_dec(float(gps_long[gps_count])));
		pDebug->sprintp(PSTR(" Ziel long "));
		Serial.println(navi_ziel_long);
		pDebug->sprintp(PSTR("Entfernung in km: "));
		Serial.println(entf);
	};
	// debug
	// die große Frage des Weiterschaltens
	if(entf<15){ // verdammt nah dran, sofort weiterschalten
		inner_circle=false;
		navi_point++;
		generate_new_order();
	}
	else if(entf<30){ // wenn man näher als 30 meter am ziel ist, also quasi dran lang schrammt, vorbereitung treffen
		inner_circle=true;
	}
	else if(entf>60 && inner_circle){ // weiter als 60 meter vom ziel entfernt, aber schon im inneren zirkel gewesen weiterschalten
		inner_circle=false;
		navi_point++;
		generate_new_order();
	};
	// die große Frage des Weiterschaltens
};

// nmea format (9 47 3208 => 9 788680) umrechnen, eingabe als ul grad+6"nachkommastellen" ohne punkt getrennt
unsigned long speedo_gps::nmea_to_dec(unsigned long nmea){
	return floor(nmea/1000000.0)*1000000+round(mod(nmea,1000000.0)*10/6);
}

// gibt kein mod für ul ? son müll!
unsigned long speedo_gps::mod(unsigned long zahl,unsigned long teiler){
	return (unsigned long)(zahl-(floor(zahl/teiler)*teiler));
};

// gib distanz,richtung,straße zurück
int speedo_gps::get_order(char char_buffer[]){
	// wenn kein gps signal am start war für mehr als 15 sec
	char_buffer[0]='\0'; // leeren
	if(valid>15){
		return -1;
	};
	int value=0;
	int value2=0;

	sprintf(char_buffer,"! calc failed !"); // das hier ist vorgabe, damit da nicht "no gps" stehen bleibt obwohl wir gps haben und nur die berechnung abgeschmiert ist

	if(entf>=10000){ // mehr als 10 km
		value=int(floor(entf/1000)); // wert als km
		//3+2+1+1+1+10+1+2=21+'\0'=22 -- 25 max
		sprintf(char_buffer,"%3ikm %c XXXXXXXXXX %02i",value,(125+navi_ziel_rl),navi_point%100); // max 999km
	}
	else if(entf>=1000){ // mehr als 1 km
		value=int(floor(entf/1000)); // wert als km
		value2=int(floor((entf%1000)/100)); // wert als km
		sprintf(char_buffer,"%i.%ikm %c XXXXXXXXXX %02i",value,value2,(125+navi_ziel_rl),navi_point%100); // max 9.9km
		value=value*10+value2;
	}
	else {  // weniger als 1000 m
		value=int(floor(entf/10)*10); // auf 10m runterbrechen
		sprintf(char_buffer," %3im %c XXXXXXXXXX %02i",value,(125+navi_ziel_rl),navi_point%100); //max 999m
	};
	for(unsigned int i=0;i<10; i++){ // klappt anders nicht ...grmpf in arduino ging das, in eclipse nicht mehr
		char_buffer[i+8]=navi_ziel_name[i];
	};
	return value;
};

// die globalen variablen neu befüllen, auf basis des zuvor gesetzten navi_point
void speedo_gps::generate_new_order(){ // eine neue Order auslesen
	// da diese funktion nur aufgerufen wird wenn vorher der navi_pointer geändert wurde speichern wir hier den pointer um das so selten wie nötig zu machen
	byte tempByte = (navi_point & 0xFF);
	EEPROM.write(147,tempByte);

	if(NAVI_DEBUG){ pDebug->sprintlnp(PSTR("Versuche NAVI.TXT zu oeffnen")); };

	SdFile root;
	SdFile file;
	SdFile subdir;
	root.openRoot(&pSD->volume);
	if(!subdir.open(&root, NAVI_FOLDER, O_READ)) {
		pSD->sd_failed=true;
		pDebug->sprintlnp(PSTR("open subdir /config failed"));
	};
	char *navi_filename;
	navi_filename = (char*) malloc (13);
	if (navi_filename==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
	else memset(navi_filename,'\0',13);

	sprintf(navi_filename,"NAVI%i.TXT",active_file%100);

	if(file.open(&subdir, navi_filename, O_READ)) {
		int16_t n,i;
		uint8_t buf[36];// immer 35 byte lesen, das ist genau eine anweisung (wobei das array wieder ein Feld größer sein muss)
		// die ersten bytes sind sowas wie der titel, die können wir zur navigation nicht nutzen, weglesen
		// suche den ersten zeilen umbruch
		bool found=false;
		while(!found){
			file.read(buf, sizeof(buf[0]));
			if(buf[0]=='\n') found=true;
		};

		found=false;
		i=0;
		while ((n = file.read(buf, sizeof(buf))) > 0) { // n=wieviele byte gelesen wurden
			if(i==navi_point){// hier haben wir unsere "zeile" gefunden
				navi_ziel_lati=0;
				navi_ziel_long=0;

				// lati/long einlesen
				for(int a=0;a<=8;a++){ // vorwärts
					//sprintf(serialbuffer,"Ich lese ein: %c und mache daraus %i",buf[a],buf[a]-48);
					//Serial.println(serialbuffer);
					navi_ziel_lati=navi_ziel_lati*10+(int(buf[a])-48);
					navi_ziel_long=navi_ziel_long*10+(int(buf[a+10])-48);
				};
				// ziel name
				for(int a=0;a<=9;a++){ // vorwärts  -> 128/6=21  : 2.1km g helmholtzs 11 --> 10 buchstaben
					navi_ziel_name[a]=buf[a+22];
				};
				navi_ziel_name[10]='\0';

				// r oder l oder g
				navi_ziel_rl=int(buf[20])-48;
				if(NAVI_DEBUG){
					pDebug->sprintp(PSTR("Long: ")); Serial.print(navi_ziel_long); pDebug->sprintp(PSTR(" lati: ")); Serial.print(navi_ziel_lati);
					pDebug->sprintp(PSTR(" name: ")); Serial.print(navi_ziel_name);
					pDebug->sprintp(PSTR(" rlg: ")); Serial.println(navi_ziel_rl);
				};
				found=true;
				break;
			};
			i++; // zeilen nummer
		}
		if(NAVI_DEBUG){ Serial.print(i); pDebug->sprintlnp(PSTR(" Datensaetze durchsucht.")); };
		if(!found){
			if(NAVI_DEBUG){ pDebug->sprintlnp(PSTR("soviele punkte gibbet nicht")); };
			navi_point=i-1;
			if(navi_point<0) navi_point=0;
			generate_new_order(); // das hier ist das zurücksetzen, wenn man über den letzten Punkt hinweg klickt
		};
		file.close();
	} else { // sd datei nicht gefunden
		sprintf(navi_ziel_name,"SD failed!");
		if(SD_DEBUG){
			pDebug->sprintlnp(PSTR("Konnte Navigations Daten nicht laden"));
		};
	};
	free(navi_filename);
	subdir.close();
	root.close();
};

/****************************************************************
 * Wird in der main loop aufgerufen,
 * quasi als "zeig mir alle werte die ich so kenne" - ding
 ***************************************************************/
void speedo_gps::loop(){
	for(int i=0;i<8;i++){ // ob das so schlau ist, 3 mal get_info aufzurufen ?!
		if(pSpeedo->disp_zeile_bak[i]!=get_info(i)){ // nur aktualisiern wenn sich was ändert
			char buffer[10];
			switch(i){
			case 0: sprintf(buffer,"%06lu",get_info(i)); break;
			case 1: sprintf(buffer,"%06lu",get_info(i)); break;
			case 2: sprintf(buffer,"%09lu{",get_info(i)); break;
			case 3: sprintf(buffer,"%09lu{",get_info(i)); break;
			case 4: sprintf(buffer,"%05lu dm",get_info(i)); break;
			case 5: sprintf(buffer,"%03i km/h",int(get_info(i))); break;
			case 6: sprintf(buffer,"#%i | %i",int(get_info(i)),int(get_info(8))); break;
			case 7: sprintf(buffer,"%i",gps_write_status); break; // fix
			};
			pOLED->string(pSpeedo->default_font,buffer,10,i,0,DISP_BRIGHTNESS,0);
			pSpeedo->disp_zeile_bak[i]=int(get_info(i));
		}
	};
};


/************************************************
 * Das speichern wird zukünftig die Config klasse übernehmen
 * wir triggern da das write und das wird uns hier aufrufen
 * um die infos zu bekommen
 ****************************************************/
int speedo_gps::get_logged_points(char* buffer,int a){
	gps_write_status=9;
	if(a<=gps_count && a<=gps_count2){
		sprintf(buffer,"%06lu,%06lu,%09lu,%09lu,%03i,%05u,%05lu,%i,%i,%i\n",gps_time[a],gps_date[a],gps_lati[a],gps_long[a],gps_speed_arr[a],gps_course[a],gps_alt[a],gps_sats[a],gps_fix[a],gps_special[a]);
		written_gps_points++;
		return 0;
	} else {
		return -1;
	};
};


void speedo_gps::SendByte(unsigned char data){
	while((UCSR1A & (1<< UDRE1)) == 0) {};
	UDR1=data;
};

/*! \brief Sends a string.
 *
 *  Loops thru a string and send each byte with uart_SendByte.
 *  If TX buffer is full it will hang until space.
 *
 *  \param Str  String to be sent.
 */
void speedo_gps::SendString(const char Str[])
{
	unsigned char n = 0;
	while(Str[n])
		SendByte(Str[n++]);
}
