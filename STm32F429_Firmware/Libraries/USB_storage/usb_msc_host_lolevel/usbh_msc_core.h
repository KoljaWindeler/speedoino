//--------------------------------------------------------------
// File     : usbh_msc_core.h
//--------------------------------------------------------------
 


#ifndef __USBH_MSC_CORE_H
#define __USBH_MSC_CORE_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_core.h"
#include "usbh_stdreq.h"
#include "usb_bsp.h"
#include "usbh_ioreq.h"
#include "usbh_hcs.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"




/* Structure for MSC process */
typedef struct _MSC_Process
{
  uint8_t              hc_num_in; 
  uint8_t              hc_num_out; 
  uint8_t              MSBulkOutEp;
  uint8_t              MSBulkInEp;
  uint16_t             MSBulkInEpSize;
  uint16_t             MSBulkOutEpSize;
  uint8_t              buff[USBH_MSC_MPS_SIZE];
  uint8_t              maxLun;
}
MSC_Machine_TypeDef; 




#define USB_REQ_BOT_RESET                0xFF
#define USB_REQ_GET_MAX_LUN              0xFE
    


extern USBH_Class_cb_TypeDef  USBH_MSC_cb;
extern MSC_Machine_TypeDef    MSC_Machine;
extern uint8_t MSCErrorCount;



#endif  /* __USBH_MSC_CORE_H */




