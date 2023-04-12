/*
 * test.c
 *
 *  Created on: Apr 11, 2023
 *      Author: philip
 */


#include "stm32f0xx.h"

#include "rfid.h"
#include <stdio.h>
#include <stdint.h>

extern uint8_t rfid_tag[20];
extern uint8_t uid_buf[7];
int rfid_test(void)
{

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


    // set max retry attempts
    // setPassiveActivationRetries(0xff);

    // Configure Secure Access Module to read RFID cards
    SAMConfig();

    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)


    success = readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 0);
    enable_DMA1();
    printf("pass");
    for(;;){
        ;
    }
}
