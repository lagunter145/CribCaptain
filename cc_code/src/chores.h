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

//extern Roommate roommates[];

typedef struct Chore{
    char name[20];
    char day;
//    uint8_t nameLength;
//    struct Node_LL_rm *done_chore;
//    struct Node_LL_rm *not_done_chore;
//    // linked list of events
//    // queue for chores
} Chore;

typedef struct Event{
    char name[20];
    char day;
} Event;

//typedef struct Chore_List {
//	struct Chore *head;
//};


#endif /* CHORES_H_ */
