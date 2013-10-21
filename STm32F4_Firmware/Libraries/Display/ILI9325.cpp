/**
 ********************************************************************************
 *
 * Pin configuration
 *
 *        D7 : LCD Pin D15 -> PD10 [FSMC D15] (1.42)
 *        D6 : LCD Pin D14 -> PD9  [FSMC D14] (1.41)
 *        D5 : LCD Pin D13 -> PD8  [FSMC D13] (1.40)
 *        D4 : LCD Pin D12 -> PE15 [FSMC D12] (1.33)
 *        D3 : LCD Pin D11 -> PE14 [FSMC D11] (1.32)
 *        D2 : LCD Pin D10 -> PE13 [FSMC D10] (1.31)
 *        D1 : LCD Pin D9  -> PE12 [FSMC D09] (1.30)
 *        D0 : LCD Pin D8  -> PE11 [FSMC D08] (1.29)
 *
 *        RS :     		   -> PD11 [FSMS A16] (1.43)
 *       ~RW :             -> PD5  [FSMS NWE] (2.29) (USB OTG Over Current LED)
 *       ~RD :             -> PD4  [FSMS NOE] (2.32)
 *       ~CS :             -> PD7  [FSMS NE1] (2.27)
 *       ~RST:   		   -> PE10 (1.28)
 *       LED Backlight :   -> PE9  (1.27) (Timer 1 channel 1)
 */
#include "stm32f4xx_conf.h"
#include "ILI9325.h"
#include "AsciiLib.h"
#include "stdlib.h"
#include "global.h"


#define LCD_REG      (*((volatile short *) 0x60000000))
#define LCD_RAM      (*((volatile short *) 0x60020000))

/* Global variables to set the text color */
volatile u8 textRed = 0xFF;
volatile u8 textGreen = 0xFF;
volatile u8 textBlue = 0xFF;

volatile u8 backRed = 0x00;
volatile u8 backGreen = 0x00;
volatile u8 backBlue = 0x00;

volatile uint16_t asciisize = 16;

void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);


void LCD_Init(void) {
	u16 readValue;
	LCD_CtrlLinesConfig();
	LCD_FSMCConfig();

	//Pulse reset
	GPIO_ResetBits(GPIOE, GPIO_Pin_10);
	Delay(300);
	GPIO_SetBits(GPIOE, GPIO_Pin_10);
	Delay(100);

	readValue = LCD_ReadReg(R0);

	/* Start Initial Sequence ----------------------------------------------------*/
	//LCD_WriteReg(R227, 0x3008); // Internal clock
	//LCD_WriteReg(R231, 0x0012); // Internal clock
	//LCD_WriteReg(R239, 0x1231); // Internal clock
	LCD_WriteReg(R0, 0x0001); /* Start oscillator */
	LCD_WriteReg(R1, 0x0000); /* set SS and SM bit */
	LCD_WriteReg(R2, 0x0200); /* set 1 line inversion */
	LCD_WriteReg(R3, 0xC030 | BRG_Mode); /* 8bits/262K BGR=x. */
	LCD_WriteReg(R4, 0x0000); /* Resize */
	LCD_WriteReg(R8, 0x0207); /* set the back porch and front porch */
	LCD_WriteReg(R9, 0x0000); /* set non-display area refresh cycle ISC[3:0] */
	LCD_WriteReg(R10, 0x0000); /* FMARK function */
	LCD_WriteReg(R12, 0x0002); /* RGB interface setting */
	LCD_WriteReg(R13, 0x0000); /* Frame marker Position */
	LCD_WriteReg(R15, 0x0000); /* RGB interface polarity */

	/* Power On sequence ---------------------------------------------------------*/
	LCD_WriteReg(R16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	LCD_WriteReg(R17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
	LCD_WriteReg(R18, 0x0000); /* VREG1OUT voltage */
	LCD_WriteReg(R19, 0x0000); /* VDV[4:0] for VCOM amplitude */
	Delay(200); /* Dis-charge capacitor power voltage (200ms) */
	LCD_WriteReg(R16, R16_VAL); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	LCD_WriteReg(R17, R17_VAL); /* DC1[2:0], DC0[2:0], VC[2:0] */
	Delay(50); /* Delay 50 ms */

	LCD_WriteReg(R18, R18_VAL); /* VREG1OUT voltage */
	Delay(50); /* Delay 50 ms */
	LCD_WriteReg(R19, R19_VAL); /* VDV[4:0] for VCOM amplitude */
	LCD_WriteReg(R41, R41_VAL); /* VCM[4:0] for VCOMH */
	LCD_WriteReg(R43, 0x000B); // Set Frame Rate
	Delay(50); /* Delay 50 ms */

	/* Adjust the Gamma Curve ----------------------------------------------------*/
	LCD_WriteReg(R48, 0x0001); // Fine Gamma KP1 - KP0 	[2:0] [2:0]
	LCD_WriteReg(R49, 0x0101); // Fine Gamma KP2 - KP3 	[2:0] [2:0]
	LCD_WriteReg(R50, 0x0101); // Fine Gamma KP4 - KP5 	[2:0] [2:0]
	LCD_WriteReg(R53, 0x0202); // Gradient RP1 - RP0 		[2:0] [2:0]
	LCD_WriteReg(R54, 0x0A00); // Amplitude VRP1 - VRP0	[4:0] [4:0]
	LCD_WriteReg(R55, 0x0507); // Fine Gamma KN1 - KN0 	[2:0] [2:0]
	LCD_WriteReg(R56, 0x0101); // Fine Gamma KN3 - KN2 	[2:0] [2:0]
	LCD_WriteReg(R57, 0x0101); // Fine Gamma KN5 - KN4 	[2:0] [2:0]
	LCD_WriteReg(R60, 0x0202); // Gradient RN1 - RN0 		[2:0] [2:0]
	LCD_WriteReg(R61, 0x0000); // Amplitude VRN1 - VRN0	[4:0] [4:0]

	/* Set GRAM area -------------------------------------------------------------*/
	LCD_WriteReg(R80, 0x0000); /* Horizontal GRAM Start Address */
	LCD_WriteReg(R81, 0x00EF); /* Horizontal GRAM End Address */
	LCD_WriteReg(R82, 0x0000); /* Vertical GRAM Start Address */
	LCD_WriteReg(R83, 0x013F); /* Vertical GRAM End Address */
	LCD_WriteReg(R32, 0x0000); /* GRAM horizontal Address */
	LCD_WriteReg(R33, 0x0000); /* GRAM Vertical Address */

	LCD_WriteReg(R96, 0x2700); /* Gate Scan Line */
	LCD_WriteReg(R97, 0x0001); /* NDL,VLE, REV */
	LCD_WriteReg(R106, 0x0000); /* set scrolling line */

	/* Partial Display Control ---------------------------------------------------*/
	LCD_WriteReg(R128, 0x0000);
	LCD_WriteReg(R129, 0x0000);
	LCD_WriteReg(R130, 0x0000);
	LCD_WriteReg(R131, 0x0000);
	LCD_WriteReg(R132, 0x0000);
	LCD_WriteReg(R133, 0x0000);

	/* Panel Control -------------------------------------------------------------*/
	LCD_WriteReg(R144, 0x0010);
	LCD_WriteReg(R146, 0x0600);

	Delay(50);

	/* Set GRAM write direction and BGR = 1 */
	/* I/D=01 (Horizontal : increment, Vertical : decrement) */
	/* AM=1 (address is updated in vertical writing direction) */
	/* TRI=1, DFM=1 8 bit mode and 262K */
	LCD_WriteReg(R7, 0x0133); /* 262K color and display ON */
	/* Clear the LCD */
	Delay(50);

	LCD_Clear(0, 0, 0);
	Delay(50);
}

/*******************************************************************************
 * Function Name  : LCD_Clear
 * Description    : Clears the hole LCD.
 * Input          : Color: the color of the background.
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_Clear(u8 r, u8 g, u8 b) {
	u32 index = 0;

	LCD_SetCursor(0x00, 0x00);

	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	for (index = 0; index < 76800; index++) {
		LCD_RAM = r << 8;
		LCD_RAM = g << 8;
		LCD_RAM = b << 8;
	}
}

/*******************************************************************************
 * Function Name  : LCD_DisplayOn
 * Description    : Enables the Display.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_DisplayOn(void) {
	/* Display On */
	LCD_WriteReg(R7, 0x0133); /* 262K color and display ON */
}

/*******************************************************************************
 * Function Name  : LCD_DisplayOff
 * Description    : Disables the Display.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_DisplayOff(void) {
	/* Display Off */
	LCD_WriteReg(R7, 0x0);
}

/*******************************************************************************
 * Function Name  : LCD_Disp_Image
 * Description    : Disables the Display.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_Disp_Image(unsigned short *gImage_ptr) {
	u32 i = 0;
	u16 raw;
	u16 red;
	u16 green;
	u16 blue;

	LCD_SetCursor(0x00, 0x00);

	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

	for (i = 0; i < 76800; i++) {
		raw = gImage_ptr[i];
		blue = raw & 0XF800;
		green = (raw << 5) & 0xFC00;
		red = (raw & 0x1F) << 11;
		LCD_RAM = red;
		LCD_RAM = green;
		LCD_RAM = blue;
	}
}

void LCD_SetColors(u8 text_r, u8 text_g, u8 text_b, u8 back_r, u8 back_g,
		u8 back_b) {
	textRed = text_r;
	textGreen = text_g;
	textBlue = text_b;
	backRed = back_r;
	backGreen = back_g;
	backBlue = back_b;
}

void LCD_GetColors(u8 *text_r, u8 *text_g, u8 *text_b, u8 *back_r, u8 *back_g,
		u8 *back_b) {
	*text_r = textRed;
	*text_g = textGreen;
	*text_b = textBlue;
	*back_r = backRed;
	*back_g = backGreen;
	*back_b = backBlue;
}

void LCD_SetTextColor(u8 red, u8 green, u8 blue) {
	textRed = red;
	textGreen = green;
	textBlue = blue;
}

void LCD_SetBackColor(u8 red, u8 green, u8 blue) {
	backRed = red;
	backGreen = green;
	backBlue = blue;
}

/* 8x8=8 12x12=12 8x16=16 12x12=14 16x24=24 */
void LCD_CharSize(uint16_t size) {
	asciisize = size;
}

void PutPixel(int16_t x, int16_t y) {
	if ((x > 239) || (y > 319))
		return;
	LCD_SetCursor(x, y);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(textRed, textGreen, textBlue);
}

void Pixel(int16_t x, int16_t y, u8 r, u8 g, u8 b) {
	if ((x > 239) || (y > 319))
		return;
	LCD_SetCursor(x, y);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(r, g, b);
}

void LCD_PutChar(int16_t PosX, int16_t PosY, char c) {
	uint8_t i = 0;
	uint8_t j = 0;
	if (asciisize == 8) {
		char Buffer[8];
		uint8_t TmpChar = 0;

		GetASCIICode1(Buffer, c);
		for (i = 0; i < 8; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 8; j++) {
				if (((TmpChar >> (7 - j)) & 0x01) == 0x01) {
					Pixel(PosX + j, PosY + i, textRed, textGreen, textBlue);
				} else {
					Pixel(PosX + j, PosY + i, backRed, backGreen, backBlue);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 12) {
		char Buffer[12];
		uint8_t TmpChar = 0;

		GetASCIICode2(Buffer, c);
		for (i = 0; i < 12; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 8; j++) {
				if (((TmpChar >> (7 - j)) & 0x01) == 0x01) {
					Pixel(PosX + j, PosY + i, textRed, textGreen, textBlue);
				} else {
					Pixel(PosX + j, PosY + i, backRed, backGreen, backBlue);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 16) {
		char Buffer[16];
		uint8_t TmpChar = 0;

		GetASCIICode4(Buffer, c);
		for (i = 0; i < 16; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 8; j++) {
				if (((TmpChar >> (7 - j)) & 0x01) == 0x01) {
					Pixel(PosX + j, PosY + i, textRed, textGreen, textBlue);
				} else {
					Pixel(PosX + j, PosY + i, backRed, backGreen, backBlue);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 14) {
		char Buffer[12];
		uint16_t TmpChar = 0;

		GetASCIICode3((char*) Buffer, c);
		for (i = 0; i < 12; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 12; j++) {
				if (((TmpChar >> j) & (0x01)) == 0x01) {
					Pixel(PosX + j, PosY + i, textRed, textGreen, textBlue);
				} else {
					Pixel(PosX + j, PosY + i, backRed, backGreen, backBlue);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	if (asciisize == 24) {
		uint16_t Buffer[24];
		uint16_t TmpChar = 0;
		GetASCIICode5((char*) Buffer, c);
		for (i = 0; i < 24; i++) {
			TmpChar = Buffer[i];
			for (j = 0; j < 16; j++) {
				if (((TmpChar >> j) & (0x01)) == 0x01) {
					Pixel(PosX + j, PosY + i, textRed, textGreen, textBlue);
				} else {
					Pixel(PosX + j, PosY + i, backRed, backGreen, backBlue);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
}
void LCD_StringLine(uint16_t PosX, uint16_t PosY, char *str) {
	char TempChar;

	do {
		TempChar = *str++;
		LCD_PutChar(PosX, PosY, TempChar);
		if (PosX < 232) {
			PosX += 8;
			if (asciisize == 24) {
				PosX += 8;
			} else if (asciisize == 14) {
				PosX += 4;
			}
		}

		else if (PosY < 304) {

			PosX = 0;
			PosY += 16;
		} else {
			PosX = 0;
			PosY = 0;
		}
	} while (*str != 0);
}

void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length,
		uint8_t Direction) {
	uint32_t i = 0;

	LCD_SetCursor(Xpos, Ypos);
	if (Direction == Horizontal) {
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		for (i = 0; i < Length; i++) {
			LCD_WriteRAM(textRed, textGreen, textBlue);
		}
	} else {
		for (i = 0; i < Length; i++) {
			LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
			LCD_WriteRAM(textRed, textGreen, textBlue);
			Ypos++;
			LCD_SetCursor(Xpos, Ypos);
		}
	}

}

void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width) {
	int x, y;
	x = 0;
	y = 0;
	while (x < Height + 1) {
		PutPixel(Xpos + x, Ypos);
		PutPixel(Xpos + x, Ypos + Width);
		x++;
	}
	while (y < Width + 1) {
		PutPixel(Xpos, Ypos + y);
		PutPixel(Xpos + Height, Ypos + y);
		y++;
	}
}

void LCD_DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a) {
	int x, y;
	x = 0;
	y = 0;
	while (x < a + 1) {
		PutPixel(Xpos + x, Ypos);
		PutPixel(Xpos + x, Ypos + a);
		x++;
	}
	while (y < a + 1) {
		PutPixel(Xpos, Ypos + y);
		PutPixel(Xpos + a, Ypos + y);
		y++;
	}
}

void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius) {
	int16_t D;/* Decision Variable */
	uint16_t CurX;/* Current X Value */
	uint16_t CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;
	while (CurX <= CurY) {
		PutPixel(Xpos + CurX, Ypos + CurY);
		PutPixel(Xpos + CurX, Ypos - CurY);
		PutPixel(Xpos - CurX, Ypos + CurY);
		PutPixel(Xpos - CurX, Ypos - CurY);
		PutPixel(Xpos + CurY, Ypos + CurX);
		PutPixel(Xpos + CurY, Ypos - CurX);
		PutPixel(Xpos - CurY, Ypos + CurX);
		PutPixel(Xpos - CurY, Ypos - CurX);
		if (D < 0) {
			D += (CurX << 2) + 6;
		} else {
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius) {
	int16_t D;
	uint16_t CurX;
	uint16_t CurY;
	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY) {
		LCD_DrawUniLine(Xpos + CurX, Ypos + CurY, Xpos + CurX, Ypos - CurY);
		LCD_DrawUniLine(Xpos - CurX, Ypos + CurY, Xpos - CurX, Ypos - CurY);
		LCD_DrawUniLine(Xpos + CurY, Ypos + CurX, Xpos + CurY, Ypos - CurX);
		LCD_DrawUniLine(Xpos - CurY, Ypos + CurX, Xpos - CurY, Ypos - CurX);

		if (D < 0) {
			D += (CurX << 2) + 6;
		} else {
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		uint16_t Height) {
	int x, y;
	x = 0;
	y = 0;
	while (x < Width) {
		while (y < Height) {
			PutPixel(Xpos + x, Ypos + y);
			y++;
		}
		y = 0;
		x++;
	}
}

void LCD_DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a) {
	int x, y;
	x = 0;
	y = 0;
	while (x < a) {
		while (y < a) {
			PutPixel(Xpos + x, Ypos + y);
			y++;
		}
		y = 0;
		x++;
	}
}

void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =
			0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

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
		PutPixel(x, y); /* Draw the current pixel */
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

void LCD_Cross(u16 posX, u16 posY, u8 size) {
	u8 count = 0;
	if (posX < size || posY < size)
		return;

	LCD_SetCursor(posX - size, posY);
	LCD_WriteRAM_Prepare();
	for (count = 0; count < (size << 1) + 1; count++) {
		LCD_RAM = textRed << 8;
		LCD_RAM = textGreen << 8;
		LCD_RAM = textBlue << 8;
	}

	for (count = 0; count < (size << 1) + 1; count++) {
		LCD_SetCursor(posX, posY - size + count);
		LCD_WriteRAM_Prepare();
		LCD_RAM = textRed << 8;
		LCD_RAM = textGreen << 8;
		LCD_RAM = textBlue << 8;
	}
}

/*******************************************************************************
 * Function Name  : LCD_SetCursor
 * Description    : Sets the cursor position.
 * Input          : - Xpos: specifies the X position.
 *                  - Ypos: specifies the Y position.
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_SetCursor(u8 Xpos, u16 Ypos) {
	LCD_WriteReg(R32, Xpos);
	LCD_WriteReg(R33, Ypos);
}

/*******************************************************************************
 * Function Name  : LCD_WriteReg
 * Description    : Writes to the selected LCD register.
 * Input          : - LCD_Reg: address of the selected register.
 *                  - LCD_RegValue: value to write to the selected register.
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue) {
	LCD_REG = 0x00;
	LCD_REG = LCD_Reg << 8;
	/* Write 16-bit Reg */LCD_RAM = LCD_RegValue;
	LCD_RAM = LCD_RegValue << 8;
}

/*******************************************************************************
 * Function Name  : LCD_ReadReg
 * Description    : Reads the selected LCD Register.
 * Input          : None
 * Output         : None
 * Return         : LCD Register Value.
 *******************************************************************************/
u16 LCD_ReadReg(u8 LCD_Reg) {
	u16 val = 0;
	// Write 16-bit Index (then Read Reg) *
	LCD_REG = 0x00;
	LCD_REG = LCD_Reg << 8;
	/* Read 16-bit Reg */
	val = LCD_RAM;
	val |= (LCD_RAM >> 8);
	return val;
}

/*******************************************************************************
 * Function Name  : LCD_WriteRAM_Prepare
 * Description    : Prepare to write to the LCD RAM.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_WriteRAM_Prepare(void) {
	LCD_REG = 0x00;
	LCD_REG = R34 << 8; /* Select GRAM Reg */
}

/*******************************************************************************
 * Function Name  : LCD_WriteRAM
 * Description    : Writes to the LCD RAM.
 * Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
 * Output         : None
 * Return         : None
 *******************************************************************************/
void LCD_WriteRAM(u8 r, u8 g, u8 b) {
	LCD_RAM = r << 8;
	LCD_RAM = g << 8;
	LCD_RAM = b << 8;
}

/*******************************************************************************
 * Function Name  : LCD_ReadRAM
 * Description    : Reads the LCD RAM.
 * Input          : None
 * Output         : None
 * Return         : LCD RAM Value.
 *******************************************************************************/
u32 LCD_ReadRAM(void) {
	uint32_t val = 0;
	uint16_t low = 0;
	uint16_t high = 0;

	u8 red;
	u8 green;
	u8 blue;

	// Write 16-bit Index (then Read Reg)
	LCD_REG = 0x00;
	LCD_REG = R34 << 8; /* Select GRAM Reg */
	/* Read 16-bit Reg */
	val = LCD_RAM; //Dummy Read
	val = LCD_RAM; //Dummy Read
	high = LCD_RAM; //Read upper 8 bits
	low = LCD_RAM; //Read lower 8 bites
	val = (high & 0xFF00) | (low >> 8);
#ifdef BRG
	blue = (val >> 8) & 0xF8; //Keep five highest bits
	red = (val & 0x1F) << 3; //Keep five highest bits
#else
			red = (val >> 8) & 0xF8; //Keep five highest bits
			blue = (val & 0x1F) << 3;//Keep five highest bits
#endif
	green = (val >> 3) & 0xFC; //Keep six highest bits
	val = (red << 16) | (green << 8) | blue;
	return val;
}

/**
 *************************************************************************
 *
 * Set control lines in FSMC mode
 *
 *************************************************************************
 */
void LCD_CtrlLinesConfig() {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO D and E clocks */
	RCC_AHB1PeriphClockCmd(
			RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE,
			ENABLE);

	// Create GPIO D Init structure for used pins
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7
			| GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	/* Initialize pins */
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure GPIO D pins as FSMC alternate functions */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC); // NOE -> RD
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC); // NWE -> WR
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC); // NE1 -> CS
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC); // D13 -> D5
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC); // D14 -> D6
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC); // D15 -> D7
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC); // A16 -> RS

	/* Create GPIO E Init structure for used pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
			| GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	/* Initialize GPIOE pins */
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Configure GPIO E pins as FSMC alternate functions */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC); // D8  -> D0
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC); // D9  -> D1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC); // D10 -> D2
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC); // D11 -> D3
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC); // D12 -> D4

	/* Configure Reset Pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	/* Initialize GPIOE pins */
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	return;
}

/**
 **********************************************************************************
 *
 * Configure FSMC controller
 * Read and write timing parameters are distinct
 *
 **********************************************************************************
 */
void LCD_FSMCConfig() {
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructureRead;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructureWrite;

	/* Enable FSMC Clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* Define Read timing parameters */
	FSMC_NORSRAMTimingInitStructureRead.FSMC_AddressSetupTime = 3;
	FSMC_NORSRAMTimingInitStructureRead.FSMC_AddressHoldTime = 0;
	FSMC_NORSRAMTimingInitStructureRead.FSMC_DataSetupTime = 15;
	FSMC_NORSRAMTimingInitStructureRead.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStructureRead.FSMC_CLKDivision = 1;
	FSMC_NORSRAMTimingInitStructureRead.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStructureRead.FSMC_AccessMode = FSMC_AccessMode_A;

	/* Define Write Timing parameters */
	FSMC_NORSRAMTimingInitStructureWrite.FSMC_AddressSetupTime = 3;
	FSMC_NORSRAMTimingInitStructureWrite.FSMC_AddressHoldTime = 0;
	FSMC_NORSRAMTimingInitStructureWrite.FSMC_DataSetupTime = 6;
	FSMC_NORSRAMTimingInitStructureWrite.FSMC_BusTurnAroundDuration = 0;
	FSMC_NORSRAMTimingInitStructureWrite.FSMC_CLKDivision = 1;
	FSMC_NORSRAMTimingInitStructureWrite.FSMC_DataLatency = 0;
	FSMC_NORSRAMTimingInitStructureWrite.FSMC_AccessMode = FSMC_AccessMode_A;

	/* Define protocol type */
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1; //Bank1
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //No mux
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM; //SRAM type
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b; //16 bits wide
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =
			FSMC_BurstAccessMode_Disable; //No Burst
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait =
			FSMC_AsynchronousWait_Disable; // No wait
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity =
			FSMC_WaitSignalPolarity_Low; //Don'tcare
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable; //No wrap mode
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive =
			FSMC_WaitSignalActive_BeforeWaitState; //Don't care
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable; //Don't care
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; //Allow distinct Read/Write parameters
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; //Don't care

	// Set read timing structure
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct =
			&FSMC_NORSRAMTimingInitStructureRead;

	// Set write timing structure
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct =
			&FSMC_NORSRAMTimingInitStructureWrite;

	// Initialize FSMC for read and write
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	// Enable FSMC
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

	return;
}

void LCD_GetType(char model[]) {
	u16 regValue = 0x0000;
	const u8 zero = 0x30;
	regValue = LCD_ReadReg(0x00);
	model[0] = ((regValue >> 12) & 0x0F) + zero;
	model[1] = ((regValue >> 8) & 0x0F) + zero;
	model[2] = ((regValue >> 4) & 0x0F) + zero;
	model[3] = (regValue & 0x0F) + zero;
	model[4] = 0;
}

/**
 * Function Name  : SetBackLightLevel
 * Description    : Set back light intensity
 * Input          : Intensity level ranging from 0% to 100%
 * Output         : None
 * Return         : None
 *
 */
void LCD_SetBackLight(unsigned short level) {

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t CCR1_Val = level;
	uint16_t PrescalerValue;

	// Start Timer 1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	//Enable port E clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// Configure PE9 as output for PWM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// Connect PE9 to Timer 1 channel 1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);

	/* Compute the prescaler value
	 * Timer 1,8,9,10 and 11 are connected to APB2 which runs at SystemCoreClock
	 * Other timers are connected to APB1 and runs at SystemCoreClock/2
	 Prescaler = (TIM1CLK / TIM1 counter clock) - 1
	 Prescaler = ((SystemCoreClock) /Frequency) - 1
	 */
	PrescalerValue = (uint16_t)((48000000UL) / 100000) - 1; // TODO

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 100;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);

	/* TIM1 enable counter */
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
