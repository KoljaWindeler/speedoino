//--------------------------------------------------------------
// File     : usb_core.c
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------  
#include "usb_core.h"
#include "usb_bsp.h"








//--------------------------------------------------------------
static void USB_OTG_EnableCommonInt(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_GINTMSK_TypeDef  int_mask;
  
  int_mask.d32 = 0;
  /* Clear any pending USB_OTG Interrupts */
  USB_OTG_WRITE_REG32( &pdev->regs.GREGS->GOTGINT, 0xFFFFFFFF);
  /* Clear any pending interrupts */
  USB_OTG_WRITE_REG32( &pdev->regs.GREGS->GINTSTS, 0xBFFFFFFF);
  /* Enable the interrupts in the INTMSK */
  int_mask.b.wkupintr = 1;
  int_mask.b.usbsuspend = 1; 
  
  USB_OTG_WRITE_REG32( &pdev->regs.GREGS->GINTMSK, int_mask.d32);
}

//--------------------------------------------------------------
static USB_OTG_STS USB_OTG_CoreReset(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  __IO USB_OTG_GRSTCTL_TypeDef  greset;
  uint32_t count = 0;
  
  greset.d32 = 0;
  /* Wait for AHB master IDLE state. */
  do
  {
    USB_OTG_BSP_uDelay(3);
    greset.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GRSTCTL);
    if (++count > 200000)
    {
      return USB_OTG_OK;
    }
  }
  while (greset.b.ahbidle == 0);
  /* Core Soft Reset */
  count = 0;
  greset.b.csftrst = 1;
  USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GRSTCTL, greset.d32 );
  do
  {
    greset.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GRSTCTL);
    if (++count > 200000)
    {
      break;
    }
  }
  while (greset.b.csftrst == 1);
  /* Wait for 3 PHY Clocks*/
  USB_OTG_BSP_uDelay(3);
  return status;
}

//--------------------------------------------------------------
USB_OTG_STS USB_OTG_WritePacket(USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t             *src, 
                                uint8_t             ch_ep_num, 
                                uint16_t            len)
{
  USB_OTG_STS status = USB_OTG_OK;
  if (pdev->cfg.dma_enable == 0)
  {
    uint32_t count32b= 0 , i= 0;
    __IO uint32_t *fifo;
    
    count32b =  (len + 3) / 4;
    fifo = pdev->regs.DFIFO[ch_ep_num];
    for (i = 0; i < count32b; i++, src+=4)
    {
      USB_OTG_WRITE_REG32( fifo, *((__packed uint32_t *)src) );
    }
  }
  return status;
}


//--------------------------------------------------------------
void *USB_OTG_ReadPacket(USB_OTG_CORE_HANDLE *pdev, 
                         uint8_t *dest, 
                         uint16_t len)
{
  uint32_t i=0;
  uint32_t count32b = (len + 3) / 4;
  
  __IO uint32_t *fifo = pdev->regs.DFIFO[0];
  
  for ( i = 0; i < count32b; i++, dest += 4 )
  {
    *(__packed uint32_t *)dest = USB_OTG_READ_REG32(fifo);
    
  }
  return ((void *)dest);
}

//--------------------------------------------------------------
USB_OTG_STS USB_OTG_SelectCore(USB_OTG_CORE_HANDLE *pdev, 
                               USB_OTG_CORE_ID_TypeDef coreID)
{
  uint32_t i , baseAddress = 0;
  USB_OTG_STS status = USB_OTG_OK;
  
  pdev->cfg.dma_enable       = 0;
  
  /* at startup the core is in FS mode */
  pdev->cfg.speed            = USB_OTG_SPEED_FULL;
  pdev->cfg.mps              = USB_OTG_FS_MAX_PACKET_SIZE ;    
  
  /* initialize device cfg following its address */
  if (coreID == USB_OTG_FS_CORE_ID)
  {
    baseAddress                = USB_OTG_FS_BASE_ADDR;
    pdev->cfg.coreID           = USB_OTG_FS_CORE_ID;
    pdev->cfg.host_channels    = 8 ;
    pdev->cfg.dev_endpoints    = 4 ;
    pdev->cfg.TotalFifoSize    = 320; /* in 32-bits */
    pdev->cfg.phy_itface       = USB_OTG_EMBEDDED_PHY;          
  }
  else if (coreID == USB_OTG_HS_CORE_ID)
  {
    baseAddress                = USB_OTG_HS_BASE_ADDR;
    pdev->cfg.coreID           = USB_OTG_HS_CORE_ID;    
    pdev->cfg.host_channels    = 12 ;
    pdev->cfg.dev_endpoints    = 6 ;
    pdev->cfg.TotalFifoSize    = 1280;/* in 32-bits */    
    pdev->cfg.phy_itface       = USB_OTG_EMBEDDED_PHY;
         
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED    
    pdev->cfg.dma_enable       = 1;    
#endif   
    
  }
  
  pdev->regs.GREGS = (USB_OTG_GREGS *)(baseAddress + \
    USB_OTG_CORE_GLOBAL_REGS_OFFSET);
  pdev->regs.DREGS =  (USB_OTG_DREGS  *)  (baseAddress + \
    USB_OTG_DEV_GLOBAL_REG_OFFSET);
  
  for (i = 0; i < pdev->cfg.dev_endpoints; i++)
  {
    pdev->regs.INEP_REGS[i]  = (USB_OTG_INEPREGS *)  \
      (baseAddress + USB_OTG_DEV_IN_EP_REG_OFFSET + \
        (i * USB_OTG_EP_REG_OFFSET));
    pdev->regs.OUTEP_REGS[i] = (USB_OTG_OUTEPREGS *) \
      (baseAddress + USB_OTG_DEV_OUT_EP_REG_OFFSET + \
        (i * USB_OTG_EP_REG_OFFSET));
  }
  pdev->regs.HREGS = (USB_OTG_HREGS *)(baseAddress + \
    USB_OTG_HOST_GLOBAL_REG_OFFSET);
  pdev->regs.HPRT0 = (uint32_t *)(baseAddress + USB_OTG_HOST_PORT_REGS_OFFSET);
  
  for (i = 0; i < pdev->cfg.host_channels; i++)
  {
    pdev->regs.HC_REGS[i] = (USB_OTG_HC_REGS *)(baseAddress + \
      USB_OTG_HOST_CHAN_REGS_OFFSET + \
        (i * USB_OTG_CHAN_REGS_OFFSET));
  }
  for (i = 0; i < pdev->cfg.host_channels; i++)
  {
    pdev->regs.DFIFO[i] = (uint32_t *)(baseAddress + USB_OTG_DATA_FIFO_OFFSET +\
      (i * USB_OTG_DATA_FIFO_SIZE));
  }
  pdev->regs.PCGCCTL = (uint32_t *)(baseAddress + USB_OTG_PCGCCTL_OFFSET);
  
  return status;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_CoreInit(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GUSBCFG_TypeDef  usbcfg;
  USB_OTG_GCCFG_TypeDef    gccfg;
  USB_OTG_GAHBCFG_TypeDef  ahbcfg;
  
  usbcfg.d32 = 0;
  gccfg.d32 = 0;
  ahbcfg.d32 = 0;
  
  
  
  if (pdev->cfg.phy_itface == USB_OTG_ULPI_PHY)
  {
    gccfg.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GCCFG);
    gccfg.b.pwdn = 0;
    
    if (pdev->cfg.Sof_output)
    {
      gccfg.b.sofouten = 1;   
    }
    USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GCCFG, gccfg.d32);
    
    /* Init The ULPI Interface */
    usbcfg.d32 = 0;
    usbcfg.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GUSBCFG);
    
    usbcfg.b.physel            = 0; /* HS Interface */
    
    usbcfg.b.ulpi_ext_vbus_drv = 1; /* Use external VBUS */
 
    usbcfg.b.term_sel_dl_pulse = 0; /* Data line pulsing using utmi_txvalid */    
    
    usbcfg.b.ulpi_fsls = 0;
    usbcfg.b.ulpi_clk_sus_m = 0;
    USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GUSBCFG, usbcfg.d32);
    
    /* Reset after a PHY select  */
    USB_OTG_CoreReset(pdev);
    
    if(pdev->cfg.dma_enable == 1)
    {
      
      ahbcfg.b.hburstlen = 5; /* 64 x 32-bits*/
      ahbcfg.b.dmaenable = 1;
      USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GAHBCFG, ahbcfg.d32);
      
    }    
  }
  else /* FS interface (embedded Phy) */
  {
    
    usbcfg.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GUSBCFG);;
    usbcfg.b.physel  = 1; /* FS Interface */
    USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GUSBCFG, usbcfg.d32);
    /* Reset after a PHY select and set Host mode */
    USB_OTG_CoreReset(pdev);
    /* Deactivate the power down*/
    gccfg.d32 = 0;
    gccfg.b.pwdn = 1;
    
    gccfg.b.vbussensingA = 1 ;
    gccfg.b.vbussensingB = 1 ;     

    gccfg.b.disablevbussensing = 1; 
   
    
    if(pdev->cfg.Sof_output)
    {
      gccfg.b.sofouten = 1;  
    }
    
    USB_OTG_WRITE_REG32 (&pdev->regs.GREGS->GCCFG, gccfg.d32);
    USB_OTG_BSP_mDelay(20);
  }
  /* case the HS core is working in FS mode */
  if(pdev->cfg.dma_enable == 1)
  {
    
    ahbcfg.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GAHBCFG);
    ahbcfg.b.hburstlen = 5; /* 64 x 32-bits*/
    ahbcfg.b.dmaenable = 1;
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GAHBCFG, ahbcfg.d32);
    
  }
  /* initialize OTG features */

  return status;
}

//--------------------------------------------------------------
USB_OTG_STS USB_OTG_EnableGlobalInt(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GAHBCFG_TypeDef  ahbcfg;
  
  ahbcfg.d32 = 0;
  ahbcfg.b.glblintrmsk = 1; /* Enable interrupts */
  USB_OTG_MODIFY_REG32(&pdev->regs.GREGS->GAHBCFG, 0, ahbcfg.d32);
  return status;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_DisableGlobalInt(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GAHBCFG_TypeDef  ahbcfg;
  ahbcfg.d32 = 0;
  ahbcfg.b.glblintrmsk = 1; /* Enable interrupts */
  USB_OTG_MODIFY_REG32(&pdev->regs.GREGS->GAHBCFG, ahbcfg.d32, 0);
  return status;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_FlushTxFifo (USB_OTG_CORE_HANDLE *pdev , uint32_t num )
{
  USB_OTG_STS status = USB_OTG_OK;
  __IO USB_OTG_GRSTCTL_TypeDef  greset;
  
  uint32_t count = 0;
  greset.d32 = 0;
  greset.b.txfflsh = 1;
  greset.b.txfnum  = num;
  USB_OTG_WRITE_REG32( &pdev->regs.GREGS->GRSTCTL, greset.d32 );
  do
  {
    greset.d32 = USB_OTG_READ_REG32( &pdev->regs.GREGS->GRSTCTL);
    if (++count > 200000)
    {
      break;
    }
  }
  while (greset.b.txfflsh == 1);
  /* Wait for 3 PHY Clocks*/
  USB_OTG_BSP_uDelay(3);
  return status;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_FlushRxFifo( USB_OTG_CORE_HANDLE *pdev )
{
  USB_OTG_STS status = USB_OTG_OK;
  __IO USB_OTG_GRSTCTL_TypeDef  greset;
  uint32_t count = 0;
  
  greset.d32 = 0;
  greset.b.rxfflsh = 1;
  USB_OTG_WRITE_REG32( &pdev->regs.GREGS->GRSTCTL, greset.d32 );
  do
  {
    greset.d32 = USB_OTG_READ_REG32( &pdev->regs.GREGS->GRSTCTL);
    if (++count > 200000)
    {
      break;
    }
  }
  while (greset.b.rxfflsh == 1);
  /* Wait for 3 PHY Clocks*/
  USB_OTG_BSP_uDelay(3);
  return status;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_SetCurrentMode(USB_OTG_CORE_HANDLE *pdev , uint8_t mode)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GUSBCFG_TypeDef  usbcfg;
  
  usbcfg.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GUSBCFG);
  
  usbcfg.b.force_host = 0;
  usbcfg.b.force_dev = 0;
  
  if ( mode == HOST_MODE)
  {
    usbcfg.b.force_host = 1;
  }
  else if ( mode == DEVICE_MODE)
  {
    usbcfg.b.force_dev = 1;
  }
  
  USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GUSBCFG, usbcfg.d32);
  USB_OTG_BSP_mDelay(50);
  return status;
}


//--------------------------------------------------------------
uint32_t USB_OTG_GetMode(USB_OTG_CORE_HANDLE *pdev)
{
  return (USB_OTG_READ_REG32(&pdev->regs.GREGS->GINTSTS ) & 0x1);
}


//--------------------------------------------------------------
uint8_t USB_OTG_IsDeviceMode(USB_OTG_CORE_HANDLE *pdev)
{
  return (USB_OTG_GetMode(pdev) != HOST_MODE);
}


//--------------------------------------------------------------
uint8_t USB_OTG_IsHostMode(USB_OTG_CORE_HANDLE *pdev)
{
  return (USB_OTG_GetMode(pdev) == HOST_MODE);
}


//--------------------------------------------------------------
uint32_t USB_OTG_ReadCoreItr(USB_OTG_CORE_HANDLE *pdev)
{
  uint32_t v = 0;
  v = USB_OTG_READ_REG32(&pdev->regs.GREGS->GINTSTS);
  v &= USB_OTG_READ_REG32(&pdev->regs.GREGS->GINTMSK);
  return v;
}


//--------------------------------------------------------------
uint32_t USB_OTG_ReadOtgItr (USB_OTG_CORE_HANDLE *pdev)
{
  return (USB_OTG_READ_REG32 (&pdev->regs.GREGS->GOTGINT));
}



//--------------------------------------------------------------
USB_OTG_STS USB_OTG_CoreInitHost(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_STS                     status = USB_OTG_OK;
  USB_OTG_FSIZ_TypeDef            nptxfifosize;
  USB_OTG_FSIZ_TypeDef            ptxfifosize;  
  USB_OTG_HCFG_TypeDef            hcfg;
  
  uint32_t                        i = 0;
  
  nptxfifosize.d32 = 0;  
  ptxfifosize.d32 = 0;
  hcfg.d32 = 0;
  
  
  /* configure charge pump IO */
  USB_OTG_BSP_ConfigVBUS(pdev);
  
  /* Restart the Phy Clock */
  USB_OTG_WRITE_REG32(pdev->regs.PCGCCTL, 0);
  
  /* Initialize Host Configuration Register */
  if (pdev->cfg.phy_itface == USB_OTG_ULPI_PHY)
  {
    USB_OTG_InitFSLSPClkSel(pdev , HCFG_30_60_MHZ); 
  }
  else
  {
    USB_OTG_InitFSLSPClkSel(pdev , HCFG_48_MHZ); 
  }
  USB_OTG_ResetPort(pdev);
  
  hcfg.d32 = USB_OTG_READ_REG32(&pdev->regs.HREGS->HCFG);
  hcfg.b.fslssupp = 0;
  USB_OTG_WRITE_REG32(&pdev->regs.HREGS->HCFG, hcfg.d32);
  
  /* Configure data FIFO sizes */
  /* Rx FIFO */
 
  if (pdev->cfg.coreID == USB_OTG_HS_CORE_ID)
  {
    /* set Rx FIFO size */
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GRXFSIZ, RX_FIFO_HS_SIZE);
    nptxfifosize.b.startaddr = RX_FIFO_HS_SIZE;   
    nptxfifosize.b.depth = TXH_NP_HS_FIFOSIZ;  
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->DIEPTXF0_HNPTXFSIZ, nptxfifosize.d32);
    
    ptxfifosize.b.startaddr = RX_FIFO_HS_SIZE + TXH_NP_HS_FIFOSIZ;
    ptxfifosize.b.depth     = TXH_P_HS_FIFOSIZ;
    USB_OTG_WRITE_REG32(&pdev->regs.GREGS->HPTXFSIZ, ptxfifosize.d32);      
  }  
  
  /* Make sure the FIFOs are flushed. */
  USB_OTG_FlushTxFifo(pdev, 0x10 );         /* all Tx FIFOs */
  USB_OTG_FlushRxFifo(pdev);
  
  
  /* Clear all pending HC Interrupts */
  for (i = 0; i < pdev->cfg.host_channels; i++)
  {
    USB_OTG_WRITE_REG32( &pdev->regs.HC_REGS[i]->HCINT, 0xFFFFFFFF );
    USB_OTG_WRITE_REG32( &pdev->regs.HC_REGS[i]->HCINTMSK, 0 );
  }

  USB_OTG_DriveVbus(pdev, 1);

  
  USB_OTG_EnableHostInt(pdev);
  return status;
}

//--------------------------------------------------------------
uint8_t USB_OTG_IsEvenFrame (USB_OTG_CORE_HANDLE *pdev) 
{
  return !(USB_OTG_READ_REG32(&pdev->regs.HREGS->HFNUM) & 0x1);
}

//--------------------------------------------------------------
void USB_OTG_DriveVbus (USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
  USB_OTG_HPRT0_TypeDef     hprt0;
  
  hprt0.d32 = 0;
  
  /* enable disable the external charge pump */
  USB_OTG_BSP_DriveVBUS(pdev, state);
  
  /* Turn on the Host port power. */
  hprt0.d32 = USB_OTG_ReadHPRT0(pdev);
  if ((hprt0.b.prtpwr == 0 ) && (state == 1 ))
  {
    hprt0.b.prtpwr = 1;
    USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
  }
  if ((hprt0.b.prtpwr == 1 ) && (state == 0 ))
  {
    hprt0.b.prtpwr = 0;
    USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
  }
  
  USB_OTG_BSP_mDelay(200);
}

//--------------------------------------------------------------
USB_OTG_STS USB_OTG_EnableHostInt(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_STS       status = USB_OTG_OK;
  USB_OTG_GINTMSK_TypeDef  intmsk;
  intmsk.d32 = 0;
  /* Disable all interrupts. */
  USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTMSK, 0);
  
  /* Clear any pending interrupts. */
  USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GINTSTS, 0xFFFFFFFF);
  
  /* Enable the common interrupts */
  USB_OTG_EnableCommonInt(pdev);
  
  if (pdev->cfg.dma_enable == 0)
  {  
    intmsk.b.rxstsqlvl  = 1;
  }  
  intmsk.b.portintr   = 1;
  intmsk.b.hcintr     = 1;
  intmsk.b.disconnect = 1;  
  intmsk.b.sofintr    = 1;  
  intmsk.b.incomplisoout  = 1; 
  USB_OTG_MODIFY_REG32(&pdev->regs.GREGS->GINTMSK, intmsk.d32, intmsk.d32);
  return status;
}

//--------------------------------------------------------------
void USB_OTG_InitFSLSPClkSel(USB_OTG_CORE_HANDLE *pdev , uint8_t freq)
{
  USB_OTG_HCFG_TypeDef   hcfg;
  
  hcfg.d32 = USB_OTG_READ_REG32(&pdev->regs.HREGS->HCFG);
  hcfg.b.fslspclksel = freq;
  USB_OTG_WRITE_REG32(&pdev->regs.HREGS->HCFG, hcfg.d32);
}


//--------------------------------------------------------------
uint32_t USB_OTG_ReadHPRT0(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_HPRT0_TypeDef  hprt0;
  
  hprt0.d32 = USB_OTG_READ_REG32(pdev->regs.HPRT0);
  hprt0.b.prtena = 0;
  hprt0.b.prtconndet = 0;
  hprt0.b.prtenchng = 0;
  hprt0.b.prtovrcurrchng = 0;
  return hprt0.d32;
}


//--------------------------------------------------------------
uint32_t USB_OTG_ReadHostAllChannels_intr (USB_OTG_CORE_HANDLE *pdev)
{
  return (USB_OTG_READ_REG32 (&pdev->regs.HREGS->HAINT));
}


//--------------------------------------------------------------
uint32_t USB_OTG_ResetPort(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_HPRT0_TypeDef  hprt0;
  
  hprt0.d32 = USB_OTG_ReadHPRT0(pdev);
  hprt0.b.prtrst = 1;
  USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
  USB_OTG_BSP_mDelay (10);                                /* See Note #1 */
  hprt0.b.prtrst = 0;
  USB_OTG_WRITE_REG32(pdev->regs.HPRT0, hprt0.d32);
  USB_OTG_BSP_mDelay (20);   
  return 1;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_HC_Init(USB_OTG_CORE_HANDLE *pdev , uint8_t hc_num)
{
  USB_OTG_STS status = USB_OTG_OK;
  uint32_t intr_enable = 0;
  USB_OTG_HCINTMSK_TypeDef  hcintmsk;
  USB_OTG_GINTMSK_TypeDef    gintmsk;
  USB_OTG_HCCHAR_TypeDef     hcchar;
  USB_OTG_HCINTn_TypeDef     hcint;
  
  
  gintmsk.d32 = 0;
  hcintmsk.d32 = 0;
  hcchar.d32 = 0;
  
  /* Clear old interrupt conditions for this host channel. */
  hcint.d32 = 0xFFFFFFFF;
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCINT, hcint.d32);
  
  /* Enable channel interrupts required for this transfer. */
  hcintmsk.d32 = 0;
  
  if (pdev->cfg.dma_enable == 1)
  {
    hcintmsk.b.ahberr = 1;
  }
  
  switch (pdev->host.hc[hc_num].ep_type) 
  {
  case EP_TYPE_CTRL:
  case EP_TYPE_BULK:
    hcintmsk.b.xfercompl = 1;
    hcintmsk.b.stall = 1;
    hcintmsk.b.xacterr = 1;
    hcintmsk.b.datatglerr = 1;
    hcintmsk.b.nak = 1;  
    if (pdev->host.hc[hc_num].ep_is_in) 
    {
      hcintmsk.b.bblerr = 1;
    } 
    else 
    {
      hcintmsk.b.nyet = 1;
      if (pdev->host.hc[hc_num].do_ping) 
      {
        hcintmsk.b.ack = 1;
      }
    }
    break;
  case EP_TYPE_INTR:
    hcintmsk.b.xfercompl = 1;
    hcintmsk.b.nak = 1;
    hcintmsk.b.stall = 1;
    hcintmsk.b.xacterr = 1;
    hcintmsk.b.datatglerr = 1;
    hcintmsk.b.frmovrun = 1;
    
    if (pdev->host.hc[hc_num].ep_is_in) 
    {
      hcintmsk.b.bblerr = 1;
    }
    
    break;
  case EP_TYPE_ISOC:
    hcintmsk.b.xfercompl = 1;
    hcintmsk.b.frmovrun = 1;
    hcintmsk.b.ack = 1;
    
    if (pdev->host.hc[hc_num].ep_is_in) 
    {
      hcintmsk.b.xacterr = 1;
      hcintmsk.b.bblerr = 1;
    }
    break;
  }
  
  
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCINTMSK, hcintmsk.d32);
  
  
  /* Enable the top level host channel interrupt. */
  intr_enable = (1 << hc_num);
  USB_OTG_MODIFY_REG32(&pdev->regs.HREGS->HAINTMSK, 0, intr_enable);
  
  /* Make sure host channel interrupts are enabled. */
  gintmsk.b.hcintr = 1;
  USB_OTG_MODIFY_REG32(&pdev->regs.GREGS->GINTMSK, 0, gintmsk.d32);
  
  /* Program the HCCHAR register */
  hcchar.d32 = 0;
  hcchar.b.devaddr = pdev->host.hc[hc_num].dev_addr;
  hcchar.b.epnum   = pdev->host.hc[hc_num].ep_num;
  hcchar.b.epdir   = pdev->host.hc[hc_num].ep_is_in;
  hcchar.b.lspddev = (pdev->host.hc[hc_num].speed == HPRT0_PRTSPD_LOW_SPEED);
  hcchar.b.eptype  = pdev->host.hc[hc_num].ep_type;
  hcchar.b.mps     = pdev->host.hc[hc_num].max_packet;
  if (pdev->host.hc[hc_num].ep_type == HCCHAR_INTR)
  {
    hcchar.b.oddfrm  = 1;
  }
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.d32);
  return status;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_HC_StartXfer(USB_OTG_CORE_HANDLE *pdev , uint8_t hc_num)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_HCCHAR_TypeDef   hcchar;
  USB_OTG_HCTSIZn_TypeDef  hctsiz;
  USB_OTG_HNPTXSTS_TypeDef hnptxsts; 
  USB_OTG_HPTXSTS_TypeDef  hptxsts; 
  USB_OTG_GINTMSK_TypeDef  intmsk;
  uint16_t                 len_words = 0;   
  
  uint16_t num_packets;
  uint16_t max_hc_pkt_count;
  
  max_hc_pkt_count = 256;
  hctsiz.d32 = 0;
  hcchar.d32 = 0;
  intmsk.d32 = 0;
  
  /* Compute the expected number of packets associated to the transfer */
  if (pdev->host.hc[hc_num].xfer_len > 0)
  {
    num_packets = (pdev->host.hc[hc_num].xfer_len + \
      pdev->host.hc[hc_num].max_packet - 1) / pdev->host.hc[hc_num].max_packet;
    
    if (num_packets > max_hc_pkt_count)
    {
      num_packets = max_hc_pkt_count;
      pdev->host.hc[hc_num].xfer_len = num_packets * \
        pdev->host.hc[hc_num].max_packet;
    }
  }
  else
  {
    num_packets = 1;
  }
  if (pdev->host.hc[hc_num].ep_is_in)
  {
    pdev->host.hc[hc_num].xfer_len = num_packets * \
      pdev->host.hc[hc_num].max_packet;
  }
  /* Initialize the HCTSIZn register */
  hctsiz.b.xfersize = pdev->host.hc[hc_num].xfer_len;
  hctsiz.b.pktcnt = num_packets;
  hctsiz.b.pid = pdev->host.hc[hc_num].data_pid;
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCTSIZ, hctsiz.d32);
  
  if (pdev->cfg.dma_enable == 1)
  {
    USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCDMA, (unsigned int)pdev->host.hc[hc_num].xfer_buff);
  }
  
  
  hcchar.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[hc_num]->HCCHAR);
  hcchar.b.oddfrm = USB_OTG_IsEvenFrame(pdev);
  
  /* Set host channel enable */
  hcchar.b.chen = 1;
  hcchar.b.chdis = 0;
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.d32);
  
  if (pdev->cfg.dma_enable == 0) /* Slave mode */
  {  
    if((pdev->host.hc[hc_num].ep_is_in == 0) && 
       (pdev->host.hc[hc_num].xfer_len > 0))
    {
      switch(pdev->host.hc[hc_num].ep_type) 
      {
        /* Non periodic transfer */
      case EP_TYPE_CTRL:
      case EP_TYPE_BULK:
        
        hnptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->HNPTXSTS);
        len_words = (pdev->host.hc[hc_num].xfer_len + 3) / 4;
        
        /* check if there is enough space in FIFO space */
        if(len_words > hnptxsts.b.nptxfspcavail)
        {
          /* need to process data in nptxfempty interrupt */
          intmsk.b.nptxfempty = 1;
          USB_OTG_MODIFY_REG32( &pdev->regs.GREGS->GINTMSK, 0, intmsk.d32);  
        }
        
        break;
        /* Periodic transfer */
      case EP_TYPE_INTR:
      case EP_TYPE_ISOC:
        hptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.HREGS->HPTXSTS);
        len_words = (pdev->host.hc[hc_num].xfer_len + 3) / 4;
        /* check if there is enough space in FIFO space */
        if(len_words > hptxsts.b.ptxfspcavail) /* split the transfer */
        {
          /* need to process data in ptxfempty interrupt */
          intmsk.b.ptxfempty = 1;
          USB_OTG_MODIFY_REG32( &pdev->regs.GREGS->GINTMSK, 0, intmsk.d32);  
        }
        break;
        
      default:
        break;
      }
      
      /* Write packet into the Tx FIFO. */
      USB_OTG_WritePacket(pdev, 
                          pdev->host.hc[hc_num].xfer_buff , 
                          hc_num, pdev->host.hc[hc_num].xfer_len);
    }
  }
  return status;
}


//--------------------------------------------------------------
USB_OTG_STS USB_OTG_HC_Halt(USB_OTG_CORE_HANDLE *pdev , uint8_t hc_num)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_HNPTXSTS_TypeDef            nptxsts;
  USB_OTG_HPTXSTS_TypeDef             hptxsts;
  USB_OTG_HCCHAR_TypeDef              hcchar;
  
  nptxsts.d32 = 0;
  hptxsts.d32 = 0;
  hcchar.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[hc_num]->HCCHAR);
  hcchar.b.chen = 1;
  hcchar.b.chdis = 1;
  
  /* Check for space in the request queue to issue the halt. */
  if (hcchar.b.eptype == HCCHAR_CTRL || hcchar.b.eptype == HCCHAR_BULK)
  {
    nptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->HNPTXSTS);
    if (nptxsts.b.nptxqspcavail == 0)
    {
      hcchar.b.chen = 0;
    }
  }
  else
  {
    hptxsts.d32 = USB_OTG_READ_REG32(&pdev->regs.HREGS->HPTXSTS);
    if (hptxsts.b.ptxqspcavail == 0)
    {
      hcchar.b.chen = 0;
    }
  }
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.d32);
  return status;
}

//--------------------------------------------------------------
USB_OTG_STS USB_OTG_HC_DoPing(USB_OTG_CORE_HANDLE *pdev , uint8_t hc_num)
{
  USB_OTG_STS               status = USB_OTG_OK;
  USB_OTG_HCCHAR_TypeDef    hcchar;
  USB_OTG_HCTSIZn_TypeDef   hctsiz;  
  
  hctsiz.d32 = 0;
  hctsiz.b.dopng = 1;
  hctsiz.b.pktcnt = 1;
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCTSIZ, hctsiz.d32);
  
  hcchar.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[hc_num]->HCCHAR);
  hcchar.b.chen = 1;
  hcchar.b.chdis = 0;
  USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.d32);
  return status;  
}

//--------------------------------------------------------------
void USB_OTG_StopHost(USB_OTG_CORE_HANDLE *pdev)
{
  USB_OTG_HCCHAR_TypeDef  hcchar;
  uint32_t                i;
  
  USB_OTG_WRITE_REG32(&pdev->regs.HREGS->HAINTMSK , 0);
  USB_OTG_WRITE_REG32(&pdev->regs.HREGS->HAINT,      0xFFFFFFFF);
  /* Flush out any leftover queued requests. */
  
  for (i = 0; i < pdev->cfg.host_channels; i++)
  {
    hcchar.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[i]->HCCHAR);
    hcchar.b.chen = 0;
    hcchar.b.chdis = 1;
    hcchar.b.epdir = 0;
    USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[i]->HCCHAR, hcchar.d32);
  }
  
  /* Flush the FIFO */
  USB_OTG_FlushRxFifo(pdev);
  USB_OTG_FlushTxFifo(pdev ,  0x10 );  
}




