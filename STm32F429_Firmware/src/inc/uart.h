/*
 * uart.h
 *
 *  Created on: 21.10.2013
 *      Author: jkw
 */

#ifndef UART_H_
#define UART_H_

class uart {
#define MAX_STRLEN 128

public:
	uart();
	void init(USART_TypeDef* USARTx,uint32_t baudrate);
	void puts(USART_TypeDef* USARTx, char *s);
	void puts(USART_TypeDef* USARTx, int s);
	void puts(USART_TypeDef* USARTx, char s);
	void puts_ln(USART_TypeDef* USARTx, char *s);
	void puts_ln(USART_TypeDef* USARTx, int s);
	void recv(USART_TypeDef* USARTx,char t);
	int available(USART_TypeDef* USARTx);
	void flush(USART_TypeDef* USARTx);
	int read(USART_TypeDef* USARTx);
	bool getS(USART_TypeDef* USARTx,char* buffer);
private:
	unsigned char buffer1[MAX_STRLEN];
	int head1;
	int tail1;

	//	unsigned char buffer2[MAX_STRLEN];	// GPS has its own buffer
	//	int head2;
	//	int tail2;

	unsigned char buffer3[MAX_STRLEN];
	int head3;
	int tail3;

	unsigned char buffer4[MAX_STRLEN];
	int head4;
	int tail4;
};
extern uart Serial;


#endif /* UART_H_ */
