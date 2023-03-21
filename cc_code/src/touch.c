/*
 * touch.c
 *
 *  Created on: Jan 24, 2023
 *      Author: Lauren
 */

#include "stm32f0xx.h"
#include "touch.h"
#include "display.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int times_touched = 0; // if you're matt, look at the bottom of this code



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
/*
// taken from (https://controllerstech.com/external-interrupt-using-registers/)
// will print the x and y coords to terminal if touched
void EXTI0_1_IRQHandler (void) {
	// check the pin which triggered the interrupt
	if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0){
		char xString[80] = "X:";
		char yString[80] = "Y:";
		// acknowledges the interrupt by writing a 1 to the register
		LCD_Clear(BLACK);
		uint16_t x, y;
		LCD_RD_XY(&x, &y);
		//strcat(xString, itoa(x, xString, 10));
		LCD_DrawString(140,(80),  WHITE, BLACK, itoa(x, xString, 10), 16, 1);
		//strcat(yString, itoa(y, yString, 10));
		LCD_DrawString(140,(80 + (16)),  WHITE, BLACK, itoa(y, yString, 10), 16, 1);
		nano_wait(300000000); //REPLACE WITH A ONE SHOT TIMER
		if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0){
			EXTI->PR |= EXTI_PR_PR0;
	    }
		times_touched++;
	}
}

*/
// 2.8inch_SPI_Module_ILI9341_MSP2807_V1.1 from http://www.lcdwiki.com/2.8inch_SPI_Module_ILI9341_SKU:MSP2807


// read adc values from touch screen IC with SPI bus
// https://github.com/LonelyWolf/stm32/blob/master/ST7528/periph/spi.c
// SPIx_SendRecv()
uint16_t LCD_RD_TOUCH_DATA(uint8_t CMD) {
	CST_LOW;
	// send command to SPI, TXE cleared
	//*((uint8_t*)&SPI->DR) = CMD;
	//nano_wait(1000);
	LCD_WR_DATA(CMD);
	// wait while receive buffer is empty
	//while((SPI->SR & SPI_SR_RXNE));
	// return received byte
	uint32_t temp = SPI->DR;
//	temp = temp << 4;
//	temp = temp >> 4;
	//temp &= 0xff80;
	CST_HIGH;
	return temp;
}


/*

uint16_t LCD_RD_TOUCH_DATA(uint8_t CMD) {
	uint16_t num = 0;
	uint8_t count = 0;
	CLK_LOW;
	DIN_LOW;
	LCD_WR_DATA(CMD);
	CLK_LOW;
	nano_wait(1000);
	CLK_HIGH;
	CLK_LOW;
	for(count = 0; count < 16; count++) {
		num <<= 1;
		CLK_LOW;
		CLK_HIGH;
		//if(TDOUT_STATE) {
		if(GPIOB->ODR & GPIO_ODR_4) {
			num += 1;
		} else {
			num += 0;
		}
	}
	num >>= 4;
	return num;
} */

// read touch screen coordinates (x or y) multiple times (READ_TIMES) and averages the value
uint16_t LCD_RD_XORY(uint8_t xy) {
	uint16_t buf[READ_TIMES];
	uint16_t highest = 0;
	uint16_t lowest = 0;
	uint16_t sum = 0;

	//because the chip select line is normally held high, when touch is needed, the
	//chip select for touch (CST) must be set low and the chip select for the display
	//needs to be set high
    while(SPI1->SR & SPI_SR_BSY);
	CS_HIGH;
	//CST_LOW;
	//printf("------------------CHIP SELECT CODE: REACHED HERE IN CODE----------------");


	// takes 5 reads from the SPI interface and records the highest and lowest values (outliers)
	for(int i = 0; i < READ_TIMES; i++) {
		buf[i] = LCD_RD_TOUCH_DATA(xy);
		sum += buf[i];
		if(i == 0) {
			highest = buf[0];
			lowest = buf[0];
		} else {
			if(buf[i] > highest) {
				highest = buf[i];
			} else if (buf[i] < lowest) {
				lowest = buf[i];
			}
		}
	}
	sum -= lowest;
	sum -= highest;

	//changes the chips selects again
    while(SPI1->SR & SPI_SR_BSY);
	//CST_HIGH;
	CS_HIGH;

	// returns average of values read
	return (sum / (READ_TIMES - 2));
}

// gets touch's x and y coordinates and passes them by addresses
uint8_t LCD_RD_XY(uint16_t *x, uint16_t *y) {
	uint16_t xtemp, ytemp;
	xtemp = LCD_RD_XORY(CMD_RDX);
	ytemp = LCD_RD_XORY(CMD_RDY);
	*x = xtemp;
	*y = ytemp;
	return 1;
}

// maybe incorporate LCD_READ_XY2 to improve accuracy
// to see calibration, draw touch point with lcd.c 's LCD_DrawPoint

// Matt, she's for demonstration purposes. I will delete the global variable

