/*
 * roommates.c
 *
 *  Created on: Apr 16, 2023
 *      Author: gunter3
 */

#include <String.h>
#include "roommates.h"

Roommate roommates[MAXNUM_ROOMMATES];
Chore chores[MAXNUM_CHORES];

Roommate initRoommate(char* name, char* uid_str) {
	Roommate rm;
	strcpy(rm.name, name);
	rm.nameLength = strlen(name);
	rm.home = 0;
	strcpy(rm.uid_str, uid_str);
//	strcpy(rm.chores[0].name, "");
//	strcpy(rm.chores[1].name, "");
//	strcpy(rm.chores[2].name, "");
	strcpy(rm.events[0].name, "");
	strcpy(rm.events[1].name, "");
	for(int c = 0; c < MAXNUM_CHORES; c++) {
		rm.chores_done[c] = 0;
	}
	for(int s = 0; s < 3; s++) {
		rm.scheduled[s] = 0;
	}
	rm.chore_4_today = 0;
	//rm.num_chores = 0;
	//rm.events[0].name = "Exam";
//	rm.events[1].name = "Class";
	return rm;
}

Chore initChore(char* name) {
	Chore ch;
	strcpy(ch.name, name);
	ch.nameLength = strlen(name);
	for(int i = 0; i < MAXNUM_ROOMMATES; i++) {
		ch.whos_done[i] = 0;
	}
//	for(int i = 0; i < MAXNUM_CHORES; i++) {
//		ch.scheduled[i] = NULL;
//	}
	ch.done_today = 0;
	ch.whos_done_num = 0;
	return ch;
}

void clearChoreData(Chore toBeCleared) {
	for(int i = 0; i < MAXNUM_ROOMMATES; i++) {
		toBeCleared.whos_done[i] = 0;
	}
	toBeCleared.whos_done_num = 0;
}

void getRoommateData() {
	roommates[0] = initRoommate("Lauren Gunter", "1b1350e0");
	roommates[1] = initRoommate("Philip Yao", "be4ae2e5");
	roommates[2] = initRoommate("Matt Rahfaldt", "1b2ad5b3");
	roommates[3] = initRoommate("Claire Hite", "2ee18421");
	chores[0] = initChore("Dishes");
	chores[1] = initChore("Vacuum");
	chores[2] = initChore("Trash");
	chores[3] = initChore("Feed cat");
}

void saveRoommateData() {

}
