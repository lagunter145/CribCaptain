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

Button init_button(int x, int y, int w, int h, char* label, uint16_t color);
int check_pressed(Button but, int x, int y);
void update_button(Button but, int x, int y, int w, int h, char*label, uint16_t color);

#endif /* GUI_H_ */
