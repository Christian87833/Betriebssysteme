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

WaitList_t* firstEntry; //erste Eintrag der Liste: Anker
WaitList_t* lastEntry; //erste Eintrag der Liste: Anker
bool isEmpty;
int count;

bool* emptyQueue() {
	return &isEmpty;
}


void initWaitList() {
	WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
	firstEntry = newSpace;
	firstEntry->process = NULL;
	isEmpty = true;
	count = 0;
}


void putt(PCB_t* process) {
	if (firstEntry->process == NULL) {
		WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
		newSpace->process = process;
		newSpace->next = NULL;
		lastEntry = newSpace;
		firstEntry = newSpace;
	}
	else {
		WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
		newSpace->process = process;
		newSpace->next = NULL;
		lastEntry->next = newSpace;
		lastEntry = newSpace;
	}
	count++;
	isEmpty = false;
	
}


int sizeToPull() {
	return firstEntry->process->size;
}

PCB_t* pull() {
	if (firstEntry->process != NULL) {
		PCB_t* returnValue = firstEntry->process;
		if (firstEntry->next != NULL) {
			WaitList_t temp = *(firstEntry->next);
			free(&firstEntry);
			firstEntry = &temp;
		}
		else {
			//free(&firstEntry);
			//firstEntry->process = NULL;
		}

		count--;
		if (count == 0) {
			initWaitList();
		}

		//Copied from Core

		returnValue->status = running;	// all active processes are marked active
		runningCount++;						// and add to number of running processes
		usedMemory = usedMemory + returnValue->size;	// update amount of used memory
		systemTime = systemTime + LOADING_DURATION;	// account for time used by OS
		//logPidMem(newPid, "Process started and memory allocated");
		flagNewProcessStarted();	// process is now a running process, not a candidate any more 

		//End Copy


		return returnValue;
	}
	else {
		return NULL;
	}

}