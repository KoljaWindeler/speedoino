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

#define   GRAPHIC_USE_JPG     0   // Jpg-Funktionen

//--------------------------------------------------------------
// Struktur von einem Image
//--------------------------------------------------------------
typedef struct UB_Image_t
{
	const uint16_t *table; // Tabelle mit den Daten
	uint16_t width;        // Breite des Bildes (in Pixel)
	uint16_t height;       // Hoehe des Bildes  (in Pixel)
}UB_Image;



//--------------------------------------------------------------
// Struktur von einem Picture
//--------------------------------------------------------------
typedef struct UB_Picture_t
{
	const uint8_t *table; // Tabelle mit den Daten
	uint32_t size;        // Anzahl der Bytes
}UB_Picture;


//--------------------------------------------------------------
// Return Werte beim zeichnen
//--------------------------------------------------------------
typedef enum {
	GRAPHIC_OK =0,
	GRAPHIC_FILE_ERR,
	GRAPHIC_SIZE_ERR,
	GRAPHIC_ID_ERR,
	GRAPHIC_HEAD_ERR,
	GRAPHIC_WIDTH_ERR,
	GRAPHIC_HEIGHT_ERR,
	GRAPHIC_BPP_ERR,
	GRAPHIC_COMPR_ERR
}GRAPHIC_ERR_t;


//--------------------------------------------------------------
// Typedef für Windows-Jpg-File
//--------------------------------------------------------------
typedef uint8_t       BYTE;
typedef uint16_t      WORD;
typedef uint32_t      DWORD;



//--------------------------------------------------------------
// Defines für Windows-Jpg-File
//--------------------------------------------------------------
#define M_SOF0  0xc0
#define M_DHT   0xc4
#define M_EOI   0xd9
#define M_SOS   0xda
#define M_DQT   0xdb
#define M_DRI   0xdd
#define M_APP0  0xe0

#define W1 2841
#define W2 2676
#define W3 2408
#define W5 1609
#define W6 1108
#define W7 565

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))

#define FUNC_OK 0
#define FUNC_FORMAT_ERROR 3



//--------------------------------------------------------------
// Struktur von einem Windows-Jpg-File
//--------------------------------------------------------------
typedef struct{
	long CurX;
	long CurY;
	DWORD ImgWidth;
	DWORD ImgHeight;
	short SampRate_Y_H;
	short SampRate_Y_V;
	short SampRate_U_H;
	short SampRate_U_V;
	short SampRate_V_H;
	short SampRate_V_V;
	short H_YtoU;
	short V_YtoU;
	short H_YtoV;
	short V_YtoV;
	short Y_in_MCU;
	short U_in_MCU;
	short V_in_MCU;  // notwendig ??
	unsigned char *lpJpegBuf;
	unsigned char *lp;
	short qt_table[3][64];
	short comp_num;
	BYTE comp_index[3];
	BYTE YDcIndex;
	BYTE YAcIndex;
	BYTE UVDcIndex;
	BYTE UVAcIndex;
	BYTE HufTabIndex;
	short *YQtTable;
	short *UQtTable;
	short *VQtTable;
	short code_pos_table[4][16];
	short code_len_table[4][16];
	unsigned short code_value_table[4][256];
	unsigned short huf_max_value[4][16];
	unsigned short huf_min_value[4][16];
	short BitPos;
	short CurByte;
	short rrun;
	short vvalue;
	short MCUBuffer[10*64];
	short QtZzMCUBuffer[10*64];
	short BlockBuffer[64];
	short ycoef;
	short ucoef;
	short vcoef;
	BYTE IntervalFlag;
	short interval;
	short Y[4*64];
	short U[4*64];
	short V[4*64];
	DWORD sizei;
	DWORD sizej;
	short restart;
	long iclip[1024];
	long *iclp;
}GRAPHIC_JPG_t;



class ILI9325 {

private:
	void CtrlLinesConfig();
	void FSMCConfig();

	//--------------------------------------------------------------
	// Globale Funktionen
	//--------------------------------------------------------------

	int16_t P_Graphic_sgn(int16_t x);
	int InitTag(void);
	void InitTable(void);
	int Decode(void);
	void GetYUV(short flag);
	void StoreBuffer(void);
	int DecodeMCUBlock(void);
	int HufBlock(BYTE dchufindex,BYTE achufindex);
	int DecodeElement(void);
	void IQtIZzMCUComponent(short flag);
	void IQtIZzBlock(short  *s ,short * d,short flag);
	void Fast_IDCT(int * block);
	BYTE ReadByte(void);
	void Initialize_Fast_IDCT(void);
	void idctrow(int * blk);
	void idctcol(int * blk);

	GRAPHIC_JPG_t GRAPHIC_JPG;

public:
	/* Exported functions ------------------------------------------------------- */
	ILI9325();
	~ILI9325();
	/*----- High layer function -----*/
	void init(void);

	void WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
	u16 ReadReg(u8 LCD_Reg);
	void WriteRAM_Prepare(void);
	void WriteRAM(u8 r, u8 g, u8 b);
	u32 ReadRAM(void);
	void Disp_Image(unsigned short *gImage_ptr);
	void setRotation(uint8_t x);
	void DisplayOn(void);
	void DisplayOff(void);
	void clear_screen();
	void clear_screen(u8 b, u8 g, u8 r);
	void SetCursor(u8 Xpos, u16 Ypos);
	void SetRotatedCursor(u16 Xpos, u8 Ypos);
	void SetColors(u8 text_r, u8 text_g, u8 text_b, u8 back_r, u8 back_g,u8 back_b);
	void GetColors(u8 *text_r, u8 *text_g, u8 *text_b, u8 *back_r, u8 *back_g,u8 *back_b);
	void SetTextColor(u8 red, u8 green, u8 blue);
	void SetBackColor(u8 red, u8 green, u8 blue);
	void CharSize(__IO uint16_t size);
	void PutPixel(int16_t x, int16_t y);
	void Pixel(int16_t x, int16_t y, u8 r, u8 g, u8 b);
	void PutChar(int16_t PosX, int16_t PosY, char c);

	void zeichen_small_1x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_2x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_3x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_4x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_5x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_6x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_7x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_8x(const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);
	void zeichen_small_scale(uint8_t scale,const uint8_t *font,uint8_t z, uint16_t spalte, uint16_t zeile, uint8_t offset);

	void string(char *str,uint8_t spalte, uint8_t zeile);
	void string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile);
	void string(char *str,uint8_t spalte, uint8_t zeile, uint8_t back, uint8_t text);
	void string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile, uint8_t back, uint8_t text, uint8_t offset);
	void string(uint8_t font,char *str,uint8_t spalte, uint8_t zeile, uint8_t text_r, uint8_t text_g, uint8_t text_b, uint8_t back_r, uint8_t back_g, uint8_t back_b, uint8_t offset);


	void DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length,uint8_t Direction);
	void DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
	void DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
	void DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
	void DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
	void DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,uint16_t Height);
	void DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
	void DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void GetType(char model[]);
	void Cross(u16 posX, u16 posY, u8 size);
	void SetBackLight(unsigned short level);
	void show_storry(const char* storry,const char* title);
	void show_storry(const char* storry,unsigned int storry_length,char title[],unsigned int title_length, uint8_t type);
	void show_storry(const char* storry,unsigned int storry_length,char title[],unsigned int title_length);
	void show_storry(const char* storry,const char* title, uint8_t type);
	void string_centered(const char* text, uint8_t line);
	void string_centered(const char* text, uint8_t line, bool inverted);
	void highlight_bar(uint16_t x,uint16_t y,uint16_t width,uint16_t height);
	int animation(int a);

	void draw_oil(uint16_t x,uint16_t y);
	void draw_water(uint16_t x,uint16_t y);
	void draw_air(uint16_t x,uint16_t y);
	void draw_fuel(uint16_t x,uint16_t y);
	void draw_clock(uint16_t x,uint16_t y);
	void draw_gps(uint16_t x,uint16_t y, unsigned char sats);
	void draw_blitzer(uint16_t x,uint16_t y);
	void draw_arrow(int arrow, int spalte, int zeile, uint8_t r, uint8_t g, uint8_t b);
	void draw_arrow(int angle, int x_pos, int y_pos, uint8_t r, uint8_t g, uint8_t b,bool clean);
	uint8_t draw_bmp(uint16_t x, uint16_t y, uint8_t* filename);
	GRAPHIC_ERR_t UB_Graphic_DrawJpg(UB_Picture *jpg, uint16_t xpos, uint16_t ypos);

	void filled_rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,unsigned char color);
	void filled_rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t r,uint8_t g,uint8_t b);
	void glow(int16_t x_start,int16_t y_start,int16_t xend,int16_t y_end,uint8_t r,uint8_t g,uint8_t b,uint8_t loss);
	void check_coordinates(int16_t* x,int16_t* y);

	unsigned char startup[35]; // asdfghjk.asd,1234,1234,1234\0 == 28
	//////////////////////












};
//extern ILI9325 TFT;
#endif
