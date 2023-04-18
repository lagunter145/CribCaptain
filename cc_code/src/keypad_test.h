/*
 * keypad_test.h
 *
 *  Created on: Apr 14, 2023
 *      Author: gunter3
 */

#ifndef KEYPAD_TEST_H_
#define KEYPAD_TEST_H_

void enable_ports_keypad_LED(); //enables ports for keypad and LED
void setup_tim7(); //setup for TIM7 for keypad
void TIM7_IRQHandler(); //Timer 7 Handler for Keypad
int read_rows(); //reads row values of the keypad
void drive_column(int c); //set one column of keypad to high at a time
int Keypad_to_LEDs(int col_val, int row1, int row2, int row3, int row4); //key to convert from keypad button asserted to HEX on LED
void setn(int32_t pin_num, int32_t val); //sets GPIO pin_num to val
void mysleep(void); //slows down scanning

#endif /* KEYPAD_TEST_H_ */
