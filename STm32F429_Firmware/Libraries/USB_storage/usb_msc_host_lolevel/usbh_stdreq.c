//--------------------------------------------------------------
// File     : usbh_stdreq.c
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//-------------------------------------------------------------- 
#include "usbh_ioreq.h"
#include "usbh_stdreq.h"






__ALIGN_BEGIN uint8_t          USBH_CfgDesc[512] __ALIGN_END ;



static void USBH_ParseDevDesc (USBH_DevDesc_TypeDef* , uint8_t *buf, uint16_t length);

static void USBH_ParseCfgDesc (USBH_CfgDesc_TypeDef* cfg_desc,
                               USBH_InterfaceDesc_TypeDef* itf_desc,
                               USBH_EpDesc_TypeDef  ep_desc[][USBH_MAX_NUM_ENDPOINTS],                                                           
                               uint8_t *buf, 
                               uint16_t length);


static void USBH_ParseInterfaceDesc (USBH_InterfaceDesc_TypeDef  *if_descriptor, uint8_t *buf);
static void USBH_ParseEPDesc (USBH_EpDesc_TypeDef  *ep_descriptor, uint8_t *buf);

static void USBH_ParseStringDesc (uint8_t* psrc, uint8_t* pdest, uint16_t length);
 


//--------------------------------------------------------------
USBH_Status USBH_Get_DevDesc(USB_OTG_CORE_HANDLE *pdev,
                             USBH_HOST *phost,
                             uint8_t length)
{
  
  USBH_Status status;
  
  if((status = USBH_GetDescriptor(pdev, 
                                  phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,                          
                                  USB_DESC_DEVICE, 
                                  pdev->host.Rx_Buffer,
                                  length)) == USBH_OK)
  {
    /* Commands successfully sent and Response Received */       
    USBH_ParseDevDesc(&phost->device_prop.Dev_Desc, pdev->host.Rx_Buffer, length);
  }
  return status;      
}

//--------------------------------------------------------------
USBH_Status USBH_Get_CfgDesc(USB_OTG_CORE_HANDLE *pdev, 
                             USBH_HOST           *phost,                      
                             uint16_t length)

{
  USBH_Status status;
  uint16_t index = 0;
  
  if((status = USBH_GetDescriptor(pdev,
                                  phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,                          
                                  USB_DESC_CONFIGURATION, 
                                  pdev->host.Rx_Buffer,
                                  length)) == USBH_OK)
  {
    /*save Cfg descriptor for class parsing usage */
    for( ; index < length ; index ++)
    {
      USBH_CfgDesc[index] = pdev->host.Rx_Buffer[index];
    }
    
    /* Commands successfully sent and Response Received  */       
    USBH_ParseCfgDesc (&phost->device_prop.Cfg_Desc,
                       phost->device_prop.Itf_Desc,
                       phost->device_prop.Ep_Desc, 
                       pdev->host.Rx_Buffer,
                       length); 
    
  }
  return status;
}


//--------------------------------------------------------------
USBH_Status USBH_Get_StringDesc(USB_OTG_CORE_HANDLE *pdev,
                                USBH_HOST *phost,
                                uint8_t string_index, 
                                uint8_t *buff, 
                                uint16_t length)
{
  USBH_Status status;
  
  if((status = USBH_GetDescriptor(pdev,
                                  phost,
                                  USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD,                                    
                                  USB_DESC_STRING | string_index, 
                                  pdev->host.Rx_Buffer,
                                  length)) == USBH_OK)
  {
    /* Commands successfully sent and Response Received  */       
    USBH_ParseStringDesc(pdev->host.Rx_Buffer,buff, length);    
  }
  return status;
}

//--------------------------------------------------------------
USBH_Status USBH_GetDescriptor(USB_OTG_CORE_HANDLE *pdev,
                               USBH_HOST           *phost,                                
                               uint8_t  req_type,
                               uint16_t value_idx, 
                               uint8_t* buff, 
                               uint16_t length )
{ 
  phost->Control.setup.b.bmRequestType = USB_D2H | req_type;
  phost->Control.setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
  phost->Control.setup.b.wValue.w = value_idx;
  
  if ((value_idx & 0xff00) == USB_DESC_STRING)
  {
    phost->Control.setup.b.wIndex.w = 0x0409;
  }
  else
  {
    phost->Control.setup.b.wIndex.w = 0;
  }
  phost->Control.setup.b.wLength.w = length;           
  return USBH_CtlReq(pdev, phost, buff , length );     
}

//--------------------------------------------------------------
USBH_Status USBH_SetAddress(USB_OTG_CORE_HANDLE *pdev, 
                            USBH_HOST *phost,
                            uint8_t DeviceAddress)
{
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE | \
    USB_REQ_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQ_SET_ADDRESS;
  
  phost->Control.setup.b.wValue.w = (uint16_t)DeviceAddress;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );
}

//--------------------------------------------------------------
USBH_Status USBH_SetCfg(USB_OTG_CORE_HANDLE *pdev, 
                        USBH_HOST *phost,
                        uint16_t cfg_idx)
{
  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE |\
    USB_REQ_TYPE_STANDARD;
  phost->Control.setup.b.bRequest = USB_REQ_SET_CONFIGURATION;
  phost->Control.setup.b.wValue.w = cfg_idx;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );      
}

//--------------------------------------------------------------
USBH_Status USBH_SetInterface(USB_OTG_CORE_HANDLE *pdev, 
                        USBH_HOST *phost,
                        uint8_t ep_num, uint8_t altSetting)
{
  
  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
    USB_REQ_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQ_SET_INTERFACE;
  phost->Control.setup.b.wValue.w = altSetting;
  phost->Control.setup.b.wIndex.w = ep_num;
  phost->Control.setup.b.wLength.w = 0;           
 
  return USBH_CtlReq(pdev, phost, 0 , 0 );     
}

//--------------------------------------------------------------
USBH_Status USBH_ClrFeature(USB_OTG_CORE_HANDLE *pdev,
                            USBH_HOST *phost,
                            uint8_t ep_num, 
                            uint8_t hc_num) 
{
  
  phost->Control.setup.b.bmRequestType = USB_H2D | 
                                         USB_REQ_RECIPIENT_ENDPOINT |
                                         USB_REQ_TYPE_STANDARD;
  
  phost->Control.setup.b.bRequest = USB_REQ_CLEAR_FEATURE;
  phost->Control.setup.b.wValue.w = FEATURE_SELECTOR_ENDPOINT;
  phost->Control.setup.b.wIndex.w = ep_num;
  phost->Control.setup.b.wLength.w = 0;           
  
  if ((ep_num & USB_REQ_DIR_MASK ) == USB_D2H)
  { /* EP Type is IN */
    pdev->host.hc[hc_num].toggle_in = 0; 
  }
  else
  {/* EP Type is OUT */
    pdev->host.hc[hc_num].toggle_out = 0; 
  }
  
  return USBH_CtlReq(pdev, phost, 0 , 0 );   
}

//--------------------------------------------------------------
static void  USBH_ParseDevDesc (USBH_DevDesc_TypeDef* dev_desc,
                                uint8_t *buf, 
                                uint16_t length)
{
  dev_desc->bLength            = *(uint8_t  *) (buf +  0);
  dev_desc->bDescriptorType    = *(uint8_t  *) (buf +  1);
  dev_desc->bcdUSB             = LE16 (buf +  2);
  dev_desc->bDeviceClass       = *(uint8_t  *) (buf +  4);
  dev_desc->bDeviceSubClass    = *(uint8_t  *) (buf +  5);
  dev_desc->bDeviceProtocol    = *(uint8_t  *) (buf +  6);
  dev_desc->bMaxPacketSize     = *(uint8_t  *) (buf +  7);
  
  if (length > 8)
  { /* For 1st time after device connection, Host may issue only 8 bytes for 
    Device Descriptor Length  */
    dev_desc->idVendor           = LE16 (buf +  8);
    dev_desc->idProduct          = LE16 (buf + 10);
    dev_desc->bcdDevice          = LE16 (buf + 12);
    dev_desc->iManufacturer      = *(uint8_t  *) (buf + 14);
    dev_desc->iProduct           = *(uint8_t  *) (buf + 15);
    dev_desc->iSerialNumber      = *(uint8_t  *) (buf + 16);
    dev_desc->bNumConfigurations = *(uint8_t  *) (buf + 17);
  }
}

//--------------------------------------------------------------
static void  USBH_ParseCfgDesc (USBH_CfgDesc_TypeDef* cfg_desc,
                                USBH_InterfaceDesc_TypeDef* itf_desc,
                                USBH_EpDesc_TypeDef   ep_desc[][USBH_MAX_NUM_ENDPOINTS], 
                                uint8_t *buf, 
                                uint16_t length)
{  
  USBH_InterfaceDesc_TypeDef    *pif ;
  USBH_InterfaceDesc_TypeDef    temp_pif ;  
  USBH_EpDesc_TypeDef           *pep;  
  USBH_DescHeader_t             *pdesc = (USBH_DescHeader_t *)buf;
  uint16_t                      ptr;
  int8_t                        if_ix = 0;
  int8_t                        ep_ix = 0;  
  static uint16_t               prev_ep_size = 0;
  static uint8_t                prev_itf = 0;  
  
  
  pdesc   = (USBH_DescHeader_t *)buf;
  
  /* Parse configuration descriptor */
  cfg_desc->bLength             = *(uint8_t  *) (buf + 0);
  cfg_desc->bDescriptorType     = *(uint8_t  *) (buf + 1);
  cfg_desc->wTotalLength        = LE16 (buf + 2);
  cfg_desc->bNumInterfaces      = *(uint8_t  *) (buf + 4);
  cfg_desc->bConfigurationValue = *(uint8_t  *) (buf + 5);
  cfg_desc->iConfiguration      = *(uint8_t  *) (buf + 6);
  cfg_desc->bmAttributes        = *(uint8_t  *) (buf + 7);
  cfg_desc->bMaxPower           = *(uint8_t  *) (buf + 8);    
  
  
  if (length > USB_CONFIGURATION_DESC_SIZE)
  {
    ptr = USB_LEN_CFG_DESC;
    
    if ( cfg_desc->bNumInterfaces <= USBH_MAX_NUM_INTERFACES) 
    {
      pif = (USBH_InterfaceDesc_TypeDef *)0;
      
      while (ptr < cfg_desc->wTotalLength ) 
      {
        pdesc = USBH_GetNextDesc((uint8_t *)pdesc, &ptr);
        if (pdesc->bDescriptorType   == USB_DESC_TYPE_INTERFACE) 
        {
          if_ix             = *(((uint8_t *)pdesc ) + 2);
          pif               = &itf_desc[if_ix];
          
          if((*((uint8_t *)pdesc + 3)) < 3)
          {
          USBH_ParseInterfaceDesc (&temp_pif, (uint8_t *)pdesc);            
          ep_ix = 0;
          
          /* Parse Ep descriptors relative to the current interface */
          if(temp_pif.bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS)
          {          
            while (ep_ix < temp_pif.bNumEndpoints) 
            {
              pdesc = USBH_GetNextDesc((void* )pdesc, &ptr);
              if (pdesc->bDescriptorType   == USB_DESC_TYPE_ENDPOINT) 
              {  
                pep               = &ep_desc[if_ix][ep_ix];
                
                if(prev_itf != if_ix)
                {
                  prev_itf = if_ix;
                  USBH_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif); 
                }
                else
                {
                  if(prev_ep_size > LE16((uint8_t *)pdesc + 4))
                  {
                    break;
                  }
                  else
                  {
                    USBH_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif);    
                  }
                }
                USBH_ParseEPDesc (pep, (uint8_t *)pdesc);
                prev_ep_size = LE16((uint8_t *)pdesc + 4);
                ep_ix++;
              }
            }
          }
         }
        }
      }
    }
    prev_ep_size = 0;
    prev_itf = 0; 
  }  
}


//--------------------------------------------------------------
static void  USBH_ParseInterfaceDesc (USBH_InterfaceDesc_TypeDef *if_descriptor, 
                                      uint8_t *buf)
{
  if_descriptor->bLength            = *(uint8_t  *) (buf + 0);
  if_descriptor->bDescriptorType    = *(uint8_t  *) (buf + 1);
  if_descriptor->bInterfaceNumber   = *(uint8_t  *) (buf + 2);
  if_descriptor->bAlternateSetting  = *(uint8_t  *) (buf + 3);
  if_descriptor->bNumEndpoints      = *(uint8_t  *) (buf + 4);
  if_descriptor->bInterfaceClass    = *(uint8_t  *) (buf + 5);
  if_descriptor->bInterfaceSubClass = *(uint8_t  *) (buf + 6);
  if_descriptor->bInterfaceProtocol = *(uint8_t  *) (buf + 7);
  if_descriptor->iInterface         = *(uint8_t  *) (buf + 8);
}

//--------------------------------------------------------------
static void  USBH_ParseEPDesc (USBH_EpDesc_TypeDef  *ep_descriptor, 
                               uint8_t *buf)
{
  
  ep_descriptor->bLength          = *(uint8_t  *) (buf + 0);
  ep_descriptor->bDescriptorType  = *(uint8_t  *) (buf + 1);
  ep_descriptor->bEndpointAddress = *(uint8_t  *) (buf + 2);
  ep_descriptor->bmAttributes     = *(uint8_t  *) (buf + 3);
  ep_descriptor->wMaxPacketSize   = LE16 (buf + 4);
  ep_descriptor->bInterval        = *(uint8_t  *) (buf + 6);
}

//--------------------------------------------------------------
static void USBH_ParseStringDesc (uint8_t* psrc, 
                                  uint8_t* pdest, 
                                  uint16_t length)
{
  uint16_t strlength;
  uint16_t idx;
  
  /* The UNICODE string descriptor is not NULL-terminated. The string length is
  computed by substracting two from the value of the first byte of the descriptor.
  */
  
  /* Check which is lower size, the Size of string or the length of bytes read 
  from the device */
  
  if ( psrc[1] == USB_DESC_TYPE_STRING)
  { /* Make sure the Descriptor is String Type */
    
    /* psrc[0] contains Size of Descriptor, subtract 2 to get the length of string */      
    strlength = ( ( (psrc[0]-2) <= length) ? (psrc[0]-2) :length); 
    psrc += 2; /* Adjust the offset ignoring the String Len and Descriptor type */
    
    for (idx = 0; idx < strlength; idx+=2 )
    {/* Copy Only the string and ignore the UNICODE ID, hence add the src */
      *pdest =  psrc[idx];
      pdest++;
    }  
    *pdest = 0; /* mark end of string */  
  }
}

//--------------------------------------------------------------
USBH_DescHeader_t  *USBH_GetNextDesc (uint8_t   *pbuf, uint16_t  *ptr)
{
  USBH_DescHeader_t  *pnext;
 
  *ptr += ((USBH_DescHeader_t *)pbuf)->bLength;  
  pnext = (USBH_DescHeader_t *)((uint8_t *)pbuf + \
         ((USBH_DescHeader_t *)pbuf)->bLength);
 
  return(pnext);
}




