//--------------------------------------------------------------
// File     : usb_conf.h
//--------------------------------------------------------------



#ifndef __USB_CONF__H__
#define __USB_CONF__H__

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"


 




 
/****************** USB OTG HS CONFIGURATION **********************************/

 #define RX_FIFO_HS_SIZE                          512
 #define TXH_NP_HS_FIFOSIZ                        256
 #define TXH_P_HS_FIFOSIZ                         256


 #define USB_OTG_HS_INTERNAL_DMA_ENABLED









/****************** C Compilers dependant keywords ****************************/
/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */    
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED                          
  #define __ALIGN_END    __attribute__ ((aligned (4)))
  #define __ALIGN_BEGIN     
#else
  #define __ALIGN_BEGIN
  #define __ALIGN_END   
#endif



#define __packed    __attribute__ ((__packed__))





#endif //__USB_CONF__H__

