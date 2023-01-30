/*
 * rfid.c
 *
 *  Created on: Jan 27, 2023
 *      Author: phili
 */

#include "stm32f0xx.h"


// USART5 was used from 362 Lab 10. I just copied over the initiation and
// basic read/write functions.
// According to the STM32F0 Reference Manual, USART5/6/7/8 are missing a
// lot of features (don't know if we actually need). USART1/2 are fully featured.
// USART1_TX: PA9 AF1, PB6 AF0
// USART1_RX: PA10 AF1, PB7 AF0

void init_usart5()
{
    // enable RCC clock to GPIOC and GPIOD
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    // config PC12 to be routed to USART5_TX (AF2)
    GPIOC->MODER &=~0x03000000;
    GPIOC->MODER |= 0x02000000;
    GPIOC->AFR[1] |=0x00020000;
    // config PD2 to be routed to USART5_RX (AF2)
    GPIOD->MODER &=~0x00000030;
    GPIOD->MODER |= 0x00000020;
    GPIOD->AFR[0] |=0x00000200;
    // enable RCC clock to USART5
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    // configure USART5
    USART5->CR1 &=~USART_CR1_UE;    // disable USART5
    USART5->CR1 &=~USART_CR1_M;     // set word size of 8 bits
    USART5->CR1 &=~(1<<28);
    USART5->CR2 &=~USART_CR2_STOP;  // set 1 stop bit
    USART5->CR1 &=~USART_CR1_PCE;   // set no parity
    USART5->CR1 &=~USART_CR1_OVER8; // use 16x oversampling
    USART5->BRR = 0;                // use baud rate 115200
    USART5->BRR |= 0x1a1;
    USART5->CR1 |= USART_CR1_TE | USART_CR1_RE; // enable transmitter and receiver by setting TE and RE
    USART5->CR1 |= USART_CR1_UE;    // enable USART
    // check TEACK and REACK bits of ISR to be set
    while(!((USART5->ISR & USART_ISR_TEACK) && (USART5->ISR & USART_ISR_REACK)));
}

int write_byte(int c)
{
    while (!(USART5->ISR & USART_ISR_TXE)) {}

    USART5->TDR = c;

    return c;
}

int read_byte(void)
{
    while (!(USART5->ISR & USART_ISR_RXNE)) {}
    int c = USART5->RDR;

    return c;
}

