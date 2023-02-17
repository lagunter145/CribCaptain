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
#include "rfid.h"
#include <stdio.h>
#include <stdint.h>
			

int main(void)
{
    /*
	setup_spi1();
	setup_t_irq();
	LCD_Init(0,0,0);
	LCD_Clear(BLACK);
	while(1) {
		printf("Passing");
	}*/
    init_usart5();
    uint32_t versiondata = 0;
    uint32_t chip;
    uint32_t firmware0;
    uint32_t firmware1;
    int success;

    wakeup();
    versiondata = getFirmwareVersion();
    chip = (versiondata>>24) & 0xff;
    firmware0 = versiondata>>16 & 0xff;
    firmware1 = versiondata>>8  & 0xff;
    printf("%d\n", versiondata);

    // Configure Secure Access Module to read RFID cards
    SAMConfig();

    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    success = readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 0);

    printf("pass");

}
