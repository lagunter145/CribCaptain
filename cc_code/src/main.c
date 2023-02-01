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
			

int main(void)
{
	setup_spi1();
	setup_t_irq();
	LCD_Init(0,0,0);
	LCD_Clear(BLACK);
	//Display_Test();
	for(;;) {
		nano_wait(300000000);
	}
//	while(1) {
//		printf("Passing");
//	}
//	Touch_Test();
}
