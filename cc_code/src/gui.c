/*
 * gui.c
 *
 *  Created on: Mar 22, 2023
 *      Author: philip
 */

#include "gui.h"
#include <string.h>
#include "timer.h"
#define NUM_BUT  15

stateType guiMenuState = LOADING;
Button buttonArr[NUM_BUT];


Button init_button(int x, int y, int w, int h, char* label, uint16_t color){
    Button but;
    but.on = 1;
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
    /*
    textMode();
	textSetCursor(x, y);
	textEnlarge(2);
	textTransparent(0x8170);
	textWrite(but.label, but.labelLength);
	graphicsMode();
	*/

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

// LOADING screen
void guiLOADINGInit(void) {
	//clear screen
	fillScreen(GREEN);
	//initialize buttons
	buttonArr[0] = init_button(10, 350, 200, 80, "No Wifi :(", GREEN);
	/*
	textMode();
	textSetCursor(600, 10);
	textEnlarge(2);
	textColor(0x8170, RA8875_WHITE);
	textWrite("LOADIN'", 7);
	//graphicsMode();
	 *
	 */
}

// MAIN screen
void guiMAINInit(void) {
	//clear screen
	fillScreen(YELLOW);
	//initialize buttons
	buttonArr[1] = init_button(10,350, 200, 80, "button2", YELLOW);
	/*
	textMode();
	textSetCursor(600, 10);
	textEnlarge(2);
	textColor(0x8170, RA8875_WHITE);
	textWrite("MAIN", 4);
	graphicsMode();

	write_time();
	*/
}

// CHECKIN screen
void guiState2Init(void) {
	// clear screen
}

void guiStateHandler(stateType state) {
	for (int i = 0; i < NUM_BUT; i++) {
		buttonArr[i].on = 0;
	}
	switch(state) {
		case LOADING: guiLOADINGInit();
			guiMenuState = LOADING;
			break;
		case MAIN: guiMAINInit();
			guiMenuState = MAIN;
			break;
	}

}





// buttonArr[0] = (LOADING screen) no wifi -> MAIN screen
// buttonArr[1] = (MAIN screen) Next 3 Days -> 3 Days Screen
// buttonArr[2] = (MAIN screen)
// buttonArr[3] = (CHECKIN screen) Decrease num of guests -> (doesn't change screen)
// buttonArr[4] = (CHECKIN screen) Increase num of guests -> (doesn't change screen)
// buttonArr[5] = (CHECKIN screen) Done with guests checkin in -> MAIN screen


void buttonHandler(int xc, int yc) {
	if (buttonArr[0].on != 0) {
		uint8_t temp = buttonArr[0].pressed;
		buttonArr[0].pressed = check_pressed(buttonArr[0], xc, yc);
        if (buttonArr[0].pressed == 0 && temp != buttonArr[0].pressed) {
        	guiStateHandler(MAIN);
        }
	}
	if (buttonArr[1].on != 0) {
		uint8_t temp = buttonArr[1].pressed;
		buttonArr[1].pressed = check_pressed(buttonArr[1], xc, yc);
		if (buttonArr[1].pressed == 0 && temp != buttonArr[1].pressed) {
			guiStateHandler(LOADING);
		}
	}

}




