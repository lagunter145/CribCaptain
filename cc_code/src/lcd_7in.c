/*
 * lcd_7in.c
 *
 *  Created on: Feb 8, 2023
 *      Author: gunter3
 */

#include "stm32f0xx.h"
#include <stdint.h>
#include "lcd_7in.h"
#include "RA8775_commands.h"

lcd_dev_t lcddev;

//SPI FUNCTIONS

// setup GPIOB pins for display and setup spi1 to interface with the display
void setup_spi1() {
	// enable RCC clock to GPIO B Ports
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	//clear and set the MODER values for PB8,9,11,14 for outputs (01 in MODER)
	GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9| GPIO_MODER_MODER11 | GPIO_MODER_MODER14);
	GPIOB->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0);
	// clear and set the MODER for PB3,4,5 for alternate functions (10 in MODER)
	GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
	GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1| GPIO_MODER_MODER5_1);

	// sets the bits for PB8,9,11,14 for the ODR
	GPIOB->ODR |= (GPIO_ODR_8 | GPIO_ODR_9 | GPIO_ODR_11 | GPIO_ODR_14);
	// sets the AFR values for PB3,4,5 as 0000 to set them as alternative functions for SPI1
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFR3 | GPIO_AFRL_AFR4 | GPIO_AFRL_AFR5);

	// full duplex SPI peripheral
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // enables RCC clock to SPI1
	// spi1_cr1 config
	SPI1->CR1 &= ~(SPI_CR1_SPE); // disable spi enable pin so that spi can be configured
	SPI1->CR1 |= SPI_CR1_MSTR; // configure spi for master mode
	SPI1->CR1 &= ~(SPI_CR1_BR); // baud rate set to fpclk/256 (slowest)
	SPI1->CR1 |= 0x38;
	SPI1->CR1 |= (SPI_CR1_SSI | SPI_CR1_SSM); // set SSM(software slave management)/SSI(internal slave select) for SPI1
	SPI1->CR2 &= ~(SPI_CR2_DS); // clears what was set for word size
	SPI1->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2); // sets word size to 8 bits
	SPI1->CR2 |= SPI_CR2_FRXTH; // set RXFIFO threshold to 8 bits
	SPI1->CR1 |= SPI_CR1_SPE; // enable spi1
}

// function to set SPI baud rate to fpclk/16
void spi1_fast()
{
	SPI1->CR1 &= ~(SPI_CR1_SPE); // disable spi enable pin so that spi can be configured
	SPI1->CR1 &= ~(SPI_CR1_BR); // baud rate set to fpclk/16 (~3.2 MHz)
	SPI1->CR1 |= 0x18;
	SPI1->CR1 |= SPI_CR1_SPE; // enable spi1
}

// taken from A.6.2 (pg. 944)
void setup_t_irq(void) {
	// enable RCC clock to GPIO A Ports
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Select Port A for pin 0 external interrupt by writing 0000 in EXTI0
	SYSCFG->EXTICR[1] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0_PA;
	// Configure corresponding mask bit in the EXTI_IMR register (EXTI_IMR_MR0 = 0x0001)
	// Writing 1 to corresponding mask bit in EXTI_IMR means "Interrupt request from Line x(0) is not masked"
	// I think PA0 is on line 0 for EXTI0 (figure 25 on pg. 222)
	EXTI->IMR = EXTI_IMR_MR0;
	// Configure the Trigger Selection bits of the Interrupt line on rising edge (EXTI_RTSR_TR0 = 0x0001)
	// Writing 1 to corresponding mask bit in EXTI_RTSR means "Rising trigger enabled (for Event and Interrupt) for input line"
	//EXTI->RTSR = EXTI_RTSR_TR0;
	// Configure the Trigger Selection bits of the Interrupt line on falling edge (EXTI_FTSR_TR0 = 0x0001)
	// Writing 1 to corresponding mask bit in EXTI_FTSR means "Falling trigger enabled (for Event and Interrupt) for input line"
	EXTI->FTSR = EXTI_FTSR_TR0;
	// Configure NVIC for External Interrupt
	// Enable Interrupt on EXTI0_1
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	// Set priority for EXTI0_1
	NVIC_SetPriority(EXTI0_1_IRQn, 0);
}

// taken from (https://controllerstech.com/external-interrupt-using-registers/)
// will print the x and y coords to terminal if touched
void EXTI0_1_IRQHandler (void) {
	// check the pin which triggered the interrupt
	if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0){
		//char xString[80] = "X:";
		//char yString[80] = "Y:";
		// acknowledges the interrupt by writing a 1 to the register
		//LCD_Clear(BLACK);
		fillScreen(RED);
		uint16_t x, y;
		//(&x, &y);
		//strcat(xString, itoa(x, xString, 10));
		//LCD_DrawString(140,(80),  WHITE, BLACK, itoa(x, xString, 10), 16, 1);
		//strcat(yString, itoa(y, yString, 10));
		//LCD_DrawString(140,(80 + (16)),  WHITE, BLACK, itoa(y, yString, 10), 16, 1);
		nano_wait(300000000); //REPLACE WITH A ONE SHOT TIMER
		if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0){
			EXTI->PR |= EXTI_PR_PR0;
	    }
		//times_touched++;
	}
}


// Transfers a byte over SPI. Does not control NSSP
uint8_t transByte(uint8_t d)
{
    uint8_t rec;
    // writes data to DR to transmit
    while((SPI->SR & SPI_SR_BSY) != 0)
        ;
    *((uint8_t*)&SPI->DR) = d;
    while((SPI->SR & SPI_SR_BSY) != 0)
        ;
    // read back from RXFIFO
    while((SPI->SR & SPI_SR_RXNE) == 0)
        ;
    rec = SPI->DR;
    return rec;
}

// Write command to the LCD
void LCD_WR_REG(uint8_t data)
{
    CS_LOW;
    transByte(RA8875_CMDWRITE);
    transByte(data);
    CS_HIGH;
}

// Write 8-bit data to the LCD
void LCD_WR_DATA(uint8_t data)
{
    CS_LOW;
    transByte(RA8875_DATAWRITE);
    transByte(data);
    CS_HIGH;
}

uint8_t LCD_RD_REG(uint8_t reg)
{
	LCD_WR_REG(reg);
	return readData();
}

uint8_t readData()
{
	uint8_t read;
	CS_LOW;
	transByte(RA8875_DATAREAD);
	read = transByte(0x0);
	CS_HIGH;
	return read;
}

void  writeReg(uint8_t reg, uint8_t val) {
	// specify to RA8775 what register to write to
	LCD_WR_REG(reg);
	// writes the data to the register
	LCD_WR_DATA(val);
}

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void LCD_Init() {
	// setup for SPI in code, might not be needed since SPI already setup another way

	// initialize()
	// PLLinit()
	// size == 800x480
	writeReg(RA8875_PLLC1, (RA8875_PLLC1_PLLDIV1  + 11));
	nano_wait(1000000);	// need to delay 1 ms
	writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
	nano_wait(1000000);
	// out of PLLinit()
	writeReg(RA8875_SYSR, (RA8875_SYSR_16BPP | RA8875_SYSR_MCU8));
	// if size==800x480
	// Setting Timing Values
	uint8_t pixclk = (RA8875_PCSR_PDATL | RA8875_PCSR_2CLK);
	uint8_t hsync_start = 32;
	uint8_t hsync_pw = 96;
	uint8_t hsync_finetune = 0;
	uint8_t hsync_nondisp = 26;
	uint8_t vsync_pw = 2;
	uint16_t vsync_nondisp = 32;
	uint16_t vsync_start = 23;
	uint8_t voffset = 0;

	writeReg(RA8875_PCSR, pixclk);
	nano_wait(1000000);

	// Horizontal settings registers
	writeReg(RA8875_HDWR, (LCD_W / 8) - 1);
	writeReg(RA8875_HNDFTR, (RA8875_HNDFTR_DE_HIGH + hsync_finetune));
	writeReg(RA8875_HNDR, ((hsync_nondisp - hsync_finetune - 2) / 8));
	writeReg(RA8875_HSTR, ((hsync_start / 8) - 1));
	writeReg(RA8875_HPWR, (RA8875_HPWR_LOW + (hsync_pw / 8 - 1)));

	// Vertical settings registers
	writeReg(RA8875_VDHR0, (uint16_t)(LCD_H - 1 + voffset) & 0xFF);
	writeReg(RA8875_VDHR1, (uint16_t)(LCD_H - 1 + voffset) >> 8);
	writeReg(RA8875_VNDR0, vsync_nondisp - 1);
	writeReg(RA8875_VNDR1, vsync_nondisp >> 8);
	writeReg(RA8875_VSTR0, vsync_start - 1);
	writeReg(RA8875_VSTR1, vsync_start >> 8);
	writeReg(RA8875_VPWR, (RA8875_VPWR_LOW + vsync_pw - 1));

	// Set active window X
	writeReg(RA8875_HSAW0, 0);
	writeReg(RA8875_HSAW1, 0);
	writeReg(RA8875_HEAW0, (uint16_t)(LCD_W - 1) & 0xFF);
	writeReg(RA8875_HEAW1, (uint16_t)(LCD_W - 1) >> 8);

	// Set active window Y
	writeReg(RA8875_VSAW0, 0 + voffset);
	writeReg(RA8875_VSAW1, 0 + voffset);
	writeReg(RA8875_VEAW0, (uint16_t)(LCD_H - 1 + voffset) & 0xFF);
	writeReg(RA8875_VEAW1, (uint16_t)(LCD_H - 1 + voffset) >> 8);

	writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
	nano_wait(500000000);

	// set SPI speed back to fast
	spi1_fast();
}

// DRAWING FUNCTIONS
void fillScreen(uint16_t color) {
	drawRect(0,0,LCD_W-1,LCD_H-1,color,1);
}

void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, int filled) {
	x = applyRotationX(x);
	y = applyRotationY(y);
	w = applyRotationX(w);
	h = applyRotationY(h);

	// Set X0 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_X0_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(x & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_X0_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA(x >> 8);

	// Set Y0 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_Y0_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(y & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_Y0_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA(y >> 8);

	// Set X1 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_X1_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(w & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_X1_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA((w) >> 8);

	// Set Y1 (sends in 2 8-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_Y1_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(h & 0xff);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_Y1_8);
	// sends the bigger half of the coord second
	LCD_WR_DATA((h) >> 8);

	// Set Color (sends in 3 5-bit words to the display)
	// first, sends command to display to ready it to accept the first half of the x-coord
	LCD_WR_REG(SET_COL_0);
	// sends the lesser half of the coord first
	LCD_WR_DATA(color & 0x001f);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_COL_5);
	// sends the bigger half of the coord second
	LCD_WR_DATA((color & 0x07e0) >> 5);
	// second, sends command to display to ready it to accept the second half of the x-coord
	LCD_WR_REG(SET_COL_11);
	// sends the bigger half of the coord second
	LCD_WR_DATA((color & 0xf800) >> 11);

	// Display on screen
	LCD_WR_REG(RA8875_DCR);
	if (filled) {
		LCD_WR_DATA(0xB0);
	} else {
		LCD_WR_DATA(0x90);
	}

	// need to wait for command to finish
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

uint16_t applyRotationX(uint16_t x) {
	return x;
}

uint16_t applyRotationY(uint16_t y) {
	return y;
}

void displayOn(int on) {
  if (on)
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
  else
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

void GPIOX(int on){
 if (on)
	writeReg(RA8875_GPIOX, 1);
  else
	writeReg(RA8875_GPIOX, 0);
}

void PWM1out(uint8_t p){
	writeReg(RA8875_P1DCR, p);
}

void PWM1config(int on, uint8_t clock) {
  if (on) {
    writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
  } else {
    writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
  }
}

uint8_t waitPoll(uint8_t regname, uint8_t waitflag) {
  /* Wait for the command to finish */
  while (1) {
    uint8_t temp = LCD_RD_REG(regname);
    if (!(temp & waitflag))
      return 1;
  }
  return 0; // MEMEFIX: yeah i know, unreached! - add timeout?
}
