//--------------------------------------------------------------
// File     : usbh_usr.c
//-------------------------------------------------------------- 

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_usr.h"
#include "stm32_ub_usb_msc_host.h"






USBH_Usr_cb_TypeDef USR_Callbacks =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};



static uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;




//--------------------------------------------------------------
// wird einmal beim Init vom USB-Host aufgerufen
//--------------------------------------------------------------
void USBH_USR_Init(void)
{
  // leer
}

//--------------------------------------------------------------
// wird beim stecken vom Device aufgerufen
//--------------------------------------------------------------
void USBH_USR_DeviceAttached(void)
{
  USB_MSC_HOST_STATUS=USB_MSC_DEV_DETACHED;  
}

//--------------------------------------------------------------
void USBH_USR_UnrecoveredError (void)
{
  USB_MSC_HOST_STATUS=USB_MSC_DEV_DETACHED;
}

//--------------------------------------------------------------
// wird beim ziehen vom Device aufgerufen
//--------------------------------------------------------------
void USBH_USR_DeviceDisconnected (void)
{
  USB_MSC_HOST_STATUS=USB_MSC_DEV_DETACHED;
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
//--------------------------------------------------------------
void USBH_USR_ResetDevice(void)
{
  // leer
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if ((DeviceSpeed != HPRT0_PRTSPD_FULL_SPEED)&&(DeviceSpeed != HPRT0_PRTSPD_LOW_SPEED))
  {
    USB_MSC_HOST_STATUS=USB_MSC_SPEED_ERROR;
  }
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
  // leer
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_DeviceAddressAssigned(void)
{
  // leer
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
    USBH_InterfaceDesc_TypeDef *itfDesc,
    USBH_EpDesc_TypeDef *epDesc)
{
  // leer
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  // leer
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_Product_String(void *ProductString)
{
  // leer
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
// (auslesen von Daten)
//--------------------------------------------------------------
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  // leer
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// nach dem stecken vom Device
//--------------------------------------------------------------
void USBH_USR_EnumerationDone(void)
{
  // leer
} 

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// wenn das Device nicht untrestützt wird
//--------------------------------------------------------------
void USBH_USR_DeviceNotSupported(void)
{
  USB_MSC_HOST_STATUS=USB_MSC_DEV_NOT_SUPPORTED;
}


//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
//--------------------------------------------------------------
USBH_USR_Status USBH_USR_UserInput(void)
{
  return USBH_USR_RESP_OK;
}

//--------------------------------------------------------------
// wird vom USB-Handler aufgerufen
// wenn die Überstrom Überwachung angeschlagen hat
//--------------------------------------------------------------
void USBH_USR_OverCurrentDetected (void)
{
  USB_MSC_HOST_STATUS=USB_MSC_OVER_CURRENT;
}

//--------------------------------------------------------------
// wird zyklisch nach dem einstecken
// vom Device aufgerufen
//--------------------------------------------------------------
int USBH_USR_MSC_Application(void)
{
  
  switch (USBH_USR_ApplicationState) {
    case USH_USR_FS_INIT:
      //-----------------------------------------------
      // diese Case wird nur ein einzigesmal
      // nach dem einstecken vom Device durchlaufen
      //-----------------------------------------------
      USB_MSC_HOST_STATUS=USB_MSC_DEV_CONNECTED;
    
      if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED) {
        USB_MSC_HOST_STATUS=USB_MSC_DEV_WRITE_PROTECT;
      }
        
      USBH_USR_ApplicationState = USH_USR_FS_LOOP;
    break;    
    case USH_USR_FS_LOOP:
      //-----------------------------------------------
      // diese Case wird ständig durchlaufen
      // solange das Device eingesteckt ist
      //-----------------------------------------------
    break;    
    default:
    break;
  }
  return(0);
}

//--------------------------------------------------------------
// wird beim ziehen vom Device aufgerufen
//--------------------------------------------------------------
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}




