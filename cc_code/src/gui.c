/*
 * gui.c
 *
 *  Created on: Mar 22, 2023
 *      Author: philip
 */


#include <stdio.h>
#include <string.h>
#include "gui.h"
#include "timer.h"
#include "colors.h"
#include "rfid.h"
#include "roommates.h"
#include "cc_pic.h"
#include "misc.h"
#include "lcd_7in.h"
#include "esp.h"

stateType guiMenuState = LOADING;
volatile uint8_t show_sec = 0;
volatile uint8_t messaging = 0;
volatile uint8_t piccing = 0;


char* ways_to_say_goodbye[] = {"Look after yourself!",
							"Live long and prosper",
							"Bye bye, cute munchkin",
							"Don't take any elevators today!",
							"Fare thee well",
							"Don't get runover :)",
							"Ok bye, fry guy",
							"Please, take the kids with you",
							"Love, peace, and chicken grease",
							"Shine on, you crazy diamond",
							"Try not to sin today",
							"So long, King Kong",
							"Until next millenia",
							"Later, hater",
							"I'm not programmed to miss you :)",
							"Chop, chop, lollipop"};

char* intruder_msg[] = {	"Are you sure you live here?",
						"Leave, hun",
						"I'm callin' the police",
						"INTRUDER ALERT",
						"Alexa, code RED",
						"404 Not Found",
						"Don't make me grab my bat",
						"Readying the missile system",
						"Rat traps ahead.",
						"No trespassing",
						"Releasing army of rats",
						"Mmm, I don't know you ",
						"Not on the list, bucko",
						"Eat Rocks <3",
						"Miss gurl, u in da wrong place",
						"BARK BARK BARK (Get out)"
};

int ways_to_say_goodbye_size = 16;
int intruder_msg_size = 16;

// timer to control updating the screen
void setup_tim16() {
	// Enable RCC clock to Timer 16
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

	// update the TIM16 prescaler
	TIM16->PSC = 48000 - 1; // 48,000,000 / (12,000 * 1000) = 4 Hz

	// configure the auto-reload register
	TIM16->ARR = 1000 - 1;

	// configure the Timer 16 Interrupt Enable Register
	TIM16->DIER |= TIM_DIER_UIE;

	// enable tim16 by setting the cen bit in the tim16 control register
	TIM16->CR1 |= TIM_CR1_CEN;

	// enable the interrupt for timer 16 in the NVIC ISER
	NVIC->ISER[0] = (1 << TIM16_IRQn);
	NVIC_SetPriority(TIM16_IRQn, 1);
}
void TIM16_IRQHandler(void) {
	// acknowledge the interrupt
	TIM16->SR &= ~TIM_SR_UIF;
	/*
	if (guiMenuState == LOADING && timeAcquired) {
		//wifi is connected
		guiMenuState = MAIN;
		guiMAINInit();
		guiMAINDraw();
		writeReg(RA8875_INTC2, RA8875_INTC2_TP);
	}

	guiStateHandler(guiMenuState);

	if ((guiMenuState == LOADING) && (!piccing)) {
		//write loading to the screen
		write_loading();
	} else if (guiMenuState == MSG) {
		messaging++;
		if(messaging > 5) {
			messaging = 0;
		}
	} else {
		//update the time
		write_time();
	}
	*/
}

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
	// center text in button rect
	uint16_t start_x = (((but->x2 - but->x1) - (24 * but->labelLength)) / 2) + but->x1;
	uint16_t start_y = (((but->y2 - but->y1) - (50)) / 2) + but->y1;
	// textSetCursor(but->x1, but->y1);
	textSetCursor(start_x, start_y);
	textColor(text_color, button_color);
	textWrite(but->label, but->labelLength);
	graphicsMode();

}
void draw_sel_button(int mode, uint16_t button_color, uint16_t text_color) {
	//Need to draw the rectangle first
	Button *but = &buttonArr[mode];
	graphicsMode();
	drawRect(but->x1, but->y1, but->x2, but->y2, text_color, 1);
	drawRect(but->x1, but->y1, but->x2, but->y2, button_color, 0);

	//Then draw the text
	textMode();
	textEnlarge(2);
	// center text in button rect
	uint16_t start_x = (((but->x2 - but->x1) - (24 * but->labelLength)) / 2) + but->x1;
	uint16_t start_y = (((but->y2 - but->y1) - (50)) / 2) + but->y1;
	textSetCursor(start_x, start_y);
	textColor(button_color, text_color);
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
    //drawRect(x, y, x+w, y+h, color, 1);
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
Button init_small_button(int x, int y, int w, int h, char* label, int labelLength, uint16_t color) {
    Button but;
    but.on = 0;
    but.x1 = x;
    but.x2 = x+w;
    but.y1 = y;
    but.y2 = y+h;
	strcpy(but.label, label);
	but.labelLength = labelLength;
    but.pressed = 0;
    but.color = color;
    //drawRect(x, y, x+w, y+h, color, 1);
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
// buttons
// buttonArr[0] is void
// buttonArr[1] = go straight to main screen without connecting to Wifi
void guiLOADINGInit(void) {
	//clear screen
	fillScreen(base_color);
	//bmpDraw("cc_icon.bmp", 200, 200);
	//drawPic(200, 200);
	//initialize buttons
	buttonArr[0] = init_button(0, 0, 250, 100, "", base_color);
	buttonArr[1] = init_button(10, 350, 200, 80, "Hello", acce_color);
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
void guiLOADINGDraw(void) {
	// clear screen
	fillScreen(base_color);
	// draw button
	graphicsMode();
	textMode();
	textSetCursor(600, 10);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("LOADING", 7);


	draw_button(&(buttonArr[1]), base_color, acce_color);
	// draw button
	/*
	textSetCursor(buttonArr[0].x1, buttonArr[0].y1);
	textTransparent(acce_color);
	strcpy(buttonArr[0].label ,"back to main :)");
	buttonArr[0].labelLength = strlen(buttonArr[0].label);
	textWrite(buttonArr[0].label, buttonArr[0].labelLength);
	*/

	// turn off TIME button
	buttonArr[0].pressed = 0;
	buttonArr[0].on= 0;

	// turn on MAIN button
	buttonArr[1].pressed = 0;
	buttonArr[1].on= 1;
}
int pic_size = 50;
int pix_w = 3;
void drawPic(int start_x, int start_y) {
	piccing = 1;
	graphicsMode();
	uint16_t col;
	int i = 0;
	int x = start_x;
	int y = start_y;
	for(int r = 0; r < pic_size; r++) {
		for(int c = 0; c < pic_size; c++) {
			i = (r * 2 * pic_size) + c;
			col = (cc_pic[i] << 8) + cc_pic[i + 1];
			drawRect(x, y, (x + pix_w), (y + pix_w), col, 1);
			x += pix_w;
		}
		x = start_x;
		y += pix_w;
	}
	piccing = 0;
}
#define BUFFPIXEL 5
void bmpDraw(char* filename, int start_x, int start_y) {
	FILE*     bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
	uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
	uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	int  goodBmp = 0;       // Set to true on valid header parse
	int  flip    = 1;        // BMP is stored bottom-to-top
	int      w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0;
	uint8_t  lcdidx = 0;

	if((start_x >= 800) || (start_y >= 480)) return;

	// Open requested file on SD card
	if ((bmpFile = open(filename)) == 0) {
	return;
	}

	uint16_t temp_s;
	uint32_t temp_l;
	// Parse BMP header
	read(bmpFile, temp_s, sizeof(temp_s));
	if(temp_s == 0x4D42) { // BMP signature
	read(bmpFile, temp_l, sizeof(temp_l)); // Read & ignore creator bytes
	read(bmpFile, temp_l, sizeof(temp_l));
	bmpImageoffset = temp_l; // Start of image data

	// Read DIB header
	read(bmpFile, temp_l, sizeof(temp_l));
	bmpWidth  = temp_l;
	read(bmpFile, temp_l, sizeof(temp_l));
	bmpHeight = temp_l;

	read(bmpFile, temp_s, sizeof(temp_s));
	if(temp_s == 1) { // # planes -- must be '1'
	  read(bmpFile, temp_s, sizeof(temp_s));
	  bmpDepth = temp_s; // bits per pixel
	  read(bmpFile, temp_l, sizeof(temp_l));
	  if((bmpDepth == 24) && (temp_l == 0)) { // 0 = uncompressed
		goodBmp = 1; // Supported BMP format -- proceed!

		// BMP rows are padded (if needed) to 4-byte boundary
		rowSize = (bmpWidth * 3 + 3) & ~3;

		// If bmpHeight is negative, image is in top-down order.
		// This is not canon but has been observed in the wild.
		if(bmpHeight < 0) {
		  bmpHeight = -bmpHeight;
		  flip      = 0;
		}

		// Crop area to be loaded
		w = bmpWidth;
		h = bmpHeight;
		if((start_x+w-1) >= 800)  w = 800  - start_x;
		if((start_y+h-1) >= 480) h = 480 - start_y;

		// Set TFT address window to clipped image bounds

		for (row=0; row<h; row++) { // For each scanline...
		  // Seek to start of scan line.  It might seem labor-
		  // intensive to be doing this on every line, but this
		  // method covers a lot of gritty details like cropping
		  // and scanline padding.  Also, the seek only takes
		  // place if the file position actually needs to change
		  // (avoids a lot of cluster math in SD library).
		  if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
			pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
		  else     // Bitmap is stored top-to-bottom
		  pos = bmpImageoffset + row * rowSize;
		  if(ftell(bmpFile) != pos) { // Need seek?
			fseek(bmpFile,pos,SEEK_END);
			buffidx = sizeof(sdbuffer); // Force buffer reload
		  }

		  for (col=0; col<w; col++) { // For each column...
			// Time to read more pixel data?
			if (buffidx >= sizeof(sdbuffer)) { // Indeed
			  // Push LCD buffer to the display first
			  if(lcdidx > 0) {
				drawRect(col+start_x, row+start_y, col+start_x+1, row+start_y+1, lcdbuffer[lcdidx],1);
				lcdidx = 0;
			  }

			  read(bmpFile, sdbuffer, sizeof(sdbuffer));
			  buffidx = 0; // Set index to beginning
			}

			// Convert pixel from BMP to TFT format
			b = sdbuffer[buffidx++];
			g = sdbuffer[buffidx++];
			r = sdbuffer[buffidx++];
			lcdbuffer[lcdidx] = color565(r,g,b);
			drawRect(col+start_x, row+start_y, col+start_x+1, row+start_y+1, lcdbuffer[lcdidx],1);
		  } // end pixel

		} // end scanline

		// Write any remaining data to LCD
		if(lcdidx > 0) {
			drawRect(col+start_x, row+start_y, col+start_x+1, row+start_y+1, lcdbuffer[lcdidx],1);
		}

	  } // end goodBmp
	}
	}

	close(bmpFile);
}
uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


// MAIN screen
// buttons
// buttonArr[0] = second toggling for displaying time
// buttonArr[1] = back to Loading screen
// buttonArr[2] = Calendar screen
// buttonArr[3] = Roommates screen
void guiMAINInit(void) {
	//clear screen
	//fillScreen(YELLOW);
	//initialize buttons
	buttonArr[0] = init_button(0, 0, 300, 200, "", base_color);
	buttonArr[1] = init_button(25, 350, 200, 80, "Loading", acce_color);
	buttonArr[2] = init_button(275, 350, 200, 80, "Calendar", acce_color);
	buttonArr[3] = init_button(525, 350, 250, 80, "Roommates", acce_color);

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
	// draw button
	draw_button(&(buttonArr[1]), base_color, acce_color);
	draw_button(&(buttonArr[1]), acce_color, base_color);
	draw_button(&(buttonArr[2]), acce_color, base_color);
	draw_button(&(buttonArr[3]), acce_color, base_color);
	int x_start = rightALIGN(800, strlen("MAIN"));
	textMode();
	textSetCursor(x_start, 10);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("MAIN", 4);

//	textSetCursor(buttonArr[0].x1, buttonArr[0].y1);
//	textTransparent(base_color);
//	strcpy(buttonArr[1].label, "button2");
//	buttonArr[1].labelLength = strlen(buttonArr[1].label);
//	textWrite(buttonArr[1].label, buttonArr[1].labelLength);
	graphicsMode();


	// clear the button flags
	buttonArr[0].pressed = 0;
	buttonArr[0].on = 1;
	buttonArr[1].pressed = 0;
	buttonArr[1].on = 1;
	buttonArr[2].pressed = 0;
	buttonArr[2].on = 1;
	buttonArr[3].pressed = 0;
	buttonArr[3].on = 1;


}

uint8_t numberGuests = 0;
uint8_t found_rm = 0;
Roommate* temp_rm;
// CHECKIN screen
// buttons
// buttonArr[0] = second toggling for displaying time
// buttonArr[1] = saves number of guests that roommate brought, exits to main screen
// buttonArr[2] = decrement number of guests that roommate brought
// buttonArr[3] = increment number of guests that roommate brought
void guiCHECKINInit(void) {
	////450, 300
	buttonArr[0] = init_button(0, 0, 250, 100, "", base_color);
	buttonArr[1] = init_button(288, 300, 150, 80, "Enter", acce_color);
	buttonArr[2] = init_button(250, 200, 70, 80, "-", acce_color);
	buttonArr[3] = init_button(400, 200, 70, 80, "+", acce_color);
}
int srand_set = 0;
void guiCHECKINDraw(void) {
	if(!srand_set) {
		srand(2147483647 / second);
	}
	for(int i = 0; i < MAXNUM_ROOMMATES; i++) {
		if(strcmp(roommates[i].uid_str, uid_str) == 0) {
			temp_rm = &roommates[i];
			found_rm = 1;
			toggle_pin(GPIOA, (i + 5));
		}
	}
	if(!found_rm) { // miss gurl, you were not found in the dAtAbAsE
		//fillScreen(RED);
		char* msg = intruder_msg[rand() % intruder_msg_size];
		//char* msg = "BAD";
		guiMSGInit(msg);
		guiMenuState = MSG;
		return;
	}
	if(temp_rm->home){	// check out
		temp_rm->home = 0;
		temp_rm->num_guests = 0;
		char* msg = ways_to_say_goodbye[rand() % ways_to_say_goodbye_size];
		http_setupcheckin(temp_rm->uid_str, 0, 0);
		//char* msg = "Bye Felicia";
		guiMSGInit(msg);
		guiMenuState = MSG;
		found_rm = 0;
		return;
	} else {
		fillScreen(base_color);
		draw_button(&(buttonArr[1]), acce_color, base_color);
		draw_button(&(buttonArr[2]), acce_color, base_color);
		draw_button(&(buttonArr[3]), acce_color, base_color);
		int x_start = rightALIGN(800, strlen("CHECKIN"));
		textMode();
		textSetCursor(x_start, 10);
		textEnlarge(2);
		textColor(acce_color, base_color);
		textWrite("CHECKIN", 7);
		textSetCursor(100, 80);
		textWrite(temp_rm->name, temp_rm->nameLength);
		temp_rm->home = 1;
		textSetCursor(200, 140);
		//textEnlarge(2);
		//textColor(acce_color, base_color);
		textWrite("How many guests?", 16);
		textSetCursor(350, 215);
		char numGuests[2];
		itoa(numberGuests, numGuests, 10);
	    textWrite(numGuests, 2);
	}

	buttonArr[0].pressed = 0;
	buttonArr[0].on = 1;
	buttonArr[1].pressed = 0;
	buttonArr[1].on = 1;
	buttonArr[2].pressed = 0;
	buttonArr[2].on = 1;
	buttonArr[3].pressed = 0;
	buttonArr[3].on = 1;

	graphicsMode();
}

// CALENDAR Screen
// buttons
// buttonArr[0] = toggling seconds for time display
// buttonArr[1] = go back to Main
// buttonArr[2] = show all events and chores
// buttonArr[3] = show roommate 1 events and chores
// buttonArr[4] = show roommate 2 events and chores
// buttonArr[5] = show roommate 3 events and chores
// buttonArr[6] = show roommate 4 events and chores
void guiCALENDARInit() {
	buttonArr[0] = init_button(0, 0, 250, 100, "", base_color);
	buttonArr[1] = init_button(25, 390, 125, 75, "MAIN", acce_color);
	buttonArr[2] = init_small_button(200, 390, 100, 75, "ALL", 3, acce_color);
	buttonArr[3] = init_small_button(325, 390, 100, 75, roommates[0].name, 3, acce_color);
	buttonArr[4] = init_small_button(450, 390, 100, 75, roommates[1].name, 3, acce_color);
	buttonArr[5] = init_small_button(575, 390, 100, 75, roommates[2].name, 3, acce_color);
	buttonArr[6] = init_small_button(700, 390, 100, 75, roommates[3].name, 3, acce_color);

	buttonArr[0].pressed = 0;
	buttonArr[0].on = 1;
	buttonArr[1].pressed = 0;
	buttonArr[1].on = 1;
	buttonArr[2].pressed = 0;
	buttonArr[2].on = 1;
	buttonArr[3].pressed = 0;
	buttonArr[3].on = 1;
	buttonArr[4].pressed = 0;
	buttonArr[4].on = 1;
	buttonArr[5].pressed = 0;
	buttonArr[5].on = 1;
	buttonArr[6].pressed = 0;
	buttonArr[6].on = 1;
}
int old_mode = 0;
void guiCALENDARDraw(int mode, int redraw) {
	if(redraw) {
		fillScreen(base_color);
		for(int i = 1; i < 7; i++) {
			if(i != mode) {
				draw_button(&buttonArr[i], acce_color, base_color);
			}
		}
		draw_sel_button(mode, acce_color, base_color);
		int x_start = rightALIGN(800, strlen("CALENDAR"));
		textMode();
		textSetCursor(x_start, 10);
		textEnlarge(2);
		textColor(acce_color, base_color);
		textWrite("CALENDAR", 8);

		textSetCursor(127, 64);
		textWrite("MON", 3);

		textSetCursor(382, 64);
		textWrite("TUE", 3);

		textSetCursor(640, 64);
		textWrite("WED", 3);

		graphicsMode();

		drawRect(30, 62, 280, 370, acce_color, 0); // box that surrounds left day
		drawRect(285, 62, 535, 370, acce_color, 0); // box that surrounds middle day
		drawRect(540, 62, 790, 370, acce_color, 0); // box that surrounds right day


	}
	if(old_mode != mode) {
		if(old_mode > 0) {
			draw_button(&buttonArr[old_mode], acce_color, base_color);
		}
		draw_sel_button(mode, acce_color, base_color);
		// draw over data in boxes
		drawRect(31, 107, 279, 369, base_color, 1);
		drawRect(286, 107, 534, 369, base_color, 1);
		drawRect(541, 107, 789, 369, base_color, 1);
		textMode();
		int lm_x = 33;  // left most x for table
		int um_y = 105; // upper most y for table
		int rm_x = 277; // right most x for table
		int bm_y = 320; // bottom most y for table
		int x = lm_x;
		int y = um_y;
		int move_y = 25;
		textEnlarge(0);
		textColor(acce_color, base_color);
		int max_rows = 0;
		int max_length = 15;
		int length = 0;
		char day_arr[3] = {'m', 't', 'w'};
		// repeatedly draw data for all 3 days
		for(int d = 0; d < 3; d++) {
			for(int i = 0; i < MAXNUM_ROOMMATES; i++) {
				// draw data if all roommates are selected or if one roommate is
				if((mode == 2) || (mode == (i + 3))) {
					// iterate through chores of roommate
					for(int c = 0; (c < (sizeof(roommates[i].chores) / sizeof(roommates[i].chores[0]))) && (!max_rows); c++) {
						// don't print chore if it's null or day doesn't match
						if (strcmp(roommates[i].chores[c].name, NULL) && roommates[i].chores[c ].day == day_arr[d]) {
						textSetCursor(x, y);
						if(mode == 2) {
							textWrite(roommates[i].name, 3);
							textWrite(": ", 2);
							max_length -= 5;
						}
						if(strlen(roommates[i].chores[c].name) > max_length) {
							length = max_length;
						} else {
							length = strlen(roommates[i].chores[c].name);
						}
						textWrite(roommates[i].chores[c].name, length);
						y += move_y;
						x = lm_x;
						if(y > bm_y) {
							max_rows = 1;
						}
						max_length = 15;
						}
					}
					// iterate through events of roommate
					for(int e = 0; (e < (sizeof(roommates[i].events) / sizeof(roommates[i].events[0]))) && (!max_rows); e++) {
						if (strcmp(roommates[i].events[e].name, NULL) && roommates[i].events[e].day == day_arr[d]) {
						textSetCursor(x, y);
						if(mode == 2) {
							textWrite(roommates[i].name, 3);
							textWrite(": ", 2);
							max_length -= 5;
						}
						if(strlen(roommates[i].events[e].name) > max_length) {
							length = max_length;
						} else {
							length = strlen(roommates[i].events[e].name);
						}
						textWrite(roommates[i].events[e].name, length);
						y += move_y;
						x = lm_x;
						if(y > bm_y) {
							max_rows = 1;
						}
						max_length = 15;
						}
					}
				}
			}
			y = um_y;
			lm_x += 255;
			rm_x += 255;
			x = lm_x;
			max_rows = 0;
		}
	}

	old_mode = mode;
}

// ROOMMATES Screen
// letter = 24x50
// buttons
// buttonArr[0] = toggling seconds for time display
// buttonArr[1] = go back to Main
void guiROOMMATESInit() {
	buttonArr[0] = init_button(0, 0, 250, 100, "", base_color);
	buttonArr[1] = init_button(25, 400 ,125, 75, "MAIN", acce_color);
}

void guiROOMMATESDraw() {
	fillScreen(base_color);
	draw_button(&(buttonArr[1]), acce_color, base_color);
	int x_start = rightALIGN(800, strlen("ROOMMATES"));
	textMode();
	textSetCursor(x_start, 10);
	textEnlarge(2);
	textColor(acce_color, base_color);
	textWrite("ROOMMATES", 9);

	textSetCursor(50, 65);
	textWrite("Roommate", 8);
	textSetCursor(300, 65);
	textWrite("Home", 4);
	textSetCursor(450, 65);
	textWrite("Away", 4);
	textSetCursor(600, 65);
	textWrite("Guests", 8);
	// display roommate names
	int y = 130;
	int length = 0;
	for(int i = 0; i < MAXNUM_ROOMMATES; i++) {
		if(roommates[0].nameLength > 9) {
			length = 9;
		} else {
			length = roommates[i].nameLength;
		}
		textSetCursor(50, y);
		textWrite(roommates[i].name, length);
		y += 60;
	}

	// DISPLAY ! THAT ! DATA !
	uint16_t height = 150;
	char numGuests[2];
	for(int i = 0; i < MAXNUM_ROOMMATES; i++) {
		// first place checkmark in right spot
		if(roommates[i].home) {
			checkmark(338, height);
			// if home, write their number of guests
			itoa(roommates[i].num_guests, numGuests, 10);
			if(roommates[i].num_guests < 10) {
				textSetCursor(660, (height - 20));
				textWrite(numGuests, 2);
			} else {
				textSetCursor(645, (height - 20));
				textWrite(numGuests, 2);
			}

		} else {
			checkmark(485, height);
			// if away, write no guests
			textSetCursor(660, (height - 20));
			textWrite("-", 1);
		}
		height += 58;
	}

	graphicsMode();

	// make her look like a table
	drawRect(40, 62, 775, 363, acce_color, 0); // box that surrounds entire thing
	drawRect(40, 130, 775, 184, acce_color, 0); // box that surrounds second row
	drawRect(40, 245, 775, 304, acce_color, 0); // box that surrounds fourth row
	drawRect(280, 62, 420, 363, acce_color, 0); // box that surrounds second col
	drawRect(575, 62, 775, 363, acce_color, 0); // box that surrounds fourth col


	//checkmark(300, 225);

	buttonArr[0].pressed = 0;
	buttonArr[0].on = 1;
	buttonArr[1].pressed = 0;
	buttonArr[1].on = 1;
}
void checkmark(uint16_t start_x, uint16_t start_y) {
	uint8_t w = 4;
	drawRect(start_x, start_y, start_x + w, start_y + w, acce_color, 1);
	drawRect(start_x + w, start_y + w, start_x + (2 * w), start_y + (2 * w), acce_color, 1);
	drawRect(start_x + (2 * w), start_y + (2 * w), start_x + (3 * w), start_y + (3 * w), acce_color, 1);
	drawRect(start_x + (3 * w), start_y + (1 * w), start_x + (4 * w), start_y + (2 * w), acce_color, 1);
	drawRect(start_x + (4 * w), start_y + (0 * w), start_x + (5 * w), start_y + (1 * w), acce_color, 1);
	drawRect(start_x + (5 * w), start_y + (-1 * w), start_x + (6 * w), start_y + (0 * w), acce_color, 1);
	drawRect(start_x + (6 * w), start_y + (-2 * w), start_x + (7 * w), start_y + (-1 * w), acce_color, 1);
}

// MSG screen
// buttons
void guiMSGInit(char* msg) {
	//clear screen
	fillScreen(acce_color);
	//initialize buttons
//	buttonArr[0] = init_button(0, 0, 250, 100, "", base_color);
//	buttonArr[1] = init_button(10, 350, 200, 80, "Hello", acce_color);
	textMode();
	textSetCursor((((800) - (24 * strlen(msg))) / 2), 200);
	//textSetCursor(100,100);
	textEnlarge(2);
	textColor(base_color, acce_color);
	textWrite(msg, strlen(msg));
	//textWrite(msg, 10);
	graphicsMode();
	buttonArr[0].pressed = 0;
	buttonArr[0].on= 0;
	buttonArr[1].pressed = 0;
	buttonArr[1].on= 0;
	buttonArr[2].pressed = 0;
	buttonArr[2].on= 0;
	buttonArr[3].pressed = 0;
	buttonArr[3].on= 0;
	messaging = 1;
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
			break;
		case CALENDAR:
			old_mode = -1;
			guiCALENDARDraw(2, 1);
			break;
		case ROOMMATES:
			guiROOMMATESDraw();
			break;
}
	// acknowledge touch interrupt
	writeReg(RA8875_INTC2, RA8875_INTC2_TP);
}

int rightALIGN(int right_side, uint8_t string_length){
	int x_start = right_side - 24*string_length;
	return x_start;
}

// =======================================================================================
// GUI State Handler is called to draw the GUI according to the input GUI State variable
// Structure:
// - switch statement selects current GUI State
// - check if the buttons used in the current GUI state have been pressed
//		- if so, call appropriate drawing functions to update screen, or switch GUI states
//			- also acknowledge touch interrupt for RA8875
//			- maybe reset the button pressed parameter (if not calling a guiInit() function)
//		- if not, do nothing
// =======================================================================================
void guiStateHandler(stateType state) {
	switch(state) {
		case LOADING: // current state is LOADING
			if (buttonArr[1].pressed){
				// switch states
				guiMAINInit();
				guiMAINDraw();
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			break;
		case MAIN:
			// if time button is pressed, show/unshow seconds
			if (buttonArr[0].pressed){
				buttonArr[0].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[1].pressed){
				// switch states
				guiLOADINGInit();
				guiLOADINGDraw();
				guiMenuState = LOADING;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[2].pressed) {
				// switch states
				old_mode = 0;
				guiCALENDARInit();
				guiCALENDARDraw(2, 1);
				guiMenuState = CALENDAR;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[3].pressed) {
				buttonArr[3].pressed = 0;
				// switch states
				guiROOMMATESInit();
				guiROOMMATESDraw();
				guiMenuState = ROOMMATES;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}

			break;
		case CHECKIN:
			if (card_scanned) {
				guiCHECKINInit();
				guiCHECKINDraw();
			} else if (buttonArr[0].pressed){
				buttonArr[0].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[1].pressed) { //enter
				if(found_rm != 0) {
					temp_rm->num_guests = numberGuests;
					temp_rm->home = 1;
				}
				http_setupcheckin(temp_rm->uid_str, temp_rm->home, temp_rm->num_guests);
				found_rm = 0;
				temp_rm = NULL;
				numberGuests = 0;
				guiMAINInit();
				guiMAINDraw(); //return to the Main state for now
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[2].pressed) { //-
				if (numberGuests > 0)
					numberGuests--;
				if(numberGuests == 9) {
					drawRect(335, 215, 385, 265, base_color, 1);
				}
				textMode();
				textEnlarge(2);
				textColor(acce_color, base_color);
				char numGuests[2];
				itoa(numberGuests, numGuests, 10);
				if(numberGuests < 10) {
					textSetCursor(350, 215);
					textWrite(numGuests, 1);
				} else {
					textSetCursor(335, 215);
					textWrite(numGuests, 2);
				}
				graphicsMode();
				buttonArr[2].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[3].pressed) { //+
				if (numberGuests < 25)
					numberGuests++;
				textMode();
				textEnlarge(2);
				textColor(acce_color, base_color);
				char numGuests[2];
				itoa(numberGuests, numGuests, 10);
				if(numberGuests < 10) {
					textSetCursor(350, 215);
					textWrite(numGuests, 1);
				} else {
					textSetCursor(335, 215);
					textWrite(numGuests, 2);
				}
				graphicsMode();
				buttonArr[3].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			card_scanned = 0;
			break;
		case CALENDAR:
			if(buttonArr[0].pressed) {
				buttonArr[0].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[1].pressed) {
				guiMAINInit();
				guiMAINDraw(); //return to the Main state for now
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[2].pressed) {
				guiCALENDARDraw(2, 0);
				buttonArr[2].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[3].pressed) {
				guiCALENDARDraw(3, 0);		// have to reset the 'pressed' state
				buttonArr[3].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[4].pressed) {
				guiCALENDARDraw(4, 0);
				buttonArr[4].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[5].pressed) {
				guiCALENDARDraw(5, 0);
				buttonArr[5].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[6].pressed) {
				guiCALENDARDraw(6, 0);
				buttonArr[6].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			break;
		case ROOMMATES:
			if(buttonArr[0].pressed) {
				buttonArr[0].pressed = 0;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			} else if (buttonArr[1].pressed) {
				buttonArr[1].pressed = 0;
				guiMAINInit();
				guiMAINDraw(); //return to the Main state for now
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
			break;
		case MSG:
			if(messaging == 0) {
				guiMAINInit();
				guiMAINDraw(); //return to the Main state for now
				guiMenuState = MAIN;
				// acknowledge touch interrupt
				writeReg(RA8875_INTC2, RA8875_INTC2_TP);
			}
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
	uint8_t state_flag = 0;	// flag to indicate a GUI state is boutta change
	if (buttonArr[0].on) {
        if (buttonArr[0].pressed == 0) {
        	// if button wasn't pressed, check if it's been pressed
    		buttonArr[0].pressed = check_pressed(buttonArr[0], xc, yc);
    		if(buttonArr[0].pressed) {
    			if(show_sec) {
					show_sec = 0;
				} else {
					show_sec = 1;
				}
    			return 1;
    		}
        }
        else {
			if(show_sec) {
				show_sec = 0;
			} else {
				show_sec = 1;
			}
			return 1;
        }
        // otherwise keep it 'pressed'
	}
	if (buttonArr[1].on) {
		if (buttonArr[1].pressed == 0) {
			buttonArr[1].pressed = check_pressed(buttonArr[1], xc, yc);
			if(buttonArr[1].pressed) {
				state_flag = 1;
				return 1;
			}
		}
		else {
			state_flag = 1;
			return 1;
		}
	}
	if (buttonArr[2].on) {
		if (buttonArr[2].pressed == 0) {
			buttonArr[2].pressed = check_pressed(buttonArr[2], xc, yc);
			if(buttonArr[2].pressed) {
				state_flag = 1;
				return 1;
			}
		}
		else {
			state_flag = 1;
			return 1;
		}
	}
	if (buttonArr[3].on) {
		if (buttonArr[3].pressed == 0) {
			buttonArr[3].pressed = check_pressed(buttonArr[3], xc, yc);
			if(buttonArr[3].pressed) {
				state_flag = 1;
				return 1;
			}
		}
		else {
			state_flag = 1;
			return 1;
		}
	}
	if (buttonArr[4].on) {
		if (buttonArr[4].pressed == 0) {
			buttonArr[4].pressed = check_pressed(buttonArr[4], xc, yc);
			if(buttonArr[4].pressed) {
				state_flag = 1;
				return 1;
			}
		}
		else {
			state_flag = 1;
			return 1;
		}
	}
	if (buttonArr[5].on) {
		if (buttonArr[5].pressed == 0) {
			buttonArr[5].pressed = check_pressed(buttonArr[5], xc, yc);
			if(buttonArr[5].pressed) {
				state_flag = 1;
				return 1;
			}
		}
		else {
			state_flag = 1;
			return 1;
		}
	}
	if (buttonArr[6].on) {
		if (buttonArr[6].pressed == 0) {
			buttonArr[6].pressed = check_pressed(buttonArr[6], xc, yc);
			if(buttonArr[6].pressed) {
				state_flag = 1;
				return 1;
			}
		}
		else {
			state_flag = 1;
			return 1;
		}
	}
	if (state_flag) {
		return 1;
	}
	//drawCircle(xc, yc, 4, acce_color, 1);
	//writeReg(RA8875_INTC2, RA8875_INTC2_TP);
	return 0;
}





