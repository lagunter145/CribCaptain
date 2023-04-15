/*
 * keypad_support.c
 *
 *  Created on: Mar 22, 2023
 *      Author: clair
 *
 */

/*helper functions for the keypad */

#include "stm32f0xx.h"
#include <string.h> // for memmove()
#include "keypad_support.h"

// 16 history bytes.  Each byte represents the last 8 samples of a button.
uint8_t hist[16];
char queue[2];  // A two-entry queue of button press/release events.
int qin;        // Which queue entry is next for input
int qout;       // Which queue entry is next for output
int n;

const char keymap[] = "DCBA#9630852*741";

void push_queue(int n) {
    queue[qin] = 0;
    qin ^= 1;
}

char pop_queue() {
    char tmp = queue[qout];
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int c, int rows)
{
    // We used to make students do this in assembly language.
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 0x01)
            push_queue(0x80 | keymap[4*c+i]);
        if (hist[4*c+i] == 0xfe)
            push_queue(keymap[4*c+i]);
    }
}

void drive_column(int c)
{
    GPIOC->BSRR = (0xf00000 | (1 << (c + 4))) << 4;
}

int read_rows()
{
    return (GPIOC->IDR & 0xf0) >> 4;
}

char get_key_event(void) {
    for(;;) {
        asm volatile ("wfi" : :);   // wait for an interrupt
       if (queue[qout] == 0)
            continue;
	   break;
    }
    return keymap[(uint8_t)pop_queue()];
}

char get_keypress() {
    char event;
    for(;;) {
        // Wait for every button event...
        event = get_key_event();
        // ...but ignore if it's a release.
        if (event & 0x80)
            break;
    }
    return event & 0x7f;
}


