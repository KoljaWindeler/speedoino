#include "spi.h"

void SPI_init(SPI_TypeDef* SPIx, unsigned int prescaler)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// enable clock for used IO pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* configure pins used by SPI1
	 * PA5 = SCK
	 * PA6 = MISO
	 * PA7 = MOSI
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	// enable peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* configure SPI1 in Mode 0 
	 * CPOL = 0 --> clock is low when idle
	 * CPHA = 0 --> data is sampled at the first edge
	 */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = prescaler; // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
	SPI_Init(SPI1, &SPI_InitStruct); 
	
	SPI_Cmd(SPI1, ENABLE); // enable SPI1
}

void SPI_send_single(SPI_TypeDef* SPIx, unsigned char data)
{
	unsigned char tmp;
	SPIx->DR = data; // write data to be transmitted to the SPI data register
	while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	tmp = SPIx->DR; // return received data from SPI data register	
}

unsigned char SPI_receive_single(SPI_TypeDef* SPIx)
{
	SPI1->DR = 0xFF; // write data to be transmitted to the SPI data register
	while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(SPI1->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return SPI1->DR; // return received data from SPI data register
}

void SPI_send(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length)
{
	while (length--)
	{
		SPI_send_single(SPIx, *data);
		data++;
	}
}

void SPI_receive(SPI_TypeDef* SPIx, unsigned char* data, unsigned int length)
{
	while (length--)
	{
		*data = SPI_receive_single(SPIx);
		data++;
	}
}

void SPI_transmit(SPI_TypeDef* SPIx, unsigned char* txbuf, unsigned char* rxbuf, unsigned int len)
{
	while (len--)
	{
		SPIx->DR = *txbuf; // write data to be transmitted to the SPI data register
		while( !(SPIx->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
		while( !(SPIx->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
		while( SPIx->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
		*rxbuf = SPIx->DR; // return received data from SPI data register
		txbuf++;
		rxbuf++;	 
	}
}
