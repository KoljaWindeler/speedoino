#include "global.h"


timing Millis;
uart Serial;
ILI9325 TFT;
Speedo_Sensors Sensors;
ssd0323 OLED;
#ifdef DEMO_MODE
	speedo_demo Demo;
#endif

void init_speedo(void){
	Millis.init();
	Serial.init(USART1,115200);
	Serial.init(UART4,115200);
	Sensors.init();
	OLED.init(0xA8,0x28);
	//	TFT.init();

	Sensors.mTemperature.check_vars();
	Serial.puts_ln(USART1, "Init complete!");
}


int main(void) {
	init_speedo();

	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure PD12, 13, 14 and PD15 in output pushpull mode */
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	OLED.clear_screen();
	OLED.filled_rect(0,0,30,20,0xff);
	OLED.filled_rect(10,10,30,20,0xff);
	OLED.string(SANS_SMALL_1X_FONT,"Party!",0,5);
	OLED.string(SANS_SMALL_1X_FONT,"das war ja einfach...",0,6);

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
	Serial.init(USART2,115200);
	char buffer[50];
	while (1) {
		_delay_ms(100);
		int16_t temp=Sensors.mRpm.get_exact();
		sprintf(buffer,"%i U/min",temp);
		OLED.string(SANS_SMALL_1X_FONT,buffer,0,7);
		Serial.puts_ln(USART1,buffer);
		Serial.puts_ln(USART2,buffer);

		Sensors.mTemperature.read_oil_temp();

	}
}
