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
	if(strlen(name) > 9){
		rm.nameLength = 9;
	} else {
		rm.nameLength = strlen(name);
	}
	rm.home = 0;
	strcpy(rm.uid_str, uid_str);
	rm.chores[0] = "Dishes";
	rm.chores[1] = "Vacuum";
	rm.events[0] = "Exam";
	rm.events[1] = "Parents";
	return rm;
}

void getRoommateData() {
	roommates[0] = initRoommate("Lauren", "1b1350e0");
	roommates[1] = initRoommate("Philip", "be4ae2e5");
	roommates[2] = initRoommate("Claire", "2ee18421");
	roommates[3] = initRoommate("Matt", "1b2ad5b3");
}

void saveRoommateData() {

}
