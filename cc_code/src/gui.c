/*
 * gui.c
 *
 *  Created on: Mar 22, 2023
 *      Author: philip
 */

#include "gui.h"
#include <string.h>

volatile uint8_t guiMenuState = 0;
volatile uint8_t but1 = 0;
volatile uint8_t but2 = 0;
Button button1;
Button button2;


Button init_button(int x, int y, int w, int h, char* label, uint16_t color){
    Button but;
    but.x1 = x;
    but.x2 = x+w;
    but.y1 = y;
    but.y2 = y+h;
    but.labelLength = strlen(label);
    for (int i = 0; i < but.labelLength; i++) {
    	but.label[i] = label[i];
    }
    but.pressed = 0;
    but.color = color;
    drawRect(x, y, x+w, y+h, color, 1);
    textMode();
	textSetCursor(x, y);
	textEnlarge(2);
	textTransparent(0x8170);
	textWrite(but.label, but.labelLength);
	graphicsMode();
    return but;
}

int check_pressed(Button but, int x, int y) {
    if (but.x1 < x && x < but.x2 && but.y1 < y && y < but.y2){
        return 1;
    }
    else{
        return 0;
    }
}
void update_button(Button but, int x, int y, int w, int h, char*label, uint16_t color){
    but.x1 = x;
    but.x2 = x+w;
    but.y1 = y;
    but.y2 = y+h;
    //but.label = label;
    but.pressed = 0;
    but.color = color;
    drawRect(x, y, x+w, y+h, color, 1);
}

void guiState0Init(void) {
	//clear screen
	fillScreen(GREEN);
	//initialize buttons
	button1 = init_button(10, 350, 200, 80, "button1", GREEN);

}

void guiState1Init(void) {
	//clear screen
	fillScreen(YELLOW);
	//initialize buttons
	button2 = init_button(10,350, 200, 80, "button2", YELLOW);
}

void guiStateHandler(uint8_t state) {
	switch(state) {
		case 0: guiState0Init();
			guiMenuState = 0;
			but1 = 1;
			but2 = 0;
			break;
		case 1: guiState1Init();
			guiMenuState = 1;
			but1 = 0;
			but2 = 1;
			break;
	}

}








void buttonHandler(int xc, int yc) {
	if (but1 != 0) {
		uint8_t temp = button1.pressed;
		button1.pressed = check_pressed(button1, xc, yc);
        if (button1.pressed == 0 && temp != button1.pressed) {
        	guiStateHandler(1);
        }
	}
	if (but2 != 0) {
		uint8_t temp = button2.pressed;
		button2.pressed = check_pressed(button2, xc, yc);
		if (button2.pressed == 0 && temp != button2.pressed) {
			guiStateHandler(0);
		}
	}

}




