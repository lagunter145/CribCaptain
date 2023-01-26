/*
 * touch.h
 *
 *  Created on: Jan 24, 2023
 *      Author: Lauren
 */

#ifndef TOUCH_H_
#define TOUCH_H_

# define READ_TIMES 5
// commands to read x or y for ILI9341 (driver for display)
# define CMD_RDX 0x90
# define CMD_RDY 0xD0

void setup_t_irq(void);
uint16_t LCD_RD_TOUCH_DATA(uint8_t CMD);
uint16_t LCD_RD_XORY(uint8_t xy);
uint8_t LCD_RD_XY(uint16_t *x, uint16_t *y);

#endif /* TOUCH_H_ */
