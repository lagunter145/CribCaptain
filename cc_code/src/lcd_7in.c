/*
 * lcd_7in.c
 *
 *  Created on: Feb 8, 2023
 *      Author: gunter3
 */

#include "stm32f0xx.h"
#include <stdint.h>
#include "lcd_7in.h"

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
	SPI1->CR1 &= ~(SPI_CR1_BR); // baud rate set high as possible (SCK divisor is as small as possible)
	SPI1->CR1 |= (SPI_CR1_SSI | SPI_CR1_SSM); // set SSM(software slave management)/SSI(internal slave select) for SPI1
	SPI1->CR2 &= ~(SPI_CR2_DS); // clears what was set for word size
	SPI1->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2); // sets word size to 8 bits
	SPI1->CR1 |= SPI_CR1_SPE; // enable spi1
}

// Set the CS pin low if val is non-zero.
// Note that when CS is being set high again, wait on SPI to not be busy.
static void tft_select(int val)
{
    if (val == 0) {
        while(SPI1->SR & SPI_SR_BSY)
            ;
        CS_HIGH;
    } else {
        while((GPIOB->ODR & (CS_BIT)) == 0) {
            ; // If CS is already low, this is an error.  Loop forever.
            // This has happened because something called a drawing subroutine
            // while one was already in process.  For instance, the main()
            // subroutine could call a long-running LCD_DrawABC function,
            // and an ISR interrupts it and calls another LCD_DrawXYZ function.
            // This is a common mistake made by students.
            // This is what catches the problem early.
        }
        CS_LOW;
    }
}

// Write to an LCD "register"
void LCD_WR_REG(uint8_t data)
{
    while((SPI->SR & SPI_SR_BSY) != 0)
        ;
    // Don't clear RS until the previous operation is done.
    lcddev.reg_select(1);
    *((uint8_t*)&SPI->DR) = data;
}

// Write 8-bit data to the LCD
void LCD_WR_DATA(uint8_t data)
{
    while((SPI->SR & SPI_SR_BSY) != 0)
        ;
    // Don't set RS until the previous operation is done.
    lcddev.reg_select(0);
    *((uint8_t*)&SPI->DR) = data;
}

// Prepare to write 16-bit data to the LCD
void LCD_WriteData16_Prepare()
{
    lcddev.reg_select(0);
    SPI->CR2 |= SPI_CR2_DS;
}

// Write 16-bit data
void LCD_WriteData16(u16 data)
{
    while((SPI->SR & SPI_SR_TXE) == 0)
        ;
    SPI->DR = data;
}

// Finish writing 16-bit data
void LCD_WriteData16_End()
{
    SPI->CR2 &= ~SPI_CR2_DS; // bad value forces it back to 8-bit mode
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
	LCD_WR_REG(DISPLAY);
	if (filled) {
		LCD_WR_DATA(FILLED);
	} else {
		LCD_WR_DATA(DISPLAY);
	}

	// need to wait
}

uint16_t applyRotationX(uint16_t x) {
	return x;
}

uint16_t applyRotationY(uint16_t y) {
	return y;
}
