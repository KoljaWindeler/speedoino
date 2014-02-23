#include "global.h"

uart::uart(){
	tail1=0;
	head1=0;
	//	head2=0;
	//	tail2=0;
	head3=0;
	tail3=0;
	head4=0;
	tail4=0;
};


void uart::init(USART_TypeDef* USARTx,uint32_t baudrate){
	if(USARTx==USART1){
		/* This is a concept that has to do with the libraries provided by ST
		 * to make development easier the have made up something similar to
		 * classes, called TypeDefs, which actually just define the common
		 * parameters that every peripheral needs to work correctly
		 *
		 * They make our life easier because we don't have to mess around with
		 * the low level stuff of setting bits in the correct registers
		 */
		GPIO_InitTypeDef  GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
		USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
		NVIC_InitTypeDef  NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

		/* enable APB2 peripheral clock for USART1
		 * note that only USART1 and USART6 are connected to APB2
		 * the other USARTs are connected to APB1
		 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		/* enable the peripheral clock for the pins used by
		 * USART1, PA9 for TX and PA10 for RX
		 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* This sequence sets up the TX and RX pins
		 * so they work correctly with the USART1 peripheral
		 */
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 	// Pins 9 (TX) and 10 (RX) are used
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 				// the pins are configured as alternate function so the USART peripheral has access to them
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;			// this defines the IO speed and has nothing to do with the baudrate!
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;				// this defines the output type as push pull mode (as opposed to open drain)
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;				// this activates the pullup resistors on the IO pins
		GPIO_Init(GPIOA, &GPIO_InitStruct);						// now all the values are passed to the GPIO_Init() function which sets the GPIO registers

	    USART_OverSampling8Cmd(USART1, ENABLE);
		/* The RX and TX pins are now connected to their AF
		 * so that the USART1 can take over control of the
		 * pins
		 */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

		/* Now the USART_InitStruct is used to define the
		 * properties of USART1
		 */
		USART_InitStruct.USART_BaudRate = baudrate;				// the baudrate is set to the value we passed into this init function
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
		USART_InitStruct.USART_StopBits = USART_StopBits_1;		// we want 1 stop bit (standard)
		USART_InitStruct.USART_Parity = USART_Parity_No;		// we don't want a parity bit (standard)
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
		USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
		USART_Init(USART1, &USART_InitStruct);					// again all the properties are passed to the USART_Init function which takes care of all the bit setting


		/* Here the USART1 receive interrupt is enabled
		 * and the interrupt controller is configured
		 * to jump to the USART1_IRQHandler() function
		 * if the USART1 receive interrupt occurs
		 */
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 // we want to configure the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 // this sets the subpriority inside the group
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART1 interrupts are globally enabled
		NVIC_Init(&NVIC_InitStructure);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff


	} else if(USARTx==USART2){ // used for GPS
		/* This is a concept that has to do with the libraries provided by ST
		 * to make development easier the have made up something similar to
		 * classes, called TypeDefs, which actually just define the common
		 * parameters that every peripheral needs to work correctly
		 *
		 * They make our life easier because we don't have to mess around with
		 * the low level stuff of setting bits in the correct registers
		 */
		NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		/* GPIOA clock enable */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* GPIOA Configuration:  USART2 TX on PA2 */
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Connect USART2 pins to AF2 */
		// TX = PA2, RX=PA3
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

		USART_InitTypeDef USART_InitStructure; // this is for the USART3 initilization
		USART_InitStructure.USART_BaudRate = 19600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);

		/* Here the USART2 receive interrupt is enabled  and the interrupt controller is configured
		 * to jump to the USART2_IRQHandler() function if the USART1 receive interrupt occurs  */
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt

		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		 	// we want to configure the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0d;// this sets the priority group of the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 	// this sets the subpriority inside the group
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// the USART1 interrupts are globally enabled
		NVIC_Init(&NVIC_InitStructure);								// the properties are passed to the NVIC_Init function which takes care of the low level stuff

	} else if(USARTx==USART3){ // inter controller comm
		//PB10/11
		USART_InitTypeDef USART_InitStructure; // this is for the USART3 initilization
		USART_InitStructure.USART_BaudRate = baudrate;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		//configure clock for USART
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		//configure clock for GPIO
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		//configure AF
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);

		//configure ports, &GPIO_InitStructure);
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);


		USART_Init(USART3, &USART_InitStructure);

		/* Enable USART */
		USART_Cmd(USART3, ENABLE);

	} else if(USARTx==UART4){
		/* This is a concept that has to do with the libraries provided by ST
		 * to make development easier the have made up something similar to
		 * classes, called TypeDefs, which actually just define the common
		 * parameters that every peripheral needs to work correctly
		 *
		 * They make our life easier because we don't have to mess around with
		 * the low level stuff of setting bits in the correct registers
		 */
		GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
		USART_InitTypeDef USART_InitStruct; // this is for the USART3 initilization
		NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

		RCC_APB2PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

		/* enable the peripheral clock for the pins used by USART3, PC10 for TX and PC11 for RX */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		/* This sequence sets up the TX and RX pins so they work correctly with the USART1 peripheral */
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // Pins 10 (TX) and 11 (RX) are used
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 			// the pins are configured as alternate function so the USART peripheral has access to them
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// this defines the IO speed and has nothing to do with the baudrate!
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;			// this defines the output type as push pull mode (as opposed to open drain)
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// this activates the pullup resistors on the IO pins
		GPIO_Init(GPIOC, &GPIO_InitStruct);					// now all the values are passed to the GPIO_Init() function which sets the GPIO registers

		/* The RX and TX pins are now connected to their AF so that the USART3 can take over control of the pins */
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4); //
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

		/* Now the USART_InitStruct is used to define the
		 * properties of USART1
		 */
		USART_InitStruct.USART_BaudRate = baudrate;				// the baudrate is set to the value we passed into this init function
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
		USART_InitStruct.USART_StopBits = USART_StopBits_1;		// we want 1 stop bit (standard)
		USART_InitStruct.USART_Parity = USART_Parity_No;		// we don't want a parity bit (standard)
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
		USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
		USART_Init(UART4, &USART_InitStruct);					// again all the properties are passed to the USART_Init function which takes care of all the bit setting


		/* Here the USART4 receive interrupt is enabled  and the interrupt controller is configured
		 * to jump to the USART4_IRQHandler() function if the USART1 receive interrupt occurs  */
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // enable the USART4 receive interrupt

		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		 	// we want to configure the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0d;// this sets the priority group of the USART1 interrupts
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 	// this sets the subpriority inside the group
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// the USART1 interrupts are globally enabled
		NVIC_Init(&NVIC_InitStructure);								// the properties are passed to the NVIC_Init function which takes care of the low level stuff
	}
	// finally this enables the complete USART1 peripheral
	USART_Cmd(USARTx, ENABLE);
}

/* This function is used to transmit a string of characters via
 * the USART specified in USARTx.
 *
 * It takes two arguments: USARTx --> can be any of the USARTs e.g. USART1, USART2 etc.
 * 						   (volatile) char *s is the string you want to send
 *
 * Note: The string has to be passed to the function as a pointer because
 * 		 the compiler doesn't know the 'string' data type. In standard
 * 		 C a string is just an array of characters
 *
 * Note 2: At the moment it takes a volatile char because the received_string variable
 * 		   declared as volatile char --> otherwise the compiler will spit out warnings
 * */
void uart::puts(USART_TypeDef* USARTx, char *s){
	while(*s){
		// wait until data register is empty
		USART_SendData(USARTx, *s);
		s++;
	}
}

void uart::puts(USART_TypeDef* USARTx, char s){
	// wait until data register is empty
	USART_SendData(USARTx, s);
}

void uart::puts(USART_TypeDef* USARTx, int s){
	char buffer[8];
	sprintf(buffer,"%i",s);
	puts(USARTx,buffer);
}

void uart::puts_ln(USART_TypeDef* USARTx, char *s){
	while(*s){
		// wait until data register is empty
		USART_SendData(USARTx, *s);
		s++;
	}
	USART_SendData(USARTx, '\r');
	USART_SendData(USARTx, '\n');
}

void uart::puts_ln(USART_TypeDef* USARTx, int s){
	char buffer[8];
	sprintf(buffer,"%i",s);
	puts_ln(USARTx,buffer);
}

void uart::recv(USART_TypeDef* USARTx,char t){
	if(USARTx==USART1){
		int i = (head1 + 1) % MAX_STRLEN;

		// if we should be storing the received character into the location
		// just before the tail (meaning that the head would advance to the
		// current location of the tail), we're about to overflow the buffer
		// and so we don't write the character or advance the head.
		if (i != tail1) {
			buffer1[head1] = t;
			head1 = i;
		}

		//		static uint8_t cnt = 0; // this counter is used to determine the string length
		//		/* check if the received character is not the LF character (used to determine end of string)
		//		 * or the if the maximum string length has been been reached
		//		 */
		//		if( (t != 'n') && (cnt < MAX_STRLEN-1) ){
		//			received_string[cnt] = t;
		//			cnt++;
		//			received_string[cnt] = 0x00;
		//		}
		//		else{ // otherwise reset the character counter and print the received string
		//			cnt = 0;
		//			puts(USART1, received_string);
		//		}
		//	} else if(USARTx==USART2){
		//		int i = (head2 + 1) % MAX_STRLEN;
		//		if (i != tail2) {
		//			buffer2[head2] = t;
		//			head2 = i;
		//		}
	} else if(USARTx==USART3){
		int i = (head3 + 1) % MAX_STRLEN;
		if (i != tail3) {
			buffer3[head3] = t;
			head3 = i;
		}
	} else if(USARTx==UART4){
		int i = (head4 + 1) % MAX_STRLEN;
		if (i != tail4) {
			buffer4[head4] = t;
			head4 = i;
		}
	}
}

int uart::available(USART_TypeDef* USARTx){
	if(USARTx==USART1){
		return (MAX_STRLEN + head1 - tail1) % MAX_STRLEN;
		//	} else if(USARTx==USART2){
		//		return (MAX_STRLEN + head2 - tail2) % MAX_STRLEN;
	} else if(USARTx==USART3){
		return (MAX_STRLEN + head3 - tail3) % MAX_STRLEN;
	} else if(USARTx==UART4){
		return (MAX_STRLEN + head4 - tail4) % MAX_STRLEN;
	} else {
		return -1;
	}
}

void uart::flush(USART_TypeDef* USARTx){
	if(USARTx==USART1){
		head1=0;
		tail1=0;
		//	} else if(USARTx==USART2){
		//		return (MAX_STRLEN + head2 - tail2) % MAX_STRLEN;
	} else if(USARTx==USART3){
		head3=0;
		tail3=0;
	} else if(USARTx==UART4){
		head4=0;
		tail4=0;
	}
}


int uart::read(USART_TypeDef* USARTx){
	if(USARTx==USART1){
		// if the head isn't ahead of the tail, we don't have any characters
		if (head1 == tail1) {
			return -1;
		} else {
			unsigned char c = buffer1[tail1];
			tail1 = (tail1 + 1) % MAX_STRLEN;
			return c;
		}
		//	} else if(USARTx==USART2){
		//		// if the head isn't ahead of the tail, we don't have any characters
		//		if (head2 == tail2) {
		//			return -1;
		//		} else {
		//			unsigned char c = buffer2[tail2];
		//			tail2 = (tail2 + 1) % MAX_STRLEN;
		//			return c;
		//		}
	} else if(USARTx==UART4){
		// if the head isn't ahead of the tail, we don't have any characters
		if (head4 == tail4) {
			return -1;
		} else {
			unsigned char c = buffer4[tail4];
			tail4 = (tail4 + 1) % MAX_STRLEN;
			return c;
		}
	} else {
		return -1;
	}
}

bool uart::getS(USART_TypeDef* USARTx,char* buffer){
	if(available(USARTx)>0){
		for(int i=0;available(USARTx)>0;i++){
			buffer[i]=read(USARTx);
			buffer[i+1]=0;
		}
		return true;
	}
	return false;
}


