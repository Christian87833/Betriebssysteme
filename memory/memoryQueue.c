#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"
#include "memorysidekick.h"
#include "memorysidekick.h"


typedef struct WaitList {
	PCB_t* process;
	struct WaitList* next;
} WaitList_t;

WaitList_t firstEntry; //erste Eintrag der Liste: Anker
WaitList_t lastEntry; //erste Eintrag der Liste: Anker
bool isEmpty;

void initWaitList() {
	firstEntry.process = NULL;
	isEmpty = true;
}
void putt(PCB_t process) {
	if (firstEntry.process == NULL) {
		WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
		newSpace->process = &process;
		newSpace->next = NULL;
		lastEntry = *newSpace;
		isEmpty = false;
	}
	else {
		WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
		newSpace->process = &process;
		newSpace->next = NULL;
		lastEntry.next = newSpace;
		lastEntry = *newSpace;
	}
	
}

PCB_t* pull() {
	if (firstEntry.process != NULL) {
		return firstEntry.process;
		if (firstEntry.next != NULL) {
			WaitList_t temp = *(firstEntry.next);
			free(&firstEntry);
			firstEntry = temp;
		}
		else {
			free(&firstEntry);
			firstEntry.process = NULL;
			isEmpty = true;
		}
	}
	else {
		return NULL;
	}

}