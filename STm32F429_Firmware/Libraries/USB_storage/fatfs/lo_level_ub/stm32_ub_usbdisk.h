//--------------------------------------------------------------
// File     : stm32_ub_usbdisk.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_USBDISK_H
#define __STM32F4_UB_USBDISK_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "diskio.h"
#include "usb_core.h"
#include "usbh_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "stm32_ub_usb_msc_host.h"




#define USB_PRESENT                                 ((uint8_t)0x01)
#define USB_NOT_PRESENT                             ((uint8_t)0x00)


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_USBDisk_Init(void);
uint8_t UB_USBDrive_CheckMedia(void);
int USB_disk_initialize(void);
int USB_disk_status(void);
int USB_disk_read(BYTE *buff, DWORD sector, BYTE count);
int USB_disk_write(const BYTE *buff, DWORD sector, BYTE count);
int USB_disk_ioctl(BYTE cmd, void *buff); 


//--------------------------------------------------------------
#endif // __STM32F4_UB_USBDISK_H
