#include "global.h"
extern "C" {
#include "stm32_ub_usb_msc_host.h"
}

timing Millis;
uart Serial;
//ILI9325 		TFT;
ILI9341 TFT;
sensing Sensors;
akting Aktors;
debugging Debug;
timer Timer;
speedo Speedo;
filemanager_v2 Filemanager_v2;
sd SD;
configuration Config;
menu Menu;        // pins aktivieren, sonst nix

sprint Sprint;
LapTime LapTimer;
speedcams SpeedCams;
#ifdef DEMO_MODE
speedo_demo Demo;
#endif

void init_speedo(void) {
	Millis.init();
	Serial.init(USART1, 115200);
	Serial.init(USART2, 115200);
	Serial.init(USART3, 115200);

	Serial.puts_ln(USART1, "=== Speedoino ===");
	Serial.puts(USART1, GIT_REV);                // print Software release
	Serial.puts(USART1, " HW:");
	Serial.puts_ln(USART1, Config.get_hw_version());   // print Hardware release

	// first, set all variables to a zero value
	Sensors.init();
	Speedo.clear_vars();        // refresh cycle
	// read configuration file from sd card
	SD.init();                 // try open SD Card
	Config.read(CONFIG_FOLDER, "BASE.TXT", READ_MODE_CONFIGFILE, ""); // load base config
	Config.read(CONFIG_FOLDER, "SPEEDO.TXT", READ_MODE_CONFIGFILE, ""); // speedovalues, avg,max,time
	Config.read(CONFIG_FOLDER, "GANG.TXT", READ_MODE_CONFIGFILE, "");   // gang
	Config.read(CONFIG_FOLDER, "TEMPER.TXT", READ_MODE_CONFIGFILE, ""); // Temperatur
	Config.read_skin();        // skinning
	//	check if read SD read was okay, if not: load your default backup values
	Aktors.check_vars();        // check if color of outer LED are OK
	Sensors.check_vars();        // check if config read was successful
	Speedo.check_vars(); // rettet das Skinning wenn SD_failed von den sensoren auf true gesetzt wird
	Sensors.single_read(); // read all sensor values once to ensure we are ready to show them
	Aktors.init(); // Start outer LEDs // ausschlag des zeigers // Motorausschlag und block bis motor voll ausgeschlagen, solange das letzte intro bild halten
	TFT.init();
	//	pOLED.init_speedo();         // Start Screen //execute this AFTER Config->init(), init() will load  phase,config,startup. PopUp will be shown if sd access fails
	Menu.init();    // Start butons // adds the connection between pins and vars
	Menu.display(); // execute this AFTER pOLED.init_speedo!! this will show the menu and, if state==11, draws speedosymbols
	Speedo.reset_bak(); // reset all storages, to force the redraw of the speedo
	Config.ram_info();
	Serial.puts_ln(USART1, "=== Setup finished ===");

}

int main(void) {
	SystemInit();
	init_speedo();

	// Setting the Background to "a lot transparancy" green + foregroud to "complete transparent" results in a dark green
	//	TFT.SetLayer(LCD_BACKGROUND_LAYER); 	TFT.clear_screen(LCD_COLOR_GREEN); 	TFT.SetTransparency(200);
	//	TFT.SetLayer(LCD_FOREGROUND_LAYER);	TFT.SetTransparency(255);	TFT.clear_screen(LCD_COLOR_BLUE);

	// Setting the Background to 100% green + Foreground to 100% transparent => full green
	//	TFT.SetLayer(LCD_BACKGROUND_LAYER);		TFT.clear_screen(LCD_COLOR_GREEN);	TFT.SetTransparency(0);
	//	TFT.SetLayer(LCD_FOREGROUND_LAYER); 	TFT.SetTransparency(255);	TFT.clear_screen(LCD_COLOR_BLUE);

	// setting back to 100%green + foreground to 180/255 transparent blue => türkies

	// setting foreground to blue 0 transparency and background to green 0 trans => green (written later!)
	//TFT.SetLayer(LCD_FOREGROUND_LAYER); 	TFT.SetTransparency(0);	TFT.clear_screen(LCD_COLOR_BLUE);
	//TFT.SetLayer(LCD_BACKGROUND_LAYER);		TFT.clear_screen(LCD_COLOR_GREEN);	TFT.SetTransparency(0);

	//	TFT.SetLayer(LCD_BACKGROUND_LAYER);
	//	TFT.clear_screen(LCD_COLOR_GREEN);
	//	TFT.filled_rect(250,170,50,50,TFT.convert_color(0,0,255));
	//	TFT.string("B",45,26);
	//	TFT.SetTransparency(0);
	//
	//	TFT.SetLayer(LCD_FOREGROUND_LAYER);
	//	TFT.clear_screen(LCD_COLOR_BLACK);
	//	TFT.filled_rect(250,20,50,50,TFT.convert_color(255,0,0));
	//	TFT.string("F",7,5);
	//	TFT.SetTransparency(000);
	//
	//	TFT.string("12345678901234567890123456",0,1);
	//	TFT.string("aaaaaaaaaaaaaaaaaaaaaaaaaa",0,5);
	//	TFT.string("bbbbbbbbbbbbbbbbbbbbbbbbbb",0,9);
	//	TFT.string("cccccccccccccccccccccccccc",0,13);
	//int i;
	//	while(1){
	//		if(i<280) i++;
	//		if(i==260) i=0;
	//		TFT.SetDisplayWindow(20,10+i,50,100);
	//		_delay_ms(10);
	//	}

	//	TFT.SetTextColor(TFT.convert_color(255,0,0));
	//	TFT.draw_line(50,50,50,100,255,0,0);
	//	TFT.draw_line(50,50,100,50,0,255,0);
	//	TFT.draw_line(100,50,50,100,0,0,255);
	//
	//	TFT.filled_rect(250,0,69,10,255,255,0);
	//	for(int i=0;i<100;i++){
	//		TFT.filled_rect(160,120+5*i,160,120,255,0,0);
	//	}

	//	TFT.SetLayer(LCD_FOREGROUND_LAYER);
	//	TFT.SetTransparency(100);
	////	TFT.SetDisplayWindow(0,0,10,10);
	//	TFT.filled_rect(0,0,100,100,255,0,0);

	//	while (1)
	//	{
	//		for (int tobuttom = 1; tobuttom < 41; tobuttom++)
	//		{
	//			/* move the picture */
	//			LTDC_LayerPosition(LTDC_Layer1, 0, (tobuttom*4));
	//			LTDC_LayerPosition(LTDC_Layer2, 0, (160 - (tobuttom*4)));
	//			/* Reload LTDC configuration  */
	//			LTDC_ReloadConfig(LTDC_IMReload);
	//			_delay_ms(10);
	//		}
	//		_delay_ms(100);
	//	}
	//	while(1);
	//	TFT.string(VISITOR_SMALL_8X_FONT,"test",2,0);
	//	while(1);
	//
	//
	//	TFT.clear_screen();
	//	TFT.draw_bmp(0,100,(uint8_t*)"/logo.bmp");
	//	_delay_ms(1000);
	//	TFT.clear_screen();
//	TFT.string(VISITOR_SMALL_1X_FONT, "Start", 20, 0);

	FIL myFile;   // Filehandler
	uint8_t write_ok = 0;


//	TFT.string(VISITOR_SMALL_1X_FONT, "sys init", 20, 1);

	// Init vom USB-OTG-Port als MSC-HOST
	// (zum lesen/schreiben auf einen USB-Stick)
	UB_USB_MSC_HOST_Init();

	while (1) {
		// pollen vom USB-Status
		if (UB_USB_MSC_HOST_Do() == USB_MSC_DEV_CONNECTED) {

			// wenn File noch nicht geschrieben wurde
			if (write_ok == 0) {
				write_ok = 1;
				// Media mounten
				if (UB_Fatfs_Mount(USB_0) == FATFS_OK) {
//					TFT.string(VISITOR_SMALL_1X_FONT, "Mounted", 0, 1);
//					// File zum schreiben im root neu anlegen
//					if (UB_Fatfs_OpenFile(&myFile, "USB_File.txt", F_WR_CLEAR)
//							== FATFS_OK) {
//						TFT.string(VISITOR_SMALL_1X_FONT, "Writing", 0, 2);
//						// ein paar Textzeilen in das File schreiben
//						UB_Fatfs_WriteString(&myFile,
//								"Test der WriteString-Funktion (STM32F429)");
//						UB_Fatfs_WriteString(&myFile, "hier Zeile zwei");
//						UB_Fatfs_WriteString(&myFile, "ENDE");
//						// File schliessen
//						UB_Fatfs_CloseFile(&myFile);
//					}
#define DRAW_BMP_DEBUG_LEVEL 0
#define READ_PIXEL 170
					FIL file;
					int16_t x_intern = 0;
					int16_t y_intern = 0;
					//	if(SD.get_file_handle((unsigned char*)filename,&file,FA_READ|FA_OPEN_EXISTING)>=0){
					if (UB_Fatfs_OpenFile(&file, "/logo.bmp", F_RD)
							== FATFS_OK) {
#if DRAW_BMP_DEBUG_LEVEL>1
						uint32_t start = Millis.get();
						uint32_t read_time = 0;
#endif

						//// read header ////
						char buf_header[16];
						UINT n_byte_read = 1;
						uint32_t byte_read_total = 0;
						f_read(&file, buf_header, 15, &n_byte_read);
						if (n_byte_read < 15) {
							return -2; // file to short, not even header inside
						}
						int16_t header_size = buf_header[10];
						f_lseek(&file, 18);
						f_read(&file, buf_header, 8, &n_byte_read); // rest vom header "weglesen"
						int32_t bmp_width = buf_header[0] | buf_header[1] << 8
								| buf_header[2] << 16 | buf_header[3] << 24;
						int32_t bmp_height = buf_header[4] | buf_header[5] << 8
								| buf_header[6] << 16 | buf_header[7] << 24;
						int64_t bmp_pixel_count = bmp_height * bmp_width;
						y_intern += bmp_height; // to map from top left corner 0,0 to bottom line 0, (bmp_height-1)

#if DRAW_BMP_DEBUG_LEVEL>2
						Serial.puts(USART1, "Header size:");
						Serial.puts_ln(USART1, (int) header_size);
						Serial.puts(USART1, "BMP width:");
						Serial.puts_ln(USART1, (int) bmp_width);
						Serial.puts(USART1, "BMP height:");
						Serial.puts_ln(USART1, (int) bmp_height);
#endif

						//// jump to payload ////
						f_lseek(&file, header_size);
						char* buffer;
						buffer = (char*) malloc(READ_PIXEL * 3);

						//		TFT.(x, y);
						//		TFT.WriteRAM_Prepare(); /* Prepare to write GRAM */

						//// read pixels ////
						while (n_byte_read > 0
								&& byte_read_total <= 3 * bmp_pixel_count) { // n=1/0=wieviele byte gelesen wurden
#if DRAW_BMP_DEBUG_LEVEL>1
							int32_t start_read = Millis.get();
#endif
							f_read(&file, buffer, 3 * READ_PIXEL, &n_byte_read); // 170*3=510 byte
#if DRAW_BMP_DEBUG_LEVEL>1
							read_time += Millis.get() - start_read;
							byte_read_total += n_byte_read;
#endif
							for (uint16_t i = 0; i < n_byte_read / 3; i++) {
								//				TFT.WriteRAM(buffer[2+3*i],buffer[1+3*i],buffer[0+3*i]);
								TFT.Pixel(x_intern, y_intern, buffer[2 + 3 * i],
										buffer[1 + 3 * i], buffer[0 + 3 * i]);
								x_intern++;
								if (x_intern >= bmp_width) { // line completed
									while ((x_intern - 0) % 4 != 0) { // bmp are padding lines with zeros unti the number is a multiple of 4
										i++;
										x_intern++;
									}
									x_intern = 0;
									y_intern--;
									//					TFT.SetRotatedCursor(x_intern, y_intern);
									//					TFT.WriteRAM_Prepare(); /* Prepare to write GRAM */
								}
							}
						}
#if DRAW_BMP_DEBUG_LEVEL>1
						//// time debug ////
						uint32_t time = Millis.get() - start;
						Serial.puts(USART1, "Time:");
						Serial.puts_ln(USART1, time);
						Serial.puts(USART1, "Time for reading:");
						Serial.puts_ln(USART1, read_time);
#endif
						free(buffer);

					}

					// Media unmounten
					UB_Fatfs_UnMount(USB_0);
				}
			}
		} else {
			// wenn kein USB-Stick vorhanden
		}
	}
	//	TFT.draw_bmp(0,0,"test.bmp");

	/******************** setup procedure ********************************************
	 * all initialisations must been made before the main loop, before THIS
	 ******************** setup procedure ********************************************/
	unsigned long previousMillis = 0;
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
		Sensors.mReset.toggle(); // toggle pin, if we don't toggle it, the ATmega8 will reset us, kind of watchdog<
		Debug.speedo_loop(21, 1, 0, " "); // intensive debug= EVERY loop access reports the Menustate
		Sensors.mGPS.check_flag();    	// check if a GPS sentence is ready
		//		pAktors.check_flag(); 				// updated expander
		Sensors.pull_values();	// very important, updates all the sensor values

		/************************* timer *********************/
		Timer.every_sec();		// 1000 ms
		Timer.every_qsec();			// 250  ms
		Timer.every_custom(); // one custom timer, redrawing the speedo, time is defined by "refresh_cycle" in the base.txt
		/************************* push buttons *********************
		 * using true as argument, this will activate bluetooth input as well
		 ************************* push buttons*********************/
		//Menu.button_test(true,false);     // important!! if we have a pushed button we will draw something, depending on the menustate
		if (Menu.button_test(true, false)) { // important!! if we have a pushed button we will draw something, depending on the menustate
			Serial.puts(USART1, "Menustate:");
			Serial.puts_ln(USART1, Menu.state);
		}
		/************************ every deamon activity is clear, now draw speedo ********************
		 * we are round about 0000[1]1 - 0000[1]9
		 ************************ every deamon activity is clear, now draw speedo ********************/
		Sensors.mCAN.check_message();

		if ((Menu.state / 10) == 1 || Menu.state == 7311111) {
			Speedo.loop(previousMillis);
		}
		//////////////////// Sprint Speedo ///////////////////
		else if (Menu.state == MENU_SPRINT * 10 + 1) {
			Sprint.loop();
		}
		////////////////// Clock Mode ////////////////////////
		else if (Menu.state == 291) {
			Sensors.mClock.loop();
		}
		////////////////// Speed Cam Check - Mode ////////////////////////
		else if (Menu.state
				== BMP(0, 0, 0, 0, M_TOUR_ASSISTS,
						SM_TOUR_ASSISTS_SPEEDCAM_STATUS, 1)) {
			//			SpeedCams.calc();
			//			SpeedCams.interface();
		}
		////////////////// race mode ////////////////////
		else if (Menu.state == M_LAP_T * 100 + 11) {
			LapTimer.waiting_on_speed_up();
		} else if (Menu.state == M_LAP_T * 1000 + 111) {
			LapTimer.race_loop();
		}
		////////////////// set gps point ////////////////////
		else if (Menu.state == M_LAP_T * 10000L + 3111) {
			LapTimer.gps_capture_loop();
		}
		//////////////////// voltage mode ///////////////////
		else if (Menu.state == 531) {
			Sensors.addinfo_show_loop();
		}
		//////////////////// stepper mode ///////////////////
		else if (Menu.state == 541) {
			//			Aktors.mStepper.loop();
		}
		//////////////////// gps scan ///////////////////
		else if (Menu.state == 511) {
			Sensors.mGPS.loop();
		}
		//////////////////// outline scan ///////////////////
		else if (Menu.state == 721) {
			Sensors.mSpeed.check_umfang();
		}
		////////////////// gear scan ///////////////
		else if (floor(Menu.state / 100) == 71) {
			Sensors.mGear.calibrate();
		}

#ifdef LOAD_CALC
		load_calc++;
		if(millis()-lasttime_calc>1000) {
			Serial.print(load_calc);
			Serial.println(" cps"); // 182 w/o interrupts, 175 w/ interrupts, 172 w/ much interrupts
			load_calc=0;
			lasttime_calc=millis();
		}
#endif
	} // end for
}
