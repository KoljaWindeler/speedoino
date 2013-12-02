//#include "ILI9341.h"
//#include "fonts.h"
#include "global.h"



#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))   
#define ABS(X)  ((X) > 0 ? (X) : -(X))

ILI9341::ILI9341(){
	CurrentTextColor   = 0x0000;
	CurrentBackColor   = 0xFFFF;
	/* Default LCD configuration with LCD Layer 1 */
	CurrentFrameBuffer = LCD_FRAME_BUFFER;
	CurrentLayer = LCD_BACKGROUND_LAYER;
	transparent_font=false;
}


void ILI9341::LCD_DeInit(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;

	/* LCD Display Off */
	DisplayOff();

	/* LCD_SPI disable */
	SPI_Cmd(LCD_SPI, DISABLE);

	/* LCD_SPI DeInit */
	SPI_I2S_DeInit(LCD_SPI);

	/* Disable SPI clock  */
	RCC_APB2PeriphClockCmd(LCD_SPI_CLK, DISABLE);

	/* Configure NCS in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LCD_NCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_NCS_GPIO_PORT, &GPIO_InitStructure);

	/* Configure SPI pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_SCK_PIN;
	GPIO_Init(LCD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MISO_PIN;
	GPIO_Init(LCD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MOSI_PIN;
	GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* GPIOA configuration */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB configuration */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_8 | GPIO_Pin_9  |  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOC configuration */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3  | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* GPIOF configuration */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* GPIOG configuration */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/**
 * @brief  Initializes the LCD.
 * @param  None
 * @retval None
 */
void ILI9341::init(void)
{ 
	LTDC_InitTypeDef       LTDC_InitStruct;

	/* Configure the LCD Control pins ------------------------------------------*/
	LCD_CtrlLinesConfig();
	LCD_ChipSelect(DISABLE);
	LCD_ChipSelect(ENABLE);

	/* Configure the LCD_SPI interface -----------------------------------------*/
	LCD_SPIConfig();

	/* Power on the LCD --------------------------------------------------------*/
	LCD_PowerOn();

	/* Enable the LTDC Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);

	/* Enable the DMA2D Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);

	/* Configure the LCD Control pins */
	LCD_AF_GPIOConfig();

	/* Configure the FMC Parallel interface : SDRAM is used as Frame Buffer for LCD */
	SDRAM_Init();

	/* LTDC Configuration *********************************************************/
	/* Polarity configuration */
	/* Initialize the horizontal synchronization polarity as active low */
	LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;
	/* Initialize the vertical synchronization polarity as active low */
	LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
	/* Initialize the data enable polarity as active low */
	LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;
	/* Initialize the pixel clock polarity as input pixel clock */
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

	/* Configure R,G,B component values for LCD background color */
	LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
	LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
	LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;

	/* Configure PLLSAI prescalers for LCD */
	/* Enable Pixel Clock */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/4 = 48 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 48/8 = 6 Mhz */
	RCC_PLLSAIConfig(192, 7, 4);
	RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8);

	/* Enable PLLSAI Clock */
	RCC_PLLSAICmd(ENABLE);
	/* Wait for PLLSAI activation */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
	{
	}

	/* Timing configuration */
	/* Configure horizontal synchronization width */
	LTDC_InitStruct.LTDC_HorizontalSync = 9;
	/* Configure vertical synchronization height */
	LTDC_InitStruct.LTDC_VerticalSync = 1;
	/* Configure accumulated horizontal back porch */
	LTDC_InitStruct.LTDC_AccumulatedHBP = 29;
	/* Configure accumulated vertical back porch */
	LTDC_InitStruct.LTDC_AccumulatedVBP = 3;
	/* Configure accumulated active width */
	LTDC_InitStruct.LTDC_AccumulatedActiveW = 269;
	/* Configure accumulated active height */
	LTDC_InitStruct.LTDC_AccumulatedActiveH = 323;
	/* Configure total width */
	LTDC_InitStruct.LTDC_TotalWidth = 279;
	/* Configure total height */
	LTDC_InitStruct.LTDC_TotalHeigh = 327;

	LTDC_Init(&LTDC_InitStruct);

	/* Initialize the LCD */
	TFT.LayerInit();
	LTDC_Cmd(ENABLE);
	TFT.SetLayer(LCD_FOREGROUND_LAYER);
	TFT.clear_screen(LCD_COLOR_BLACK);
	TFT.SetTransparency(0);
	LTDC_ReloadConfig(LTDC_IMReload);
	SDRAM_Init();												/* SDRAM Initialization */
	SDRAM_GPIOConfig();											/* FMC SDRAM GPIOs Configuration */
	FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM,DISABLE);	/* Disable write protection */
}  

/**
 * @brief  Initializes the LCD Layers.
 * @param  None
 * @retval None
 */
void ILI9341::LayerInit(void)
{
	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

	/* Windowing configuration */
	/* In this case all the active display area is used to display a picture then :
  Horizontal start = horizontal synchronization + Horizontal back porch = 30 
  Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
  Vertical start   = vertical synchronization + vertical back porch     = 4
  Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */      
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = (LCD_PIXEL_WIDTH + 30 - 1);
	LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
	LTDC_Layer_InitStruct.LTDC_VerticalStop = (LCD_PIXEL_HEIGHT + 4 - 1);

	/* Pixel Format configuration*/
	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	/* Alpha constant (255 totally opaque) */
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
	/* Default Color configuration (configure A,R,G,B component values) */
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
	/* Configure blending factors */
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

	/* the length of one line of pixels in bytes + 3 then :
  Line Lenth = Active high width x number of bytes per pixel + 3 
  Active high width         = LCD_PIXEL_WIDTH 
  number of bytes per pixel = 2    (pixel_format : RGB565) 
	 */
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * 2) + 3);
	/* the pitch is the increment from the start of one line of pixels to the
  start of the next line in bytes, then :
  Pitch = Active high width x number of bytes per pixel */ 
	LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 2);

	/* Configure the number of lines */
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;

	/* Initialize LTDC layer 1 */
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	/* Configure Layer2 */
	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM w/ Offset */
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;

	/* Configure blending factors */
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;

	/* Initialize LTDC layer 2 */
	LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

	/* LTDC configuration reload */
	LTDC_ReloadConfig(LTDC_IMReload);

	/* Enable foreground & background Layers */
	LTDC_LayerCmd(LTDC_Layer1, ENABLE);
	LTDC_LayerCmd(LTDC_Layer2, ENABLE);

	/* LTDC configuration reload */
	LTDC_ReloadConfig(LTDC_IMReload);

	/* Set default font */
	//  LCD_SetFont(&LCD_DEFAULT_FONT);

	/* dithering activation */
	LTDC_DitherCmd(ENABLE);
}

/**
 * @brief  Controls LCD Chip Select (CS) pin
 * @param  NewState CS pin state
 * @retval None
 */
void ILI9341::LCD_ChipSelect(FunctionalState NewState)
{
	if (NewState == DISABLE)
	{
		GPIO_ResetBits(LCD_NCS_GPIO_PORT, LCD_NCS_PIN); /* CS pin low: LCD disabled */
	}
	else
	{
		GPIO_SetBits(LCD_NCS_GPIO_PORT, LCD_NCS_PIN); /* CS pin high: LCD enabled */
	}
}

/**
 * @brief  Sets the LCD Layer.
 * @param  Layerx: specifies the Layer foreground or background.
 * @retval None
 */
void ILI9341::SetLayer(uint32_t Layerx)
{
	if (Layerx == LCD_BACKGROUND_LAYER)
	{
		CurrentFrameBuffer = LCD_FRAME_BUFFER;
		CurrentLayer = LCD_BACKGROUND_LAYER;
	}
	else
	{
		CurrentFrameBuffer = LCD_FRAME_BUFFER + BUFFER_OFFSET;
		CurrentLayer = LCD_FOREGROUND_LAYER;
	}
}  



/**
 * @brief  Sets the Text color.
 * @param  Color: specifies the Text color code RGB(5-6-5).
 * @retval None
 */
void ILI9341::SetTextColor(uint16_t Color)
{
	CurrentTextColor = Color;
}

/**
 * @brief  Sets the Background color.
 * @param  Color: specifies the Background color code RGB(5-6-5).
 * @retval None
 */
void ILI9341::SetBackColor(uint16_t Color)
{
	CurrentBackColor = Color;
}

/**
 * @brief  Sets the Text Font.
 * @param  fonts: specifies the font to be used.
 * @retval None
 */
//void ILI9341::LCD_SetFont(sFONT *fonts)
//{
//  LCD_Currentfonts = fonts;
//}

/**
 * @brief  Configure the transparency.
 * @param  transparency: specifies the transparency,
 *         This parameter must range from 0x00 to 0xFF.
 * @retval None
 * 000 = no transparence
 * 255 = not visible
 *
 // Setting the Background to "a lot transparancy" green + foregroud to "complete transparent" results in a dark green
 //	TFT.SetLayer(LCD_BACKGROUND_LAYER); 	TFT.clear_screen(LCD_COLOR_GREEN); 	TFT.SetTransparency(200);
 //	TFT.SetLayer(LCD_FOREGROUND_LAYER);	TFT.SetTransparency(255);	TFT.clear_screen(LCD_COLOR_BLUE);

 // Setting the Background to 100% green + Foreground to 100% transparent => full green
 //	TFT.SetLayer(LCD_BACKGROUND_LAYER);		TFT.clear_screen(LCD_COLOR_GREEN);	TFT.SetTransparency(0);
 //	TFT.SetLayer(LCD_FOREGROUND_LAYER); 	TFT.SetTransparency(255);	TFT.clear_screen(LCD_COLOR_BLUE);

 // setting back to 100%green + foreground to 180/255 transparent blue => türkies

 // setting foreground to blue 0 transparency and background to green 0 trans => green (written later!)
 //TFT.SetLayer(LCD_FOREGROUND_LAYER); 	TFT.SetTransparency(0);	TFT.clear_screen(LCD_COLOR_BLUE);
 //TFT.SetLayer(LCD_BACKGROUND_LAYER);		TFT.clear_screen(LCD_COLOR_GREEN);	TFT.SetTransparency(0);

	//////////// fading ////////////

 //	TFT.SetLayer(LCD_BACKGROUND_LAYER);
 //	TFT.clear_screen(LCD_COLOR_GREEN);
 //	TFT.filled_rect(250,170,50,50,TFT.convert_color(0,0,255));
 //	TFT.SetTransparency(0);
 //
 //	TFT.SetLayer(LCD_FOREGROUND_LAYER);
 //	TFT.clear_screen(LCD_COLOR_BLUE);
 //	TFT.filled_rect(20,20,50,50,TFT.convert_color(255,0,0));
 //
 //	bool up=true;
 //	for(int a=0;;){
 //		char temp[80];
 //		sprintf(temp,"%3i%% trans",int(a/2.55));
 //		TFT.string(temp,20,16);
 //		TFT.SetTransparency(a);
 //		_delay_ms(10);
 //		if(up){
 //			if(a<255){
 //				a++;
 //			} else {
 //				a--;
 //				up=false;
 //			}
 //		} else {
 //			if(a>0){
 //				a--;
 //			} else {
 //				a++;
 //				up=true;
 //			}
 //		}
 //	}
 */
void ILI9341::SetTransparency(uint8_t transparency)
{
	if (CurrentLayer == LCD_BACKGROUND_LAYER)
	{
		LTDC_LayerAlpha(LTDC_Layer1, 0xff-transparency);
	}
	else
	{
		LTDC_LayerAlpha(LTDC_Layer2, 0xff-transparency);
	}
	LTDC_ReloadConfig(LTDC_IMReload);
}


/**
 * @brief  Clears the hole LCD.
 * @param  Color: the color of the background.
 * @retval None
 */
void ILI9341::clear_screen(){
	clear_screen(CurrentTextColor);
}

void ILI9341::clear_screen(uint16_t Color)
{
	uint32_t index = 0;

	/* erase memory */
	for (index = 0x00; index < BUFFER_OFFSET; index++)
		//	for (index = 0x00; index < 240*320; index++)
	{
		*(__IO uint16_t*)(CurrentFrameBuffer + (2*index)) = Color;
	}
}

/**
 * @brief  Sets the cursor position.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @retval Display Address
 */
uint32_t ILI9341::SetCursor(uint32_t Xpos, uint32_t Ypos)
{  
	return CurrentFrameBuffer + (((uint32_t)(320-Xpos)*(uint32_t)(240)	+	Ypos)<<1); // *2
}

/**
 * @brief  Config and Sets the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
void ILI9341::LCD_SetColorKeying(uint32_t RGBValue)
{  
	LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;

	/* configure the color Keying */
	LTDC_colorkeying_InitStruct.LTDC_ColorKeyBlue = 0x0000FF & RGBValue;
	LTDC_colorkeying_InitStruct.LTDC_ColorKeyGreen = (0x00FF00 & RGBValue) >> 8;
	LTDC_colorkeying_InitStruct.LTDC_ColorKeyRed = (0xFF0000 & RGBValue) >> 16;

	if (CurrentLayer == LCD_BACKGROUND_LAYER)
	{
		/* Enable the color Keying for Layer1 */
		LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, ENABLE);
		LTDC_ReloadConfig(LTDC_IMReload);
	}
	else
	{
		/* Enable the color Keying for Layer2 */
		LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, ENABLE);
		LTDC_ReloadConfig(LTDC_IMReload);
	}
}

/**
 * @brief  Disable the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
void ILI9341::LCD_ReSetColorKeying(void)
{
	LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;

	if (CurrentLayer == LCD_BACKGROUND_LAYER)
	{
		/* Disable the color Keying for Layer1 */
		LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, DISABLE);
		LTDC_ReloadConfig(LTDC_IMReload);
	}
	else
	{
		/* Disable the color Keying for Layer2 */
		LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, DISABLE);
		LTDC_ReloadConfig(LTDC_IMReload);
	}
} 

void ILI9341::set_transparent_font(bool in){
	transparent_font=in;
}


void ILI9341::zeichen_small_scale(uint8_t scale,const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	unsigned int stelle;


	if((z<0x20)||(z>0x7f))z=0x20;
	stelle = 8*(z-0x20);

	for(int y_i=0;y_i<8;y_i++){ // oder <=8?
		char a = font[stelle];/* 8 px */
		for(int repeat_y=0;repeat_y<scale;repeat_y++){
			uint8_t mask=0x80;
			for(int x_i=0;x_i<7;x_i++){ // oder <8?
				if(a&mask){
					for(int repeat_x=0;repeat_x<scale;repeat_x++){
						//	LCD_SetTextColor(0xFFFF);
						//	PutPixel(spalte*6 + scale*x_i+repeat_x, zeile*7 + scale*y_i+repeat_y);
						uint16_t px_x=spalte*6 + scale*x_i+repeat_x;
						uint16_t px_y=zeile*7 + scale*y_i+repeat_y;
						*(__IO uint16_t*)(SetCursor(px_x,px_y))= CurrentTextColor;
						//*(__IO uint16_t*)(LCD_SetCursor(px_x,px_y))=0xffff;


					}
				} else if(!transparent_font) {
					for(int repeat_x=0;repeat_x<scale;repeat_x++){
						//	LCD_SetTextColor(0x0000);
						//	PutPixel(spalte*6 + scale*x_i+repeat_x, zeile*7 + scale*y_i+repeat_y);
						uint16_t px_x=spalte*6 + scale*x_i+repeat_x;
						uint16_t px_y=zeile*7 + scale*y_i+repeat_y;
						*(__IO uint16_t*)(SetCursor(px_x,px_y))= CurrentBackColor;
					}
				}
				mask=mask>>1;
			}
		}
		stelle++;
	}
}

void ILI9341::zeichen_small_1x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(1,font,z,spalte,zeile,offset);
}
void ILI9341::zeichen_small_2x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(2,font,z,spalte,zeile,offset);
}
void ILI9341::zeichen_small_3x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(3,font,z,spalte,zeile,offset);
}
void ILI9341::zeichen_small_4x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(4,font,z,spalte,zeile,offset);
}
void ILI9341::zeichen_small_5x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(5,font,z,spalte,zeile,offset);
}
void ILI9341::zeichen_small_6x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(6,font,z,spalte,zeile,offset);
}
void ILI9341::zeichen_small_7x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(7,font,z,spalte,zeile,offset);
}
void ILI9341::zeichen_small_8x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset){
	zeichen_small_scale(8,font,z,spalte,zeile,offset);
}

void ILI9341::string(char *str,uint8_t spalte, uint8_t zeile){
	string(Speedo.default_font,str,spalte,zeile,0,0,0,255,255,255,0);
}

void ILI9341::string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile){
	string(font,str,spalte,zeile,255,255,255,0,0,0,0);
}

void ILI9341::string(char *str,uint8_t spalte, uint8_t zeile, uint8_t back, uint8_t text){
	string(Speedo.default_font,str,spalte,zeile,text*15,text*15,text*15,back*15,back*15,back*15,0);
}

void ILI9341::string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile, uint8_t back, uint8_t text, uint8_t offset){
	string(font,str,spalte,zeile,text*15,text*15,text*15,back*15,back*15,back*15,offset);
}

void ILI9341::string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile, uint8_t text_r, uint8_t text_g, uint8_t text_b, uint8_t back_r, uint8_t back_g, uint8_t back_b, uint8_t offset){
	SetBackColor(convert_color(back_r,back_g,back_b));
	SetTextColor(convert_color(text_r,text_g,text_b));

	switch(font){
	case SANS_SMALL_4X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_4x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i*4),zeile,offset);
		};
		break;
	case SANS_SMALL_3X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_3x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i*3),zeile,offset);
		};
		break;
	case SANS_SMALL_2X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_2x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i*2),zeile,offset);
		};
		break;
	case SANS_SMALL_1X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_1x(&dejaVuSans5ptBitmaps[0],str[i],(spalte+i),zeile,offset);
		};
		break;
	case VISITOR_SMALL_8X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_8x(&visitor_code[0],str[i],(spalte+i*8),zeile,offset);
		};
		break;
	case VISITOR_SMALL_7X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_7x(&visitor_code[0],str[i],(spalte+i*7),zeile,offset);
		};
		break;
	case VISITOR_SMALL_6X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_6x(&visitor_code[0],str[i],(spalte+i*6),zeile,offset);
		};
		break;
	case VISITOR_SMALL_5X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_5x(&visitor_code[0],str[i],(spalte+i*5),zeile,offset);
		};
		break;
	case VISITOR_SMALL_4X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_4x(&visitor_code[0],str[i],(spalte+i*4),zeile,offset);
		};
		break;
	case VISITOR_SMALL_3X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_3x(&visitor_code[0],str[i],(spalte+i*3),zeile,offset);
		};
		break;
	case VISITOR_SMALL_2X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_2x(&visitor_code[0],str[i],(spalte+i*2),zeile,offset);
		};
		break;
	case VISITOR_SMALL_1X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_1x(&visitor_code[0],str[i],(spalte+i),zeile,offset);
		};
		break;
		//	case SANS_BIG_1X_FONT:
		//		for(unsigned int i=0;i<strlen(str);i++){
		//			zeichen_big_1x(&sans_big[0],str[i],(spalte+i*4),zeile,offset);
		//		};
		//		break;
	case STD_SMALL_2X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_2x(&std_small[0],str[i],(spalte+i*2),zeile,offset);
		};
		break;
	case STD_SMALL_3X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_3x(&std_small[0],str[i],(spalte+i*3),zeile,offset);
		};
		break;
	case STD_SMALL_4X_FONT:
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_4x(&std_small[0],str[i],(spalte+i*4),zeile,offset);
		};
		break;
	default: // das hier ist eigentlich STD_SMALL_1X_FONT
		for(unsigned int i=0;i<strlen(str);i++){
			zeichen_small_1x(&std_small[0],str[i],(spalte+i),zeile,offset);
		};
		break;
	};
}


/**
 * @brief  Sets a display window
 * @param  Xpos: specifies the X bottom left position from 0 to 240.
 * @param  Ypos: specifies the Y bottom left position from 0 to 320.
 * @param  Height: display window height, can be a value from 0 to 320.
 * @param  Width: display window width, can be a value from 0 to 240.
 * @retval None
 */
void ILI9341::SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width)
{

	if (CurrentLayer == LCD_BACKGROUND_LAYER)
	{
		/* reconfigure the layer1 position */
		LTDC_LayerPosition(LTDC_Layer1, Xpos, Ypos);
		LTDC_ReloadConfig(LTDC_IMReload);

		/* reconfigure the layer1 size */
		LTDC_LayerSize(LTDC_Layer1, Width, Height);
		LTDC_ReloadConfig(LTDC_IMReload);
	}
	else
	{
		/* reconfigure the layer2 position */
		LTDC_LayerPosition(LTDC_Layer2, Xpos, Ypos);
		LTDC_ReloadConfig(LTDC_IMReload);

		/* reconfigure the layer2 size */
		LTDC_LayerSize(LTDC_Layer2, Width, Height);
		LTDC_ReloadConfig(LTDC_IMReload);
	}
}

/**
 * @brief  Disables LCD Window mode.
 * @param  None
 * @retval None
 */
void ILI9341::LCD_WindowModeDisable(void)
{
	SetDisplayWindow(0, 0, LCD_PIXEL_HEIGHT, LCD_PIXEL_WIDTH);
}

/**
 * @brief  Displays a line.
 * @param Xpos: specifies the X position, can be a value from 0 to 240.
 * @param Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param Length: line length.
 * @param Direction: line direction.
 *   This parameter can be one of the following values: LCD_DIR_HORIZONTAL or LCD_DIR_VERTICAL.
 * @retval None
 */
void ILI9341::draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t r, uint8_t g, uint8_t b){
	SetTextColor(convert_color(r,g,b));
	draw_line(x1,y1,x2,y2);
}

void ILI9341::draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	if(x1==x2 || y1==y2){ // if it is horizontal or vertical we can use DMA Transfere
		uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;
		uint32_t  Xaddress = 0;
		DMA2D_InitTypeDef      DMA2D_InitStruct;
		if(x1>x2){
			uint16_t temp=x1;
			x1=x2;
			x2=temp;
		}
		if(y1>y2){
			uint16_t temp=y1;
			y1=y2;
			y2=temp;
		}

		if(x1==x2){
			DMA2D_InitStruct.DMA2D_OutputOffset = 0;
			DMA2D_InitStruct.DMA2D_NumberOfLine = 1;
			DMA2D_InitStruct.DMA2D_PixelPerLine = y2-y1;
			Xaddress = SetCursor(x1,y1);
		} else {
			DMA2D_InitStruct.DMA2D_OutputOffset = LCD_PIXEL_WIDTH - 1;
			DMA2D_InitStruct.DMA2D_NumberOfLine = x2-x1;
			DMA2D_InitStruct.DMA2D_PixelPerLine = 1;
			Xaddress = SetCursor(x2,y1);
		}
		Red_Value = (0xF800 & CurrentTextColor) >> 11;
		Blue_Value = 0x001F & CurrentTextColor;
		Green_Value = (0x07E0 & CurrentTextColor) >> 5;

		/* Configure DMA2D */
		DMA2D_DeInit();
		DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
		DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
		DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
		DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
		DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
		DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
		DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;

		DMA2D_Init(&DMA2D_InitStruct);
		/* Start Transfer */
		DMA2D_StartTransfer();
		/* Wait for CTC Flag activation */
		while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET){};
	}

	// increasing line
	else {
		int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =	0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, curpixel = 0;

		deltax = abs(x2 - x1); /* The difference between the x's */
		deltay = abs(y2 - y1); /* The difference between the y's */
		x = x1; /* Start x off at the first pixel */
		y = y1; /* Start y off at the first pixel */

		if (x2 >= x1) /* The x-values are increasing */
		{
			xinc1 = 1;
			xinc2 = 1;
		} else /* The x-values are decreasing */
		{
			xinc1 = -1;
			xinc2 = -1;
		}

		if (y2 >= y1) /* The y-values are increasing */
		{
			yinc1 = 1;
			yinc2 = 1;
		} else /* The y-values are decreasing */
		{
			yinc1 = -1;
			yinc2 = -1;
		}

		if (deltax >= deltay) /* There is at least one x-value for every y-value */
		{
			xinc1 = 0; /* Don't change the x when numerator >= denominator */
			yinc2 = 0; /* Don't change the y for every iteration */
			den = deltax;
			num = deltax / 2;
			numadd = deltay;
			numpixels = deltax; /* There are more x-values than y-values */
		} else /* There is at least one y-value for every x-value */
		{
			xinc2 = 0; /* Don't change the x for every iteration */
			yinc1 = 0; /* Don't change the y when numerator >= denominator */
			den = deltay;
			num = deltay / 2;
			numadd = deltax;
			numpixels = deltay; /* There are more y-values than x-values */
		}

		for (curpixel = 0; curpixel <= numpixels; curpixel++) {
			Pixel(x, y); /* Draw the current pixel */
			num += numadd; /* Increase the numerator by the top of the fraction */
			if (num >= den) /* Check if numerator >= denominator */
			{
				num -= den; /* Calculate the new numerator value */
				x += xinc1; /* Change the x as appropriate */
				y += yinc1; /* Change the y as appropriate */
			}
			x += xinc2; /* Change the x as appropriate */
			y += yinc2; /* Change the y as appropriate */
		}
	}

}

/**
 * @brief  Displays a rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Height: display rectangle height, can be a value from 0 to 320.
 * @param  Width: display rectangle width, can be a value from 0 to 240.
 * @retval None
 */
void ILI9341::LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width)
{
	/* draw horizontal lines */
	draw_line(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
	draw_line(Xpos, (Ypos+ Height), Width, LCD_DIR_HORIZONTAL);

	/* draw vertical lines */
	draw_line(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
	draw_line((Xpos + Width), Ypos, Height, LCD_DIR_VERTICAL);
}

/**
 * @brief  Draw a circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: radius of the circle.
 * @retval None
 */
void ILI9341::LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int x = -Radius, y = 0, err = 2-2*Radius, e2;
	do {
		*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
		*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
		*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;
		*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;

		e2 = err;
		if (e2 <= y) {
			err += ++y*2+1;
			if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x*2+1;
	}
	while (x <= 0);
}

/**
 * @brief  Draw a full ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: minor radius of ellipse.
 * @param  Radius2: major radius of ellipse.
 * @retval None
 */
void ILI9341::LCD_DrawFullEllipse(int Xpos, int Ypos, int Radius, int Radius2)
{
	int x = -Radius, y = 0, err = 2-2*Radius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = Radius;
	rad2 = Radius2;

	if (Radius > Radius2)
	{
		do
		{
			K = (float)(rad1/rad2);
			draw_line((Xpos+x), (Ypos-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1), LCD_DIR_VERTICAL);
			draw_line((Xpos-x), (Ypos-(uint16_t)(y/K)), (2*(uint16_t)(y/K) + 1), LCD_DIR_VERTICAL);

			e2 = err;
			if (e2 <= y)
			{
				err += ++y*2+1;
				if (-x == y && e2 <= x) e2 = 0;
			}
			if (e2 > x) err += ++x*2+1;

		}
		while (x <= 0);
	}
	else
	{
		y = -Radius2;
		x = 0;
		do
		{
			K = (float)(rad2/rad1);
			draw_line((Xpos-(uint16_t)(x/K)), (Ypos+y), (2*(uint16_t)(x/K) + 1), LCD_DIR_HORIZONTAL);
			draw_line((Xpos-(uint16_t)(x/K)), (Ypos-y), (2*(uint16_t)(x/K) + 1), LCD_DIR_HORIZONTAL);

			e2 = err;
			if (e2 <= x)
			{
				err += ++x*2+1;
				if (-y == x && e2 <= y) e2 = 0;
			}
			if (e2 > y) err += ++y*2+1;
		}
		while (y <= 0);
	}
}

/**
 * @brief  Displays an Ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: specifies Radius.
 * @param  Radius2: specifies Radius2.
 * @retval None
 */
void ILI9341::LCD_DrawEllipse(int Xpos, int Ypos, int Radius, int Radius2)
{
	int x = -Radius, y = 0, err = 2-2*Radius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = Radius;
	rad2 = Radius2;

	if (Radius > Radius2)
	{
		do {
			K = (float)(rad1/rad2);
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K))))) = CurrentTextColor;
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+(uint16_t)(y/K))))) = CurrentTextColor;
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K))))) = CurrentTextColor;
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-(uint16_t)(y/K))))) = CurrentTextColor;

			e2 = err;
			if (e2 <= y) {
				err += ++y*2+1;
				if (-x == y && e2 <= x) e2 = 0;
			}
			if (e2 > x) err += ++x*2+1;
		}
		while (x <= 0);
	}
	else
	{
		y = -Radius2;
		x = 0;
		do {
			K = (float)(rad2/rad1);
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos+(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;
			*(__IO uint16_t*) (CurrentFrameBuffer + (2*((Xpos-(uint16_t)(x/K)) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;

			e2 = err;
			if (e2 <= x) {
				err += ++x*2+1;
				if (-y == x && e2 <= y) e2 = 0;
			}
			if (e2 > y) err += ++y*2+1;
		}
		while (y <= 0);
	}
}

/**
 * @brief  Displays a mono-color picture.
 * @param  Pict: pointer to the picture array.
 * @retval None
 */
void ILI9341::LCD_DrawMonoPict(const uint32_t *Pict)
{
	uint32_t index = 0, counter = 0;


	for(index = 0; index < 2400; index++)
	{
		for(counter = 0; counter < 32; counter++)
		{
			if((Pict[index] & (1 << counter)) == 0x00)
			{
				*(__IO uint16_t*)(CurrentFrameBuffer) = CurrentBackColor;
			}
			else
			{
				*(__IO uint16_t*)(CurrentFrameBuffer) = CurrentTextColor;
			}
		}
	}
}

/**
 * @brief  Displays a bitmap picture loaded in the internal Flash.
 * @param  BmpAddress: Bmp picture address in the internal Flash.
 * @retval None
 */
void ILI9341::LCD_WriteBMP(uint32_t BmpAddress)
{
	uint32_t index = 0, size = 0, width = 0, height = 0, bit_pixel = 0;
	uint32_t Address;
	uint32_t currentline = 0, linenumber = 0;

	Address = CurrentFrameBuffer;

	/* Read bitmap size */
	size = *(__IO uint16_t *) (BmpAddress + 2);
	size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;

	/* Get bitmap data address offset */
	index = *(__IO uint16_t *) (BmpAddress + 10);
	index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;

	/* Read bitmap width */
	width = *(uint16_t *) (BmpAddress + 18);
	width |= (*(uint16_t *) (BmpAddress + 20)) << 16;

	/* Read bitmap height */
	height = *(uint16_t *) (BmpAddress + 22);
	height |= (*(uint16_t *) (BmpAddress + 24)) << 16;

	/* Read bit/pixel */
	bit_pixel = *(uint16_t *) (BmpAddress + 28);

	if (CurrentLayer == LCD_BACKGROUND_LAYER)
	{
		/* reconfigure layer size in accordance with the picture */
		LTDC_LayerSize(LTDC_Layer1, width, height);
		LTDC_ReloadConfig(LTDC_VBReload);

		/* Reconfigure the Layer pixel format in accordance with the picture */
		if ((bit_pixel/8) == 4)
		{
			LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB8888);
			LTDC_ReloadConfig(LTDC_VBReload);
		}
		else if ((bit_pixel/8) == 2)
		{
			LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_RGB565);
			LTDC_ReloadConfig(LTDC_VBReload);
		}
		else
		{
			LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_RGB888);
			LTDC_ReloadConfig(LTDC_VBReload);
		}
	}
	else
	{
		/* reconfigure layer size in accordance with the picture */
		LTDC_LayerSize(LTDC_Layer2, width, height);
		LTDC_ReloadConfig(LTDC_VBReload);

		/* Reconfigure the Layer pixel format in accordance with the picture */
		if ((bit_pixel/8) == 4)
		{
			LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_ARGB8888);
			LTDC_ReloadConfig(LTDC_VBReload);
		}
		else if ((bit_pixel/8) == 2)
		{
			LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_RGB565);
			LTDC_ReloadConfig(LTDC_VBReload);
		}
		else
		{
			LTDC_LayerPixelFormat(LTDC_Layer2, LTDC_Pixelformat_RGB888);
			LTDC_ReloadConfig(LTDC_VBReload);
		}
	}

	/* compute the real size of the picture (without the header)) */
	size = (size - index);

	/* bypass the bitmap header */
	BmpAddress += index;

	/* start copie image from the bottom */
	Address += width*(height-1)*(bit_pixel/8);

	for(index = 0; index < size; index++)
	{
		*(__IO uint8_t*) (Address) = *(__IO uint8_t *)BmpAddress;

		/*jump on next byte */
		BmpAddress++;
		Address++;
		currentline++;

		if((currentline/(bit_pixel/8)) == width)
		{
			if(linenumber < height)
			{
				linenumber++;
				Address -=(2*width*(bit_pixel/8));
				currentline = 0;
			}
		}
	}
}

/**
 * @brief  Displays a full rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Height: rectangle height.
 * @param  Width: rectangle width.
 * @retval None
 */
void ILI9341::filled_rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t r,uint8_t g,uint8_t b){
	filled_rect(x,y,width,height,convert_color(r,g,b));
}

void ILI9341::filled_rect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t color){
	DMA2D_InitTypeDef      DMA2D_InitStruct;

	uint32_t  Xaddress = 0;
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

	Red_Value = (0xF800 & color) >> 11;
	Blue_Value = 0x001F & color;
	Green_Value = (0x07E0 & color) >> 5;

	Xaddress = SetCursor(Xpos+Width,Ypos);

	/* configure DMA2D */
	DMA2D_DeInit();
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
	DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
	DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
	DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
	DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;
	DMA2D_InitStruct.DMA2D_OutputOffset = (LCD_PIXEL_WIDTH - Height);
	DMA2D_InitStruct.DMA2D_NumberOfLine = Width;
	DMA2D_InitStruct.DMA2D_PixelPerLine = Height;
	DMA2D_Init(&DMA2D_InitStruct);

	/* Start Transfer */
	DMA2D_StartTransfer();

	/* Wait for CTC Flag activation */
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
	{
	}

	SetTextColor(CurrentTextColor);
}

/**
 * @brief  Displays a full circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius
 * @retval None
 */
void ILI9341::LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t  D;    /* Decision Variable */
	uint32_t  CurX;/* Current X Value */
	uint32_t  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);

	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		if(CurY > 0)
		{
			draw_line(Xpos - CurX, Ypos - CurY, 2*CurY, LCD_DIR_VERTICAL);
			draw_line(Xpos + CurX, Ypos - CurY, 2*CurY, LCD_DIR_VERTICAL);
		}

		if(CurX > 0)
		{
			draw_line(Xpos - CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL);
			draw_line(Xpos + CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL);
		}
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}

	LCD_DrawCircle(Xpos, Ypos, Radius);
}



/**
 * @brief  Displays an triangle.
 * @param  Points: pointer to the points array.
 * @retval None
 */
void ILI9341::LCD_Triangle(pPoint Points, uint16_t PointCount)
{
	int16_t X = 0, Y = 0;
	pPoint First = Points;

	if(PointCount != 3)
	{
		return;
	}

	while(--PointCount)
	{
		X = Points->X;
		Y = Points->Y;
		Points++;
		draw_line(X, Y, Points->X, Points->Y);
	}
	draw_line(First->X, First->Y, Points->X, Points->Y);
}

/**
 * @brief  Fill an triangle (between 3 points).
 * @param  x1..3: x position of triangle point 1..3.
 * @param  y1..3: y position of triangle point 1..3.
 * @retval None
 */
void ILI9341::LCD_FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{ 

	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */

	if (x2 >= x1)                 /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)                 /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		draw_line(x, y, x3, y3);

		num += numadd;              /* Increase the numerator by the top of the fraction */
		if (num >= den)             /* Check if numerator >= denominator */
		{
			num -= den;               /* Calculate the new numerator value */
			x += xinc1;               /* Change the x as appropriate */
			y += yinc1;               /* Change the y as appropriate */
		}
		x += xinc2;                 /* Change the x as appropriate */
		y += yinc2;                 /* Change the y as appropriate */
	}


}
/**
 * @brief  Displays an poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void ILI9341::LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
	int16_t X = 0, Y = 0;

	if(PointCount < 2)
	{
		return;
	}

	while(--PointCount)
	{
		X = Points->X;
		Y = Points->Y;
		Points++;
		draw_line(X, Y, Points->X, Points->Y);
	}
}

/**
 * @brief  Displays an relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @param  Closed: specifies if the draw is closed or not.
 *           1: closed, 0 : not closed.
 * @retval None
 */
void ILI9341::LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
	int16_t X = 0, Y = 0;
	pPoint First = Points;

	if(PointCount < 2)
	{
		return;
	}
	X = Points->X;
	Y = Points->Y;
	while(--PointCount)
	{
		Points++;
		draw_line(X, Y, X + Points->X, Y + Points->Y);
		X = X + Points->X;
		Y = Y + Points->Y;
	}
	if(Closed)
	{
		draw_line(First->X, First->Y, X, Y);
	}
}

/**
 * @brief  Displays a closed poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void ILI9341::LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
	LCD_PolyLine(Points, PointCount);
	draw_line(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
 * @brief  Displays a relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void ILI9341::LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
	LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
 * @brief  Displays a closed relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void ILI9341::LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
	LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}

/**
 * @brief  Displays a  full poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void ILI9341::LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{

	int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
	uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

	IMAGE_LEFT = IMAGE_RIGHT = Points->X;
	IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

	for(counter = 1; counter < PointCount; counter++)
	{
		pixelX = POLY_X(counter);
		if(pixelX < IMAGE_LEFT)
		{
			IMAGE_LEFT = pixelX;
		}
		if(pixelX > IMAGE_RIGHT)
		{
			IMAGE_RIGHT = pixelX;
		}

		pixelY = POLY_Y(counter);
		if(pixelY < IMAGE_TOP)
		{
			IMAGE_TOP = pixelY;
		}
		if(pixelY > IMAGE_BOTTOM)
		{
			IMAGE_BOTTOM = pixelY;
		}
	}

	if(PointCount < 2)
	{
		return;
	}

	X_center = (IMAGE_LEFT + IMAGE_RIGHT)/2;
	Y_center = (IMAGE_BOTTOM + IMAGE_TOP)/2;

	X_first = Points->X;
	Y_first = Points->Y;

	while(--PointCount)
	{
		X = Points->X;
		Y = Points->Y;
		Points++;
		X2 = Points->X;
		Y2 = Points->Y;

		LCD_FillTriangle(X, X2, X_center, Y, Y2, Y_center);
		LCD_FillTriangle(X, X_center, X2, Y, Y_center, Y2);
		LCD_FillTriangle(X_center, X2, X, Y_center, Y2, Y);
	}

	LCD_FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
	LCD_FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
	LCD_FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);
}

/**
 * @brief  Writes command to select the LCD register.
 * @param  LCD_Reg: address of the selected register.
 * @retval None
 */
void ILI9341::LCD_WriteCommand(uint8_t LCD_Reg)
{
	/* Reset WRX to send command */
	LCD_CtrlLinesWrite(LCD_WRX_GPIO_PORT, LCD_WRX_PIN, Bit_RESET);

	/* Reset LCD control line(/CS) and Send command */
	LCD_ChipSelect(DISABLE);
	SPI_I2S_SendData(LCD_SPI, LCD_Reg);

	/* Wait until a data is sent(not busy), before config /CS HIGH */

	while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET) ;

	while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET);

	LCD_ChipSelect(ENABLE);
}

/**
 * @brief  Writes data to select the LCD register.
 *         This function must be used after LCD_WriteCommand() function
 * @param  value: data to write to the selected register.
 * @retval None
 */
void ILI9341::LCD_WriteData(uint8_t value)
{
	/* Set WRX to send data */
	LCD_CtrlLinesWrite(LCD_WRX_GPIO_PORT, LCD_WRX_PIN, Bit_SET);

	/* Reset LCD control line(/CS) and Send data */
	LCD_ChipSelect(DISABLE);
	SPI_I2S_SendData(LCD_SPI, value);

	/* Wait until a data is sent(not busy), before config /CS HIGH */

	while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET) ;

	while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) != RESET);

	LCD_ChipSelect(ENABLE);
}

/**
 * @brief  Configure the LCD controller (Power On sequence as described in ILI9341 Datasheet)
 * @param  None
 * @retval None
 */
void ILI9341::LCD_PowerOn(void)
{
	LCD_WriteCommand(0xCA);
	LCD_WriteData(0xC3);
	LCD_WriteData(0x08);
	LCD_WriteData(0x50);
	LCD_WriteCommand(LCD_POWERB);
	LCD_WriteData(0x00);
	LCD_WriteData(0xC1);
	LCD_WriteData(0x30);
	LCD_WriteCommand(LCD_POWER_SEQ);
	LCD_WriteData(0x64);
	LCD_WriteData(0x03);
	LCD_WriteData(0x12);
	LCD_WriteData(0x81);
	LCD_WriteCommand(LCD_DTCA);
	LCD_WriteData(0x85);
	LCD_WriteData(0x00);
	LCD_WriteData(0x78);
	LCD_WriteCommand(LCD_POWERA);
	LCD_WriteData(0x39);
	LCD_WriteData(0x2C);
	LCD_WriteData(0x00);
	LCD_WriteData(0x34);
	LCD_WriteData(0x02);
	LCD_WriteCommand(LCD_PRC);
	LCD_WriteData(0x20);
	LCD_WriteCommand(LCD_DTCB);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteCommand(LCD_FRC);
	LCD_WriteData(0x00);
	LCD_WriteData(0x1B);
	LCD_WriteCommand(LCD_DFC);
	LCD_WriteData(0x0A);
	LCD_WriteData(0xA2);
	LCD_WriteCommand(LCD_POWER1);
	LCD_WriteData(0x10);
	LCD_WriteCommand(LCD_POWER2);
	LCD_WriteData(0x10);
	LCD_WriteCommand(LCD_VCOM1);
	LCD_WriteData(0x45);
	LCD_WriteData(0x15);
	LCD_WriteCommand(LCD_VCOM2);
	LCD_WriteData(0x90);
	LCD_WriteCommand(LCD_MAC);
	LCD_WriteData(0xC8);
	LCD_WriteCommand(LCD_3GAMMA_EN);
	LCD_WriteData(0x00);
	LCD_WriteCommand(LCD_RGB_INTERFACE);
	LCD_WriteData(0xC2);
	LCD_WriteCommand(LCD_DFC);
	LCD_WriteData(0x0A);
	LCD_WriteData(0xA7);
	LCD_WriteData(0x27);
	LCD_WriteData(0x04);

	/* colomn address set */
	LCD_WriteCommand(LCD_COLUMN_ADDR);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0xEF);
	/* Page Address Set */
	LCD_WriteCommand(LCD_PAGE_ADDR);
	LCD_WriteData(0x00);
	LCD_WriteData(0x00);
	LCD_WriteData(0x01);
	LCD_WriteData(0x3F);
	LCD_WriteCommand(LCD_INTERFACE);
	LCD_WriteData(0x01);
	LCD_WriteData(0x00);
	LCD_WriteData(0x06);

	LCD_WriteCommand(LCD_GRAM);
	_delay_us(10);

	LCD_WriteCommand(LCD_GAMMA);
	LCD_WriteData(0x01);

	LCD_WriteCommand(LCD_PGAMMA);
	LCD_WriteData(0x0F);
	LCD_WriteData(0x29);
	LCD_WriteData(0x24);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x0E);
	LCD_WriteData(0x09);
	LCD_WriteData(0x4E);
	LCD_WriteData(0x78);
	LCD_WriteData(0x3C);
	LCD_WriteData(0x09);
	LCD_WriteData(0x13);
	LCD_WriteData(0x05);
	LCD_WriteData(0x17);
	LCD_WriteData(0x11);
	LCD_WriteData(0x00);
	LCD_WriteCommand(LCD_NGAMMA);
	LCD_WriteData(0x00);
	LCD_WriteData(0x16);
	LCD_WriteData(0x1B);
	LCD_WriteData(0x04);
	LCD_WriteData(0x11);
	LCD_WriteData(0x07);
	LCD_WriteData(0x31);
	LCD_WriteData(0x33);
	LCD_WriteData(0x42);
	LCD_WriteData(0x05);
	LCD_WriteData(0x0C);
	LCD_WriteData(0x0A);
	LCD_WriteData(0x28);
	LCD_WriteData(0x2F);
	LCD_WriteData(0x0F);

	LCD_WriteCommand(LCD_SLEEP_OUT);
	_delay_us(10);
	LCD_WriteCommand(LCD_DISPLAY_ON);
	/* GRAM start writing */
	LCD_WriteCommand(LCD_GRAM);
}

/**
 * @brief  Enables the Display.
 * @param  None
 * @retval None
 */
void ILI9341::DisplayOn(void)
{
	LCD_WriteCommand(LCD_DISPLAY_ON);
}

/**
 * @brief  Disables the Display.
 * @param  None
 * @retval None
 */
void ILI9341::DisplayOff(void)
{
	/* Display Off */
	LCD_WriteCommand(LCD_DISPLAY_OFF);
}

/**
 * @brief  Configures LCD control lines in Output Push-Pull mode.
 * @note   The LCD_NCS line can be configured in Open Drain mode
 *         when VDDIO is lower than required LCD supply.
 * @param  None
 * @retval None
 */
void ILI9341::LCD_CtrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clock*/
	RCC_AHB1PeriphClockCmd(LCD_NCS_GPIO_CLK | LCD_WRX_GPIO_CLK, ENABLE);

	/* Configure NCS in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LCD_NCS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_NCS_GPIO_PORT, &GPIO_InitStructure);

	/* Configure WRX in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LCD_WRX_PIN;
	GPIO_Init(LCD_WRX_GPIO_PORT, &GPIO_InitStructure);

	/* Set chip select pin high */
	LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
}

/**
 * @brief  Sets or reset LCD control lines.
 * @param  GPIOx: where x can be B or D to select the GPIO peripheral.
 * @param  CtrlPins: the Control line.
 *   This parameter can be:
 *     @arg LCD_NCS_PIN: Chip Select pin
 *     @arg LCD_NWR_PIN: Read/Write Selection pin
 *     @arg LCD_RS_PIN: Register/RAM Selection pin
 * @param  BitVal: specifies the value to be written to the selected bit.
 *   This parameter can be:
 *     @arg Bit_RESET: to clear the port pin
 *     @arg Bit_SET: to set the port pin
 * @retval None
 */
void ILI9341::LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal)
{
	/* Set or Reset the control line */
	GPIO_WriteBit(GPIOx, (uint16_t)CtrlPins, (BitAction)BitVal);
}

/**
 * @brief  Configures the LCD_SPI interface.
 * @param  None
 * @retval None
 */
void ILI9341::LCD_SPIConfig(void)
{
	SPI_InitTypeDef    SPI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;

	/* Enable LCD_SPI_SCK_GPIO_CLK, LCD_SPI_MISO_GPIO_CLK and LCD_SPI_MOSI_GPIO_CLK clock */
	RCC_AHB1PeriphClockCmd(LCD_SPI_SCK_GPIO_CLK | LCD_SPI_MISO_GPIO_CLK | LCD_SPI_MOSI_GPIO_CLK, ENABLE);

	/* Enable LCD_SPI and SYSCFG clock  */
	RCC_APB2PeriphClockCmd(LCD_SPI_CLK, ENABLE);

	/* Configure LCD_SPI SCK pin */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(LCD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* Configure LCD_SPI MISO pin */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MISO_PIN;
	GPIO_Init(LCD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	/* Configure LCD_SPI MOSI pin */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MOSI_PIN;
	GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* Connect SPI SCK */
	GPIO_PinAFConfig(LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_SOURCE, LCD_SPI_SCK_AF);

	/* Connect SPI MISO */
	GPIO_PinAFConfig(LCD_SPI_MISO_GPIO_PORT, LCD_SPI_MISO_SOURCE, LCD_SPI_MISO_AF);

	/* Connect SPI MOSI */
	GPIO_PinAFConfig(LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_SOURCE, LCD_SPI_MOSI_AF);

	SPI_I2S_DeInit(LCD_SPI);

	/* SPI configuration -------------------------------------------------------*/
	/* If the SPI peripheral is already enabled, don't reconfigure it */
	if ((LCD_SPI->CR1 & SPI_CR1_SPE) == 0)
	{
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		/* SPI baudrate is set to 5.6 MHz (PCLK2/SPI_BaudRatePrescaler = 90/16 = 5.625 MHz)
       to verify these constraints:
          - ILI9341 LCD SPI interface max baudrate is 10MHz for write and 6.66MHz for read
          - l3gd20 SPI interface max baudrate is 10MHz for write/read
          - PCLK2 frequency is set to 90 MHz 
		 */
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(LCD_SPI, &SPI_InitStructure);

		/* Enable L3GD20_SPI  */
		SPI_Cmd(LCD_SPI, ENABLE);
	}
}

/**
 * @brief  GPIO config for LTDC.
 * @param  None
 * @retval None
 */
void ILI9341::LCD_AF_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOF, GPIOG AHB Clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | \
			RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | \
			RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

	/* GPIOs Configuration */
	/*
 +------------------------+-----------------------+----------------------------+
 +                       LCD pins assignment                                   +
 +------------------------+-----------------------+----------------------------+
 |  LCD_TFT R2 <-> PC.10  |  LCD_TFT G2 <-> PA.06 |  LCD_TFT B2 <-> PD.06      |
 |  LCD_TFT R3 <-> PB.00  |  LCD_TFT G3 <-> PG.10 |  LCD_TFT B3 <-> PG.11      |
 |  LCD_TFT R4 <-> PA.11  |  LCD_TFT G4 <-> PB.10 |  LCD_TFT B4 <-> PG.12      |
 |  LCD_TFT R5 <-> PA.12  |  LCD_TFT G5 <-> PB.11 |  LCD_TFT B5 <-> PA.03      |
 |  LCD_TFT R6 <-> PB.01  |  LCD_TFT G6 <-> PC.07 |  LCD_TFT B6 <-> PB.08      |
 |  LCD_TFT R7 <-> PG.06  |  LCD_TFT G7 <-> PD.03 |  LCD_TFT B7 <-> PB.09      |
 -------------------------------------------------------------------------------
          |  LCD_TFT HSYNC <-> PC.06  | LCDTFT VSYNC <->  PA.04 |
          |  LCD_TFT CLK   <-> PG.07  | LCD_TFT DE   <->  PF.10 |
           -----------------------------------------------------

	 */

	/* GPIOA configuration */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | \
			GPIO_Pin_11 | GPIO_Pin_12;

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* GPIOB configuration */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, 0x09);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, 0x09);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | \
			GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;

	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* GPIOC configuration */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10;

	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;

	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* GPIOF configuration */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;

	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* GPIOG configuration */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, 0x09);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_LTDC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, 0x09);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | \
			GPIO_Pin_11 | GPIO_Pin_12;

	GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/**
 * @brief  Displays a pixel.
 * @param  x: pixel x.
 * @param  y: pixel y.
 * @retval None
 */
void ILI9341::Pixel(int16_t x, int16_t y){
	if(x < 0 || x > 319 || y < 0 || y > 239)
	{
		return;
	}
	//LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
	*(__IO uint16_t*)(SetCursor(x,y))=CurrentTextColor;
}

void ILI9341::Pixel(int16_t x, int16_t y,uint16_t color){
//	if(x < 0 || x > 319 || y < 0 || y > 239) {	return;		}
	*(__IO uint16_t*)(SetCursor(x,y))=color;
}

void ILI9341::Pixel(int16_t x, int16_t y,uint8_t r,uint8_t g,uint8_t b){
	if(x < 0 || x > 319 || y < 0 || y > 239) {	return;		}
	*(__IO uint16_t*)(SetCursor(x,y))=convert_color(r,g,b);
}


uint16_t ILI9341::convert_color(uint8_t r,uint8_t g,uint8_t b){
	return (((r>>3)&0b00011111) << 11) | (((g>>2)&0b00111111) << 5) | ((b>>3)&0b00011111);
}

void ILI9341::highlight_bar(uint16_t x,uint16_t y,uint16_t width,uint16_t height){
	for(int i=0;i<16;i++){
		SetTextColor(convert_color(15*i,0,0));
		draw_line(x+i,y,x+i,y+height);
		draw_line(x+width-i,y,x+width-i,y+height);
	}

	filled_rect(x+15,y,width-31,height,convert_color(255,0,0));
}


void ILI9341::show_storry(const char* storry,const char* title){
	show_storry(storry,title,0x00);
}

void ILI9341::show_storry(const char* storry,const char* title, uint8_t type){
	char storry_char[strlen(storry)+1];
	char title_char[strlen(title)+1];

	strcpy(storry_char,storry);
	strcpy(title_char,title);

	show_storry(storry_char,strlen(storry),title_char,strlen(title),type);
}

void ILI9341::show_storry(const char* storry,unsigned int storry_length,char title[],unsigned int title_length){
	show_storry(storry,storry_length,title, title_length, 0x00);
}

void ILI9341::show_storry(const char* storry,unsigned int storry_length,char title[],unsigned int title_length, uint8_t type){
#define CHARS_IN_LINE 27
#define LINES_ON_SCREEN 12

	// show title
	clear_screen();
	highlight_bar(0,4,320,24); // mit hintergrundfarbe nen kasten malen
	char title_centered[27];
	strcpy(title_centered,title);
	Menu.center_me(title_centered,25);
	string(Speedo.default_font,title_centered,2,1,DISP_BRIGHTNESS,0,0);

	// Generate borders
	unsigned int fill_line=0; // actual line
	unsigned int char_in_line=0; // count char in this line
	int von[LINES_ON_SCREEN];
	int bis[LINES_ON_SCREEN];
	for(int i=0; i<LINES_ON_SCREEN; i++){ // fuer alle anfaenge und enden die pointer setzen
		von[i]=0;
		bis[i]=0;
	}; // nutzen um zu bestimmten wo wir den "\0" setzen

	for(unsigned int i=0; i<storry_length;i++){
		if(storry[i]==' ' && char_in_line==0){ // avoid blanks in the start of a line
			char_in_line--; // will be incread at the end -> 0
			von[fill_line]++; // shift the start of our line one step further
		} else if(storry[i]==0x0D || char_in_line>CHARS_IN_LINE-10){ // wenn wir ein umbruch haben oder mindestesn mal 20 zeichen aufgenommen haben
			// entweder ist es ein umbruch, oder wir warten auf ein freizeichen, oder die line ist voll
			if( (storry[i]==0x0D || storry[i]==' ' || char_in_line==CHARS_IN_LINE) && (fill_line+1<LINES_ON_SCREEN) ){
				bis[fill_line]=i; // damit haben wir das ende dieser Zeile gefunden
				fill_line++;
				von[fill_line]=i;// und den anfang der nächsten, wobei das noch nicht save ist
				if(storry[i]==' '){ // wir haben hier ein freizeichen, hätten wir sinnvoll weitergucken können?
					von[fill_line]++; // das freizeichen brauchen wie eh nicht mehr
					//haben ein volles wort, mal sehen ob ncoh was geht
					//aktuell sind char_in_line chars im puffer
					int onemoreword=0; // wieviele chars gehts denn weiter, falls sinnvoll
					// laufe durch die verbleibenden (char_in_line-CHARS_IN_LINE=10) zeichen, und guck wo das letzte freizeichen ist
					for(unsigned int k=char_in_line; k<CHARS_IN_LINE; k++){
						if(storry[i+k-char_in_line]==' ') onemoreword=k-char_in_line; // wenn nach 4 zeichen ein freichen kam, steht hier 4
					}
					if(onemoreword>0){
						// es scheint sinnig noch onemoreword buchstaben zu nutzen
						i+=onemoreword;
						von[fill_line]+=onemoreword;
						bis[fill_line-1]+=onemoreword;
					}
				} else if(storry[i]==0x0D){ // wenn wir ein umbruch haben ueberspringen wir das zeichen
					von[fill_line]++;
				}
				char_in_line=-1; // wird gleich inc -> dann sind wir fertig und der counter bei 0
			}
		}
		char_in_line++;
	};
	// end of loop  //

	if(bis[fill_line]==0){ // letztes array ding
		bis[fill_line]=storry_length;
	};
	// we got the borders

	// draw to display
	// reserve buffer
	char *buffer2;
	buffer2 = (char*) malloc (CHARS_IN_LINE);
	if (buffer2==NULL) Serial.puts_ln(USART1,("Malloc failed"));
	else memset(buffer2,'\0',sizeof(buffer2)/sizeof(buffer2[0]));
	for(unsigned int i=0; i<LINES_ON_SCREEN; i++){ // nur X zeilen
		if(von[i]!=bis[i]){
			int k=0;
			for(int j=von[i]; j<bis[i] && k<CHARS_IN_LINE; j++){
				if(!(i==0 && storry[j]=='#')){ // in der ersten zeile, das erste "#" an stelle 0 überlesen
					buffer2[k]=storry[j];
					k++;
				};
			};
			buffer2[k]='\0';
			string(Speedo.default_font,buffer2,0,i*2+5,0,DISP_BRIGHTNESS,0);
		};
	}
	//delete buffer
	free(buffer2);

	//
	unsigned int current_state=Menu.state;
	unsigned long current_timestamp=Millis.get();

	// set buttons if needed
	if(type>=DIALOG_NO_YES){
		Menu.set_buttons(true,false,false,true);
	}

	if(type==DIALOG_NO_YES){
		highlight_bar(0,215,320,24); // mit hintergrundfarbe nen kasten malen
		string(Speedo.default_font,("\x7E back            next \x7F"),2,31,DISP_BRIGHTNESS,0,0);
	}
	else if(type==DIALOG_GO_RIGHT_200MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<200){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_200MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<200){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_500MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<500){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_500MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<500){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_1000MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<1000){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_1000MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<1000){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_2000MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<2000){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_2000MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<2000){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_left(true);
		}
	} else if(type==DIALOG_GO_RIGHT_5000MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<5000){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_right(true);
		}
	} else if(type==DIALOG_GO_LEFT_5000MS){
		while(current_state==Menu.state && (Millis.get()-current_timestamp)<5000){
			_delay_ms(1);
		}
		if(current_state==Menu.state){
			Menu.go_left(true);
		}
	} else if(type==DIALOG_SHOW_500MS){
		_delay_ms(500);
	}
}

void ILI9341::string_centered(const char* text, uint8_t line){
	string_centered(text,line,false);
}

void ILI9341::string_centered(const char* text, uint8_t line, bool inverted){
	if(strlen(text)>20){
		return;
	};

	uint16_t front_color=255;
	uint16_t back_color=0;
	uint16_t start_pos=0;
	uint16_t length_of_char=27;

	if(inverted){
		front_color=255;
		back_color=255;
		if(strlen(text)<=23){ // 6*2pixel == 2 chars. (320/12=)27 Chars - 2*2 = 23chars max
			length_of_char=23;
			start_pos=2;
			highlight_bar(0,line*7,320,16);
		} else {
			filled_rect(0,line*7,320,12,0x0f);
		}
	}
	char text_char[length_of_char]; // full display width +1
	strcpy(text_char,text);
	Menu.center_me(text_char,length_of_char); // full display width
	string(Speedo.default_font,text_char,start_pos,line,front_color,front_color,front_color,back_color,0,0,0);
}


void ILI9341::draw_gps(uint16_t x,uint16_t y, unsigned char sats){
	TFT.draw_bmp(x,y,(uint8_t*)"/sat.bmp");
};

void ILI9341::draw_oil(uint16_t x,uint16_t y){
	TFT.draw_bmp(x,y,(uint8_t*)"/temp.bmp");
};

void ILI9341::draw_water(uint16_t x,uint16_t y){
	TFT.draw_bmp(x,y,(uint8_t*)"/temp.bmp");
};

void ILI9341::draw_air(uint16_t x,uint16_t y){
	TFT.draw_bmp(x,y,(uint8_t*)"/temp.bmp");
};

void ILI9341::draw_fuel(uint16_t x,uint16_t y){
	Serial.puts(USART1,"Fuel at x/y:");
	Serial.puts(USART1,x);
	Serial.puts(USART1,"/");
	Serial.puts_ln(USART1,y);
	TFT.draw_bmp(x,y,(uint8_t*)"/fuel.bmp");
};

void ILI9341::draw_clock(uint16_t x,uint16_t y){
	TFT.draw_bmp(x,y,(uint8_t*)"/time.bmp");
};

void ILI9341::draw_blitzer(uint16_t x,uint16_t y){
	TFT.draw_bmp(x,y,(uint8_t*)"/sat.bmp");
}

uint8_t ILI9341::draw_bmp(uint16_t x, uint16_t y,uint8_t* filename){
#define DRAW_BMP_DEBUG_LEVEL 3
	FIL file;
//	if (UB_Fatfs_OpenFile(&file, "/Kojla2.bmp", F_RD)== FATFS_OK) {
	if(f_open(&file, (TCHAR*)filename, FA_OPEN_EXISTING | FA_READ)==0){
		// used vars
		uint32_t index = 0, size = 0, width = 0, height = 0, bit_pixel = 0;
		uint32_t currentline = 0, linenumber = 0;
		char* buffer;
		buffer = (char*) malloc(513);

		//// read header ////
		UINT n_byte_read;
		uint32_t byte_read_total = 0;
		f_read(&file, buffer, 29, &n_byte_read);
		if (n_byte_read < 29) {
			return -2; // file to short, not even header inside
		} else {
			index = buffer[10] 	| buffer[11] << 8 	| buffer[12] << 16 	| buffer[13] << 24;		/* Get bitmap data address offset */
			width = buffer[18] 	| buffer[19] << 8 	| buffer[20] << 16 	| buffer[21] << 24;		/* Read bitmap width */
			height = buffer[22] | buffer[23] << 8 	| buffer[24] << 16 	| buffer[25] << 24;		/* Read bitmap height */
			bit_pixel = buffer[28];																/* Read bit/pixel */
			size = width*height;																/* bitmap size */
		}

		f_lseek(&file, index-1);								/* bypass the bitmap header */

#if DRAW_BMP_DEBUG_LEVEL>1
		//// time debug ////
		uint32_t start = Millis.get();
		uint32_t read_time = 0;
		//// time debug ////
#endif
		// read pixels 512 byte per read
		while (n_byte_read > 0 && byte_read_total/(bit_pixel/8) <= size) {
#if DRAW_BMP_DEBUG_LEVEL>1
			//// time debug ////
			int32_t start_read = Millis.get();
			//// time debug ////
#endif
			f_read(&file, buffer, 512, &n_byte_read); // read complete cluster
#if DRAW_BMP_DEBUG_LEVEL>1
			//// time debug ////
			read_time += Millis.get() - start_read;
			//// time debug ////
#endif
			byte_read_total+=n_byte_read;

			// draw "512/(bit per pixel)"-pixels
			for(unsigned int i=0;i<n_byte_read;i+=2){
				TFT.Pixel(currentline,y+height-linenumber,buffer[i]<<8|buffer[i+1]);
				/*jump on next byte */
				currentline++;
				if(currentline == width){
					if(linenumber < height)	{
						linenumber++;
						currentline = 0;
					}
				}
			} // draw pixels
		} // read file

#if DRAW_BMP_DEBUG_LEVEL>1
		//// time debug ////
		uint32_t time = Millis.get() - start;
		sprintf(buffer,"Times: %i / %i",time,read_time);
		TFT.set_transparent_font(true);
		TFT.string(buffer,0,0);
		TFT.set_transparent_font(false);
		//// time debug ////
#endif
		free(buffer);
		return 0;
	} // open file
	return -2; // open file failed
	//	headerless
	//#define DRAW_BMP_DEBUG_LEVEL 3
	//	uint16_t x=0,y=0;
	//	FIL file;
	//	if (UB_Fatfs_OpenFile(&file, "/logo_r.bmp", F_RD)== FATFS_OK) {
	//		// used vars
	//		uint32_t index = 0, size = 0, width = 320, height = 240, bit_pixel = 0;
	//		uint32_t currentline = 0, linenumber = 0;
	//		char* buffer;
	//		buffer = (char*) malloc(513);
	//
	//		//// read header ////
	//		UINT n_byte_read;
	//
	//#if DRAW_BMP_DEBUG_LEVEL>1
	//		//// time debug ////
	//		uint32_t start = Millis.get();
	//		uint32_t read_time = 0;
	//		//// time debug ////
	//#endif
	//		// read pixels 512 byte per read
	//		while (n_byte_read > 0) {
	//#if DRAW_BMP_DEBUG_LEVEL>1
	//			//// time debug ////
	//			int32_t start_read = Millis.get();
	//			//// time debug ////
	//#endif
	//			f_read(&file, buffer, 512, &n_byte_read); // read complete cluster
	//#if DRAW_BMP_DEBUG_LEVEL>1
	//			//// time debug ////
	//			read_time += Millis.get() - start_read;
	//			//// time debug ////
	//#endif
	////			byte_read_total+=n_byte_read;
	//
	//			// draw "512/(bit per pixel)"-pixels
	//			for(unsigned int i=0;i<n_byte_read;i+=2){
	//				TFT.Pixel(currentline,y+height-linenumber,buffer[i]<<8|buffer[i+1]);
	//				/*jump on next byte */
	//				currentline++;
	//				if(currentline >= width){
	//					if(linenumber < height)	{
	//						linenumber++;
	//						currentline = 0;
	//					}
	//				}
	//			} // draw pixels
	//		} // read file
	//
	//#if DRAW_BMP_DEBUG_LEVEL>1
	//		//// time debug ////
	//		uint32_t time = Millis.get() - start;
	//		Serial.puts(USART1, "Time:");
	//		Serial.puts_ln(USART1, time);
	//		Serial.puts(USART1, "Time for reading:");
	//		Serial.puts_ln(USART1, read_time);
	//
	//		sprintf(buffer,"Times: %i / %i",time,read_time);
	//		TFT.string(buffer,0,0);
	//		//// time debug ////
	//#endif
	//		free(buffer);
	//		return 0;
	//	} // open file
	//	return -2; // open file failed
}

int ILI9341::animation(int a){
	return 0;
};


void ILI9341::draw_arrow(int angle, int x_pos, int y_pos, uint8_t r, uint8_t g, uint8_t b){
	draw_arrow(angle,x_pos,y_pos,r,g,b,true);
}

void ILI9341::draw_arrow(int angle, int x_pos, int y_pos, uint8_t r, uint8_t g, uint8_t b,bool clean){
	angle=(angle+360)%360;
	// raw symbol without rotation
	uint32_t symbol[32];
	symbol[ 0]=0b00000000000000000000000000000000;
	symbol[ 1]=0b00000000000000011000000000000000;
	symbol[ 2]=0b00000000000000111100000000000000;
	symbol[ 3]=0b00000000000001111110000000000000;
	symbol[ 4]=0b00000000000011111111000000000000;
	symbol[ 5]=0b00000000000111111111100000000000;
	symbol[ 6]=0b00000000001111111111110000000000;
	symbol[ 7]=0b00000000011111111111111000000000;
	symbol[ 8]=0b00000000111111111111111100000000;
	symbol[ 9]=0b00000001111111111111111110000000;
	symbol[10]=0b00000011111111111111111111000000;
	symbol[11]=0b00000111111111111111111111100000;
	symbol[12]=0b00001111111111111111111111110000;
	symbol[13]=0b00011111111111111111111111111000;
	symbol[14]=0b00111111111111111111111111111100;
	symbol[15]=0b01111111111111111111111111111110;
	symbol[16]=0b01111111111111111111111111111110;
	symbol[17]=0b01111111111111111111111111111110;
	symbol[18]=0b01111111111111111111111111111110;
	symbol[19]=0b00000000111111111111111100000000;
	symbol[20]=0b00000000111111111111111100000000;
	symbol[21]=0b00000000111111111111111100000000;
	symbol[22]=0b00000000111111111111111100000000;
	symbol[23]=0b00000000111111111111111100000000;
	symbol[24]=0b00000000111111111111111100000000;
	symbol[25]=0b00000000111111111111111100000000;
	symbol[26]=0b00000000111111111111111100000000;
	symbol[27]=0b00000000000000000000000000000000;
	symbol[28]=0b00000000000000000000000000000000;
	symbol[29]=0b00000000000000000000000000000000;
	symbol[30]=0b00000000000000000000000000000000;
	symbol[31]=0b00000000000000000000000000000000;
	symbol[32]=0b00000000000000000000000000000000;

	// clean matrix for incoming symbol
	uint32_t rot_symbol[32];
	for(int x=0;x<32;x++){
		rot_symbol[x]=0x00;
	}

	// rotate
	for(int8_t x=-15;x<16;x++){
		for(int8_t y=-15;y<16;y++){
			if(symbol[(y+16)]&(1<<(x+16))){
				int8_t x_n=(cos(PI*angle/180)*x-sin(PI*angle/180)*y)+16;
				int8_t y_n=(sin(PI*angle/180)*x+cos(PI*angle/180)*y)+16;
				if(x_n>=0 && x_n<32 && y_n>=0 && y_n<32){
					rot_symbol[y_n]|=1<<x_n;
				}
			}
		}
	}

	// clean space it if required
	if(clean){
		filled_rect(x_pos,y_pos,32,32,0,0,0);
	}

	// now draw it
	for(int8_t x=0;x<32;x++){
		for(int8_t y=0;y<32;y++){
			if(rot_symbol[y]&(1<<x)){
				Pixel(x+x_pos,y+y_pos,r,g,b);
			}
		}
	}


}

// check if x/y are in display range
void ILI9341::check_coordinates(int16_t* x,int16_t* y){
	if(*x<0){
		*x=0;
	} else if(*x>=320){
		*x=319;
	}

	if(*y<0){
		*y=0;
	} else if(*y>=240){
		*y=239;
	}
}
