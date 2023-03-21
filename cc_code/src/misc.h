/*
 * misc.h
 *
 *  Created on: Mar 9, 2023
 *      Author: mrahf
 */

#ifndef MISC_H_
#define MISC_H_

void setup_devboard_leds();
void set_pin(GPIO_TypeDef * gpio, char pin, char on);
void toggle_pin(GPIO_TypeDef * gpio, char pin);

#endif /* MISC_H_ */
