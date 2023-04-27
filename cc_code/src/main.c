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
#include "gui.h"
#include "roommates.h"
#include "ff.h"
#include "diskio.h"
#include "fifo.h"
#include "keypad.h"
			
volatile uint8_t card_scanned = 0;

int main(void)
{
	//RCC->APB2RSTR |= RCC_APB2RSTR_SYSCFGRST;
	//RCC->CSR |= RCC_CSR_SFTRSTF;
	//RCC->BDCR |= RCC_BDCR_BDRST;

	// ---------------------
	// KEYPAD INITIALIZATION
	// ---------------------
	enable_ports_keypad_LED();
	setup_tim7();
	setup_pcb_leds();

	// -------------------------
	// RFID + DMA INITIALIZATION
	// -------------------------
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

    // set max retry attempts
    // setPassiveActivationRetries(0xff);

    // Configure Secure Access Module to read RFID cards
    SAMConfig();

    success = readPassiveTargetID(PN532_MIFARE_ISO14443A, NULL, NULL, 0);
    enable_DMA1();

    // ---------------------------------------
    // EXTERNAL TIMESYNC + WIFI INITIALIZATION
    // ---------------------------------------
	setup_external_timesync();
	setup_uart1();
	setup_tim6();

	// -----------------
	// SD INITIALIZATION
	// -----------------
	//enable_tty_interrupt();
	//setup_tim14();
//	FATFS fs_storage;
//	FATFS *fs = &fs_storage;
//	f_mount(fs, "", 1);
//	FILE* test;
//	f_open(test, "test.txt", 0);
	//command_shell();

	// ------------------
	// LCD INITIALIZATION
	// ------------------
	CS_HIGH;
	RESET_LOW;
	nano_wait(100000000);
	RESET_HIGH;
	nano_wait(100000000);

	// setup functions
	setup_spi1();
	nano_wait(1000000);
	success = readReg(0);

	LCD_Init();
	displayOn(1);
	GPIOX(1);      // Enable TFT - display enable tied to GPIOX
	PWM1config(1, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	PWM1out(255);

	// hardware accelerated drawing
	fillScreen(BLACK);
		// enable touch
	touchEnable(1);

	// clear any pre-existing touch event
	if (touched()) {
		writeReg(RA8875_INTC2, RA8875_INTC2_TP);
	}

	// GUI Initialization
	setup_tim16();
	guiLOADINGInit();
	//drawPic(200,200);

	setup_t_irq();
	guiStateHandler(LOADING);

	getRoommateData();

	buttonArr[1].on = 1;

	for(;;) {

	}
}
