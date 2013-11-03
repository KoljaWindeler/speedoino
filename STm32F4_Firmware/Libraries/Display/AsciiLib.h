#ifndef __AsciiLib_H
#define __AsciiLib_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stdint.h"

/* Private function prototypes -----------------------------------------------*/
void GetASCIICode1(char* pBuffer, char ASCII);
void GetASCIICode2(char* pBuffer, char ASCII);
void GetASCIICode3(char * pBuffer, char ASCII);

void GetASCIICode4(char* pBuffer, char ASCII);
void GetASCIICode5(char * pBuffer, char ASCII);

extern char digitToAscii[16];
extern void getAsciiFromShort(char * buffer, unsigned short value);
extern void getDecimalFromShort(char* buffer, unsigned short value);
#endif

