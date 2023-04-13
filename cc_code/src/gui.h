/*
 * gui.h
 *
 *  Created on: Mar 22, 2023
 *      Author: philip
 */

#ifndef GUI_H_
#define GUI_H_

#include "stm32f0xx.h"
#include "lcd_7in.h"

typedef struct Button{
    int x1;
    int x2;
    int y1;
    int y2;
    char label[10];
    uint8_t labelLength;
    uint8_t pressed;
    uint16_t color;
} Button;

extern uint8_t _textScale;



Button init_button(int x, int y, int w, int h, char* label, uint16_t color);
int check_pressed(Button but, int x, int y);
void update_button(Button but, int x, int y, int w, int h, char*label, uint16_t color);
void guiStateHandler(uint8_t state);
void buttonHandler(int xc, int yc);

#endif /* GUI_H_ */
