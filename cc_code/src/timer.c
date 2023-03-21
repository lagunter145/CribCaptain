/*
 * timer.c
 *
 *  Created on: Feb 17, 2023
 *      Author: mrahf
 */

#include "stm32f0xx.h"
#include "esp.h"
#include "misc.h"


//
void setup_external_timesync() {


	//Enable SYSCFG bit in RCC register
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

	//Enable RCC clocks to GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	//Configure PC15  as Input
	GPIOC->MODER &= ~GPIO_MODER_MODER0;

	//sets external interrupt for pin 15 to gpio c
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC;

	//rising edge trigger enabled for pin 0
	EXTI->RTSR |= EXTI_RTSR_TR0;
	//disable falling edge trigger
	EXTI->FTSR &= ~EXTI_FTSR_TR0;

	//make the interrupt not masked
	EXTI->IMR |= EXTI_IMR_MR0;

	//enable external interrupt on pin 0
	NVIC->ISER[0] = (1 << EXTI0_1_IRQn);

	/*


	//Enable SYSCFG bit in RCC register
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	//Enable RCC clocks to GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	//Configure PC15  as Input
	GPIOC->MODER &= ~GPIO_MODER_MODER15;

	//sets external interrupt for pin 15 to gpio c
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI15;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PC;

	//rising edge trigger enabled for pin 0
	EXTI->RTSR |= EXTI_RTSR_TR15;
	//disable falling edge trigger
	EXTI->FTSR &= ~EXTI_FTSR_TR15;

	//make the interrupt not masked
	EXTI->IMR |= EXTI_IMR_MR15;

	//enable external interrupt on pin 0
	NVIC->ISER[0] = (1 << EXTI4_15_IRQn);

	*/


}



//Default timer takes half a second
void setup_tim6() {
	//Enable RCC clock to Timer 6
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	//update the TIM6 prescaler
	TIM6->PSC = 48000 - 1; // 48,000,000 / (47 + 1) = 1 kHz

	//Configure the auto-reload register
	TIM6->ARR = 500;

	//configure the Timer 6 DMA/Interrupt Enable Register
	//(enable the UIE falg)
	TIM6->DIER |= TIM_DIER_UIE;

	//Enable TIM6 by setting the CEN bit in
	//the TIM6 control register1
	TIM6->CR1 |= TIM_CR1_CEN;
	//TIM6->CR2 |= 0x20;

	//Enable the interrupt for Timer 6 in the NVIC ISER
	NVIC->ISER[0] = (1 << TIM6_DAC_IRQn);
}

volatile int state = 0;

//will update the timers configuration to have a delay of ms
// in milliseconds
void tim6_changeTimer(int ms) {
	//Disable the TIM6 by reseting the CEN bit in CR1
	TIM6->CR1 &= ~TIM_CR1_CEN;
	//set the new auto-reload register
	TIM6->ARR = ms;
	//reset the TIM6 counter
	TIM6->CNT &= ~TIM_CNT_CNT;
	//Enable TIM6 by setting the CEN bit in CR1
	TIM6->CR1 |= TIM_CR1_CEN;

}

volatile int counter = 0;

//Deals with the external interrupt for the Timing Synchronization
void EXTI4_15_IRQHandler(void) {

	//Check if the interrupt is the Timing Sychronization
	if (EXTI->PR & EXTI_PR_PR15) {
		//acknowledge the interrupt
		EXTI->PR |= EXTI_PR_PR15;


		counter++;
		int x = counter;
		if (counter == 60){
			counter = 0;
		}

	}
}

//Deals with the external interrupt for the Timing Synchronization
void EXTI0_1_IRQHandler(void) {

	//Check if the interrupt is the Timing Sychronization
	if (EXTI->PR & EXTI_PR_PR0) {
		//acknowledge the interrupt
		EXTI->PR |= EXTI_PR_PR0;

		counter++;
		int x = counter;

		if (counter == 60){
			counter = 0;
			toggle_pin(GPIOC, 6);
		}

	}
}

void TIM6_DAC_IRQHandler(void) {
	//Acknowledge the interrupt
	TIM6->SR &= ~TIM_SR_UIF;

	if (state == 0) {
		wifi_sendstring("AT\r\n");
	}
	if (state == 1) {
		wifi_sendstring("AT+CWMODE=3\r\n");

	}
	if (state == 2) {
		wifi_sendstring("AT+CWJAP=\"Xyz\",\"team4crib\"\r\n");
		tim6_changeTimer(10000);
	}
	if (state == 3) {
		wifi_sendstring("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
		tim6_changeTimer(3000);
	}
	if (state == 4) {
		wifi_sendstring("AT+CIPSEND=52\r\n");
		tim6_changeTimer(1000);
	}
	if (state == 5)	{
		wifi_sendstring("GET /update?api_key=2155L8AXXZLPF57M&field1=42\r\n\r\n\r\n");	}
	if (state <= 5)
		state++;

}
