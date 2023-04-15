/*
 * gui.c
 *
 *  Created on: Mar 22, 2023
 *      Author: philip
 */

#include "gui.h"
#include <string.h>
#include "timer.h"
#include "colours.h"
#include "rfid.h"

extern uint16_t base_color;
extern uint16_t acce_color;
extern uint8_t colorUpdated;
stateType guiMenuState = LOADING;



Button init_button(int x, int y, int w, int h, char* label, uint16_t color){
    Button but;
    but.on = 0;
    but.x1 = x;
    but.x2 = x+w;
    but.y1 = y;
    but.y2 = y+h;
    but.labelLength = strlen(label);
    for (int i = 0; i < but.labelLength; i++) {
    	but.label[i] = label[i];
    }
    but.pressed = 0;
    but.color = base_color;
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
    but.color = base_color;
    //drawRect(x, y, x+w, y+h, color, 1);
}

// LOADING screen
void guiLOADINGInit(void) {
	//clear screen
	//fillScreen(GREEN);
	//initialize buttons
	buttonArr[0] = init_button(10, 350, 200, 80, "No Wifi :(", base_color);
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
	//fillScreen(YELLOW);
	//initialize buttons
	buttonArr[1] = init_button(10, 350, 200, 80, "button2", base_color);
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

void guiMAINDraw(void) {
	// clear screen
	fillScreen(base_color);
	// draw screen
	textMode();
	textSetCursor(600, 10);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("MAIN", 4);

	// draw button
	textSetCursor(buttonArr[0].x1, buttonArr[0].y1);
	textTransparent(acce_color);
	textWrite(buttonArr[0].label, buttonArr[0].labelLength);
	graphicsMode();

	// clear the button flags
	buttonArr[0].pressed = 0;
	buttonArr[0].on = 0;
	// turn on MAIN button
	buttonArr[1].pressed = 0;
	buttonArr[1].on= 1;
}

void guiLOADINGDraw(void) {
	// clear screen
	fillScreen(base_color);
	// draw button
	textMode();
	textSetCursor(600, 10);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("LOADING", 7);

	// draw button
	textSetCursor(buttonArr[1].x1, buttonArr[1].y1);
	textTransparent(acce_color);
	textWrite(buttonArr[1].label, buttonArr[1].labelLength);

	graphicsMode();

	// clear the button flags
	buttonArr[1].pressed = 0;
	buttonArr[1].on = 0;
	// turn on MAIN button
	buttonArr[0].pressed = 0;
	buttonArr[0].on= 1;
}

// CHECKIN screen
void guiCHECKINDraw(void) {
	fillScreen(base_color);
	textMode();
	textSetCursor(600, 10);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("CHECKIN", 7);

	textSetCursor(300, 240);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("How many guests?", 16);




}

void guiState2Init(void) {
	// clear screen
}

void guiRedraw() {
	//redraws the gui to whatever the current state is
	switch(guiMenuState) {
		case LOADING:
			guiLOADINGDraw();
			break;
		case MAIN:
			guiMAINDraw();
			break;
	}
	// acknowledge touch interrupt
	writeReg(RA8875_INTC2, RA8875_INTC2_TP);
}

void guiStateHandler(stateType state) {
//	for (int i = 0; i < 2; i++) {
//		buttonArr[i].on = 0;
//	}

	switch(state) {
		case LOADING: // current state is LOADING
			if (buttonArr[0].pressed){
				// switch states
				guiMAINDraw();
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			break;
		case MAIN:
			if (buttonArr[1].pressed){
				// switch states
				guiLOADINGDraw();
				guiMenuState = LOADING;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			break;
		case CHECKIN:
			guiCHECKINDraw();
			// acknowledge touch interrupt
			writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			break;
	}

	//if the color has been updated from the keypad, then redraw the gui and
	//reset the gloabal variable
	if (colorUpdated) {
		guiRedraw();
		colorUpdated = 0;
	}


}





// buttonArr[0] = (LOADING screen) no wifi -> MAIN screen
// buttonArr[1] = (MAIN screen) Next 3 Days -> 3 Days Screen
// buttonArr[2] = (MAIN screen)
// buttonArr[3] = (CHECKIN screen) Decrease num of guests -> (doesn't change screen)
// buttonArr[4] = (CHECKIN screen) Increase num of guests -> (doesn't change screen)
// buttonArr[5] = (CHECKIN screen) Done with guests checkin in -> MAIN screen


uint8_t buttonHandler(int xc, int yc) {
//	if (buttonArr[0].on != 0) {
//		uint8_t temp = buttonArr[0].pressed;
//		buttonArr[0].pressed = check_pressed(buttonArr[0], xc, yc);
//        if (buttonArr[0].pressed == 0 && temp != buttonArr[0].pressed) {
//        	guiStateHandler(MAIN);
//        }
//	}
//	if (buttonArr[1].on != 0) {
//		uint8_t temp = buttonArr[1].pressed;
//		buttonArr[1].pressed = check_pressed(buttonArr[1], xc, yc);
//		if (buttonArr[1].pressed == 0 && temp != buttonArr[1].pressed) {
//			guiStateHandler(LOADING);
//		}
//	}
	uint8_t flag = 0;	// flag to indicate a GUI state is boutta change
	if (buttonArr[0].on) {
        if (buttonArr[0].pressed == 0) {
        	// if button wasn't pressed, check if it's been pressed
    		buttonArr[0].pressed = check_pressed(buttonArr[0], xc, yc);
    		if(buttonArr[0].pressed) {
    			flag = 1;
    		}
        }
        else {
        	flag = 1;
        }
        // otherwise keep it 'pressed'
	}
	if (buttonArr[1].on) {
		if (buttonArr[1].pressed == 0) {
			buttonArr[1].pressed = check_pressed(buttonArr[1], xc, yc);
			if(buttonArr[1].pressed) {
				flag = 1;
			}
		}
		else {
			flag = 1;
		}
	}
	if (flag) {
		return 1;
	}
	else {
		return 0;
	}
}




