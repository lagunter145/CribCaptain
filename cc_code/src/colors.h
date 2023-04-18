/*
 * colours.h
 *
 *  Created on: Apr 13, 2023
 *      Author: gunter3
 */

#ifndef COLORS_H_
#define COLORS_H_

extern volatile uint16_t base_color; //background
extern volatile uint16_t acce_color; //text
extern volatile uint8_t colorUpdated;
extern uint16_t color_pairs[16][2];
#endif /* COLORS_H_ */
