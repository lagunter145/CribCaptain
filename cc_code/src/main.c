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
#include "gui.h"

extern Button button1;

int main(void)
{
	uint8_t x;
	char string[15] = "Hello, World! ";
    // manually assert n_rst pin
	CS_HIGH;
    RESET_LOW;
    nano_wait(100000000);
    RESET_HIGH;
    nano_wait(100000000);

    // setup functions
	setup_spi1();

    nano_wait(1000000);
	x = readReg(0);
	LCD_Init();
	displayOn(1);
    GPIOX(1);      // Enable TFT - display enable tied to GPIOX
	PWM1config(1, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	PWM1out(255);

	// hardware accelerated drawing
	fillScreen(GREEN);
	drawRect(10, 10, 400, 200, RED, 1);
	drawCircle(700, 400, 50, RA8875_BLUE, 1);

	// enable touch
	touchEnable(1);

	// switch to text mode
	textMode();
	cursorBlink(32);
	textEnlarge(0);
	textSetCursor(10, 10);
	textColor(0x8170, RA8875_WHITE);
	textWrite(string, 15);
	textSetCursor(100, 150);
	textEnlarge(2);
	textWrite(string, 15);

	// switch back to graphics mode
	graphicsMode();

	button1 = init_button(10, 350, 200, 80, "button", YELLOW);
	setup_t_irq();
	for(;;) {

	}
}
