/*
 * Rpm.cpp
 *
 *  Created on: 25.10.2013
 *      Author: jkw
 */

#include <global.h>

Rpm::Rpm() {
	// TODO Auto-generated constructor stub

}

Rpm::~Rpm() {
	// TODO Auto-generated destructor stub
}

uint16_t Rpm::check_vars(){
	if(blitz_dz==0){
		Serial.puts_ln(USART1,"DZ failed");
		blitz_dz=12500; // hornet maessig
		blitz_en=true; // gehen wir mal von "an" aus
		return 1;
	}
	return 0;
};

void Rpm::init(){
	// Clock enable (GPIO)
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Config als Digital-Eingang
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3) ;


	/* Enable the TIM3 global Interrupt */
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM3 ch1 configuration: Input Capture mode ---------------------
	     The external signal is connected to TIM3 CH1 pin (PA0)
	     The Rising edge is used as active edge,
	     The TIM3 CCR? is used to compute the frequency value
	  ------------------------------------------------------------ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0xf;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_PrescalerConfig(TIM3, 41, TIM_PSCReloadMode_Immediate); // 2MHz


	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);

	/* Enable the CompareChannel1 Interrupt Request */
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	overflows=0;
	old_timerValue=0;

	Serial.puts_ln(USART1,"RPM init done");
}

void Rpm::overflow(){
	overflows++;
	if(overflows>4){ // no spark for 164ms -> less than 183 rpm
		set_exact(0);
		overflows=0;
		old_timerValue=0;
	}
}

void Rpm::set_exact(int16_t i){
	if(i>15000){
		exact=15000;
	} else if(i==0) {
		exact=0;
	} else {
		/* PID - Track
		 * kp=1/4
		 * kd=1/2
		 * ki=1/256
		 * Ta=4
		 * --------------------
		 * esum=esum+differ
		 * regler_P=int(kp*differ)
		 * regler_I=int(ki*Ta*esum)
		 * regler_D=int(kd*(differ-ealt)/Ta)
		 * ealt=differ
		 * --------------------
		 * P=1/4*differ                = differ>>2
		 * I=(1/256)*4*esum            = 1/64*esum = e_sum>>6
		 * D=(1/2)/4*(differ-ealt)    = 1/8*(differ-ealt) = (differ-ealt)>>3
		 */
		int16_t differ=i-exact;
		e_sum+=differ;
		//exact+=(differ>>2)+(e_sum>>6)+((differ-e_old)>>3); // great!! 10.10.2013
		exact+=(differ>>3)+(e_sum>>7)+((differ-e_old)>>4); // since the above was very fast and a bit bouncy ... lets try to low pass it even more
		e_old=differ;
	}
}

uint16_t Rpm::get_exact(){
	return exact;
}

void Rpm::shutdown(){

};
