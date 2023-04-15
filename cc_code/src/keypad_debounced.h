/*
 * keypad.h
 *
 *  Created on: Feb 1, 2023
 *      Author: clair
 */

#ifndef KEYPAD_DEBOUNCED_H_
#define KEYPAD_DEBOUNCED_H_

void enable_ports_keypad();
void setup_tim7(); //setup for TIM7 for keypad
void TIM7_IRQHandler(); //Timer 7 Handler for Keypad

void keypad_values(char); //Given keypad value asserted, do something


#endif
