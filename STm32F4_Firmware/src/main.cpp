#include "global.h"


timing 			Millis;
uart 			Serial;
ILI9325 		TFT;
Speedo_Sensors 	Sensors;
debugging 		Debug;
timer       	Timer;
speedo  		Speedo;
//filemanager_v2 	Filemanager_v2;
//sd				SD;
//configuration	Config;
//menu         	Menu;        // pins aktivieren, sonst nix

sprint      	Sprint;
//aktors      	Aktors;
LapTime  		LapTimer;
//speedcams   	SpeedCams;
#ifdef DEMO_MODE
speedo_demo Demo;
#endif


void init_speedo(void){
	Millis.init();
	Serial.init(USART1,115200);
	Serial.init(UART4,115200);

	Serial.puts_ln(USART1,"=== Speedoino ===");
	Serial.puts(USART1,GIT_REV);                // print Software release
	Serial.puts_ln(USART1," HW:");
	//	Serial.println(pConfig.get_hw_version());    // print Hardware release

	//	SD.init();                 // try open SD Card
	//	// first, set all variables to a zero value
	Sensors.init();
	//	Speedo.clear_vars();        // refresh cycle
	//	// read configuration file from sd card
	//	Config.read(CONFIG_FOLDER,"BASE.TXT",READ_MODE_CONFIGFILE,"");     // load base config
	//	Config.read(CONFIG_FOLDER,"SPEEDO.TXT",READ_MODE_CONFIGFILE,"");    // speedovalues, avg,max,time
	//	Config.read(CONFIG_FOLDER,"GANG.TXT",READ_MODE_CONFIGFILE,"");    // gang
	//	Config.read(CONFIG_FOLDER,"TEMPER.TXT",READ_MODE_CONFIGFILE,"");    // Temperatur
	//	Config.read_skin();        // skinning
	//	// check if read SD read was okay, if not: load your default backup values
	//	Aktors.check_vars();        // check if color of outer LED are OK
	//	Sensors.check_vars();        // check if config read was successful
	//	Speedo.check_vars();        // rettet das Skinning wenn SD_failed von den sensoren auf true gesetzt wird
	//	Sensors.single_read();    // read all sensor values once to ensure we are ready to show them
	//	Aktors.init();            // Start outer LEDs // ausschlag des zeigers // Motorausschlag und block bis motor voll ausgeschlagen, solange das letzte intro bild halten
	TFT.init();
	//	pOLED.init_speedo();         // Start Screen //execute this AFTER Config->init(), init() will load  phase,config,startup. PopUp will be shown if sd access fails
	//	Menu.init();                 // Start butons // adds the connection between pins and vars
	//	Menu.display();             // execute this AFTER pOLED.init_speedo!! this will show the menu and, if state==11, draws speedosymbols
	//	Speedo.reset_bak();         // reset all storages, to force the redraw of the speedo
	//	Config.ram_info();
	Serial.puts_ln(USART1,"=== Setup finished ===");






}


int main(void) {
	init_speedo();
	Debug.init(); // used to init the led's D12..15


	TFT.clear_screen();

	//////////////////////////////////////////////////////
	//	FIL myFile;   // Filehandler
	//	// Init vom FATFS-System
	//	Serial.puts_ln(USART1,"1");
	//	UB_Fatfs_Init();
	//	Serial.puts_ln(USART1,"2");
	//
	//	// Check ob Medium eingelegt ist
	//	if(UB_Fatfs_CheckMedia(MMC_0)==FATFS_OK) {
	//		Serial.puts_ln(USART1,"3");
	//		// Media mounten
	//		if(UB_Fatfs_Mount(MMC_0)==FATFS_OK) {
	//			Serial.puts_ln(USART1,"4");
	//			// File zum schreiben im root neu anlegen
	//			if(UB_Fatfs_OpenFile(&myFile, "0:/UB_File.txt", F_WR_CLEAR)==FATFS_OK) {
	//				Serial.puts_ln(USART1,"5");
	//				// ein paar Textzeilen in das File schreiben
	//				UB_Fatfs_WriteString(&myFile,"Test der WriteString-Funktion");
	//				UB_Fatfs_WriteString(&myFile,"hier Zeile zwei");
	//				UB_Fatfs_WriteString(&myFile,"ENDE");
	//				// File schliessen
	//				UB_Fatfs_CloseFile(&myFile);
	//				Serial.puts_ln(USART1,"6");
	//			}
	//			// Media unmounten
	//			UB_Fatfs_UnMount(MMC_0);
	//			Serial.puts_ln(USART1,"7");
	//		}
	//	}
	//	Serial.puts_ln(USART1,"8");
	//////////////////////////////////////////////////////
	//	Serial.init(USART2,115200);
	char buffer[50];
	int16_t temp=Sensors.mRpm.get_exact();
	sprintf(buffer,"%i U/min",temp);
	TFT.string(SANS_SMALL_1X_FONT,buffer,7,6);
	Serial.puts_ln(USART1,buffer);
	//		Serial.puts_ln(USART2,buffer);

	Sensors.mTemperature.read_oil_temp();

	TFT.CharSize(24);
	TFT.StringLine(60,100,"Speedoino 2.0");
	TFT.CharSize(16);
	TFT.StringLine(105,120,"STm32 powered");

	TFT.SetTextColor(255,0,0);
	for(int i=0;i<20;i++){
		int16_t x_from=0;
		int16_t y_from=160+2*i;
		int16_t x_to=60+7*i;
		int16_t y_to=239;
		TFT.DrawUniLine(x_from,y_from,x_to,y_to);

		x_from=319-x_to;
		x_to=319;

		y_to=239-y_from;
		y_from=0;
		TFT.DrawUniLine(x_from,y_from,x_to,y_to);
	}

	while(1);

	/******************** setup procedure ********************************************
	 * all initialisations must been made before the main loop, before THIS
	 ******************** setup procedure ********************************************/
	unsigned long   previousMillis = 0;
#ifdef LOAD_CALC
	unsigned long load_calc=0;
	unsigned long lasttime_calc=0;
#endif

	for (;;) {
		Sensors.mCAN.check_message();
		//////////////////////////////////////////////////
		//		Sensors.m_reset->set_deactive(false,false);
		//		Serial3.end();
		//		Serial3.begin(115200);
		//		while(true){
		//			while(Serial3.available()>0){
		//				Serial.print(Serial3.read(),BYTE);
		//			}
		//			while(Serial.available()>0){
		//				Serial3.print(Serial.read(),BYTE);
		//			}
		//		}
		//////////////////////////////////////////////////

		Sensors.mReset.toggle(); 		// toggle pin, if we don't toggle it, the ATmega8 will reset us, kind of watchdog<
		Debug.speedo_loop(21,1,0," "); 	// intensive debug= EVERY loop access reports the Menustate
		Sensors.mGPS.check_flag();    	// check if a GPS sentence is ready
		pAktors.check_flag(); 				// updated expander
		Sensors.pull_values();			// very important, updates all the sensor values

		/************************* timer *********************/
		Timer.every_sec();		// 1000 ms
		Timer.every_qsec();			// 250  ms
		Timer.every_custom();  		// one custom timer, redrawing the speedo, time is defined by "refresh_cycle" in the base.txt
		/************************* push buttons *********************
		 * using true as argument, this will activate bluetooth input as well
		 ************************* push buttons*********************/
		Menu.button_test(true,false);     // important!! if we have a pushed button we will draw something, depending on the menustate
		/************************ every deamon activity is clear, now draw speedo ********************
		 * we are round about 0000[1]1 - 0000[1]9
		 ************************ every deamon activity is clear, now draw speedo ********************/
		Sensors.mCAN.check_message();

		if((Menu.state/10)==1 || Menu.state==7311111)  {
			Speedo.loop(previousMillis);
		}
		//////////////////// Sprint Speedo ///////////////////
		else if( Menu.state==MENU_SPRINT*10+1 ) {
			Sprint.loop();
		}
		////////////////// Clock Mode ////////////////////////
		else if(Menu.state==291){
			Sensors.mClock.loop();
		}
		////////////////// Speed Cam Check - Mode ////////////////////////
		else if(Menu.state==BMP(0,0,0,0,M_TOUR_ASSISTS,SM_TOUR_ASSISTS_SPEEDCAM_STATUS,1)){
//			SpeedCams.calc();
//			SpeedCams.interface();
		}
		////////////////// race mode ////////////////////
		else if(Menu.state==M_LAP_T*100+11){
			LapTimer.waiting_on_speed_up();
		}
		else if(Menu.state==M_LAP_T*1000+111){
			LapTimer.race_loop();
		}
		////////////////// set gps point ////////////////////
		else if(Menu.state==M_LAP_T*10000L+3111){
			LapTimer.gps_capture_loop();
		}
		//////////////////// voltage mode ///////////////////
		else if(Menu.state==531){
			Sensors.addinfo_show_loop();
		}
		//////////////////// stepper mode ///////////////////
		else if(Menu.state==541){
//			Aktors.mStepper.loop();
		}
		//////////////////// gps scan ///////////////////
		else if(Menu.state==511){
			Sensors.mGPS.loop();
		}
		//////////////////// outline scan ///////////////////
		else if(Menu.state==721){
			Sensors.mSpeed.check_umfang();
		}
		////////////////// gear scan ///////////////
		else if(floor(Menu.state/100)==71){
			Sensors.mGear.calibrate();
		}

#ifdef LOAD_CALC
		load_calc++;
		if(millis()-lasttime_calc>1000){
			Serial.print(load_calc);
			Serial.println(" cps"); // 182 w/o interrupts, 175 w/ interrupts, 172 w/ much interrupts
			load_calc=0;
			lasttime_calc=millis();
		}
#endif
	} // end for
}
