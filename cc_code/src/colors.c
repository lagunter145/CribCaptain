/*
 * colors.c
 *
 *  Created on: Apr 14, 2023
 *      Author: gunter3
 */

#include "stm32f0xx.h"
#include <stdint.h>
#include "colors.h"

volatile uint16_t base_color = 0x0000; //background
volatile uint16_t acce_color = 0xFFFF; //text
volatile uint8_t colorUpdated = 0;

// color pairs, 1st color is base and 2nd is accent
uint16_t color_pairs[16][2] = {
		// Black and White : "1"
		{0x0000, 0xFFFF},
		// White and Black : "2"
		{0xFFFF, 0x0000},
		// Black and Green : "3"
		{0x0000, 0x07E0},
		// Lavendar and Teal : "4"
		{0xE69F, 0x33AE},
		// Peach and Burnt Orange : "5"
		{0xFF7B, 0xEA66},
		// Bubblegum Pink and Cherry Red : "6"
		{0xF639, 0xC987},
		// Moss Green and Forest Green : "7"
		{0x95EC, 0x2AE5},
		// Pale Yellow and Royal Blue : "8"
		{0xFFD7, 0x226E},
		// Orange and Warm Yellow * : " 9"
		{0xEBCB, 0xF5C8},
		// Pale Yellow and Soft Pink : "A"
		{0xFF6F, 0xE30E},
		// Brown and Light Pink : "B"
		{0x5123, 0xFF5C},
		// Graystone and Emerald : "C"
		{0xDEDB, 0x4C0A},
		// Orange and Warm Yellow * : "D"
		{ 0xF5C8, 0xEBCB},
		// Pale Yellow and Soft Pink : "E"
		{0xE30E, 0xFF6F},
		// Brown and Light Pink : "F"
		{0xFF5C, 0x5123},
		// Graystone and Emerald
		{0x4C0A, 0xDEDB}
};

