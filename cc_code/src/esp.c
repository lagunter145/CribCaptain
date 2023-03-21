/*
 * esp.c
 *
 *  Created on: Feb 3, 2023
 *      Author: mrahf
 */


#include "stm32f0xx.h"
#include "esp.h"
#include <stdint.h>
#include <string.h>

char readBuffer[10];

void setup_uart1() {
	//Enable RCC clocks to GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	//Configure PA9  as USART1_TX
	GPIOA->MODER &= ~GPIO_MODER_MODER9;
	GPIOA->MODER |= GPIO_MODER_MODER9_1;
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFR9;
	GPIOA->AFR[1] |= 0x00000010; //Alternate function 1
	//Configure PA10 as USART1_RX
	GPIOA->MODER &= ~GPIO_MODER_MODER10;
	GPIOA->MODER |= GPIO_MODER_MODER10_1;
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFR10;
	GPIOA->AFR[1] |= 0x00000100; //Alternate function 1
	//Configure PA11 as RESET
	GPIOA->MODER &= ~GPIO_MODER_MODER11;
	GPIOA->MODER |= GPIO_MODER_MODER10_0;  //output
	GPIOA->BSRR |= GPIO_BSRR_BS_11; //set the reset high
	//Enable the RCC clock to the USART1 peripheral
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//disable USART1
	USART1->CR1 &= ~USART_CR1_UE;
	//Set a Word Size of 8 bits
	USART1->CR1 &= ~(USART_CR1_M | 0x10000000);
	//Set it for one stop bit
	USART1->CR2 &= ~USART_CR2_STOP;
	//Set it for no parity
	USART1->CR1 &= ~USART_CR1_PCE;
	//use 16x oversampling
	USART1->CR1 &= ~USART_CR1_OVER8;

	//Not Working
	//NVIC_EnableIRQ(USART1_IRQn);
	//NVIC->ISER[0] = (1 << USART1_IRQn);
	//Enable the recieve not empty interrupt enable
	//USART1->CR1 |= USART_CR1_RXNEIE;

	//Enable DMA reciever access
	USART1->CR3 |= USART_CR3_DMAR;

	//Baud rate of 115200
	USART1->BRR = 0;
	USART1->BRR |= 0x1A1;
	//Enable the transmitter and receiver
	USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;
	//enable the USART1
	USART1->CR1 |= USART_CR1_UE;
	//Wait for the TE and RE bits to be acknowledged
	while(!((USART1->ISR & USART_ISR_TEACK) && (USART1->ISR & USART_ISR_REACK)));

	//Enable the DMA clock
	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	//

}

//The WI-FI will be reset when PA11 is set low
void wifi_reset() {
	GPIOA->BSRR |= GPIO_BSRR_BR_11; //sets the reset pin low
	GPIOA->BSRR |= GPIO_BSRR_BS_11; //sets the reset pin high
}

uint8_t wifi_sendchar(int txChar) {
	//Wait for the USART5 ISR TXE (checks if the transmit data register is empty) to be set
	while(!(USART1->ISR & USART_ISR_TXE));
	//write the argunment to the USART TDR (transmit data register)
	USART1->TDR = txChar;

	return txChar;
}

uint8_t wifi_getchar() {
	//Wait for the USART5 ISR RXNE (read data register is not empty) bit to be set
	while(!(USART1->ISR & USART_ISR_RXNE));
	//return the value of the receive data register
	return USART1->RDR;

}

char * wifi_sendstring(char * cmd) {
	//loops through the string and sends individual characters
	for (int i = 0; i < strlen(cmd); i++) {
		wifi_sendchar(cmd[i]);
	}
	return cmd;
}


char wifi_checkstring(char * response) {

	for (int i = 0; i < strlen(response); i++) {
		if (!(response[i] == wifi_getchar()))
			return 0;
	}
	return 1;
}

/* Not Working
void USART1_IRQHandler(void) {
	uint8_t rxByte = 0;
	if (USART1->ISR & USART_ISR_RXNE) {
		rxByte = USART1->RDR;

	}
}
*/

