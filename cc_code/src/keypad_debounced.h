/*
 * keypad.h
 *
 *  Created on: Feb 1, 2023
 *      Author: clair
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

void enable_ports_keypad();
void enable_ports_LED();
void setup_tim7(); //setup for TIM7 for keypad
void TIM7_IRQHandler(); //Timer 7 Handler for Keypad

void keypad_values(char); //Given keypad value asserted, do something

//use this function to light Hex equivalent of button press using keypad_values function
void setn(int32_t pin_num, int32_t val); //sets GPIOA pin_num to val

#endif
