/*
 * test.c
 *
 *  Created on: Jan 24, 2023
 *      Author: Lauren
 */

#include "stm32f0xx.h"
#include "lcd.h"
#include "touch.h"

void Display_Test(void) {
	LCD_DrawLine(10,20,100,200, WHITE);
	LCD_DrawRectangle(10,20,100,200, GREEN);
	LCD_DrawFillRectangle(120,20,220,200, RED);
	LCD_Circle(50, 260, 50, 1, BLUE);
	LCD_DrawFillTriangle(130,130, 130,200, 190,160, YELLOW);
	LCD_DrawChar(150,155, BLACK, WHITE, 'Working for', 16, 1);
	LCD_DrawString(140,60,  WHITE, BLACK, "ECE 477", 16, 0);
	LCD_DrawString(140,80,  WHITE, BLACK, "YAY!", 16, 1);
	LCD_DrawString(130,100, BLACK, GREEN, "green text block ", 12, 0);
}

void Touch_Test(void) {

}
