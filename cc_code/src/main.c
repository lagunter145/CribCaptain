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
#include "lcd.h"
#include "touch.h"
#include "test.h"
#include <stdio.h>
#include <stdint.h>
#include "esp.h"
#include "timer.h"
#include "misc.h"
			

int main(void)
{

	setup_uart1();
	//setup_spi1();
	setup_tim6();
	setup_devboard_leds();
	setup_external_timesync();
	set_pin(GPIOC, 6, 0);
	set_pin(GPIOC, 7, 1);
	set_pin(GPIOC, 8, 1);
	//LCD_Init(0,0,0);
	//LCD_Clear(BLACK);

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




}

