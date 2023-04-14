/*
 * timer.h
 *
 *  Created on: Feb 17, 2023
 *      Author: mrahf
 */

#ifndef TIMER_H_
#define TIMER_H_

typedef enum {
	LOADING,
	MAIN,
	CHECKIN
} stateType;


void setup_tim6();
void setup_external_timesync();
void write_time();



#endif /* TIMER_H_ */
