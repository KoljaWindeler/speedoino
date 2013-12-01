//--------------------------------------------------------------
// File     : usbh_msc_core.c
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "usbh_core.h"




 
#define USBH_MSC_ERROR_RETRY_LIMIT 10





__ALIGN_BEGIN MSC_Machine_TypeDef         MSC_Machine __ALIGN_END ;


__ALIGN_BEGIN USB_Setup_TypeDef           MSC_Setup __ALIGN_END ;
uint8_t MSCErrorCount = 0;




static USBH_Status USBH_MSC_InterfaceInit  (USB_OTG_CORE_HANDLE *pdev , 
                                            void *phost);

static void USBH_MSC_InterfaceDeInit  (USB_OTG_CORE_HANDLE *pdev , 
                                       void *phost);

static USBH_Status USBH_MSC_Handle(USB_OTG_CORE_HANDLE *pdev , 
                            void *phost);

static USBH_Status USBH_MSC_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                         void *phost);

static USBH_Status USBH_MSC_BOTReset(USB_OTG_CORE_HANDLE *pdev,
                              USBH_HOST *phost);
static USBH_Status USBH_MSC_GETMaxLUN(USB_OTG_CORE_HANDLE *pdev,
                               USBH_HOST *phost);


USBH_Class_cb_TypeDef  USBH_MSC_cb = 
{
  USBH_MSC_InterfaceInit,
  USBH_MSC_InterfaceDeInit,
  USBH_MSC_ClassRequest,
  USBH_MSC_Handle,
};

void USBH_MSC_ErrorHandle(uint8_t status);



//--------------------------------------------------------------
static USBH_Status USBH_MSC_InterfaceInit ( USB_OTG_CORE_HANDLE *pdev, 
                                        void *phost)
{	 
  USBH_HOST *pphost = phost;
  
  if((pphost->device_prop.Itf_Desc[0].bInterfaceClass == MSC_CLASS) && \
     (pphost->device_prop.Itf_Desc[0].bInterfaceProtocol == MSC_PROTOCOL))
  {
    if(pphost->device_prop.Ep_Desc[0][0].bEndpointAddress & 0x80)
    {
      MSC_Machine.MSBulkInEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
      MSC_Machine.MSBulkInEpSize  = pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
    }
    else
    {
      MSC_Machine.MSBulkOutEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
      MSC_Machine.MSBulkOutEpSize  = pphost->device_prop.Ep_Desc[0] [0].wMaxPacketSize;      
    }
    
    if(pphost->device_prop.Ep_Desc[0][1].bEndpointAddress & 0x80)
    {
      MSC_Machine.MSBulkInEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
      MSC_Machine.MSBulkInEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;      
    }
    else
    {
      MSC_Machine.MSBulkOutEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
      MSC_Machine.MSBulkOutEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;      
    }
    
    MSC_Machine.hc_num_out = USBH_Alloc_Channel(pdev, 
                                                MSC_Machine.MSBulkOutEp);
    MSC_Machine.hc_num_in = USBH_Alloc_Channel(pdev,
                                                MSC_Machine.MSBulkInEp);  
    
    /* Open the new channels */
    USBH_Open_Channel  (pdev,
                        MSC_Machine.hc_num_out,
                        pphost->device_prop.address,
                        pphost->device_prop.speed,
                        EP_TYPE_BULK,
                        MSC_Machine.MSBulkOutEpSize);  
    
    USBH_Open_Channel  (pdev,
                        MSC_Machine.hc_num_in,
                        pphost->device_prop.address,
                        pphost->device_prop.speed,
                        EP_TYPE_BULK,
                        MSC_Machine.MSBulkInEpSize);    
    
  }
  
  else
  {
    pphost->usr_cb->DeviceNotSupported(); 
  }
  
  return USBH_OK ;
 
}


//--------------------------------------------------------------
void USBH_MSC_InterfaceDeInit ( USB_OTG_CORE_HANDLE *pdev,
                                void *phost)
{	
  if ( MSC_Machine.hc_num_out)
  {
    USB_OTG_HC_Halt(pdev, MSC_Machine.hc_num_out);
    USBH_Free_Channel  (pdev, MSC_Machine.hc_num_out);
    MSC_Machine.hc_num_out = 0;     /* Reset the Channel as Free */
  }
   
  if ( MSC_Machine.hc_num_in)
  {
    USB_OTG_HC_Halt(pdev, MSC_Machine.hc_num_in);
    USBH_Free_Channel  (pdev, MSC_Machine.hc_num_in);
    MSC_Machine.hc_num_in = 0;     /* Reset the Channel as Free */
  } 
}


//--------------------------------------------------------------
static USBH_Status USBH_MSC_ClassRequest(USB_OTG_CORE_HANDLE *pdev , 
                                        void *phost)
{   
  
  USBH_Status status = USBH_OK ;
  USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOT_INIT_STATE;
  
  return status; 
}



//--------------------------------------------------------------
static USBH_Status USBH_MSC_Handle(USB_OTG_CORE_HANDLE *pdev , 
                                   void   *phost)
{
  USBH_HOST *pphost = phost;
    
  USBH_Status status = USBH_BUSY;
  uint8_t mscStatus = USBH_MSC_BUSY;
  uint8_t appliStatus = 0;
  
  static uint8_t maxLunExceed = FALSE;
  
    
  if(HCD_IsDeviceConnected(pdev))
  {   
    switch(USBH_MSC_BOTXferParam.MSCState)
    {
    case USBH_MSC_BOT_INIT_STATE:
      USBH_MSC_Init(pdev);
      USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOT_RESET;  
      break;
      
    case USBH_MSC_BOT_RESET:   
      /* Issue BOT RESET request */
      status = USBH_MSC_BOTReset(pdev, phost);
      if(status == USBH_OK )
      {
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_GET_MAX_LUN;
      }
      
      if(status == USBH_NOT_SUPPORTED )
      {
       /* If the Command has failed, then we need to move to Next State, after
        STALL condition is cleared by Control-Transfer */
        USBH_MSC_BOTXferParam.MSCStateBkp = USBH_MSC_GET_MAX_LUN; 

        /* a Clear Feature should be issued here */
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_CTRL_ERROR_STATE;
      }  
      break;
      
    case USBH_MSC_GET_MAX_LUN:
      /* Issue GetMaxLUN request */
      status = USBH_MSC_GETMaxLUN(pdev, phost);
      
      if(status == USBH_OK )
      {
        MSC_Machine.maxLun = *(MSC_Machine.buff) ;
        
        /* If device has more that one logical unit then it is not supported */
        if((MSC_Machine.maxLun > 0) && (maxLunExceed == FALSE))
        {
          maxLunExceed = TRUE;
          pphost->usr_cb->DeviceNotSupported();
          
          break;
        }
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_TEST_UNIT_READY;
      }
      
      if(status == USBH_NOT_SUPPORTED )
      {
               /* If the Command has failed, then we need to move to Next State, after
        STALL condition is cleared by Control-Transfer */
        USBH_MSC_BOTXferParam.MSCStateBkp = USBH_MSC_TEST_UNIT_READY; 
        
        /* a Clear Feature should be issued here */
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_CTRL_ERROR_STATE;
      }    
      break;
      
    case USBH_MSC_CTRL_ERROR_STATE:
      /* Issue Clearfeature request */
      status = USBH_ClrFeature(pdev,
                               phost,
                               0x00,
                               pphost->Control.hc_num_out);
      if(status == USBH_OK )
      {
        /* If GetMaxLun Request not support, assume Single LUN configuration */
        MSC_Machine.maxLun = 0;  
        
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOTXferParam.MSCStateBkp;     
      }
      break;  
      
    case USBH_MSC_TEST_UNIT_READY:
      /* Issue SCSI command TestUnitReady */ 
      mscStatus = USBH_MSC_TestUnitReady(pdev);
      
      if(mscStatus == USBH_MSC_OK )
      {
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_READ_CAPACITY10;
        MSCErrorCount = 0;
        status = USBH_OK;
      }
      else
      {
        USBH_MSC_ErrorHandle(mscStatus);
      } 
      break;
      
    case USBH_MSC_READ_CAPACITY10:
      /* Issue READ_CAPACITY10 SCSI command */
      mscStatus = USBH_MSC_ReadCapacity10(pdev);
      if(mscStatus == USBH_MSC_OK )
      {
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_MODE_SENSE6;
        MSCErrorCount = 0;
        status = USBH_OK;
      }
      else
      {
        USBH_MSC_ErrorHandle(mscStatus);
      }
      break;

    case USBH_MSC_MODE_SENSE6:
      /* Issue ModeSense6 SCSI command for detecting if device is write-protected */
      mscStatus = USBH_MSC_ModeSense6(pdev);
      if(mscStatus == USBH_MSC_OK )
      {
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_DEFAULT_APPLI_STATE;
        MSCErrorCount = 0;
        status = USBH_OK;
      }
      else
      {
        USBH_MSC_ErrorHandle(mscStatus);
      }
      break;
      
    case USBH_MSC_REQUEST_SENSE:
      /* Issue RequestSense SCSI command for retreiving error code */
      mscStatus = USBH_MSC_RequestSense(pdev);
      if(mscStatus == USBH_MSC_OK )
      {
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_BOTXferParam.MSCStateBkp;
        status = USBH_OK;
      }
      else
      {
        USBH_MSC_ErrorHandle(mscStatus);
      }  
      break;
      
    case USBH_MSC_BOT_USB_TRANSFERS:
      /* Process the BOT state machine */
      USBH_MSC_HandleBOTXfer(pdev , phost);
      break;
    
    case USBH_MSC_DEFAULT_APPLI_STATE:
      /* Process Application callback for MSC */
      appliStatus = pphost->usr_cb->UserApplication();
      if(appliStatus == 0)
      {
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_DEFAULT_APPLI_STATE;
      }
      else if (appliStatus == 1) 
      {
        /* De-init requested from application layer */
        status =  USBH_APPLY_DEINIT;
      }
      break;
      
    case USBH_MSC_UNRECOVERED_STATE:
      
      status = USBH_UNRECOVERED_ERROR;
      
      break;
      
    default:
      break; 
      
    }
  }
   return status;
}



//--------------------------------------------------------------
static USBH_Status USBH_MSC_BOTReset(USB_OTG_CORE_HANDLE *pdev,
                              USBH_HOST *phost)
{
  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = USB_REQ_BOT_RESET;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USBH_CtlReq(pdev, phost, 0 , 0 ); 
}


//--------------------------------------------------------------
static USBH_Status USBH_MSC_GETMaxLUN(USB_OTG_CORE_HANDLE *pdev , USBH_HOST *phost)
{
  phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = USB_REQ_GET_MAX_LUN;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 1;           
  
  return USBH_CtlReq(pdev, phost, MSC_Machine.buff , 1 ); 
}


//--------------------------------------------------------------
void USBH_MSC_ErrorHandle(uint8_t status)
{  
    if(status == USBH_MSC_FAIL)
    { 
      MSCErrorCount++;
      if(MSCErrorCount < USBH_MSC_ERROR_RETRY_LIMIT)
      { /* Try MSC level error recovery, Issue the request Sense to get 
        Drive error reason  */
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_REQUEST_SENSE;
        USBH_MSC_BOTXferParam.CmdStateMachine = CMD_SEND_STATE;
      }
      else
      {
        /* Error trials exceeded the limit, go to unrecovered state */
        USBH_MSC_BOTXferParam.MSCState = USBH_MSC_UNRECOVERED_STATE;
      }
    } 
    else if(status == USBH_MSC_PHASE_ERROR)
    {
      /* Phase error, Go to Unrecoovered state */
      USBH_MSC_BOTXferParam.MSCState = USBH_MSC_UNRECOVERED_STATE;
    }
    else if(status == USBH_MSC_BUSY)
    {
      /*No change in state*/
    }
}

