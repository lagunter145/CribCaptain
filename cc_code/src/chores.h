/*
 * chores.h
 *
 *  Created on: Apr 17, 2023
 *      Author: gunter3
 */

#ifndef CHORES_H_
#define CHORES_H_

#include "stm32f0xx.h"
#include "stdlib.h"

// #include "Node_LL.h"

typedef struct Event{
    char name[20];
    char day;
} Event;

// global array of chores
#define MAX_CHORES 10
//extern Chore chore_q [MAX_CHORES];
extern int chore_head;
extern int chore_tail;

void assign_chores();

#endif /* CHORES_H_ */
