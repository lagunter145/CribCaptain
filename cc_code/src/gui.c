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
extern uint32_t uid;
extern char uid_str[10];
extern uint8_t rfid_tag[20];
stateType guiMenuState = LOADING;

void draw_button(struct Button *but, uint16_t button_color, uint16_t text_color) {
	//old mode
	//textMode();
	/*
	textEnlarge(2);
	textColor(acce_color, base_color);
	textSetCursor(buttonArr[0].x1, buttonArr[0].y1);
	textTransparent(acce_color);
	strcpy(buttonArr[0].label, "button2");
	buttonArr[0].labelLength = strlen(buttonArr[0].label);
	textWrite(buttonArr[0].label, buttonArr[0].labelLength);
	graphicsMode();
	*/
	//Need to draw the rectangle first
	graphicsMode();
	drawRect(but->x1, but->y1, but->x2, but->y2, button_color, 1);
	//Then draw the text
	textMode();
	textEnlarge(2);
	textSetCursor(but->x1, but->y1);
	textColor(text_color, button_color);
	but->labelLength = strlen(but->label);
	textWrite(but->label, but->labelLength);
	graphicsMode();

}

Button init_button(int x, int y, int w, int h, char* label, uint16_t color){
    Button but;
    but.on = 0;
    but.x1 = x;
    but.x2 = x+w;
    but.y1 = y;
    but.y2 = y+h;
	strcpy(but.label, label);
	but.labelLength = strlen(but.label);
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
    but.color = base_color;
    //drawRect(x, y, x+w, y+h, color, 1);
}

// LOADING screen
void guiLOADINGInit(void) {
	//clear screen
	//fillScreen(GREEN);
	//initialize buttons
	buttonArr[0] = init_button(10, 350, 200, 80, "Hello", MAGENTA);
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
	buttonArr[0] = init_button(10, 350, 200, 80, "button2", base_color);
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

//CHECKIN screen
void guiCHECKINInit(void) {
	////450, 300
	buttonArr[2] = init_button(400, 300, 20, 80, "+", base_color);
	buttonArr[3] = init_button(500, 300, 20, 80, "-", base_color);


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
	draw_button(&(buttonArr[0]), YELLOW, GREEN);
	/*
	textSetCursor(buttonArr[0].x1, buttonArr[0].y1);
	textTransparent(acce_color);
	strcpy(buttonArr[0].label, "button2");
	buttonArr[0].labelLength = strlen(buttonArr[0].label);
	textWrite(buttonArr[0].label, buttonArr[0].labelLength);
	graphicsMode();
	*/

	// clear the button flags
	buttonArr[0].pressed = 0;
	buttonArr[0].on = 1;
	// turn on MAIN button
	buttonArr[1].pressed = 0;
	buttonArr[1].on = 0;
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


	draw_button(&(buttonArr[0]), CYAN, BLUE);
	// draw button
	/*
	textSetCursor(buttonArr[0].x1, buttonArr[0].y1);
	textTransparent(acce_color);
	strcpy(buttonArr[0].label ,"back to main :)");
	buttonArr[0].labelLength = strlen(buttonArr[0].label);
	textWrite(buttonArr[0].label, buttonArr[0].labelLength);
	*/
	graphicsMode();

	// clear the button flags
	buttonArr[1].pressed = 0;
	buttonArr[1].on = 0;
	// turn on MAIN button
	buttonArr[0].pressed = 0;
	buttonArr[0].on= 1;
}


uint8_t numberGuests = 0;

// CHECKIN screen
void guiCHECKINDraw(void) {
	fillScreen(base_color);
	textMode();
	textSetCursor(600, 10);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("CHECKIN", 7);

	textSetCursor(100, 150);
	textWrite(uid_str, rfid_tag[12] * 2);

	textSetCursor(300, 240);
	//textEnlarge(2);
	//textColor(acce_color, base_color);
	textWrite("How many guests?", 16);
	textSetCursor(450, 300);
	char numGuests[2];
	itoa(numberGuests, numGuests, 10);
    textWrite(numGuests, 2);

	//buttons
	textSetCursor(buttonArr[0].x1, buttonArr[0].y1);
	textTransparent(acce_color);
	strcpy(buttonArr[0].label ,"Enter");
	buttonArr[0].labelLength = strlen(buttonArr[0].label);
	textWrite(buttonArr[0].label, buttonArr[0].labelLength);

	textSetCursor(buttonArr[2].x1, buttonArr[2].y1);
	strcpy(buttonArr[2].label ,"+");
	buttonArr[2].labelLength = strlen(buttonArr[2].label);
	textWrite(buttonArr[2].label, buttonArr[2].labelLength);

	textSetCursor(buttonArr[3].x1, buttonArr[3].y1);
	strcpy(buttonArr[3].label ,"-");
	buttonArr[3].labelLength = strlen(buttonArr[3].label);
	textWrite(buttonArr[3].label, buttonArr[3].labelLength);

	buttonArr[0].pressed = 0;
	buttonArr[0].on = 1;
	buttonArr[2].pressed = 0;
	buttonArr[2].on = 1;
	buttonArr[3].pressed = 0;
	buttonArr[3].on = 1;

	graphicsMode();





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
		case CHECKIN:
			guiCHECKINDraw();
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
				guiMAINInit();
				guiMAINDraw();
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			break;
		case MAIN:
			if (buttonArr[0].pressed){
				// switch states
				guiLOADINGInit();
				guiLOADINGDraw();
				guiMenuState = LOADING;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			break;
		case CHECKIN:
			if (card_scanned) {
				guiCHECKINDraw();
			}

			if (buttonArr[0].pressed) { //enter
				guiMAINDraw(); //return to the Main state for now
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			if (buttonArr[2].pressed) { //+
				numberGuests++;
				guiCHECKINDraw();
			}
			if (buttonArr[3].pressed) { //-
				if (numberGuests > 0)
					numberGuests--;
				guiCHECKINDraw();
			}


			// acknowledge touch interrupt
			writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			card_scanned = 0;
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
	if (buttonArr[2].on) {
		if (buttonArr[2].pressed == 0) {
			buttonArr[2].pressed = check_pressed(buttonArr[2], xc, yc);
			if(buttonArr[2].pressed) {
				flag = 1;
			}
		}
		else {
			flag = 1;
		}
	}
	if (buttonArr[3].on) {
		if (buttonArr[3].pressed == 0) {
			buttonArr[3].pressed = check_pressed(buttonArr[3], xc, yc);
			if(buttonArr[3].pressed) {
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




