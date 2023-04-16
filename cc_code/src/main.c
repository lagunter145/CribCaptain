/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <stdio.h>
#include <stdint.h>
#include "stm32f0xx.h"
#include "lcd_7in.h"
#include "rfid.h"
#include "esp.h"
#include "timer.h"
#include "misc.h"
//#include "keypad_debounced.h"
//#include "keypad_support.h"
#include "keypad_test.h"
#include "gui.h"
			
extern uint8_t rfid_tag[20];
extern char uid_str[10];
extern uint32_t uid;
extern uint8_t card_scanned;
//extern Button button1;


int main(void)
{
	card_scanned = 0;
	// RFID + DMA INITIALIZATION ROUTINE
    uint32_t versiondata = 0;
    uint32_t chip;
    uint32_t firmware0;
    uint32_t firmware1;
    int success;


    init_usart5();
    wakeup();
    versiondata = getFirmwareVersion();
    chip = (versiondata>>24) & 0xff;
    firmware0 = versiondata>>16 & 0xff;
    firmware1 = versiondata>>8  & 0xff;
    //printf("%d\n", versiondata);


    // set max retry attempts
    // setPassiveActivationRetries(0xff);

    // Configure Secure Access Module to read RFID cards
    SAMConfig();


    success = readPassiveTargetID(PN532_MIFARE_ISO14443A, NULL, NULL, 0);
    enable_DMA1();


	//set_pin(GPIOA, 5, 1);
	setup_external_timesync();

	setup_uart1();
	setup_tim6();

	enable_ports_keypad_LED();
	setup_tim7();


	//setup_pcb_leds();


	//GPIOA->BSRR |= 1 << 5;


	uint8_t x;
	char string[15] = "Hello, World!";



	//wifi_sendstring("AT\r\n");
	//wifi_checkstring("AT\r\r\n\r\nOK\r\n");

	//setup_spi1();
	//setup_tim15();




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
	fillScreen(BLACK);
	drawRect(10, 10, 400, 200, RED, 1);
	drawCircle(700, 400, 50, RA8875_BLUE, 1);

	// enable touch
	touchEnable(1);

	// clear pre-existing touch event??
	if (touched()) {
		writeReg(RA8875_INTC2, RA8875_INTC2_TP);
	}



	// switch to text mode
	textMode();
	//cursorBlink(32);
	textEnlarge(0);
	textSetCursor(10, 10);
	textColor(0x8170, RA8875_WHITE);
	textWrite(string, 15);
	textSetCursor(100, 150);
	textEnlarge(2);
	textWrite(string, 15);
	char buff[] = "                  ";
	textSetCursor(100, 150);

	textWrite(buff, 15);
	textSetCursor(100, 150);

	graphicsMode();


	fillScreen(BLACK);
	//setup_devboard_leds();
	//setup_external_timesync();
	//set_pin(GPIOC, 6, 0);
	//set_pin(GPIOC, 7, 1);
	//set_pin(GPIOC, 8, 1);


	guiLOADINGInit();
	guiMAINInit();
	guiCHECKINInit();
	buttonArr[0].on = 1;
	setup_t_irq();
	guiStateHandler(LOADING);


	char a;


	/*
	char a = wifi_getchar();
	textWrite(&a, 1);
	char b = wifi_getchar();
	textWrite(&b, 1);
	char c = wifi_getchar();
	textWrite(&c, 1);
	char d = wifi_getchar();
	textWrite(&d, 1);
	char e = wifi_getchar();
	textWrite(&e, 1);
	*/


	// switch back to graphics mode
	//graphicsMode();
//	uint16_t tx, ty;
//	float xScale = 1024.0F/800;
//	float yScale = 1024.0F/480;
	for(;;) {
		//nano_wait(100000000);
//		if (!ra8875INT()) {
//			if (touched()) {
//			touchRead(&tx, &ty);
//			// Draw a circle
//			graphicsMode();
//			drawCircle((uint16_t)(tx/xScale), (uint16_t)(ty/yScale), 4, RA8875_WHITE, 1);
			//textMode();
//			}
//		}

//		a = get_keypress();
//		keypad_values(a);

	}





	/*
	simple_putchar('A');
	simple_putchar('T');
	simple_putchar('\r');
	simple_putchar('\n');
	*/
	/*

	wifi_sendstring("AT\r\n");
	//response should AT\r\r\n\r\nOK\r\n
	wifi_checkstring("AT\r\r\n\r\nOK\r\n");
	//LCD_DrawString(10,10,  WHITE, BLACK, "AT", 12, 1);
	//LCD_DrawString(10,34,  WHITE, BLACK, "OK", 12, 1);

	//int firstChar = simple_getchar();
	//int secondChar = simple_getchar();
	//wifi_sendstring("AT+RST\r\n");
	//wifi_checkstring("AT+RST\r\r\n\r\nOK\r\n");

	wifi_sendstring("AT+CWMODE=3\r\n");
	wifi_checkstring("AT+CWMODE=3\r\r\n\r\nOK\r\n");

	wifi_sendstring("AT+CWJAP=\"Xyz\",\"team4crib\"\r\n");
	//wifi_checkstring("AT+CWJAP=\"Xyz\",\"team4crib\"\r\r\n\r\nOK\r\n");

	//nano_wait(4000000000);

	wifi_sendstring("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
	//wifi_checkstring("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\r\nCONNECT\r\nOK\r\n");



	wifi_sendstring("AT+CIPSEND=52\r\n");
	wifi_sendstring("GET /update?api_key=2155L8AXXZLPF57M&field1=42\r\n\r\n\r\n");
	//wifi_checkstring("AT+CIPSEND=51\r\r\n\r\nOK\r\n");


	//wifi_checkstring("GET /update?api_key=2155L8AXXZLPF57M&field1=143\r\r\n\r\nOK\r\n");

	wifi_sendstring("AT+CIPSEND=73\r\n");

	wifi_sendstring("GET /channels/2029438/feeds.json?api_key=YDF0Z4NBW4US5DTU&results=1\r\n\r\n\r\n");

	 */

	for(;;)
		//simple_putchar(10);
		nano_wait(100000000);




/*
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

	//button1 = init_button(10, 350, 200, 80, "button", YELLOW);
	guiStateHandler(0);
	setup_t_irq();
	for(;;) {

//	    if (!ra8875INT()){
//	    if (touched()) {
//	        touchRead(&tx, &ty);
//	        // Draw a circle
//	        drawCircle((uint16_t)(tx/xScale), (uint16_t)(ty/yScale), 4, RA8875_WHITE, 1);
//	    }
//	    }


	}
	*/

}

