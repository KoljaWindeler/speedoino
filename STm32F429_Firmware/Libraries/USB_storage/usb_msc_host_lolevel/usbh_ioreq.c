//--------------------------------------------------------------
// File     : usbh_ioreq.c
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_ioreq.h"






static USBH_Status USBH_SubmitSetupRequest(USBH_HOST *phost,
                                           uint8_t* buff, 
                                           uint16_t length);

//--------------------------------------------------------------
USBH_Status USBH_CtlReq     (USB_OTG_CORE_HANDLE *pdev, 
                             USBH_HOST           *phost, 
                             uint8_t             *buff,
                             uint16_t            length)
{
  USBH_Status status;
  status = USBH_BUSY;
  
  switch (phost->RequestState)
  {
  case CMD_SEND:
    /* Start a SETUP transfer */
    USBH_SubmitSetupRequest(phost, buff, length);
    phost->RequestState = CMD_WAIT;
    status = USBH_BUSY;
    break;
    
  case CMD_WAIT:
     if (phost->Control.state == CTRL_COMPLETE ) 
    {
      /* Commands successfully sent and Response Received  */       
      phost->RequestState = CMD_SEND;
      phost->Control.state =CTRL_IDLE;  
      status = USBH_OK;      
    }
    else if  (phost->Control.state == CTRL_ERROR)
    {
      /* Failure Mode */
      phost->RequestState = CMD_SEND;
      status = USBH_FAIL;
    }   
     else if  (phost->Control.state == CTRL_STALLED )
    {
      /* Commands successfully sent and Response Received  */       
      phost->RequestState = CMD_SEND;
      status = USBH_NOT_SUPPORTED;
    }
    break;
    
  default:
    break; 
  }
  return status;
}

//--------------------------------------------------------------
USBH_Status USBH_CtlSendSetup ( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint8_t hc_num){
  pdev->host.hc[hc_num].ep_is_in = 0;
  pdev->host.hc[hc_num].data_pid = HC_PID_SETUP;   
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = USBH_SETUP_PKT_SIZE;   

  return (USBH_Status)HCD_SubmitRequest (pdev , hc_num);   
}


//--------------------------------------------------------------
USBH_Status USBH_CtlSendData ( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{
  pdev->host.hc[hc_num].ep_is_in = 0;
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
 
  if ( length == 0 )
  { /* For Status OUT stage, Length==0, Status Out PID = 1 */
    pdev->host.hc[hc_num].toggle_out = 1;   
  }
 
 /* Set the Data Toggle bit as per the Flag */
  if ( pdev->host.hc[hc_num].toggle_out == 0)
  { /* Put the PID 0 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;    
  }
 else
 { /* Put the PID 1 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA1 ;
 }

  HCD_SubmitRequest (pdev , hc_num);   
   
  return USBH_OK;
}


//--------------------------------------------------------------
USBH_Status USBH_CtlReceiveData(USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t* buff, 
                                uint16_t length,
                                uint8_t hc_num)
{

  pdev->host.hc[hc_num].ep_is_in = 1;
  pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;  

  HCD_SubmitRequest (pdev , hc_num);   
  
  return USBH_OK;
  
}


//--------------------------------------------------------------
USBH_Status USBH_BulkSendData ( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{ 
  pdev->host.hc[hc_num].ep_is_in = 0;
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;  

 /* Set the Data Toggle bit as per the Flag */
  if ( pdev->host.hc[hc_num].toggle_out == 0)
  { /* Put the PID 0 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;    
  }
 else
 { /* Put the PID 1 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA1 ;
 }

  HCD_SubmitRequest (pdev , hc_num);   
  return USBH_OK;
}


//--------------------------------------------------------------
USBH_Status USBH_BulkReceiveData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{
  pdev->host.hc[hc_num].ep_is_in = 1;   
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  

  if( pdev->host.hc[hc_num].toggle_in == 0)
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  HCD_SubmitRequest (pdev , hc_num);  
  return USBH_OK;
}


//--------------------------------------------------------------
USBH_Status USBH_InterruptReceiveData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint8_t length,
                                uint8_t hc_num)
{

  pdev->host.hc[hc_num].ep_is_in = 1;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  

  
  if(pdev->host.hc[hc_num].toggle_in == 0)
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  /* toggle DATA PID */
  pdev->host.hc[hc_num].toggle_in ^= 1;  
  
  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
}

//--------------------------------------------------------------
USBH_Status USBH_InterruptSendData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint8_t length,
                                uint8_t hc_num)
{

  pdev->host.hc[hc_num].ep_is_in = 0;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  
  if(pdev->host.hc[hc_num].toggle_in == 0)
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  pdev->host.hc[hc_num].toggle_in ^= 1;  
  
  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
}


//--------------------------------------------------------------
static USBH_Status USBH_SubmitSetupRequest(USBH_HOST *phost,
                                           uint8_t* buff, 
                                           uint16_t length)
{
  
  /* Save Global State */
  phost->gStateBkp =   phost->gState; 
  
  /* Prepare the Transactions */
  phost->gState = HOST_CTRL_XFER;
  phost->Control.buff = buff; 
  phost->Control.length = length;
  phost->Control.state = CTRL_SETUP;  

  return USBH_OK;  
}


//--------------------------------------------------------------
USBH_Status USBH_IsocReceiveData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num)
{    
  
  pdev->host.hc[hc_num].ep_is_in = 1;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  

  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
}

//--------------------------------------------------------------
USBH_Status USBH_IsocSendData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num)
{
  
  pdev->host.hc[hc_num].ep_is_in = 0;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  
  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
}



