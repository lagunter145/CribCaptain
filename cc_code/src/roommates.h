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
#include "chores.h"

#define MAXNUM_ROOMMATES 4
#define MAXNUM_CHORES 4

typedef struct Roommate{
    char name[30];
    uint8_t nameLength;
    uint8_t home;
    char uid_str[10];
    uint8_t num_guests;
    // linked list of events
    // queue for chores
    // dummy holder for events and chores to test gui
    //char* events[2];
    //char* chores[3];
    Event events[2];
    //Chore chores[3];
    uint8_t chores_done[MAXNUM_CHORES];
    uint8_t scheduled[3];
    uint8_t chore_4_today;
} Roommate;

// chores occur DAILY, no exceptions hunny
typedef struct Chore{
    char name[20];
    uint8_t nameLength;
    uint8_t whos_done[MAXNUM_ROOMMATES];
    uint8_t whos_done_num;
    Roommate scheduled[3];
    uint8_t done_today;
//    uint8_t nameLength;
//    struct Node_LL_rm *done_chore;
//    struct Node_LL_rm *not_done_chore;
//    // linked list of events
//    // queue for chores
} Chore;

extern Roommate roommates[MAXNUM_ROOMMATES];
extern Chore chores[MAXNUM_CHORES];

Roommate initRoommate(char* name, char* uid_str);
void getRoommateData();
void saveRoommateData();
void clearChoreData(Chore toBeCleared);

#endif /* ROOMMATES_H_ */
