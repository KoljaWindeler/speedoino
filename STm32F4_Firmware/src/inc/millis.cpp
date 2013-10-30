#include "global.h"

timing::timing(){
	millis_value=0;
}

void timing::init(){
	// Takt für Timer 7 einschalten
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	// Timer7 konfigurieren
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 100;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 839; // warum ist das nicht 840?
	TIM_TimeBaseInit(TIM7, &TIM_TimeBase_InitStructure);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	// Timer7 einschalten
	TIM_Cmd(TIM7, ENABLE);

	// Interruptcontroller konfigurieren
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_Init(&NVIC_InitStructure);
}


void timing::inc(){
	millis_value++;
}

uint32_t timing::get(){
	uint32_t temp=millis_value;
	return temp;
};



