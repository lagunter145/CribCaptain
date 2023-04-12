/*
 * misc.c
 *
 *  Created on: Mar 9, 2023
 *      Author: mrahf
 *      includes miscellaneous functions
 */

#include "stm32f0xx.h"
#include "misc.h"

//sets up PC6-PC9 to be LEDs because of the 362 board
void setup_devboard_leds() {
	//Enable RCC clocks to GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	//Configure PA9-PC9 as output
	GPIOC->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
	GPIOC->MODER |= GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;

}

//Sets PA5-PA8 as LEDS for the PCB boards LEDs
void setup_pcb_leds() {
	//Enable RCC clocks to GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//Configure PA5-PA8 as output
	GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER8);
	GPIOA->MODER |=   GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER8_0;


}

//sets a GPIO pin high or low. high if on is 1 or low if on is 0
void set_pin(GPIO_TypeDef * gpio, char pin, char on) {
	//shifts to get the right pin
	uint32_t pinValue = 1 << pin;
	//if pin is low, so shift to get to the Bit Reset part of the register
	if (!on)
		pinValue = pinValue << 16;

	//sets the specific pin high
	gpio->BSRR |= pinValue;
}

//toggles a GPIO pin
void toggle_pin(GPIO_TypeDef * gpio, char pin) {
	//shifts the pin to get the right pin
	uint32_t pinValue = 1 << pin;

	//if the pin is high, then set it low
	//if the pin is low, then set it high
	set_pin(gpio, pin, !(gpio->ODR & pinValue));
}

