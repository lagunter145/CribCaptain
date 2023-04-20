/*
 * roommates.c
 *
 *  Created on: Apr 16, 2023
 *      Author: gunter3
 */

#include <String.h>
#include "roommates.h"

Roommate roommates[MAXNUM_ROOMMATES];

Roommate initRoommate(char* name, char* uid_str) {
	Roommate rm;
	strcpy(rm.name, name);
	rm.nameLength = strlen(name);
	rm.home = 0;
	strcpy(rm.uid_str, uid_str);
	strcpy(rm.chores[0].name, NULL);
	strcpy(rm.chores[1].name, NULL);
	strcpy(rm.chores[2].name, NULL);
	//rm.events[0].name = "Exam";
//	rm.events[1].name = "Class";
	return rm;
}

void getRoommateData() {
	roommates[0] = initRoommate("Lauren Gunter", "1b1350e0");
	roommates[1] = initRoommate("Philip Yao", "be4ae2e5");
	roommates[2] = initRoommate("Matt Pissboy", "1b2ad5b3");
	roommates[3] = initRoommate("Claire Hite", "2ee18421");
}

void saveRoommateData() {

}
