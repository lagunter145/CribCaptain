/*
 * gui.h
 *
 *  Created on: Mar 22, 2023
 *      Author: philip
 */

#ifndef GUI_H_
#define GUI_H_

#include "stm32f0xx.h"

#define NUM_BUT  15

typedef enum {
	LOADING,
	MAIN,
	CHECKIN,
	CALENDAR,
	ROOMMATES,
	MSG
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

extern stateType guiMenuState;
extern volatile uint8_t piccing;
Button buttonArr[NUM_BUT];


Button init_button(int x, int y, int w, int h, char* label, uint16_t color);
Button init_small_button(int x, int y, int w, int h, char* label, int labelLength, uint16_t color);
int check_pressed(Button but, int x, int y);
void update_button(Button but, int x, int y, int w, int h, char*label, uint16_t color);
void guiStateHandler(uint8_t state);
uint8_t buttonHandler(int xc, int yc);
void guiMAINInit();
void guiMAINDraw(void);
void guiLOADINGInit();
void guiLOADINGDraw();
void drawPic(int start_x, int start_y);
void bmpDraw(char* filename, int start_x, int start_y);
uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
void guiCHECKINInit();
void substring(char* source, char* dest, int s, int e);
void guiCALENDARInit();
void guiCALENDARDraw(int mode, int redraw);
void guiROOMMATESInit();
void guiROOMMATESDraw();
void checkmark(uint16_t start_x, uint16_t start_y);
void guiMSGInit();


#endif /* GUI_H_ */
