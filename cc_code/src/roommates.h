/*
 * roommates.h
 *
 *  Created on: Apr 16, 2023
 *      Author: gunter3
 */

#ifndef ROOMMATES_H_
#define ROOMMATES_H_

#include "stm32f0xx.h"
#include "stdlib.h"

#define MAXNUM_ROOMMATES 4

typedef struct Roommate{
    char name[9];
    uint8_t nameLength;
    uint8_t home;
    char uid_str[10];
    uint8_t num_guests;
    // linked list of events
    // queue for chores
    // dummy holder for events and chores to test gui
    char* events[2];
    char* chores[2];
} Roommate;

Roommate initRoommate(char* name, char* uid_str);
void getRoommateData();
void saveRoommateData();


#endif /* ROOMMATES_H_ */
