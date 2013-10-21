#ifndef __ILI9325_H
#define __ILI9325_H
/* LCD color */
#define BRG
#ifdef BRG
#define BRG_Mode	   0x1000
#else
#define BRG_Mode	   0x0000
#endif

#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216

#define Horizontal     0x00
#define Vertical       0x01

/* LCD Registers */
#define R0             0x00
#define R1             0x01
#define R2             0x02
#define R3             0x03
#define R4             0x04
#define R5             0x05
#define R6             0x06
#define R7             0x07
#define R8             0x08
#define R9             0x09
#define R10            0x0A
#define R12            0x0C
#define R13            0x0D
#define R14            0x0E
#define R15            0x0F
#define R16            0x10
#define R17            0x11
#define R18            0x12
#define R19            0x13
#define R20            0x14
#define R21            0x15
#define R22            0x16
#define R23            0x17
#define R24            0x18
#define R25            0x19
#define R26            0x1A
#define R27            0x1B
#define R28            0x1C
#define R29            0x1D
#define R30            0x1E
#define R31            0x1F
#define R32            0x20
#define R33            0x21
#define R34            0x22
#define R36            0x24
#define R37            0x25
#define R40            0x28
#define R41            0x29
#define R43            0x2B
#define R45            0x2D
#define R48            0x30
#define R49            0x31
#define R50            0x32
#define R51            0x33
#define R52            0x34
#define R53            0x35
#define R54            0x36
#define R55            0x37
#define R56            0x38
#define R57            0x39
#define R59            0x3B
#define R60            0x3C
#define R61            0x3D
#define R62            0x3E
#define R63            0x3F
#define R64            0x40
#define R65            0x41
#define R66            0x42
#define R67            0x43
#define R68            0x44
#define R69            0x45
#define R70            0x46
#define R71            0x47
#define R72            0x48
#define R73            0x49
#define R74            0x4A
#define R75            0x4B
#define R76            0x4C
#define R77            0x4D
#define R78            0x4E
#define R79            0x4F
#define R80            0x50
#define R81            0x51
#define R82            0x52
#define R83            0x53
#define R96            0x60
#define R97            0x61
#define R106           0x6A
#define R118           0x76
#define R128           0x80
#define R129           0x81
#define R130           0x82
#define R131           0x83
#define R132           0x84
#define R133           0x85
#define R134           0x86
#define R135           0x87
#define R136           0x88
#define R137           0x89
#define R139           0x8B
#define R140           0x8C
#define R141           0x8D
#define R143           0x8F
#define R144           0x90
#define R145           0x91
#define R146           0x92
#define R147           0x93
#define R148           0x94
#define R149           0x95
#define R150           0x96
#define R151           0x97
#define R152           0x98
#define R153           0x99
#define R154           0x9A
#define R157           0x9D
#define R192           0xC0
#define R193           0xC1
#define R227           0xE3
#define R231           0xE7
#define R239           0xEF

#define R16_VAL		   0x1290
#define R17_VAL		   0x0007			//(0>0.95 1>0.90 2>0.85 3>0.80 4>0.75 5>0.70 6>Off 7>1.0
#define R18_VAL        0x0090 | 0XE		// 8->F >> 1.60->1.95        1->7 > 2.0->2.40
#define R19_VAL		   0x0F00   		//F00
#define R41_VAL        0x0005

/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void LCD_Init(void);

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u8 r, u8 g, u8 b);
u32 LCD_ReadRAM(void);
void LCD_Disp_Image(unsigned short *gImage_ptr);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(u8 b, u8 g, u8 r);
void LCD_SetCursor(u8 Xpos, u16 Ypos);
void LCD_SetColors(u8 text_r, u8 text_g, u8 text_b, u8 back_r, u8 back_g,
		u8 back_b);
void LCD_GetColors(u8 *text_r, u8 *text_g, u8 *text_b, u8 *back_r, u8 *back_g,
		u8 *back_b);
void LCD_SetTextColor(u8 red, u8 green, u8 blue);
void LCD_SetBackColor(u8 red, u8 green, u8 blue);
void LCD_CharSize(__IO uint16_t size);
void PutPixel(int16_t x, int16_t y);
void Pixel(int16_t x, int16_t y, u8 r, u8 g, u8 b);
void LCD_PutChar(int16_t PosX, int16_t PosY, char c);
void LCD_StringLine(uint16_t PosX, uint16_t PosY, char *str);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length,
		uint8_t Direction);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
void LCD_DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		uint16_t Height);
void LCD_DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_GetType(char model[]);
void LCD_Cross(u16 posX, u16 posY, u8 size);
void LCD_SetBackLight(unsigned short level);
#endif
