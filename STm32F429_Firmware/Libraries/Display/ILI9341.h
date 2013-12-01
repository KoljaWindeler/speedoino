#ifndef __ILI9341_H
#define __ILI9341_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_sdram.h"
//#include "../Common/fonts.h"

typedef struct 
{
	int16_t X;
	int16_t Y;
} Point, * pPoint;   

/* LCD Size (Width and Height) */
#define  LCD_PIXEL_HEIGHT    ((uint16_t)320)
#define  LCD_PIXEL_WIDTH     ((uint16_t)240)

#define LCD_FRAME_BUFFER       ((uint32_t)0xD0000000)
#define BUFFER_OFFSET          ((uint32_t)0x50000)

/** 
 * @brief  LCD Control pin
 */
#define LCD_NCS_PIN             GPIO_Pin_2                  
#define LCD_NCS_GPIO_PORT       GPIOC                      
#define LCD_NCS_GPIO_CLK        RCC_AHB1Periph_GPIOC 

/** 
 * @brief  LCD Command/data pin
 */
#define LCD_WRX_PIN             GPIO_Pin_13                  
#define LCD_WRX_GPIO_PORT       GPIOD
#define LCD_WRX_GPIO_CLK        RCC_AHB1Periph_GPIOD 

/** 
 * @brief  LCD SPI Interface pins
 */
#define LCD_SPI_SCK_PIN               GPIO_Pin_7                     /* PF.07 */
#define LCD_SPI_SCK_GPIO_PORT         GPIOF                          /* GPIOF */
#define LCD_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOF  
#define LCD_SPI_SCK_SOURCE            GPIO_PinSource7
#define LCD_SPI_SCK_AF                GPIO_AF_SPI5
#define LCD_SPI_MISO_PIN              GPIO_Pin_8                     /* PF.08 */
#define LCD_SPI_MISO_GPIO_PORT        GPIOF                          /* GPIOF */
#define LCD_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOF  
#define LCD_SPI_MISO_SOURCE           GPIO_PinSource8
#define LCD_SPI_MISO_AF               GPIO_AF_SPI5
#define LCD_SPI_MOSI_PIN              GPIO_Pin_9                     /* PF.09 */
#define LCD_SPI_MOSI_GPIO_PORT        GPIOF                          /* GPIOF */
#define LCD_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOF  
#define LCD_SPI_MOSI_SOURCE           GPIO_PinSource9
#define LCD_SPI_MOSI_AF               GPIO_AF_SPI5
#define LCD_SPI                       SPI5
#define LCD_SPI_CLK                   RCC_APB2Periph_SPI5 

/** 
 * @brief  LCD Registers
 */
#define LCD_SLEEP_OUT            0x11   /* Sleep out register */
#define LCD_GAMMA                0x26   /* Gamma register */
#define LCD_DISPLAY_OFF          0x28   /* Display off register */
#define LCD_DISPLAY_ON           0x29   /* Display on register */
#define LCD_COLUMN_ADDR          0x2A   /* Colomn address register */ 
#define LCD_PAGE_ADDR            0x2B   /* Page address register */ 
#define LCD_GRAM                 0x2C   /* GRAM register */   
#define LCD_MAC                  0x36   /* Memory Access Control register*/
#define LCD_PIXEL_FORMAT         0x3A   /* Pixel Format register */
#define LCD_WDB                  0x51   /* Write Brightness Display register */
#define LCD_WCD                  0x53   /* Write Control Display register*/
#define LCD_RGB_INTERFACE        0xB0   /* RGB Interface Signal Control */
#define LCD_FRC                  0xB1   /* Frame Rate Control register */
#define LCD_BPC                  0xB5   /* Blanking Porch Control register*/
#define LCD_DFC                  0xB6   /* Display Function Control register*/
#define LCD_POWER1               0xC0   /* Power Control 1 register */
#define LCD_POWER2               0xC1   /* Power Control 2 register */
#define LCD_VCOM1                0xC5   /* VCOM Control 1 register */
#define LCD_VCOM2                0xC7   /* VCOM Control 2 register */
#define LCD_POWERA               0xCB   /* Power control A register */
#define LCD_POWERB               0xCF   /* Power control B register */
#define LCD_PGAMMA               0xE0   /* Positive Gamma Correction register*/
#define LCD_NGAMMA               0xE1   /* Negative Gamma Correction register*/
#define LCD_DTCA                 0xE8   /* Driver timing control A */
#define LCD_DTCB                 0xEA   /* Driver timing control B */
#define LCD_POWER_SEQ            0xED   /* Power on sequence register */
#define LCD_3GAMMA_EN            0xF2   /* 3 Gamma enable register */
#define LCD_INTERFACE            0xF6   /* Interface control register */
#define LCD_PRC                  0xF7   /* Pump ratio control register */

/** 
 * @brief  LCD color
 */
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

/** 
 * @brief  LCD Lines depending on the chosen fonts.
 */
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)
#define LCD_LINE_20              LINE(20)
#define LCD_LINE_21              LINE(21)
#define LCD_LINE_22              LINE(22)
#define LCD_LINE_23              LINE(23)
#define LCD_LINE_24              LINE(24)
#define LCD_LINE_25              LINE(25)
#define LCD_LINE_26              LINE(26)
#define LCD_LINE_27              LINE(27)
#define LCD_LINE_28              LINE(28)
#define LCD_LINE_29              LINE(29)
#define LCD_LINE_30              LINE(30)
#define LCD_LINE_31              LINE(31)
#define LCD_LINE_32              LINE(32)
#define LCD_LINE_33              LINE(33)
#define LCD_LINE_34              LINE(34)
#define LCD_LINE_35              LINE(35)
#define LCD_LINE_36              LINE(36)
#define LCD_LINE_37              LINE(37)
#define LCD_LINE_38              LINE(38)
#define LCD_LINE_39              LINE(39)

/** 
 * @brief LCD default font
 */
#define LCD_DEFAULT_FONT         Font16x24

/** 
 * @brief  LCD Direction
 */
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

/**
 * @}
 */

/** 
 * @brief  LCD Layer
 */
#define LCD_BACKGROUND_LAYER     0x0000
#define LCD_FOREGROUND_LAYER     0x0001

/**
 * @}
 */

/** @defgroup STM32F429I_DISCOVERY_LCD_Exported_Macros
 * @{
 */
#define ASSEMBLE_RGB(R, G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))  

/**
 * @}
 */

/** @defgroup STM32F429I_DISCOVERY_LCD_Exported_Functions
 * @{
 */
class ILI9341 {
public:
	// control functions
	void     init(void);
	void     clear_screen();
	void     clear_screen(uint16_t Color);
	void     LayerInit(void);
	void     SetLayer(uint32_t Layerx);
	void     SetTransparency(uint8_t transparency);
	void     DisplayOn(void);
	void     DisplayOff(void);
	void     SetTextColor(uint16_t Color);
	void     SetBackColor(uint16_t Color);
	void     SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);

	// text functions
	void 	 string(char *str,uint8_t spalte, uint8_t zeile);
	void 	 string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile);
	void 	 string(char *str,uint8_t spalte, uint8_t zeile, uint8_t back, uint8_t text);
	void 	 string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile, uint8_t back, uint8_t text, uint8_t offset);
	void 	 string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile, uint8_t text_r, uint8_t text_g, uint8_t text_b, uint8_t back_r, uint8_t back_g, uint8_t back_b, uint8_t offset);

	void 	 show_storry(const char* storry,const char* title);
	void 	 show_storry(const char* storry,unsigned int storry_length,char title[],unsigned int title_length, uint8_t type);
	void 	 show_storry(const char* storry,unsigned int storry_length,char title[],unsigned int title_length);
	void 	 show_storry(const char* storry,const char* title, uint8_t type);
	void 	 string_centered(const char* text, uint8_t line);
	void 	 string_centered(const char* text, uint8_t line, bool inverted);

	// icon functions
	void 	 draw_oil(uint16_t x,uint16_t y);
	void 	 draw_water(uint16_t x,uint16_t y);
	void 	 draw_air(uint16_t x,uint16_t y);
	void 	 draw_fuel(uint16_t x,uint16_t y);
	void 	 draw_clock(uint16_t x,uint16_t y);
	void 	 draw_gps(uint16_t x,uint16_t y, unsigned char sats);
	void 	 draw_blitzer(uint16_t x,uint16_t y);
	void 	 draw_arrow(int arrow, int spalte, int zeile, uint8_t r, uint8_t g, uint8_t b);
	void 	 draw_arrow(int angle, int x_pos, int y_pos, uint8_t r, uint8_t g, uint8_t b,bool clean);
	uint8_t  draw_bmp(uint16_t x, uint16_t y, uint8_t* filename);
	int 	 animation(int a);

	// basic drawing functions
	void 	 Pixel(int16_t x, int16_t y);
	void	 Pixel(int16_t x, int16_t y,uint8_t r,uint8_t g,uint8_t b);
	uint16_t convert_color(uint8_t r,uint8_t g,uint8_t b);
	void 	 draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void 	 draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t r, uint8_t g, uint8_t b);

	// extended drawing functions
	void     filled_rect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t color);
	void 	 filled_rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t r,uint8_t g,uint8_t b);
	void 	 highlight_bar(uint16_t x,uint16_t y,uint16_t width,uint16_t height);



	unsigned char startup[35]; // asdfghjk.asd,1234,1234,1234\0 == 28

private:
	void	 zeichen_small_1x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_2x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_3x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_4x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_5x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_6x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_7x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_8x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void 	 zeichen_small_scale(uint8_t scale,const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);

	void 	 check_coordinates(int16_t* x,int16_t* y);
	uint32_t SetCursor(uint32_t Xpos, uint32_t Ypos);

	void     LCD_DeInit(void);
	void     LCD_ChipSelect(FunctionalState NewState);
//	void     LCD_SetColors(uint16_t _TextColor, uint16_t _BackColor);
//	void     LCD_GetColors(uint16_t *_TextColor, uint16_t *_BackColor);
	void     LCD_ClearLine(uint16_t Line);

	void     LCD_SetColorKeying(uint32_t RGBValue);
	void     LCD_ReSetColorKeying(void);
	void     LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c);
	void     LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii);
	//void     LCD_SetFont(sFONT *fonts);
	//sFONT *  LCD_GetFont(void);

	void     LCD_WindowModeDisable(void);

	void     LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);
	void     LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
	void     LCD_DrawEllipse(int Xpos, int Ypos, int Radius, int Radius2);
	void     LCD_DrawFullEllipse(int Xpos, int Ypos, int Radius, int Radius2);
	void     LCD_DrawMonoPict(const uint32_t *Pict);
	void     LCD_WriteBMP(uint32_t BmpAddress);

	void     LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
	void     LCD_PolyLine(pPoint Points, uint16_t PointCount);
	void     LCD_PolyLineRelative(pPoint Points, uint16_t PointCount);
	void     LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount);
	void     LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
	void     LCD_FillPolyLine(pPoint Points, uint16_t PointCount);
	void     LCD_Triangle(pPoint Points, uint16_t PointCount);
	void     LCD_FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
	void     LCD_WriteCommand(uint8_t LCD_Reg);
	void     LCD_WriteData(uint8_t value);
	void     LCD_PowerOn(void);

	void     LCD_CtrlLinesConfig(void);
	void     LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal);
	void     LCD_SPIConfig(void);
	void     LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);
	void     LCD_AF_GPIOConfig(void);
	/**
	 * @}
	 */

};
extern ILI9341 TFT;
#endif
