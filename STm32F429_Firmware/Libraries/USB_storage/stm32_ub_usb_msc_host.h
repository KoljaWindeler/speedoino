//--------------------------------------------------------------
// File     : stm32_ub_usb_msc_host.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_USB_MSCHOST_H
#define __STM32F4_UB_USB_MSCHOST_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "usbh_usr.h"
#include "stm32_ub_fatfs.h"




//--------------------------------------------------------------
// Status der USB-Verbindung
//--------------------------------------------------------------
typedef enum {
  USB_MSC_HOST_NO_INIT =0,   // USB-Schnittstelle noch nicht initialisiert
  USB_MSC_DEV_DETACHED,      // kein Device angeschlossen
  USB_MSC_SPEED_ERROR,       // USB-Speed wird nicht unterst�tzt
  USB_MSC_DEV_NOT_SUPPORTED, // Device wird nicht unters�tzt
  USB_MSC_DEV_WRITE_PROTECT, // Device ist schreibgesch�tzt
  USB_MSC_OVER_CURRENT,      // �berstrom erkannt
  USB_MSC_DEV_CONNECTED      // Device verbunden und bereit
}USB_MSC_HOST_STATUS_t;
USB_MSC_HOST_STATUS_t USB_MSC_HOST_STATUS;


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_USB_MSC_HOST_Init(void);
USB_MSC_HOST_STATUS_t UB_USB_MSC_HOST_Do(void);


//--------------------------------------------------------------
#endif // __STM32F4_UB_USB_MSCHOST_H
