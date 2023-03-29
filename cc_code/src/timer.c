/*
 * timer.c
 *
 *  Created on: Feb 17, 2023
 *      Author: mrahf
 */

#include "stm32f0xx.h"
#include "esp.h"
#include "misc.h"
#include <stdlib.h>
#include "lcd_7in.h"
#include "timer.h"

volatile int tim6semaphore = 0; //0 for wifi setup, 1 for http get request

volatile int jiffy = 0;
volatile int second = 0;
volatile int minute = 0;
volatile int hour = 0;
//Sets up the time synchronization for the device
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


//Deals with the external interrupt for the Timing Synchronization
void EXTI4_15_IRQHandler(void) {

	//Check if the interrupt is the Timing Sychronization
	if (EXTI->PR & EXTI_PR_PR15) {
		//acknowledge the interrupt
		EXTI->PR |= EXTI_PR_PR15;


		jiffy++;
		if (jiffy == 60){
			jiffy = 0;
		}

	}
}

void write_time(int second) {
	textSetCursor(100, 150);
	textEnlarge(2);
	//char buff[] = "                  ";
	//textWrite(buff, 15);
	//textSetCursor(100, 150);

	//itoa(second, buff, 10);

	char time[8] = "00,00,00";
	itoa((hour % 12) / 10,(&time[0]),10);
	itoa((hour % 12) % 10,(&time[1]),10);
	itoa(minute / 10,(&time[3]),10);
	itoa(minute % 10,(&time[4]),10);
	itoa(second / 10,(&time[6]),10);
	itoa(second % 10,(&time[7]),10);
	time[2] = ':';
	time[5] = ':';
	textWrite(time, 8);


}


//Deals with the external interrupt for the Timing Synchronization
void EXTI0_1_IRQHandler(void) {

	//Check if the interrupt is the Timing Sychronization
	if (EXTI->PR & EXTI_PR_PR0) {
		//acknowledge the interrupt
		EXTI->PR |= EXTI_PR_PR0;

		jiffy++;

		//second
		if (jiffy == 60){
			jiffy = 0;
			second++;
			toggle_pin(GPIOC, 6);
			if (second == 60) {
				second = 0;
				minute++;
				//write_time(second);
				if (minute == 60) {
					minute = 0;
					hour++;
				}

			}
			write_time(second);

		}






	}
}


volatile int wifiInitialState = 0;
volatile int wifiHTTPState = 0;

void TIM6_DAC_IRQHandler(void) {
	//Acknowledge the interrupt
	TIM6->SR &= ~TIM_SR_UIF;

	//wifi is initializing
	if (tim6semaphore ==0) {
		//check if the wifi device is connected
		if (wifiInitialState == 0) {
			wifi_sendstring("AT\r\n");
		}
		//set the moded to be an access point and a station
		if (wifiInitialState == 1) {
			wifi_sendstring("AT+CWMODE=3\r\n");

		}
		//connect to Wifi
		if (wifiInitialState == 2) {
			wifi_sendstring("AT+CWJAP=\"Xyz\",\"team4crib\"\r\n");
			tim6_changeTimer(10000);
		}
		//set the timer 6 semaphore to the http get request mode
		if (wifiInitialState == 3) {
			tim6semaphore = 1;
			tim6_changeTimer(500);
		}
		/*
		if (wifiInitialState == 4) {
			//wifi_sendstring("AT+CIPSEND=52\r\n");
			tim6_changeTimer(1000);
		}
		if (wifiInitialState == 5)	{
			//wifi_sendstring("GET /update?api_key=2155L8AXXZLPF57M&field1=42\r\n\r\n\r\n");
		}
		*/
		if (wifiInitialState <= 5)
			wifiInitialState++;
	}

	//HTTP get requests
	if (tim6semaphore == 1) {
		//char url[200] = "api.thingspeak.com/update?api_key=2155L8AXXZLPF57M&field1=53";
		// ***** Maybe try worldtimeapi instead
		// worldtimeapi.org/api/timezone/America/new_york.txt
		//char url[200] = "worldclockapi.com/api/json/est/now";
		char url[200] = "worldtimeapi.org/api/timezone/America/new_york.txt";
		//connect the socket (AT+CIPSTART)
		if (wifiHTTPState == 0) {
			http_getrequest(url, 0);
			tim6_changeTimer(3000);
		}
		//Send the number of bytes in the get request (AT+CIPSEND)
		if (wifiHTTPState == 1) {
			http_getrequest(url, 1);
			tim6_changeTimer(500);
		}
		//send the get request
		if (wifiHTTPState == 2) {
			http_getrequest(url, 2);
		}
		if (wifiHTTPState <= 2)
			wifiHTTPState++;
	}


}
