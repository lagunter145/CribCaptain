/*
 * timer.h
 *
 *  Created on: Feb 17, 2023
 *      Author: mrahf
 */

#ifndef TIMER_H_
#define TIMER_H_


extern volatile uint8_t show_sec;
extern volatile char wifiConnected;
extern volatile char timeAcquired;
extern volatile uint8_t messaging;
extern volatile int second;
extern volatile int jiffy;
extern volatile char url[200];
extern volatile int dayOfWeek;

void setup_tim6();
void setup_external_timesync();
void write_time();



#endif /* TIMER_H_ */
