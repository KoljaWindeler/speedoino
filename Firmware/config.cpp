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
#include      <EEPROM.h>
configuration::configuration(){
};

void configuration::ram_info() {
	int size = 8192; // Use 2048 with ATmega328
	byte *buf;
	while ((buf = (byte *) malloc(--size)) == NULL);
	free(buf);
	Serial.print(size);

	pDebug->sprintlnp(PSTR(" Byte heap free"));
}

configuration::~configuration(){

};
/*********** write config *******************
 * hier die config datei schreiben, und anhand des
 * Dateinamens checken welche var hier gespeichert
 * wird. vorher checken ob sd_failed nicht true ist
 *********** write config ******************/
int configuration::write(const char *filename){
	pSensors->m_gps->gps_write_status=4;
	if(!pSD->sd_failed){
		pSensors->m_gps->gps_write_status=5;
		if(storage_outdated){ // jede Minute
			pSensors->m_gps->gps_write_status=6;
			SdFile root;
			SdFile file;
			SdFile subdir;
			root.openRoot(&pSD->volume);
			/*************** SPEEDO.TXT **************************
			 * Write those fast changing things like 
			 * avg, dist, time for each of the 10 storages
			 ****************************************************/
			if(strncmp("speedo.txt",filename,10)==0 && storage_outdated){
				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  pDebug->sprintlnp(PSTR("open subdir /config failed")); return -1; };
				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
					Serial.print("platzhalter");
					return -2;
				} else {
					// get some buffer
					char buffer[35];
					memset(buffer,'\0',35);

					// write heading to file
					strcpy_P(buffer, PSTR("#This is an auto-generated file from (MonDay) "));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_clock->getdate());
					pSD->writeString(file, buffer);
					for(unsigned int i=1;i<sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]);i++){
						if(pSpeedo->max_speed[i]>350) pSpeedo->max_speed[i]=200;
						sprintf(buffer,"max_%i=%i;\n",i+1,pSpeedo->max_speed[i]%1000); // 12 chars max: max_1=300\n\0
						pSD->writeString(file, buffer);
						sprintf(buffer,"avg_time_%i=%lu;\n",i+1,pSpeedo->avg_timebase[i]); // 23 chars max: avg_time_1=4294967295\n\0
						pSD->writeString(file, buffer);
						sprintf(buffer,"trip_%i=%lu;\n\n",i+1,pSpeedo->trip_dist[i]); // 19 chars max: trip_1=4294967295\n\0
						pSD->writeString(file, buffer);
					};

					file.close();
					storage_outdated=false;
				}
				/*************** GANG.TXT **************************
				 * as soon as the user has calibrated at least one gear
				 ****************************************************/
			} else if(strncmp("GANG.TXT",filename,8)==0){
				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  pDebug->sprintlnp(PSTR("open subdir /config failed")); return -1; };
				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
					Serial.print("platzhalter");
					return -2;
				} else {
					// get some buffer
					char buffer[20];
					memset(buffer,'\0',20);

					// write heading to file
					strcpy_P(buffer, PSTR("#This is an auto-generated file from (MonDay) "));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_clock->getdate());
					pSD->writeString(file, buffer);


					for(unsigned int i=1;i<=6;i++){
						sprintf(buffer,"n_gang_%i=%i;\n",i,pSensors->m_gear->n_gang[i]); // 12 chars max: max_1=300\n\0
						pSD->writeString(file, buffer);
					};

					file.close();
					storage_outdated=false;
				}
				/*************** 311211.GPS **************************
				 * this function is triggered by the m_gps Sensor, every 30 sec
				 * we will the other way round ask the m_gps for information
				 * about the logged points as long as they aren't empty
				 ****************************************************/
			} else if(filename[6]=='.' && filename[7]=='G' && filename[8]=='P' && filename[9]=='S'){
				if(!subdir.open(&root, "GPS", O_READ)) {
					pDebug->sprintlnp(PSTR("open subdir /config failed"));
					return -1;
				};
				if (!file.open(&subdir, filename, O_CREAT |  O_APPEND | O_WRITE)){
					Serial.print("platzhalter");
					pSensors->m_gps->gps_write_status=-2;
					return -2;
				} else {
					pSensors->m_gps->gps_write_status=7;
					// get some buffer
					char buffer[60];
					memset(buffer,'\0',60);

					// get the info from the gps class
					int i=0;
					while(i<100 && pSensors->m_gps->get_logged_points(buffer,i)>=0){
						pSD->writeString(file, buffer);
						i++;
					}

					// free buffer and close file
					file.close();
					storage_outdated=false;
				}	
				/*************** BASE.TXT **************************
				 * save the basic stuff like tire outline aka meters
				 * per tick, flasher warning distance and so on
				 ****************************************************/
			} else if(strncmp("BASE.TXT",filename,8)==0){
				if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {
					pDebug->sprintlnp(PSTR("open subdir /config failed"));
					return -1;
				};
				if (!file.open(&subdir, filename, O_CREAT |  O_TRUNC | O_WRITE)){
					Serial.print("platzhalter");
					return -2;
				} else {
					// get some buffer
					char buffer[45];
					memset(buffer,'\0',45);

					// write heading to file
					strcpy_P(buffer, PSTR("#This is an auto-generated file from (MonDay) "));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_clock->getdate());
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("startup="));
					pSD->writeString(file, buffer);
					pSD->writeString(file, pOLED->startup);
					pSD->writeString(file, (char *)";\n");

					strcpy_P(buffer, PSTR("blinker="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_blinker->dist_to_warn%10000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("blinker_high="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_blinker->high_speed_add%10000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("fuel_blink_start="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_fuel->blink_start%1000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("fuel_blink_freq="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_fuel->blink_freq%10000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("reifen_umfang="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i.%i\n",int(floor(pSensors->m_speed->reifen_umfang)),int(floor(pSensors->m_speed->reifen_umfang*100))%100);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("gps_takeover="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_speed->gps_takeover%1000);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("refresh_cyle="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSpeedo->refresh_cycle);
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("active_navi_file="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_gps->active_file%10); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("skin_file="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pConfig->skin_file%10); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("oil_dist="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_oiler->grenze%10000); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("dz_flash="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",pSensors->m_dz->blitz_dz); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("dz_flash_en="));
					pSD->writeString(file, buffer);
					if(pSensors->m_dz->blitz_en){
						sprintf(buffer,"1\n"); // 12 chars max: max_=1=300\n\0
					} else {
						sprintf(buffer,"0\n"); // 12 chars max: max_=1=300\n\0
					}
					pSD->writeString(file, buffer);

					// RGB LEDs
					strcpy_P(buffer, PSTR("rgb_in_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->RGB.inner.r.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_in_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->RGB.inner.g.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_in_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->RGB.inner.b.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->RGB.outer.r.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->RGB.outer.g.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_out_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->RGB.outer.b.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_flash_r="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.r.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_flash_g="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.g.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);

					strcpy_P(buffer, PSTR("rgb_flash_b="));
					pSD->writeString(file, buffer);
					sprintf(buffer,"%i\n",int(pAktors->dz_flasher.b.actual)); // 12 chars max: max_=1=300\n\0
					pSD->writeString(file, buffer);
					// RGB LEDs
					file.close();
					storage_outdated=false;
				};
			};
			subdir.close();
			root.close();
			return 0;
		}; // storeage outdated
	}; // sd_failed
	return -3;
};

/********** check values ********************
 * zumindest in den trip, max und avg nachsehen ob werte
 * drinstehen. Falls die sd karte nicht gelesen werden konnte
 * wird das schon gemeldet, aber falls nur die datei für trips
 * nicht gelesen werden kann sollte man das hier merken weil
 * alle(!) max value=0 sind wie von der initialisierung
 ********** check values ********************/
int configuration::check(){
	int nulls=0;
	for(unsigned int i=2; i<sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]); i++){
		if(pSpeedo->max_speed[i]==-99){
			nulls++;
		}
	};

	if(nulls == (sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]))-2){
		pOLED->clear_screen();
		pOLED->string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,0,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("SD access strange"),1,2,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("all max values==-99"),0,3,0,DISP_BRIGHTNESS,0);
		pOLED->string_P(pSpeedo->default_font,PSTR("!! WARNING !!"),4,7,0,DISP_BRIGHTNESS,0);

		pDebug->sprintp(PSTR("!!!! WARNING !!!!"));
		pDebug->sprintp(PSTR("SD access strange"));
		pDebug->sprintp(PSTR("!!!! WARNING !!!!"));

		pSD->sd_failed=true;
		//delay(30000);
		pOLED->clear_screen();
	};
	return 0;
};


/********** load backup values ********************
 * Backup mig schon mal wert hineinschreiben
 * falls die SD Karte nicht geladen werden kann
 * Notberiebsmig
 **************************************************/
int configuration::init(){
	// Temperatur und Widerstands LookUp
	int r_werte[19]={1000,700,550,400,330,250,230,210,195,150,140,135,110,100, 90, 80, 20, 15, 10}; // widerstandswerte
	int t_werte[19]={  27, 35, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95,100,105,110,115,120,125}; // passender Temperaturwert

	for(unsigned int j=0; j<sizeof(pSensors->m_temperature->r_werte)/sizeof(pSensors->m_temperature->r_werte[0]); j++){
		pSensors->m_temperature->r_werte[j]=r_werte[j];
		pSensors->m_temperature->t_werte[j]=t_werte[j];
	};
	// Startup sequenz im Tacho
	memset(pOLED->startup,'\0',200);
	sprintf(pOLED->startup,"ERROR,0,0,0;");
	pSensors->m_gps->active_file=2; //default datei navi0.txt


	pSensors->m_blinker->high_speed_add=200;
	pSensors->m_blinker->dist_to_warn=200;
	pSensors->m_fuel->blink_freq=500; // blinkt mit 500ms Periodenlaenge
	pSensors->m_fuel->blink_start=140; // bei mehr als 140 km auf der Tankuhr fngt die an zu blinken
	pSensors->m_speed->reifen_umfang=1.99; // Reifenumfang in metern
	pSensors->m_speed->gps_takeover=120; // bei 120 km/h nehmen wir die Daten vom GPS statt des Reed wenn moeglich
	pSpeedo->refresh_cycle=-1; // anzahl an ms nachdem der haupttacho gecleared wird
	pSensors->m_oiler->grenze=4000;
	pSensors->m_dz->blitz_dz=12500; // hornet mäßig
	pSensors->m_dz->blitz_en=true; // gehen wir mal von "an" aus

	// gaenge einlesen
	int temp[7]={160,120,90,73,60,53,45};
	for(unsigned int j=0;j<sizeof(temp)/sizeof(temp[0]);j++){ // 0..6
		pSensors->m_gear->n_gang[j]=temp[j];
	};

	// erstmal alle zwischenspeicher loeschen
	for(unsigned int i=0; i<sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]); i++){
		pSpeedo->trip_dist[i]=0;
		pSpeedo->avg_timebase[i]=0;
		pSpeedo->max_speed[i]=-99;
	};

	// skinning
	pSpeedo->oil_widget.x=0;
	pSpeedo->oil_widget.y=0;
	pSpeedo->oil_widget.symbol=true;
	pSpeedo->oil_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->air_widget.x=15;
	pSpeedo->air_widget.y=0;
	pSpeedo->air_widget.symbol=true;
	pSpeedo->air_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->arrow_widget.x=0;
	pSpeedo->arrow_widget.y=1;

	pSpeedo->kmh_widget.x=6;
	pSpeedo->kmh_widget.y=2;
	pSpeedo->kmh_widget.font=STD_SMALL_2X_FONT;

	pSpeedo->kmhchar_widget.x=12;
	pSpeedo->kmhchar_widget.y=3;
	pSpeedo->kmhchar_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->dz_widget.x=0;
	pSpeedo->dz_widget.y=4;
	pSpeedo->dz_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->addinfo_widget.x=0;
	pSpeedo->addinfo_widget.y=5;
	pSpeedo->addinfo_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->addinfo2_widget.x=0;
	pSpeedo->addinfo2_widget.y=6;
	pSpeedo->addinfo2_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->clock_widget.x=1;
	pSpeedo->clock_widget.y=7;
	pSpeedo->clock_widget.symbol=true;
	pSpeedo->clock_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->gear_widget.x=11;
	pSpeedo->gear_widget.y=7;
	pSpeedo->gear_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->fuel_widget.x=14;
	pSpeedo->fuel_widget.y=7;
	pSpeedo->fuel_widget.symbol=true;
	pSpeedo->fuel_widget.font=STD_SMALL_1X_FONT;

	pSpeedo->default_font=STD_SMALL_1X_FONT;
	pSpeedo->default_font=VISITOR_SMALL_1X_FONT;
	// skinning

	// beleuchtung
	pAktors->set_rgb_in(255,255,255,0);

	pAktors->out_base_color.r.actual=0;
	pAktors->out_base_color.g.actual=0;
	pAktors->out_base_color.b.actual=255;

	pAktors->dz_flasher.r.actual=255;
	pAktors->dz_flasher.g.actual=0;
	pAktors->dz_flasher.b.actual=0;
	// beleuchtung

	last_speed_value=0;
	storage_outdated=false;
	skin_file=0;
	Serial.println("Wert initialisiert");
	return 0;
};

/********** prozedure zum lesen des skinfiles **************
 * mit der absicherung das nur files im richtigen bereich
 * oder notfalls das defaultfile geladen wird
 */
int configuration::read_skin(){
	if(skin_file>-1 && skin_file<9){
		char filename[10];
		sprintf(filename,"SKIN%i.TXT",skin_file);
		return read(filename);
	} else {
		return read("SKIN0.TXT"); // load default
	}
}


/******* configurations datei einlesen *************
 * filename = Dateiname im config.fOLDER verzeichniss, kein Unterverzeichniss
 * Liest zeichenweise (max 200B) ein und ruft  mit jeder Zeile parse_config auf
 ***************************************************/
int configuration::read(const char* filename){
	SdFile root;
	SdFile file;
	SdFile subdir;

	root.openRoot(&pSD->volume);
	if(!subdir.open(&root, CONFIG_FOLDER, O_READ)) {  pDebug->sprintlnp(PSTR("open subdir /config failed")); return -1; };
	// datei im lese Modus oeffnen da config hier nicht geschrieben wird
	if (file.open(&subdir, filename, O_READ)) {
		int n,i=0;
		boolean skip_row=false;
		// store string
		char *buf;
		buf = (char*) malloc (200);
		if (buf==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));
		else memset(buf,'\0',200);

		uint8_t* buffer; // buffer fuer jedes Zeichen
		buffer = (uint8_t*) malloc (2);
		if (buffer==NULL) Serial.println("Malloc failed");
		else memset(buffer,'\0',2);

		while ((n = file.read(buffer, 1)) > 0) { // n=1/0=wieviele byte gelesen wurden
			if(char(buffer[0])=='\n' || char(buffer[0])=='\r') {  // auswerten
				buf[i]='\0'; // eigentlich unnötig da das ganze array mit den dinger voll ist .. oder ?
				if(i>0){
					// wir haben mehr als kein zeichen gelesen, und einen Zeilenumbruch gefunden => attake
					int return_value=parse(&buf[0]);
					if(return_value<0 ) {
						Serial.print("parse_config erzeugte Fehlercode ");
						Serial.print(return_value);
						Serial.print(". Eingabe war:");
						Serial.println(buf);
					};
				};
				i=0; // buffer leeren
				memset(buf,'\0',200); // warum geht hier nicht memset(buf,'1',200) ?
				skip_row=false; // grundstzlich geh ich mal davon aus das keine Kommentarzeile kommt
			} else if(char(buffer[0])=='#' && i==0){ // wenn wir ein # drin finden und das ganz vorne dann skippen wir die row
				skip_row=true;
			} else if(skip_row) {
				i=0;
			} else { // einlesen
				// copy it to buffer
				buf[i]=(char) buffer[0];
				i++;
				if(i>=200) i=0;
			};
			memset(buffer,'\0',2);
		};
		free(buf);
	} else {
		pDebug->sprintlnp(PSTR("!! File open failed !!"));
		Serial.println(filename);
		pDebug->sprintlnp(PSTR("!! File open failed !!"));
		return -1;
	};
	file.close();
	subdir.close();
	root.close();
	return 0;
}

/******* configurationszeile auswerten *************
 * bekommt eine Zeile als char-array und sucht nach dem "=" und dem \0
 * ruft dann entsprechend dessen was vor dem "=" steht den entsprechenden parser auf
 * schreibt dann den wert in config.f,config.i,config.a
 ***************************************************/
int configuration::parse(char* buffer){
	char *name; // store string
	name = (char*)malloc (50);
	if (name==NULL) pDebug->sprintlnp(PSTR("Malloc failed"));

	int seperator=0;
	while(1){ // der name soll maximal 50 zeichen lang sein
		if(char(buffer[seperator])!='='){
			name[seperator]=buffer[seperator];
		} else {
			name[seperator]='\0';
			break;
		};
		seperator++;
		if(seperator>49){ return -1; }; // programmabbruch
	};
	pDebug->parse(0,name);
	if(PARSE_SHORT){
		Serial.print(name);
		Serial.print("=");
	};

	int temp=0;

	// hier wissen wir wie der name ist
	if(strcmp_P(name,PSTR("startup"))==0){
		parse_a(buffer,seperator,pOLED->startup,sizeof(pOLED->startup)/sizeof(pOLED->startup[0]));
	} else if(strcmp_P(name,PSTR("active_navi_file"))==0){ // welche datei ist die aktive ?
		parse_int(buffer,seperator,&pSensors->m_gps->active_file);
	} else if(strcmp_P(name,PSTR("skin_file"))==0){ // welche datei ist der aktive skin
		parse_int(buffer,seperator,&pConfig->skin_file);
	} else if(strcmp_P(name,PSTR("oil_dist"))==0){ // distanz in meter nachder geölt wird
		parse_int(buffer,seperator,&pSensors->m_oiler->grenze);
	} else if(strncmp("temp_r_",name,7)==0){ // ganzen Block auslesen, alle temp_rXXX gehen hier rein
		char var_name[10];
		for(int j=0;j<19;j++){ // alle mglichen strings von temp_r_0 bis temp_r_18 erzeugen
			sprintf(var_name,"temp_r_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSensors->m_temperature->r_werte[j]);
			};
		};
	} else if(strncmp("temp_t_",name,7)==0){// ganzen Block auslesen, alle temp_tXXX gehen hier rein
		char var_name[10];
		for(int j=0;j<19;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"temp_t_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSensors->m_temperature->t_werte[j]);
			};
		};
	} else if(strncmp("n_gang_",name,7)==0){// ganzen Block auslesen, alle n_gangXXX gehen hier rein
		char var_name[9];
		for(int j=1;j<=6;j++){ // alle mglichen strings erzeugen
			sprintf(var_name,"n_gang_%i",j);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSensors->m_gear->n_gang[j]);
			};
		};
	} else if(strncmp("max_",name,4)==0){// ganzen Block auslesen, alle max_XXX gehen hier rein
		char var_name[6];
		for(int j=1;j<9;j++){ // alle mglichen strings erzeugen: max_1 ... max_8
			sprintf(var_name,"max_%i",j+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_int(buffer,seperator,&pSpeedo->max_speed[j]);
			};
		};
	} else if(strncmp("avg_time_",name,9)==0){// ganzen Block auslesen, alle avg_time_XXX gehen hier rein
		char var_name[11];
		for(int i=1;i<9;i++){ // alle mglichen strings erzeugen: avg_time_1 ... avg_time_8
			sprintf(var_name,"avg_time_%i",i+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_ul(buffer,seperator,&pSpeedo->avg_timebase[i]);
			};
		};
	} else if(strncmp("trip_",name,5)==0){// ganzen Block auslesen, alle trip_XXX gehen hier rein
		char var_name[7];
		for(int i=1;i<9;i++){ // alle mglichen strings erzeugen: trip_1 ... trip_8
			sprintf(var_name,"trip_%i",i+1);
			if(strcmp(var_name,name)==0){ // testen welcher denn nun der richtige ist und den fllen
				parse_ul(buffer,seperator,&pSpeedo->trip_dist[i]);
			};
		};
	} else if(strcmp_P(name,PSTR("blinker"))==0){
		parse_int(buffer,seperator,&pSensors->m_blinker->dist_to_warn);
	} else if(strcmp_P(name,PSTR("blinker_high"))==0){
		parse_int(buffer,seperator,&pSensors->m_blinker->high_speed_add);
	} else if(strcmp_P(name,PSTR("fuel_blink_start"))==0){
		parse_int(buffer,seperator,&pSensors->m_fuel->blink_start);
	} else if(strcmp_P(name,PSTR("fuel_blink_freq"))==0){
		parse_int(buffer,seperator,&pSensors->m_fuel->blink_freq);
	} else if(strcmp_P(name,PSTR("reifen_umfang"))==0){
		parse_float(buffer,seperator,&pSensors->m_speed->reifen_umfang);
	} else if(strcmp_P(name,PSTR("gps_takeover"))==0){
		parse_int(buffer,seperator,&pSensors->m_speed->gps_takeover);
	} else if(strcmp_P(name,PSTR("refresh_cyle"))==0){
		parse_int(buffer,seperator,&pSpeedo->refresh_cycle);
	} else if(strcmp_P(name,PSTR("oil_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->oil_widget.x);
	} else if(strcmp_P(name,PSTR("oil_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->oil_widget.y);
	} else if(strcmp_P(name,PSTR("oil_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->oil_widget.symbol);
	} else if(strcmp_P(name,PSTR("oil_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->oil_widget.font);
	} else if(strcmp_P(name,PSTR("air_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->air_widget.x);
	} else if(strcmp_P(name,PSTR("air_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->air_widget.y);
	} else if(strcmp_P(name,PSTR("air_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->air_widget.symbol);
	} else if(strcmp_P(name,PSTR("air_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->air_widget.font);
	} else if(strcmp_P(name,PSTR("kmh_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->kmh_widget.x);
	} else if(strcmp_P(name,PSTR("kmh_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->kmh_widget.y);
	} else if(strcmp_P(name,PSTR("kmh_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->kmh_widget.font);
	} else if(strcmp_P(name,PSTR("kmhchar_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->kmhchar_widget.x);
	} else if(strcmp_P(name,PSTR("kmhchar_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->kmhchar_widget.y);
	} else if(strcmp_P(name,PSTR("kmhchar_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->kmhchar_widget.font);
	} else if(strcmp_P(name,PSTR("arrow_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->arrow_widget.x);
	} else if(strcmp_P(name,PSTR("arrow_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->arrow_widget.y);
	} else if(strcmp_P(name,PSTR("dz_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->dz_widget.x);
	} else if(strcmp_P(name,PSTR("dz_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->dz_widget.y);
	} else if(strcmp_P(name,PSTR("dz_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->dz_widget.font);
	} else if(strcmp_P(name,PSTR("addinfo_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->addinfo_widget.x);
	} else if(strcmp_P(name,PSTR("addinfo_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->addinfo_widget.y);
	} else if(strcmp_P(name,PSTR("addinfo_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->addinfo_widget.font);
	} else if(strcmp_P(name,PSTR("addinfo2_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->addinfo2_widget.x);
	} else if(strcmp_P(name,PSTR("addinfo2_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->addinfo2_widget.y);
	} else if(strcmp_P(name,PSTR("addinfo2_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->addinfo2_widget.font);
	} else if(strcmp_P(name,PSTR("clock_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->clock_widget.x);
	} else if(strcmp_P(name,PSTR("clock_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->clock_widget.y);
	} else if(strcmp_P(name,PSTR("clock_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->clock_widget.symbol);
	} else if(strcmp_P(name,PSTR("clock_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->clock_widget.font);
	} else if(strcmp_P(name,PSTR("gear_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->gear_widget.x);
	} else if(strcmp_P(name,PSTR("gear_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->gear_widget.y);
	} else if(strcmp_P(name,PSTR("gear_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->gear_widget.font);
	} else if(strcmp_P(name,PSTR("fuel_widget.x"))==0){
		parse_short(buffer,seperator,&pSpeedo->fuel_widget.x);
	} else if(strcmp_P(name,PSTR("fuel_widget.y"))==0){
		parse_short(buffer,seperator,&pSpeedo->fuel_widget.y);
	} else if(strcmp_P(name,PSTR("fuel_widget.symbol"))==0){
		parse_bool(buffer,seperator,&pSpeedo->fuel_widget.symbol);
	} else if(strcmp_P(name,PSTR("fuel_widget.font"))==0){
		parse_short(buffer,seperator,&pSpeedo->fuel_widget.font);
	} else if(strcmp_P(name,PSTR("default_font"))==0){
		parse_short(buffer,seperator,&pSpeedo->default_font);
	} else if(strcmp_P(name,PSTR("dz_flash"))==0){
		parse_int(buffer,seperator,&pSensors->m_dz->blitz_dz);
	} else if(strcmp_P(name,PSTR("dz_flash_en"))==0){
		parse_bool(buffer,seperator,&pSensors->m_dz->blitz_en);
		/////// RGB LEDs /////////
	} else if(strcmp_P(name,PSTR("rgb_flash_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_flasher.r.actual=temp;
	} else if(strcmp_P(name,PSTR("rgb_flash_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_flasher.g.actual=temp;
	} else if(strcmp_P(name,PSTR("rgb_flash_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->dz_flasher.b.actual=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->out_base_color.r.actual=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->out_base_color.g.actual=temp;
	} else if(strcmp_P(name,PSTR("rgb_out_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->out_base_color.b.actual=temp;
	} else if(strcmp_P(name,PSTR("rgb_in_r"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->RGB.inner.r.actual=temp;
		pAktors->set_rgb_in(pAktors->RGB.inner.r.actual,pAktors->RGB.inner.g.actual,pAktors->RGB.inner.b.actual,1);
	} else if(strcmp_P(name,PSTR("rgb_in_g"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->RGB.inner.g.actual=temp;
		pAktors->set_rgb_in(pAktors->RGB.inner.r.actual,pAktors->RGB.inner.g.actual,pAktors->RGB.inner.b.actual,1);
	} else if(strcmp_P(name,PSTR("rgb_in_b"))==0){
		parse_int(buffer,seperator,&temp);
		pAktors->RGB.inner.b.actual=temp;
		pAktors->set_rgb_in(pAktors->RGB.inner.r.actual,pAktors->RGB.inner.g.actual,pAktors->RGB.inner.b.actual,1);
		/////// RGB LEDs /////////
	} else {
		return -2; // ungltiger identifier
	}
	free(name);
	return 0; // alles bestens
};



/******* float parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab dem gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * gleichzeitig wird berprft ob der zahlenbereich hinkommen kann
 ***************************************************/
int configuration::parse_float(char* buffer,int i,float* wert){
	pDebug->parse_float(0,buffer,0,0,0);

	float t_wert=0;
	boolean decade_active=false; // false=vorkommabetrieb
	int decade_count=0; // anzahl an decadenstellen
	int vz=1; // um neg zahl zu parsen
	i++; // das "=" weglesen
	while(1){
		pDebug->parse_float(1,buffer,i,0,0);

		if(char(buffer[i])=='\0' || char(buffer[i])=='\n' || char(buffer[i])==';'){
			break;
		}
		// unterscheiden zwischen punkten, minus und zahlen und ungültigen
		if(int(buffer[i])==46){
			decade_active=true;
		} else if(int(buffer[i])==45){ // "-" TODO: das checken, scheint als obs nicht klappt
			pDebug->parse_float(2,buffer,i,0,0);
			vz=-1;
		} else if(  int(buffer[i])>57 || int(buffer[i])<48   ){
			return -1; // keine zahl
		} else {
			if(decade_active){  decade_count++;        };
			t_wert=10*t_wert+(int(buffer[i])-48);
		};
		i++;
		if(i>198){
			return -2; // ich glaub soweit kann float gar nicht, aber solang wre das array max
		};
	}

	pDebug->parse_float(3,buffer,t_wert,decade_count,vz);
	t_wert=t_wert/(pow(10,decade_count))*vz;
	pDebug->parse_float(4,buffer,t_wert,decade_count,vz);

	*wert=t_wert;
	if(PARSE_SHORT){
		Serial.print(*wert);
		Serial.println("<<--");
	}
	//debug
	return 0;
};

/******* float parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab dem gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * leerzeichen werden ignoriert
 ***************************************************/
int configuration::parse_bool(char* buffer,int i,bool* wert){
	bool t_wert = false;
	i++; // das "=" weglesen
	while(1){ 
		if(char(buffer[i])=='\0' || char(buffer[i])=='\n' || char(buffer[i])==';') 
			break;
		if(int(buffer[i])==49){ // ASCII(49) = "1"
			t_wert=true;
			break;
		} else if(int(buffer[i])!=32) { // Alles was nicht "1" und nicht " " ist ist "0" ;-)
			t_wert=false;
			break;
		};
		i++;
		if(i>198){
			break;
		};
	}

	*wert=t_wert; 
	//debug
	if(PARSE_SHORT){
		Serial.print(*wert);
		Serial.println("<<--");
	}
	//debug
	return 0;
};

/******* int parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab wo gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * wird von parse_float bedient und gecastet ..
 ***************************************************/
int configuration::parse_int(char* buffer,int i,int* wert){
	float temp;
	int return_value=parse_float(buffer,i,&temp);
	//	Serial.println(temp);
	*wert=int(temp);

	pDebug->parse_int();
	return return_value;
};

/******* short parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab wo gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * wird von parse_float bedient und gecastet ..
 ***************************************************/
short configuration::parse_short(char* buffer,int i,short* wert){
	float temp;
	int return_value=parse_float(buffer,i,&temp);
	//	Serial.println(temp);
	*wert=short(temp);

	//pDebug->parse_short();
	return return_value;
};

/******* chararray aus chararray parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab wo gesucht werden soll
 ***************************************************/
int configuration::parse_a(char* buffer,int i,char* wert,int max_length){
	int j=0;
	i++;
	while(j<max_length-1){
		if(char(buffer[i])=='\0' || char(buffer[i])=='\n'  || char(buffer[i])==';') break;
		wert[j]=buffer[i];
		j++; i++;
		if(i>198){ // buffer got 200 byte, last should be '\0' so read max 0 to 198
			return -2;
		}
	}
	wert[j]='\0';

	pDebug->parse_a(0,wert);
	return 0;
};

/******* unsigned long parsen *************
 * buffer ist das char-array in dem die zahl steht,
 * i der identifier ab dem gesucht werden soll
 * *wert ist der Zeiger auf das Ergebniss
 * gleichzeitig wird ueberprueft ob der zahlenbereich hinkommen kann
 * Im prinzip wie parse_float, nur ohne vorzeichen und ohne punkt interpretation
 ***************************************************/
int configuration::parse_ul(char* buffer,int i,unsigned long* wert){
	unsigned long t_wert=0;
	i++; // das "=" weglesen
	while(1){
		if(char(buffer[i])=='\0' || char(buffer[i])=='\n' || char(buffer[i])==';') break;
		// unterscheiden zwischen punkten, minus und zahlen und ungültigen
		if(  int(buffer[i])>57 || int(buffer[i])<48   ){
			return -1; // keine zahl
		} else {
			t_wert=10*t_wert+(int(buffer[i])-48);
		};
		i++;
		if(i>198){
			return -2; // ich glaub soweit kann float gar nicht, aber solang wre das array max
		};
	}
	*wert=t_wert;

	if(PARSE_SHORT){
		Serial.print(*wert);
		Serial.println("<<--");
	}
	pDebug->parse_ul(0,*wert);
	return 0;
};

// in diese routine werden die km hochgezählt und eventuell das auf die karte/eeprom speichern veranlasst
void configuration::km_save(){
	int speed_value=pSensors->m_speed->getSpeed();
	// debug
	if(STORAGE_DEBUG){   pDebug->sprintp(PSTR("calling km_save"));  };
	// debug
	if(speed_value>0){
		// debug
		if(STORAGE_DEBUG){     pDebug->sprintlnp(PSTR("speed>0 => storage outdated"));    };
		// debug
		storage_outdated=true;
		unsigned int strecke_m=round(speed_value/3.6*1); // km/h => m/1sec
		// debug
		if(STORAGE_DEBUG){  Serial.print("Prozentualer Unterschied zum letzen mal (muss zwischen 95 und 105 liegen): "); Serial.println(round(last_speed_value*100/speed_value));  };
		if(STORAGE_DEBUG){  Serial.print("Speed value: "); Serial.println(speed_value);  };
		// debug
		for(unsigned int a=0;a<sizeof(pSpeedo->max_speed)/sizeof(pSpeedo->max_speed[0]);a++){
			// rescue after error
			if(pSpeedo->max_speed[a]>300){
				pSpeedo->max_speed[a]=speed_value;
			};
			// trips hochzählen, die sekunden basis +1 und die avg_speed + den aktuellen speed
			pSpeedo->trip_dist[a]+=strecke_m;
			pSpeedo->avg_timebase[a]++;
			// falls die geschwindigkeit zwischen 95% und 105% liegt
			// das dient dazu das ein peak gefiltert wird, wenn man tatsächlich so schnell ist, wird man auch 1 sek lang ähnlich schnell bleiben
			if((last_speed_value*100/speed_value)>=95 && (last_speed_value*100/speed_value)<=105){
				// nur übernehmen wenn unter 300 aber über der letzten max
				if(speed_value>pSpeedo->max_speed[a] && speed_value<256){
					pSpeedo->max_speed[a]=speed_value;
					// debug
					if(STORAGE_DEBUG){  Serial.print("====> Max updated: "); Serial.print(pSpeedo->max_speed[a]); Serial.println(" <==========");   };
					// debug
				};
			};
		};
		// save as backup
		last_speed_value=speed_value;
	} else { // laufender motor => sprit verbrauch => speichern
		pSensors->m_dz->calc();
		if(pSensors->m_dz->exact>0){
			storage_outdated=true;
		};
	};
	//save it
	if(pSensors->m_clock->get_ss()==59){
		write("speedo.txt");
	};
};




void configuration::EEPROM_init(){
	pDebug->sprintlnp(PSTR("Lade EEPROM"));
	//Serial.println("-> Lade KM von SD Karte");
	pSpeedo->m_trip_mode=EEPROM.read(2);
	if(pSpeedo->m_trip_mode>9 || pSpeedo->m_trip_mode<0) {    pSpeedo->m_trip_mode=1;       };

	// immer reseten
	pSpeedo->trip_dist[0]=0;
	pSpeedo->max_speed[0]=0;
	pSpeedo->avg_timebase[0]=0;

	//load date_of_today
	int temp =EEPROM.read(3);
	int temp2=EEPROM.read(4);
	int date_of_today=temp+100*temp2;
	// reset day trip at next day
	// wenn keine RTC verbunden, dann gibt clock_getdate() 0 zurück.
	// wenn nun noch gps verbunden ist, dann wird ein richtiges datum
	// abgespeichert und das hier bei jedem start durchlaufen.
	// aber da clock_getdate()==0 ist, wird keine datei gelöscht werden.
	if(STORAGE_DEBUG){
		Serial.print("Laut speicher war das Datum zuletzt der ");
		Serial.print(date_of_today);
		Serial.print(" und heute ist der ");
		Serial.println(pSensors->m_clock->getdate());
	};
	if(pSensors->m_clock->getdate()!=date_of_today){
		pSpeedo->trip_dist[1]=0;
		pSpeedo->max_speed[1]=0;
		pSpeedo->avg_timebase[1]=0;

		if(pSensors->m_clock->getdate()!=0){ // wenn wir heute einen anderen tag haben als im "heute" vom eeprom steht dann schreiben wir das heute in den eeprom
			// write "today"
			byte tempByte = ((int)floor(pSensors->m_clock->getdate()/100) & 0xFF);
			EEPROM.write(4,tempByte);
			tempByte = ((int)floor(pSensors->m_clock->getdate()%100) & 0xFF);
			EEPROM.write(3,tempByte);

			storage_outdated=true; // zum speichern zwingen
			write("speedo.txt"); // und ab dafür
		};
	};


	// load winterzeit
	pSensors->m_clock->set_date_time(-1,-1,-1,-1,-1,-1,-1,EEPROM.read(148));

	// load naviposition
	pSensors->m_gps->navi_point=EEPROM.read(147);

	// display setup
	pOLED->phase=EEPROM.read(144);
	pOLED->ref=EEPROM.read(145);

	// load navi on off
	if(EEPROM.read(146)==1){
		pSensors->m_gps->navi_active=true;
		pSensors->m_gps->generate_new_order();
	} else {
		pSensors->m_gps->navi_active=false;
	};

};

