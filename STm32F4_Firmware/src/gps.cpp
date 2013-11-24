
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
GPS::GPS(){
	// parsen
	gps_buffer1[0]='*'; // leere internen buffer
	gps_buffer2[0]='*'; // '*' ist endchar fuer gps signale, super "filler"

	ringbuf_counter=0;
	first_dataset=true;
	gps_ready1=false;
	gps_ready2=false;
	gps_state=0;
	written_gps_points=0;
	gps_fix_temp=0;
	gps_sats_temp=0;
	gps_alt_temp=0;
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
	// speichern

	// Debug
	gps_write_status=0;
	speed=0;

	motion_start=-1;
	active_file=0; //default datei navi0.smf

	navi_ziel_lati=0;
	navi_ziel_long=0;
	navi_ziel_rl=0;
	navi_point=0;

	gps_timestamp=0;
	for(int i=0; i<30; i++){
		gps_speed_arr[i]=0;
		gps_course[i]=0;
		gps_sats[i]=0;
		gps_fix[i]=0;
		gps_special[i]=0;
		gps_alt[i]=0;
		gps_date[i]=0;
		gps_time[i]=0;
	}

	use_compressed_log_format=false; // reduces filesize to 24%

	gps_goody.latitude=0;
	gps_goody.longitude=0;
	gps_lati_cos=100;							// current location weight factor
	checksum=0x00;
};

GPS::~GPS(){
};


void GPS::init(){
	// interrupt + UART

#define BAUDRATE_9600 9600
#define BAUDRATE_19200 19200
#define BAUDRATE_115200 115200
	// set own UART datarate to 19200 @16MHz clock
	// $GPRMC,191410,A,4735.5634,N,00739.3538,E,0.0,0.0,181102,0.4,E,A*19\0D\0A$GPGGA,191410,4735.5634,N,00739.3538,E,1,04,4.4,351.5,M,48.0,M,,*45\0D\0A
	// At 100ms spacing: 141*10 = byte per sec
	// 1410 byte per sec * 10(baud/byte) = 14100 baud -> use 19200
	Serial.init(USART2,BAUDRATE_19200);
	Serial.puts_ln(USART1,("GPS init done"));
};


int GPS::check_vars(){
	if(navi_active){
		generate_new_order();
	}
	return 0;
};

bool GPS::wait_on_gps(){
	return first_dataset;
}

void GPS::update_rate_1Hz(){
	Serial.puts(USART2,"$PMTK220,1000*1F\r\n");
	Serial.puts(USART2,"$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
}

void GPS::update_rate_10Hz(){
	Serial.puts(USART2,"$PMTK220,100*2F\r\n");
	Serial.puts(USART2,"$PMTK314,0,1,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n");
}


void GPS::reconfigure(){
	//Serial.puts_ln(USART1,"GPS failed, reconfiguring");
	// Berechnungen
	unsigned long baud_rates[3]={9600,19200,115200};
	for(uint8_t i=0;i<sizeof(baud_rates)/sizeof(baud_rates[0]);i++){
		Serial.init(USART2,baud_rates[i]);
		Serial.puts(USART2,"$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
		// set GPS UART datarate to 19200:
		Serial.puts(USART2,"$PMTK251,19200*22\r\n");
		_delay_ms(100); // time needed to send the last uint8_t
	}

	Serial.init(USART2,BAUDRATE_19200);
}

// wird als interrupt aufgerufen
// prüft ob daten da sind, die valid sind
// wenn ja dann wird get_GPS damit aufgerufen
void GPS::recv_data(uint8_t byteGPS){
	//	Serial.puts(USART1,(char)byteGPS); // remove me
	switch(gps_state){
	case 0:  // hier sitzen wir und warten auf das startzeichen
		if(byteGPS=='$'){
			gps_state=1;
			checksum=0x00;
		};
		break;
	case 1:  // das wird dann ein "G" sein
		gps_state=2;
		checksum^=byteGPS;
		break;
	case 2: // ein "P"
		gps_state=3;
		checksum^=byteGPS;
		break;
	case 3: // hier wirds interessant: isses ein ein R oder G ?
		if(byteGPS=='R'){
			gps_state=4;
			checksum^=byteGPS;
		} else if(byteGPS=='G'){
			gps_state=14; // hier mal einfach um 10 weiterspringen
			checksum^=byteGPS;
		} else {
			gps_state=0;
		};
		break;
	case 4: // auch noch interessant, hier müsste auf das R ein M folgen
		if(byteGPS=='M'){ // jaha
			gps_state=5;
			checksum^=byteGPS;
		} else {			// schade, doch nüscht
			gps_state=0;
		}
		break;
	case 5: // soweit gut $GPRM fehlt das C
		if(byteGPS=='C'){// jaha, ab in den "lese modus"
			ringbuf_counter=0; // position in die geschrieben werden soll
			gps_state=6;
			checksum^=byteGPS;
		} else			// schade, doch nüscht
			gps_state=0;
		break;
	case 6:
		if(ringbuf_counter>=SERIAL_BUFFER_SIZE-1){ gps_state=0; break; }    // überlauf, kein gps wert ist länger als 75 Byte. Mist
		gps_buffer1[ringbuf_counter]=byteGPS;   // in den buffer schmei�?en
		if(byteGPS=='*'){                // * das ist mein end signal dann is ende im gelände und abmarsch
			gps_state=7;
		} else if(byteGPS=='$'){ // damn da haben wir was verpasst
			checksum=0x00;
			gps_state=1; // jump start
		} else { // es scheint also ein ganz normales zeichen zu sein, dann könnte nur noch der hier schief gehen
			ringbuf_counter++;
			checksum^=byteGPS;
		}
		break;
	case 7: // checksum
		if(byteGPS>='A' && byteGPS<='F'){
			byteGPS=byteGPS-'A'+10;
		} else {
			byteGPS=byteGPS-'0';
		}
		checksum-=(byteGPS<<4);
		gps_state=8;
		break;
	case 8:
		if(byteGPS>='A' && byteGPS<='F'){
			byteGPS=byteGPS-'A'+10;
		} else {
			byteGPS=byteGPS-'0';
		}

		if(byteGPS==checksum){
			gps_ready1=true;
		}
		gps_state=0;
		break;
	case 14: // auch noch interessant, hier müsste auf das GPG ein G und dann ein A folgen
		if(byteGPS=='G'){ // jaha
			gps_state=15;
			checksum^=byteGPS;
		} else			// schade, doch nüscht
			gps_state=0;
		break;
	case 15: // jetzt fehlt nur noch das A
		if(byteGPS=='A'){ // jaha
			ringbuf_counter=0; // position in die geschrieben werden soll
			gps_state=16;
			checksum^=byteGPS;
		} else			// schade, doch nüscht
			gps_state=0;
		break;
	case 16:
		if(ringbuf_counter>=SERIAL_BUFFER_SIZE){ gps_state=0; break; }    // überlauf, kein gps wert ist länger als 75 Byte. Mist
		gps_buffer2[ringbuf_counter]=byteGPS;   // in den buffer schmei�?en
		if(byteGPS=='*'){                // * das ist mein end signal dann is ende im gelände und abmarsch
			gps_state=17;
		} else if(byteGPS=='$'){ // damn da haben wir was verpasst
			gps_state=1; // jump start
			checksum=0x00;
		} else { // es scheint also ein ganz normales zeichen zu sein, dann könnte nur noch der hier schief gehen
			ringbuf_counter++;
			checksum^=byteGPS;
		}
		break;
	case 17: // checksum
		if(byteGPS>='A' && byteGPS<='F'){
			byteGPS=byteGPS-'A'+10;
		} else {
			byteGPS=byteGPS-'0';
		}
		checksum-=(byteGPS<<4);
		gps_state=18;
		break;
	case 18:
		if(byteGPS>='A' && byteGPS<='F'){
			byteGPS=byteGPS-'A'+10;
		} else {
			byteGPS=byteGPS-'0';
		}

		if(byteGPS==checksum){
			gps_ready2=true;
		}
		gps_state=0;
		break;
		// end case 
	}
};

/* diese funktion wird in jeder for schleife abgegra�?t und fall hier ein flag auf true steht
 * werden die daten geparst
 */
void GPS::check_flag(){
	if(gps_ready1){
		// debug
#ifdef GPS_DEBUG
		Serial.puts_ln(USART1,("calling get_gps buffer1:"));
		Serial.puts_ln(USART1,(char*)gps_buffer1);
#endif
#ifdef TACHO_SMALLDEBUG
		Serial.puts(USART1,("-g"));
#endif
		// debug
		parse(gps_buffer1,1);         // Daten übergeben
		gps_ready1=false;
#ifdef TACHO_SMALLDEBUG
		Serial.puts_ln(USART1,("."));
#endif
	};

	if(gps_ready2){
		// debug
#ifdef GPS_DEBUG
		Serial.puts_ln(USART1,("calling get_gps buffer2:"));
		Serial.puts_ln(USART1,(char*)gps_buffer2);
#endif
#ifdef TACHO_SMALLDEBUG
		Serial.puts(USART1,("-g"));
#endif
		// debug
		parse(gps_buffer2,2);         // Daten übergeben
		gps_ready2=false;
#ifdef TACHO_SMALLDEBUG
		Serial.puts_ln(USART1,("."));
#endif
	};
};

/* parsen von nmea daten zu gps daten
 * im dümmsten fall haben wir die beiden Datensätze direkt hinter einander
 * dann haben wir durch die Statemashine 2 Byte für die Prüfsumme und 6 Byte für die anfangssequnz bis wir
 * in den gleichen serial_buffer reinschreiben
 * bei 4800 Baud sind das 1,6 ms für den Spass hier unten .. nicht gerade üppig
 * datensatz=1 => $GPRMC
 */
void GPS::parse(uint8_t *linea,int datensatz){
	int cont=0;
	uint8_t status;
	int indices[13]; // positionsmarker für die kommata

	//debug
#ifdef GPS_DEBUG
	Serial.puts_ln(USART1,("get_gps hat bekommen: "));
	for(int i=0;i<SERIAL_BUFFER_SIZE;i++){
		Serial.puts(USART1,(char)linea[i]);
	};
	Serial.puts_ln(USART1,"");
#endif
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
		/*
	        $GPRMC,191410,A,4735.5634,N,00739.3538,E,0.0,0.0,181102,0.4,E,A*19
				   ^      ^ ^           ^            ^   ^   ^      ^     ^
				   |      | |           |            |   |   |      |     |
				   |      | |           |            |   |   |      |     Neu in NMEA 2.3:
				   |      | |           |            |   |   |      |     Art der Bestimmung
				   |      | |           |            |   |   |      |     A=autonomous (selbst)
				   |      | |           |            |   |   |      |     D=differential
				   |      | |           |            |   |   |      |     E=estimated (geschätzt)
				   |      | |           |            |   |   |      |     N=not valid (ungültig)
				   |      | |           |            |   |   |      |     S=simulator
				   |      | |           |            |   |   |      |
				   |      | |           |            |   |   |      Missweisung (mit Richtung)
				   |      | |           |            |   |   |
				   |      | |           |            |   |   Datum: 18.11.2002
				   |      | |           |            |   |
				   |      | |           |            |   Bewegungsrichtung in Grad (wahr)
				   |      | |           |            |
				   |      | |           |            Geschwindigkeit über Grund (Knoten)
				   |      | |           |
				   |      | |           Längengrad mit (Vorzeichen)-Richtung (E=Ost, W=West)
				   |      | |           007° 39.3538' Ost
				   |      | |
				   |      | Breitengrad mit (Vorzeichen)-Richtung (N=Nord, S=Süd)
				   |      | 46° 35.5634' Nord
				   |      |
				   |      Status der Bestimmung: A=Active (gültig); V=void (ungültig)
				   |
				   Uhrzeit der Bestimmung: 19:14:10 (UTC-Zeit)
		 *
		 */
		status=linea[indices[1]+1];

		// zeit in jedem fall berechnen, auch wenn das sample nicht valid ist, das ist der RTC
		long temp_gps_time=0;
		long temp_gps_date=0;
		for (int j=indices[0]+1;j<indices[1];j++){ // format 234500.000 für füddelvorzwölf
			if(linea[j]>='0' && linea[j]<='9'){
				temp_gps_time=temp_gps_time*10+(linea[j]-'0');
			}
		}

		for (int j=indices[8]+1;j<(indices[9]) && linea[j]>='0' && linea[j]<='9';j++){
			temp_gps_date=temp_gps_date*10+(linea[j]-'0'); //181102 für 18.Nov 2002
		}

		if(first_dataset || status=='A'){ // wenn die daten gültig sind oder es der erste datensatz ist
			if(temp_gps_date>=010100 && temp_gps_date<=311299 && temp_gps_date%100!=180){ // 0X0180 is power up date <- ignore that
				Sensors.mClock.set_date_time(
						int(mod(temp_gps_date,100)),
						int(floor(temp_gps_date/100))%100,
						int(floor(temp_gps_date/10000)),
						int(floor(temp_gps_time/10000000))%24, 						// 234512000 für füddelvorzwölf -> 234512000/10000000 = 23 -> 23%24=23
						int(floor(temp_gps_time/100000))%100,  						// 234512000 für füddelvorzwölf -> 234512000/100000 = 2345 -> 2345%100=45
						int(mod((unsigned long)(floor(temp_gps_time/1000)),100)), 	// 234512000 für füddelvorzwölf -> 234512000/1000 = 234512 -> 234512 mod 100 = 12 // std "%" can be use case 23.45.12 > 32768
						first_dataset
				);
			};

			if(first_dataset){
				//Serial.puts_ln(USART1,"GPS connected, configuring data");
				Config.day_trip_check();
				//configure GPS
				update_rate_1Hz();
				first_dataset=false;
			}
		};

		if(status=='A'){ // only evaluate if valid (A means valid)
			valid=0;
			if(gps_count<(signed(sizeof(gps_time)/sizeof(gps_time[0])-1))){ // [0] .. [29] = 30 Einträge
				gps_count++;
			}
			// alle leeren
			gps_long[gps_count]=0;
			gps_lati[gps_count]=0;
			gps_speed_arr[gps_count]=0;
			gps_course[gps_count]=0;
			gps_special[gps_count]=note_this_place;
			set_gps_mark(STD_MARK); // reset
			gps_time[gps_count]=temp_gps_time;
			gps_date[gps_count]=temp_gps_date;
			// calc some nice values
			gps_timestamp=GpsTimeToTimeStamp(temp_gps_time);

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

			gps_alt[gps_count]=gps_alt_temp;
			gps_sats[gps_count]=gps_sats_temp;
			gps_fix[gps_count]=gps_fix_temp;

			// notify other modules
			if(Menu.state==11 || Menu.state==M_TOUR_ASSISTS*100+SM_TOUR_ASSISTS_SPEEDCAM_STATUS*10+1){ // regular speedo or "show" mode
				if(SpeedCams.get_active()){
					SpeedCams.set_gps_outdated();
				}
			}


			// debug
#ifdef GPS_DEBUG
			Serial.puts(USART1,("Time in UTC (HhMmSs): "));
			Serial.puts_ln(USART1,gps_time[gps_count]);
			Serial.puts(USART1,("Date UTC (DdMmAa) "));
			Serial.puts_ln(USART1,gps_date[gps_count]);
			Serial.puts(USART1,("Heading in degrees:(*10)"));
			Serial.puts_ln(USART1,gps_course[gps_count]);
			Serial.puts(USART1,("Latitude: "));
			Serial.puts_ln(USART1,gps_lati[gps_count]);
			Serial.puts(USART1,("Longitude: "));
			Serial.puts_ln(USART1,gps_long[gps_count]);
			Serial.puts(USART1,("Speed kmh: "));
			Serial.puts_ln(USART1,gps_speed_arr[gps_count]);
#endif
			//Debug
			// uiuiuiuiuiuiui
#ifdef NAVI_DEBUG
			Serial.puts(USART1,("Zeit vorm gps_navi: "));
			Serial.puts_ln(USART1,Millis.get());
#endif
			if(navi_active){ // only calc if needed
				calc_navi();
			}

#ifdef NAVI_DEBUG
			Serial.puts(USART1,("Zeit nach gps_navi: "));
			Serial.puts_ln(USART1,Millis.get());
#endif
			// uiuiuiuiuiuiui
		}
		set_drive_status(speed,int(floor(temp_gps_time/1000))%100,gps_sats[gps_count],status);
	} // anderer modus, gpgga empfangen
	else if(datensatz==2){ // altitude, fix ok?,sats,
		/* GPS Datensatz 2:
        $GPGGA,191410,4735.5634,N,00739.3538,E,1,04,4.4,351.5,M,48.0,M,,*45
			   ^      ^           ^            ^ ^  ^   ^       ^
			   |      |           |            | |  |   |       |
			   |      |           |            | |  |   |       Höhe Geoid minus
			   |      |           |            | |  |   |       Höhe Ellipsoid (WGS84)
			   |      |           |            | |  |   |       in Metern (48.0,M)
			   |      |           |            | |  |   |
			   |      |           |            | |  |   Höhe über Meer (über Geoid)
			   |      |           |            | |  |   in Metern (351.5,M)
			   |      |           |            | |  |
			   |      |           |            | |  HDOP (horizontal dilution
			   |      |           |            | |  of precision) Genauigkeit
			   |      |           |            | |
			   |      |           |            | Anzahl der erfassten Satelliten
			   |      |           |            |
			   |      |           |            Qualität der Messung
			   |      |           |            (0 = ungültig)
			   |      |           |            (1 = GPS)
			   |      |           |            (2 = DGPS)
			   |      |           |            (6 = geschätzt nur NMEA-0183 2.3)
			   |      |           |
			   |      |           Längengrad
			   |      |
			   |      Breitengrad
			   |
			   Uhrzeit
		 *
		 *	fix=Qualität der Messung (zwischen dem 5. und 6. ",")
		 *	sats=Anzahl der erfassten Satelliten (zwischen dem 6. und 7. ",")
		 *
		 */
		gps_alt_temp=0;
		gps_sats_temp=0;
		for (int j=indices[5]+1;j<(indices[6]);j++){
			gps_fix_temp=(linea[j]-48);
		};
		if(gps_fix_temp>0 && valid<5){ //innerhalb der letzten 5 sec ein valid bekommen && fix>0
			for (int j=indices[6]+1;j<(indices[7]);j++){
				gps_sats_temp=gps_sats_temp*10+(linea[j]-48);
			}
			for (int j=indices[8]+1;j<(indices[9]);j++){
				if(linea[j]!=46){
					gps_alt_temp=gps_alt_temp*10+(linea[j]-48);
				};
			}
			if(gps_alt_temp>100000){ // sind wir höher als 10 km?
				gps_alt_temp=0;
			}
		};

		//debug
#ifdef GPS_DEBUG
		Serial.puts(USART1,("alt: "));
		Serial.puts_ln(USART1,gps_alt_temp);
		Serial.puts(USART1,("sats: "));
		Serial.puts_ln(USART1,gps_sats_temp);
		Serial.puts(USART1,("fix? 1==ja: "));
		Serial.puts_ln(USART1,gps_fix_temp);
#endif
		//debug
	};
	if(gps_count>=29){ // save every 30 datapoints as burst ... compromise between RAM usage and burst size

#ifdef SD_DEBUG
		Serial.puts_ln(USART1,("Vor store_to_sd()"));
		Serial.puts_ln(USART1,Millis.get());
#endif

		gps_write_status=1;
		if(Sensors.get_RPM(RPM_TYPE_DIRECT)>0){
			if(store_to_sd()>=0){ //to save 30 datapoints about 70ms SD write time are needed
				gps_count=-1; // after writing the points to sd => erase them, parse will count up to 0 for us
			}
		};

#ifdef SD_DEBUG
		Serial.puts_ln(USART1,("nach store_to_sd()"));
		Serial.puts_ln(USART1,Millis.get());
#endif
	} else {
#ifdef SD_DEBUG
		Serial.puts(USART1,("store_to_sd() not enough records "));
		Serial.puts(USART1,gps_count);
		Serial.puts(USART1," written ");
		Serial.puts(USART1,written_gps_points);
		Serial.puts(USART1," datensatz ");
		Serial.puts(USART1,datensatz);
		Serial.puts(USART1," ");
		Serial.puts_ln(USART1,Millis.get());
#endif
	}
};

// return values of the gps_field, convertet as long
long GPS::get_info(uint8_t select){
	int inner_gps_count=gps_count;
	if(gps_count==-1){
		inner_gps_count=29;
	};

	switch(select){
	case 0:
		return floor(gps_time[inner_gps_count]/1000);
		break;
	case 1:
		return gps_date[inner_gps_count];
		break;
	case 2:
#ifdef GPS_FAKE_MODE
		//return 9456000+(int)(floor(Millis.get()/50))%200;
		return 6960577;
#endif
		return gps_long[inner_gps_count];
		break;
	case 3:
#ifdef GPS_FAKE_MODE
		//return 52123000+(int)(floor(Millis.get()/50))%200;
		return 50937661;
#endif
		return gps_lati[inner_gps_count];
		break;
	case 4:
		return gps_alt[inner_gps_count];
		break;
	case 5:
		return long(gps_speed_arr[inner_gps_count]);
		break;
	case 6:
#ifdef GPS_FAKE_MODE
		return 6;
#endif
		if(valid>=3){ return 0; };
		if(long(gps_sats[inner_gps_count])>20) { return 20; }; //höhö
		if(long(gps_sats[inner_gps_count])<0) { return 0; };
		return long(gps_sats[inner_gps_count]);
		break;
	case 7:
		return long(gps_fix[inner_gps_count]);
		break;
	case 8:
		return long(gps_course[inner_gps_count]);
		break;
	case 9:
#ifdef GPS_FAKE_MODE
		return 0; //todo
#endif
		return valid;
		break;
	case 10:
#ifdef GPS_FAKE_MODE
		return floor(Millis.get()/100)*100; //
#endif
		return gps_time[inner_gps_count];
		break;
	case 11:
#ifdef GPS_FAKE_MODE
		return floor(Millis.get()/100)*100; //
#endif
		return gps_timestamp;
	}
	return -1;
};

// alle 30 gültigen gps sample einmalauf der SD speichern bitte
// im kolja_gps format -> yeah blo�? nix bekanntes, kompatibles verwenden, alles properitär machen :D
int GPS::store_to_sd(){
	// generate filename && store
	uint8_t *filename;
	filename = (uint8_t*) malloc (11);
	if (filename==NULL) Serial.puts_ln(USART1,("Malloc failed"));
	else memset(filename,'\0',11);

	// 20.3.2011 => 110320
	if(Sensors.mClock.get_long_date()!=999999){	// TODO
		sprintf((char*)filename,"%06lu.GPS",Sensors.mClock.get_long_date());
#ifdef SD_DEBUG
		Serial.puts_ln(USART1,("filename is "));
		Serial.puts_ln(USART1,filename);
#endif
	} else {
		return -1;
	}


	gps_write_status=2;
	// das kann auch ruhig alle 30 sec gemacht werden
	Config.storage_outdated=true; // damit geschrieben werden darf
	//	gps_write_status=3;
	//	pConfig->write(filename); // schreiben

	free(filename);
	//done
	return 0;
};

/********* berechnung der distanz ***********
 * eingabe als ul in dezimalgrad*10^6 => 9473208.0;
 * aktuelle Position ist im array gps_lati/gps_long
 * LATI ist immer !! 111km pro Grad und ist quasi die y koodinate
 * LONG ist x Richtung und im Abstand ist abhängig von der Latitude, bei 0 ist er auch 111km bei 90° = 0km
 * Ausgabe als meter! och dann können wir maximal ein paar mio km fahren, wirs nix mit zum mars ..
 * Laufzeit ca 757µs bei 16MHz = 12112 Takte .. puh
 ********************************************/
unsigned long GPS::calc_dist(unsigned long longitude,unsigned long latitude){
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
		// hier kommen grad*106 raus
		dist_x=(float)(actual_long - longitude); // erstmal ungewichtet
	} else {
		dist_x=(float)(longitude - actual_long); // erstmal ungewichtet
	}

	dist_x=round(dist_x*cos(floor((latitude+actual_lati)/2000000.0)*2*M_PI/360)); // gewichtet
	return round((unsigned long)sqrt((dist_x*dist_x)+(dist_y*dist_y))*0.111);// berechne distanz
};
// get distance between aktueller und ziel punkt

// overloaded function  - see below
unsigned long GPS::calc_dist_supported(simple_coordinate A){
	return calc_dist_supported(gps_goody,A,gps_lati_cos,false);
}

// overloaded function  - see below
unsigned long GPS::calc_dist_supported(simple_coordinate A,bool return_square){
	return calc_dist_supported(gps_goody,A,gps_lati_cos,return_square);
}

// 3000 calc = 397ms  ==> 1 calc ~ 132 us == 2112 Ticks
unsigned long GPS::calc_dist_supported(simple_coordinate A,simple_coordinate B,float gps_lati_cos,bool return_square){

	float dist_y,dist_x; // abstands vars
	// dont use abs() its unsigned
	if( B.latitude > A.latitude){
		dist_y=(float)(B.latitude - A.latitude); // direkt
	} else {
		dist_y=(float)(A.latitude - B.latitude); // direkt
	}
	// dont use abs() its unsigned
	if( A.longitude > B.longitude){
		// hier kommen grad*106 raus
		dist_x=(((float)(A.longitude - B.longitude))*gps_lati_cos)/100; // gewichtet, GPS_lati_cos is *100
	} else {
		dist_x=(((float)(B.longitude - A.longitude))*gps_lati_cos)/100; // gewichtet, GPS_lati_cos is *100
	}

	//	if(return_square){
	//		return ((unsigned long)(((unsigned long)dist_x*(unsigned long)dist_x)+((unsigned long)dist_y*(unsigned long)dist_y)));// berechne distanz ohne *0.111 and without sqrt ... only for comparison
	//	};
	return round((unsigned long)sqrt((dist_x*dist_x)+(dist_y*dist_y))*0.111);// berechne distanz
};

/*******************************************************************
 * kern Funktion des gps,
 * gobale winkel var für den zeiger berechnen und vor allem
 * feststellen ob wir die ziel koodinaten erreicht haben
 * falls ja werden die funktionen für "lad das nächste" angetippt
 *******************************************************************/
void GPS::calc_navi(){
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
	winkel=winkel%360; // falls zu gro�? geworden
	//Serial.puts(USART1,"Von meiner Position: "); Serial.puts(USART1,nmea_to_dec(float(gps_lati[gps_count]))); Serial.puts(USART1,nmea_to_dec(float(gps_long[gps_count])));
	//Serial.puts(USART1," befindet sich das Ziel "); Serial.puts(USART1,navi_ziel_lati); Serial.puts(USART1,navi_ziel_long);
	//Serial.puts(USART1," in einem 0-Winkel von "); Serial.puts_ln(USART1,gps_winkel);
	//Soll fahrt Winkel - Fahrt winkel ergibt winkel im vergleich zu gerade aus ...
	winkel-=gps_course[gps_count];
	if(winkel<0){
		winkel+=360;
	};


	// debug
#ifdef NAVI_DEBUG
	Serial.puts(USART1,("Aktuelle lati: "));
	Serial.puts(USART1,nmea_to_dec(float(gps_lati[gps_count])));
	Serial.puts(USART1,(" Ziel Lati "));
	Serial.puts_ln(USART1,navi_ziel_lati);
	Serial.puts(USART1,("Aktuelle long: "));
	Serial.puts(USART1,nmea_to_dec(float(gps_long[gps_count])));
	Serial.puts(USART1,(" Ziel long "));
	Serial.puts_ln(USART1,navi_ziel_long);
	Serial.puts(USART1,("Entfernung in km: "));
	Serial.puts_ln(USART1,entf);
#endif
	// debug
	// die gro�?e Frage des Weiterschaltens
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
	// die gro�?e Frage des Weiterschaltens
};

// nmea format (9 47 3208 => 9 788680) umrechnen, eingabe als ul grad+6"nachkommastellen" ohne punkt getrennt
unsigned long GPS::nmea_to_dec(unsigned long nmea){
	return floor(nmea/1000000.0)*1000000+round(mod(nmea,1000000.0)*10/6);
}

// gibt kein mod für ul ? son müll!
unsigned long GPS::mod(unsigned long zahl,unsigned long teiler){
	return (unsigned long)(zahl-(floor(zahl/teiler)*teiler));
};

// gib distanz,richtung,stra�?e zurück
int GPS::get_order(uint8_t char_buffer[], int* dist){
	// wenn kein gps signal am start war für mehr als 15 sec
	char_buffer[0]='\0'; // leeren
	if(valid>15){
		return -1;
	};
	int value=0;
	int value2=0;

	sprintf((char*)char_buffer,"! calc failed !"); // das hier ist vorgabe, damit da nicht "no gps" stehen bleibt obwohl wir gps haben und nur die berechnung abgeschmiert ist

	if(entf>=10000){ // mehr als 10 km
		value=int(floor(entf/1000)); // wert als km
		//3+2+1+1+1+10+1+2=21+'\0'=22 -- 25 max
		sprintf((char*)char_buffer,"%3ikm %c            %02i",value,(125+navi_ziel_rl),navi_point%100); // max 999km
	}
	else if(entf>=1000){ // mehr als 1 km
		value=int(floor(entf/1000)); // wert als km
		value2=int(floor((entf%1000)/100)); // wert als km
		sprintf((char*)char_buffer,"%i.%ikm %c            %02i",value,value2,(125+navi_ziel_rl),navi_point%100); // max 9.9km
		value=value*10+value2;
	}
	else {  // weniger als 1000 m
		value=int(floor(entf/10)*10); // auf 10m runterbrechen
		sprintf((char*)char_buffer," %3im %c            %02i",value,(125+navi_ziel_rl),navi_point%100); //max 999m
	};
	for(unsigned int i=0;i<10 && i<strlen((char*)navi_ziel_name); i++){
		char_buffer[i+8]=navi_ziel_name[i];
	};

	*dist=value;
	return 0;
};


void GPS::set_gps_mark(int type){
	note_this_place=type;
}

// die globalen variablen neu befüllen, auf basis des zuvor gesetzten navi_point
void GPS::generate_new_order(){ // eine neue Order auslesen
	// da diese funktion nur aufgerufen wird wenn vorher der navi_pointer geändert wurde speichern wir hier den pointer um das so selten wie nötig zu machen
	byte tempByte = (navi_point & 0xFF);
	//	eeprom_write_byte((uint8_t *)147,tempByte);

	//////////// DEBUG /////////////////
#ifdef NAVI_DEBUG
	Serial.puts_ln(USART1,("Versuche NAVI.SMF zu oeffnen"));
	pConfig->ram_info();
#endif
	//////////// DEBUG /////////////////


	uint8_t navi_filename[13];
	sprintf((char*)navi_filename,"/%s/NAVI%i.SMF",NAVI_FOLDER,active_file%100);

	//////////// DEBUG /////////////////
#ifdef NAVI_DEBUG
	Serial.puts_ln(USART1,navi_filename);
	Serial.puts(USART1,("Punkt:"));
	Serial.puts_ln(USART1,navi_point);
#endif
	//////////// DEBUG /////////////////

	uint8_t buf[36];// immer 35 byte lesen, das ist genau eine anweisung (wobei das array wieder ein Feld grö�?er sein muss)
	// die ersten bytes sind sowas wie der titel, die können wir zur navigation nicht nutzen, weglesen
	uint16_t zeilen;
	int16_t result=SD.get_line_n(navi_point+1,navi_filename,buf,sizeof(buf),&zeilen);
	if(result>=0){ // 0=found
		navi_ziel_lati=0;
		navi_ziel_long=0;

		// lati/long einlesen
		for(int a=0;a<=8;a++){ // vorwärts
			//sprintf(serialbuffer,"Ich lese ein: %c und mache daraus %i",buf[a],buf[a]-48);
			//Serial.puts_ln(USART1,serialbuffer);
			navi_ziel_lati=navi_ziel_lati*10+(int(buf[a])-48);
			navi_ziel_long=navi_ziel_long*10+(int(buf[a+10])-48);
		};
		// ziel name
		for(int a=0;a<=9;a++){ // forwards -> 128/6=21  : 2.1km g helmholtzs 11 --> 10 buchstaben
			navi_ziel_name[a]=buf[a+22];
		};
		navi_ziel_name[10]='\0';

		// r oder l oder g
		navi_ziel_rl=int(buf[20])-48; // gradeaus(93+0x20=125),links,rechts: g=125+('0'-48),l=125+('1'-48)

		//////////// DEBUG /////////////////
#ifdef NAVI_DEBUG
		Serial.puts(USART1,("Long: ")); Serial.puts(USART1,navi_ziel_long); Serial.puts(USART1,(" lati: ")); Serial.puts(USART1,navi_ziel_lati);
		Serial.puts(USART1,(" name: ")); Serial.puts(USART1,navi_ziel_name);
		Serial.puts(USART1,(" rlg: ")); Serial.puts_ln(USART1,navi_ziel_rl);
#endif
		//////////// DEBUG /////////////////
	}else if(result==1){ // 1=less lines in file than you think my dear

		//////////// DEBUG /////////////////
#ifdef NAVI_DEBUG
		Serial.puts_ln(USART1,("soviele punkte gibbet nicht"));
#endif
		//////////// DEBUG /////////////////

		navi_point=zeilen-2; // set to max
		if(navi_point<0) navi_point=0; // hmm reason to stop?
		generate_new_order(); // das hier ist das zurücksetzen, wenn man über den letzten Punkt hinweg klickt

	} else { // sd datei nicht gefunden
		sprintf((char*)navi_ziel_name,"SD failed!");

		//////////// DEBUG /////////////////
#ifdef SD_DEBUG
		Serial.puts_ln(USART1,("Konnte Navigations Daten nicht laden"));
#endif
		//////////// DEBUG /////////////////
	};
};

/****************************************************************
 * Wird in der main loop aufgerufen,
 * quasi als "zeig mir alle werte die ich so kenne" - ding
 ***************************************************************/
void GPS::loop(){
	for(int i=0;i<=8;i++){ // ob das so schlau ist, 3 mal get_info aufzurufen ?!
		if(Speedo.disp_zeile_bak[i]!=(int16_t)(get_info(i))){ // nur aktualisiern wenn sich was ändert
			uint8_t buffer[10];
			switch(i){
			case 0: sprintf((char*)buffer,"%06lu",get_info(i)); break;
			case 1: sprintf((char*)buffer,"%06lu",get_info(i)); break;
			case 2: sprintf((char*)buffer,"%09lu{",get_info(i)); break;
			case 3: sprintf((char*)buffer,"%09lu{",get_info(i)); break;
			case 4: sprintf((char*)buffer,"%05lu dm",get_info(i)); break;
			case 5: sprintf((char*)buffer,"%03i km/h",int(get_info(i))); break;
			case 6: sprintf((char*)buffer,"#%i | %i",int(get_info(i)),int(get_info(7))); break;
			case 7: sprintf((char*)buffer,"%i",gps_write_status); break; // fix
			case 8: sprintf((char*)buffer,"%03i{",int(get_info(i))); break;
			};
			TFT.string(Speedo.default_font,(char*)buffer,30,i*3+4,0,DISP_BRIGHTNESS,0);
			Speedo.disp_zeile_bak[i]=(int16_t)(get_info(i));
		}
	};
};


/************************************************
 * Das speichern wird zukünftig die Config klasse übernehmen
 * wir triggern da das write und das wird uns hier aufrufen
 * um die infos zu bekommen
 ****************************************************/
int GPS::get_logged_points(uint8_t* buffer,int a,int* nbytes){
	gps_write_status=9;
	if(a<=gps_count){
		if(gps_time[a]>240000000L) gps_time[a]=000000;
		if(gps_date[a]>311299) gps_date[a]=311299;
		if(gps_lati[a]>180000000) gps_lati[a]=0;	// NMEA Format!
		if(gps_long[a]>180000000) gps_long[a]=0;	// NMEA Format!
		if(gps_speed_arr[a]>300) gps_speed_arr[a]=300;
		if(gps_course[a]>3600) gps_course[a]=0;
		if(gps_alt[a]>50000 || gps_alt[a]<50000) gps_alt[a]=50000;
		if(gps_sats[a]>20) gps_sats[a]=20;
		if(gps_fix[a]>1) gps_fix[a]=2;
		if(gps_special[a]>9) gps_special[a]=9;

		if(use_compressed_log_format){
			/* compressed gps format contains just:
			 * gps_time: 23:59:59.900 => 235.959.900 => 4 Byte [1..4]
			 * gps_lati: 009456123 => 4 Byte [5..8]
			 * gps_long: 052144879 => 4 Byte [9..12]
			 * speed: 0..255 + [13,14]
			 * gps_special => 2 Byte <-- gps special is high nibble ob byte 13
			 * 4+4+4+2=14 + 1 (Spacer) = 15 Byte
			 */

			buffer[0]='*';
			buffer[1]=(gps_time[a]&0xff000000)>>24;
			buffer[2]=(gps_time[a]&0x00ff0000)>>16;
			buffer[3]=(gps_time[a]&0x0000ff00)>>8;
			buffer[4]=(gps_time[a]&0x000000ff)>>0;
			buffer[5]=(gps_lati[a]&0xff000000)>>24;
			buffer[6]=(gps_lati[a]&0x00ff0000)>>16;
			buffer[7]=(gps_lati[a]&0x0000ff00)>>8;
			buffer[8]=(gps_lati[a]&0x000000ff)>>0;
			buffer[9]=(gps_long[a]&0xff000000)>>24;
			buffer[10]=(gps_long[a]&0x00ff0000)>>16;
			buffer[11]=(gps_long[a]&0x0000ff00)>>8;
			buffer[12]=(gps_long[a]&0x000000ff)>>0;
			buffer[13]=(gps_speed_arr[a]&0x0f00)>>8 | (gps_special[a]&0x0f)<<4;
			buffer[14]=(gps_speed_arr[a]&0x00ff)>>0;
			*nbytes=15;
		} else {
			/* std gps format contains:
			 * gps_time: 9 digits, 19:23:45.451 => 192345451
			 * gps_date: 6 digits, 25.7.2013 => 250713
			 * gps_lati: 9 digits, 52.14,2356° => 052142356
			 * gps_long: 9 digits, 9.11,457 => 00911457
			 * gps_speed_arr: 3 digits, 278
			 * gps_course: 5 digits, deci deg "172.3"
			 * gps_alt: 5 digits, decimeter
			 * gps_sats: 2 digits, number of
			 * gps_fix: 1 digit, sec
			 * gps_special: 1 digit, marker 0..9
			 * 9+1+6+1+9+1+9+1+3+1+5+1+5+1+2+1+1+1+1+\n = 62
			 */
			sprintf((char*)buffer,"%09lu,%06lu,%09lu,%09lu,%03i,%05u,%05lu,%02i,%i,%i\n",
					gps_time[a],gps_date[a],gps_lati[a],gps_long[a],gps_speed_arr[a],gps_course[a],gps_alt[a],gps_sats[a],gps_fix[a],gps_special[a]);
			*nbytes=62;
		}

		written_gps_points++;
		return 0;
	} else {
		return -1;
	};
	return -1;
};




int GPS::calc_gps_goodies(){
	if(get_info(9)>5){ return -1; }; // if there is no GPS than calculation is non sense
	gps_goody.latitude=nmea_to_dec(get_info(3));
	gps_goody.longitude=nmea_to_dec(get_info(2));
	gps_lati_cos=cos(floor(gps_goody.latitude/1000000.0)*2*M_PI/360)*100;
	return 0;
}


void GPS::set_drive_status(int speed, int ss, int sat, uint8_t status){
	if(status=='A' && sat>5){ //measurement valid
		if(speed>=15){ // you are driving faster than 15 kmh
			if(motion_start==-1){
				motion_start=(Millis.get()/1000)%32000;
			};
		} else {
			motion_start=-1;
		}
	} else {
		motion_start=-1;
	}
};

bool GPS::get_drive_status(){
	if(motion_start<0){
		return false;
	} else {
		int sec_in_motion=(Millis.get()/1000)-motion_start;
		if(sec_in_motion>5){
			return true;
		}
	}
	return false;
};

unsigned long GPS::GpsTimeToTimeStamp(unsigned long input){
	// input is something like 151932100 at 15:19:32.100
	// output should be 15*3600000+19*60000+32100=55172100
	unsigned int min=(int)floor(input/100000)%100;
	unsigned int hour=floor(input/10000000);
	unsigned int sec_with_frac=input-(unsigned long)((hour*100+min)*100000);

	unsigned long return_value=((unsigned long)(hour*60+min))*60000+sec_with_frac;

	return return_value;
}
