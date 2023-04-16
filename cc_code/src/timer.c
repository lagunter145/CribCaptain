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
#include "gui.h"
#include "colours.h"

extern uint16_t base_color;
extern uint16_t acce_color;
extern uint8_t show_sec;
volatile int tim6semaphore = 0; //0 for wifi setup, 1 for http get request

volatile int jiffy = 0;
volatile int second = 0;
volatile int minute = 0;
volatile int hour = 0;
extern volatile uint8_t canTouch;
extern stateType guiMenuState;
//Sets up the time synchronization for the device
void setup_external_timesync() {

	/*
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
	*/
	///*


	//Enable SYSCFG bit in RCC register
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	//Enable RCC clocks to GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	//Configure PC15  as Input
	GPIOC->MODER &= ~GPIO_MODER_MODER13;

	//sets external interrupt for pin 15 to gpio c
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	//rising edge trigger enabled for pin 0
	EXTI->RTSR |= EXTI_RTSR_TR13;
	//disable falling edge trigger
	EXTI->FTSR &= ~EXTI_FTSR_TR13;

	//make the interrupt not masked
	EXTI->IMR |= EXTI_IMR_MR13;

	//enable external interrupt on pin 0
	NVIC->ISER[0] |= (1 << EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 0);
	//*/


}



//Default timer takes half a second
void setup_tim6() {
	//Enable RCC clock to Timer 6
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	//update the TIM6 prescaler
	TIM6->PSC = 48000 - 1; // 48,000,000 / (47 + 1) = 1 kHz

	//Configure the auto-reload register
	TIM6->ARR = 500 - 1;

	//configure the Timer 6 DMA/Interrupt Enable Register
	//(enable the UIE falg)
	TIM6->DIER |= TIM_DIER_UIE;

	//Enable TIM6 by setting the CEN bit in
	//the TIM6 control register1
	TIM6->CR1 |= TIM_CR1_CEN;
	//TIM6->CR2 |= 0x20;

	//Enable the interrupt for Timer 6 in the NVIC ISER
	NVIC->ISER[0] = (1 << TIM6_DAC_IRQn);
	NVIC_SetPriority(TIM6_DAC_IRQn, 1);
}

/*
//timer to wait for the touch interrupt
void setup_tim15() {
	//Enable RCC clock to Timer 15
	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
	//update the TI15 prescaler
	TIM15->PSC = 48000 - 1;
	//Configure the auto-reload register
	TIM15->ARR = 500 - 1;
	//configure the Timer 15 DMA/Interrupt Enable Register
	//Enable TIM15 by setting the CEN bit in
	TIM15->DIER |= TIM_DIER_UIE;
	//the TIM15 control register1
	TIM15->CR1 |= TIM_CR1_CEN;
	//Enable the interrupt for Timer 15 in the NVIC ISER
	NVIC->ISER[0] = (1 << TIM15_IRQn);
}
*/

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

void tim6_triggerInterrupt(void) {
	//Disable the TIM6 by reseting the CEN bit in CR1
	TIM6->CR1 &= ~TIM_CR1_CEN;
	//set the TIM6 counter to the auto-reload register, so it triggers the timer interrupt automatically
	TIM6->CNT = TIM6->ARR;
	//Enable TIM6 by setting the CEN bit in CR1
	TIM6->CR1 |= TIM_CR1_CEN;
}

uint8_t loadingCounter = 0;
void write_loading() {

	textMode();
	textSetCursor(250, 100);
	textEnlarge(4);
	textColor(acce_color, base_color);
	if (loadingCounter == 0)
		textWrite("LOADING   ", 10);
	if (loadingCounter == 1)
			textWrite("LOADING.  ", 10);
	if (loadingCounter == 2)
			textWrite("LOADING.. ", 10);
	if (loadingCounter == 3)
			textWrite("LOADING...", 10);
	graphicsMode();
	if (loadingCounter >= 3)
		loadingCounter = 0;
	else {
		loadingCounter++;
	}
}

void write_time() {

	int hourToDisplay = 0;
	if (hour == 12 || hour == 0) {
		hourToDisplay = 12;
	} else {
		hourToDisplay = hour % 12;
	}

	char time[8] = "00,00,00";
	itoa(hourToDisplay / 10,(&time[0]),10);
	itoa(hourToDisplay % 10,(&time[1]),10);
	itoa(minute / 10,(&time[3]),10);
	itoa(minute % 10,(&time[4]),10);
	itoa(second / 10,(&time[6]),10);
	itoa(second % 10,(&time[7]),10);
	time[2] = ':';
	time[5] = ':';


	textMode();
	textSetCursor(25, 25);
	textEnlarge(2);
	textColor(acce_color, base_color);
	if(show_sec) {
		textWrite(time, 8);
	} else {
		textWrite(time, 5);
		// don't like how it redraws every second, but putting in button handler
		// caused issues with show_sec
		drawRect(150, 25, 220, 75, base_color, 1);
	}
	graphicsMode();

}

//Deals with the external interrupt for the Timing Synchronization
void EXTI4_15_IRQHandler(void) {
	//Check if the interrupt is the Timing Sychronization
	if (EXTI->PR & EXTI_PR_PR13) {
		//acknowledge the interrupt
		EXTI->PR |= EXTI_PR_PR13;

		//uint8_t a = (GPIOC->IDR & (1 << 13)) ? 1 : 0;
		//uint8_t b = (GPIOC->IDR & (1 << 13)) ? 1 : 0;
		//uint8_t c = (GPIOC->IDR & (1 << 13)) ? 1 : 0;
		//uint8_t d = (GPIOC->IDR & (1 << 13)) ? 1 : 0;
		//uint8_t e = (GPIOC->IDR & (1 << 13)) ? 1 : 0;

		//if ((a + b + c + d + e) > 2) {
			jiffy++;

			//second
			if (jiffy == 60){
				jiffy = 0;
				second++;
				//toggle_pin(GPIOA, 5);
				if (second == 60) {
					second = 0;
					minute++;
					//write_time(second);
					if (minute == 60) {
						minute = 0;
						hour++;
					}

				}


				guiStateHandler(guiMenuState);
				if (guiMenuState == LOADING) {
					//write loading to the screen
					write_loading();
				}
				else {
					//update the time
					write_time();
				}

			}

	}
}


void TIM15_IRQHandler(void) {
	TIM15->SR &= ~TIM_SR_UIF;


	//reset flag
	canTouch = 1;
	//stop TIM15
	//tim15_stopTimer();
	toggle_pin(GPIOA, 6);


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
			//tim6_changeTimer(11000);

		}
		//set the moded to be an access point and a station
		if (wifiInitialState == 1) {
			wifi_sendstring("AT+CWMODE=3\r\n");
			tim6_changeTimer(11000);

		}
		//connect to Wifi
		if (wifiInitialState == 2) {
			wifi_sendstring("AT+CWJAP=\"Xyz\",\"team4crib\"\r\n");
			tim6_changeTimer(30000);
		}
		//set the timer 6 semaphore to the http get request mode
		if (wifiInitialState == 3) {
			tim6semaphore = 1;
			tim6_changeTimer(11000);
			wifiInitialState++;
		}


	}

	//HTTP get requests
	if (tim6semaphore == 1) {
		//char url[200] = "api.thingspeak.com/update?api_key=2155L8AXXZLPF57M&field1=53";
		// ***** Maybe try worldtimeapi instead
		// worldtimeapi.org/api/timezone/America/new_york.txt
		//char url[200] = "worldclockapi.com/api/json/est/now";
		//char url[200] = "worldtimeapi.org/api/timezone/America/new_york.txt";
		//char url[200] = "timeapi.io/api/Time/current/zone?timeZone=America/Indiana/Indianapolis";
		char url[200] = "timezone.abstractapi.com/v1/current_time/?api_key=9e51598312064a7494ae4b60562fbc71&location=Indianapolis";
		//char url[200] = "192.168.175.87";

		//connect the socket (AT+CIPSTART)
		if (wifiHTTPState == 0) {
			http_getrequest(url, 0);
			//tim6_changeTimer(4000);
		}
		//Send the number of bytes in the get request (AT+CIPSEND)
		if (wifiHTTPState == 1) {
			http_getrequest(url, 1);
			//tim6_changeTimer(500);
		}
		//send the get request
		if (wifiHTTPState == 2) {
			http_getrequest(url, 2);
			wifiHTTPState++;
			guiMenuState = MAIN;
			guiMAINDraw();
		}

	}


}
