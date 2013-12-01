#include "global.h"
/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}



/**
  * @brief  This function handles DMA2 Stream3 or DMA2 Stream6 global interrupts
  *         requests.
  * @param  None
  * @retval None
  */
//void SD_SDIO_DMA_IRQHANDLER(void)
//{
//  /* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
//  SD_ProcessDMAIRQ();
//}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/******************************************************************************/
/**
  * @brief  This function handles SDIO global interrupt request.
  * @param  None
  * @retval None
  */
//extern "C" void SDIO_IRQHandler(void)
//{
//  /* Process All SDIO Interrupt Sources */
//  SD_ProcessIRQSrc();
//}


// aka Millis.get()
extern "C" void TIM7_IRQHandler(){
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	Millis.inc();
}

// aka rpm()
extern "C" void TIM3_IRQHandler(){
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		Sensors.mRpm.overflow();

		//////////// TODO: remove me, I'm DEBUG //////////
		//				char buffer[50];
		//				sprintf(buffer,"->O<-\r\n");
		//				Serial.puts(USART1, buffer);
		//////////// TODO: remove me, I'm DEBUG //////////
	} else if (TIM_GetITStatus(TIM3,TIM_IT_CC1) != RESET){
		uint32_t new_timerValue=TIM_GetCapture1(TIM3);
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		// timer runs with 2 MHz
		// if the engine runs with 3.000 rpm we should have 3000/60*2(double ignition)=100 pulses per second
		// that leads to 10ms per Pulse. Timer1 value will be at 20.000 after 10ms.

		// calc timer counts between this and the last pulse ... including overflows
		uint32_t delta_timerValue=0;
		if(Sensors.mRpm.overflows>0){ // check if we have at least one overflow
			delta_timerValue=Sensors.mRpm.overflows<<16; // preload with n*65536
		}
		delta_timerValue+=(new_timerValue-Sensors.mRpm.old_timerValue);

		// store value
		Sensors.mRpm.set_exact((uint16_t)((uint32_t)60000000UL / delta_timerValue));

		// maintaine temp vars
		Sensors.mRpm.overflows=0;
		Sensors.mRpm.old_timerValue=new_timerValue;

		//////////// TODO: remove me, I'm DEBUG //////////
		//				char buffer[50];
		//				sprintf(buffer,"--> %8lu,%4lu,%4i <--\r\n",Millis.get(),TIM_GetCounter(TIM3),Sensors.mRpm.get_exact());
		//				Serial.puts(USART1, buffer);
		//////////// TODO: remove me, I'm DEBUG //////////
	}
}


// aka serial1
extern "C" void USART1_IRQHandler(void){
	// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
		// the character from the USART1 data register is saved in t
		Serial.recv(USART1,USART1->DR);
	}
}

// aka serial2
extern "C" void USART2_IRQHandler(void){
	// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART2, USART_IT_RXNE) ){
		// the character from the USART1 data register is saved in t
		//		Serial.recv(USART2,USART2->DR);
		Sensors.mGPS.recv_data(USART2->DR);
	}

}

extern "C" void UART4_IRQHandler(void){
	// check if the USART3 receive interrupt flag was set
	if( USART_GetITStatus(UART4, USART_IT_RXNE) ){
		// the character from the USART1 data register is saved in t
		Serial.recv(UART4,UART4->DR);
	}
}

// buttons on D12..D15
extern "C" void EXTI15_10_IRQHandler(void){
	EXTI_ClearITPendingBit(EXTI_Line15);
	EXTI_ClearITPendingBit(EXTI_Line14);
	EXTI_ClearITPendingBit(EXTI_Line13);
	EXTI_ClearITPendingBit(EXTI_Line12);
	Menu.button_test(false,true);
}

//// aka PORTA PIN0 // todo: if timer capture runs, this isn't used anymore
//extern "C" void EXTI0_IRQHandler(void){
//	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
//		// wenn Interrupt aufgetreten
//		EXTI_ClearITPendingBit(EXTI_Line0);
//		uint16_t timerValue=TIM_GetCounter(TIM3);
//		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)!=Bit_RESET && timerValue>10000) {
//			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
//
//			TIM_Cmd(TIM3, DISABLE);
//			TIM_SetCounter(TIM3,0);
//			TIM_Cmd(TIM3, ENABLE);
//
//			// timer runs with 2 MHz
//			// if the engine runs with 3.000 rpm we should have 3000/60*2(double ignition)=100 pulses per second
//			// that leads to 10ms per Pulse. Timer1 value will be at 20.000 after 10ms.
//			Sensors.mRpm.set_exact((uint32_t)60000000UL / (((uint32_t)Sensors.mRpm.overflows<<16) + timerValue));
//			Sensors.mRpm.overflows=0;
//
//			//	char buffer[50];
//			//	sprintf(buffer,"--> %8lu,%8lu,%8i <--\r\n",Millis.get(),TIM_GetCounter(TIM3),Sensors.mRpm.get_exact());
//			//	Serial.puts(USART1, buffer);
//		}
//	}
//}
