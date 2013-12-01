//--------------------------------------------------------------
// File     : usb_bsp.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usb_bsp.h"
#include "usbh_usr.h"
#include "usb_hcd_int.h"



extern USB_OTG_CORE_HANDLE           USB_OTG_Core;



 

//--------------------------------------------------------------
void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
  GPIO_InitTypeDef GPIO_InitStructure;    

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
 
  /* Configure SOF VBUS ID DM DP Pins */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_OTG2_FS) ;   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_OTG_HS, ENABLE) ;  
}

//--------------------------------------------------------------
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  
   
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
}


//--------------------------------------------------------------
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
  if (0 == state)
  { 
    /* DISABLE is needed on output of the Power Switch */
    GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
  else
  {
    /*ENABLE the Power Switch by driving the Enable LOW */
    GPIO_ResetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
}


//--------------------------------------------------------------
void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
   
  RCC_AHB1PeriphClockCmd(HOST_POWERSW_PORT_RCC , ENABLE);  
  
  GPIO_InitStructure.GPIO_Pin = HOST_POWERSW_VBUS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(HOST_POWERSW_PORT,&GPIO_InitStructure);

  /* By Default, DISABLE is needed on output of the Power Switch */
  GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  
  USB_OTG_BSP_mDelay(200);   /* Delay is need for stabilising the Vbus Low 
  in Reset Condition, when Vbus=1 and Reset-button is pressed by user */
}



//--------------------------------------------------------------
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  __IO uint32_t count = 0;
  const uint32_t utime = (120 * usec / 7);
  do
  {
    if ( ++count > utime )
    {
      return ;
    }
  }
  while (1);  
  
}


//--------------------------------------------------------------
void USB_OTG_BSP_mDelay (const uint32_t msec)
{ 
  USB_OTG_BSP_uDelay(msec * 1000);   
}


//--------------------------------------------------------------
void OTG_HS_IRQHandler(void)
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}





