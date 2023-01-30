/*
 * rfid.c
 *
 *  Created on: Jan 27, 2023
 *      Author: phili
 */

#include "stm32f0xx.h"
#include "rfid.h"

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
// is multibuffer communication needed? If so, must also enable DMA

// overrun error (ORE bit) can occur if the RXNE flag is set when the next data is received.
    // interrupt only sent if RXNEIE or EIE bit is set
// framing error (FE bit) can occur if stop bit not recognized
    // no interrupt is generated for single byte comm.
    // if EIE bit set in CR3 and multibuffer comm, then interrupt is sent


int write_byte(int c)
{
    // wait for TXE bit to be set
    while (!(USART5->ISR & USART_ISR_TXE)) {}

    USART5->TDR = c;

    return c;
}
// pg 687: "8. After writing the last data into the USARTx_TDR register, wait
// until TC=1. This indicates that the transmission of the last frame is complete.
// This is required for instance when the USART is disabled or enters the Halt
// mode to avoid corrupting last transmission.

int read_byte(void)
{
    // wait for RXNE bit to be set
    while (!(USART5->ISR & USART_ISR_RXNE)) {}
    int c = USART5->RDR;

    return c;
}

// not really sure what the return value should be
int writeCommand(uint8_t * buf, int buf_len)
{
    for(int i = 0; i < buf_len; i++)
    {
        write_byte(buf[i]);
    }
    return 0;
}
// not really sure what the return value should be
int readResponse(uint8_t * buf, int buf_len)
{
    for(int i = 0; i < buf_len; i++)
    {
        buf[i] = read_byte();
    }
    return 1;
}

// from Elechouse Github
// https://github.com/elechouse/PN532/blob/PN532_HSU/PN532/PN532.cpp
// input arg 'reg' is the 16 bit register address
// returns register value
int readRegister(uint16_t reg)
{
    int response;
    // in order to read, have to first write a READREGISTER command
    pn532_packetbuffer[0] = PN532_COMMAND_READREGISTER;
    pn532_packetbuffer[1] = (reg >> 8) & 0xFF;
    pn532_packetbuffer[2] = reg & 0xFF;

    if (writeCommand(pn532_packetbuffer, 3)) {
        return 0;
    }

    // read data packet
    int16_t status = readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer));
    if (0 > status) {
        return 0;
    }

    response = pn532_packetbuffer[0];

    return response;
}

// input arg 'reg' is the 16 bit register address
// input arg 'val' is the 8 bit value to write
// returns 0 for failure, 1 for success
int writeRegister(uint16_t reg, uint8_t val)
{
    int response;

    pn532_packetbuffer[0] = PN532_COMMAND_WRITEREGISTER;
    pn532_packetbuffer[1] = (reg >> 8) & 0xFF;
    pn532_packetbuffer[2] = reg & 0xFF;
    pn532_packetbuffer[3] = val;


    if (writeCommand(pn532_packetbuffer, 4)) {
        return 0;
    }

    // read data packet
    int16_t status = readResponse(pn532_packetbuffer, sizeof(pn532_packetbuffer));
    if (0 > status) {
        return 0;
    }

    return 1;
}
