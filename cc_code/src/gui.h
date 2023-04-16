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

#define NUM_BUT  15

typedef enum {
	LOADING,
	MAIN,
	CHECKIN,
	CALENDAR,
	ROOMMATES
} stateType;

typedef struct Button{
    int x1;
    int x2;
    int y1;
    int y2;
    char label[15];
    uint8_t on;
    uint8_t labelLength;
    uint8_t pressed;
    uint16_t color;
} Button;

extern uint8_t _textScale;
Button buttonArr[NUM_BUT];


Button init_button(int x, int y, int w, int h, char* label, uint16_t color);
int check_pressed(Button but, int x, int y);
void update_button(Button but, int x, int y, int w, int h, char*label, uint16_t color);
void guiStateHandler(uint8_t state);
uint8_t buttonHandler(int xc, int yc);
void guiMAINInit();
void guiLOADINGInit();
void guiCHECKINInit();
void guiCALENDARInit();
void guiCALENDARDraw();
void guiROOMMATESInit();
void guiROOMMATESDraw();


#endif /* GUI_H_ */
