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
    /*uint32_t versiondata = 0;
    wakeup();
    versiondata = getFirmwareVersion();
    printf("%d\n", versiondata);
    printf("pass");*/
    while(1)
    {
        write_byte(0x55);
    }
//	Touch_Test();
}
