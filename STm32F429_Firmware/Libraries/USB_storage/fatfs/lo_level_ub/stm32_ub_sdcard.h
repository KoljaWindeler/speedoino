//--------------------------------------------------------------
// File     : stm32_ub_usbdisk.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SDCARD_H
#define __STM32F4_UB_SDCARD_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "diskio.h"






//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_SDCard_Init(void);
uint8_t UB_SDCard_CheckMedia(void);
int MMC_disk_initialize(void);
int MMC_disk_status(void);
int MMC_disk_read(BYTE *buff, DWORD sector, BYTE count);
int MMC_disk_write(const BYTE *buff, DWORD sector, BYTE count);
int MMC_disk_ioctl(BYTE cmd, void *buff);



//--------------------------------------------------------------
#endif // __STM32F4_UB_SDCARD_H
