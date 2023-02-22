/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
//#include "lcd.h"
#include "lcd_7in.h"
//#include "touch.h"
#include "test.h"
#include <stdio.h>
#include <stdint.h>
			

int main(void)
{
	uint8_t x;
	setup_spi1();
	setup_t_irq();
	x = LCD_RD_REG(0);
	LCD_Init();
	displayOn(1);
    GPIOX(1);      // Enable TFT - display enable tied to GPIOX
	PWM1config(1, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	PWM1out(255);
	fillScreen(GREEN);
	nano_wait(100000000);
	drawRect(10, 10, 400, 200, RED, 1);
	//LCD_Clear(BLACK);
	//Display_Test();
	for(;;) {
		nano_wait(300000000);
	}
//	while(1) {
//		printf("Passing");
//	}
//	Touch_Test();
}
