/*
 * keypad_support.h
 *
 *  Created on: Mar 24, 2023
 *      Author: clair
 */

#ifndef KEYPAD_SUPPORT_H_
#define KEYPAD_SUPPORT_H_


char pop_queue();
void push_queue(int n);
void drive_column(int);   // energize one of the column outputs
int  read_rows(); // read the four row inputs
void update_history(int col, int rows); // record the buttons of the driven column
char get_key_event(void); // wait for a button event (press or release)
char get_keypress();

#endif /* KEYPAD_SUPPORT_H_ */
