#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"
#include "memorysidekick.h"
#include "memorysidekick.h"

/*
Methoden die nicht speziell mit Namen Gekennzeichnet wurden, wurden in gemeinsamer Arbeit von Christian Weber und Jonathan Bohnet erstellt.
*/

typedef struct WaitList {
	PCB_t* process;			//der Prozess als pointer
	struct WaitList* next;	//der naechste Prozess
} WaitList_t;

WaitList_t* firstEntry; //erste Eintrag der Liste: Anker
WaitList_t* lastEntry; //erste Eintrag der Liste: Anker
bool isEmpty;
int count;

//zum pruefen ob es was zu pullen gibt
bool* emptyQueue() {
	return &isEmpty;
}

//Inizialisieren der Liste, beim Start und zuruecksetzen zum inizial zustand wenn letztes Element gpulled wird
void initWaitList() {
	WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
	firstEntry = newSpace;
	firstEntry->process = NULL;
	isEmpty = true;
	count = 0;
}

//Prozess In WarteSchlange einfuegen
void putt(PCB_t* process) {
	printf("--PUSH PROCESS WITH SIZE: %d\n", process->size);
	//Wenn vorher warteschlange leer war
	if (firstEntry->process == NULL) {
		WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
		newSpace->process = process;
		newSpace->next = NULL;
		lastEntry = newSpace;
		firstEntry = newSpace;
		isEmpty = false;	//Schlange nicht mehr leer
	}
	//sonnst an vorhandenen Schlange anhaengen
	else {
		WaitList_t* newSpace = (WaitList_t*)malloc(sizeof(WaitList_t));
		newSpace->process = process;
		newSpace->next = NULL;
		lastEntry->next = newSpace;
		lastEntry = newSpace;
	}
	count++;		//zaehlt Elemente in Schlange
		
	
}

//zum Testen ob es genug platzt gibt um den naechsten Prozess zu laden
int sizeToPull() {
	return firstEntry->process->size;
}


//Aeltesten Prozess aus der Warteschlange bekommen
PCB_t* pull() {
	count--;	//Zaeher fuer Elemente in der Warteschlange runter zaehlen

	if (firstEntry->process != NULL) {	//Vorraussetzung Es gibt einen Prozess in der Warteschlange
		PCB_t* returnValue = firstEntry->process;	//der gesuchte Prozess (pointer) 
		if (firstEntry->next != NULL) {	//wenn es weitere Elemente in der Warteschlange gab
			WaitList_t* temp = firstEntry->next;	//ruecken diese nach
			firstEntry = temp;
		}
		
		if (count == 0) {	// war das der Letzte Prozess in Warteschlange?
			initWaitList();	//Resetten der Warteschlange
		}

		//Copied from Core

		returnValue->status = running;	// all active processes are marked active
		runningCount++;						// and add to number of running processes
		usedMemory = usedMemory + returnValue->size;	// update amount of used memory
		systemTime = systemTime + LOADING_DURATION;	// account for time used by OS
		logPidMem(returnValue->pid, "Process started and memory allocated");
		flagNewProcessStarted();	// process is now a running process, not a candidate any more 

		//End Copy


		return returnValue;	//Erst hier wird der Returnvalue aus Zeile 2 der Methode returned
	}
	else {
		return NULL;	//leere Schlange
	}

}