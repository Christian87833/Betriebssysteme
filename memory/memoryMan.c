#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"
#include "memorysidekick.h"
#include "memorysidekick.h"

typedef struct List{
	bool free;
	unsigned size;
	PCB_t* process;
	struct List* next;
} ListEntry_t;


ListEntry_t firstListEntry;

void initilList();
void addProcessRec(PCB_t* process, ListEntry_t* currentEntry);
void addProcess(PCB_t* process);
void memoryCompaction();
void speicherGraphischAusgeben();



void initilList() {
	firstListEntry.free = true;
	firstListEntry.size = MEMORY_SIZE;
	firstListEntry.next = NULL;
}

void addProcessRec(PCB_t* process, ListEntry_t* currentEntry) {

	if (currentEntry->free && currentEntry->size >= process->size){
		int tempSize = currentEntry->size;

		currentEntry->process = process;
		currentEntry->free = false;
		currentEntry->size = process->size;
		process->baseRegister = //vorheriger eintrag
		process->limitRegister = process->baseRegister + process->size;

		
		if (tempSize != process->size) {
			ListEntry_t* newSpace = (ListEntry_t*)malloc(sizeof(ListEntry_t));
			newSpace->free = true;
			newSpace->process = NULL;
			newSpace->size = tempSize - process->size;
			newSpace->next = currentEntry->next;
			currentEntry->next = newSpace;
		}

	}
	else if (currentEntry->next == NULL){
		memoryCompaction();
		addProcessRec(process, &firstListEntry);
	}
	else {
		addProcessRec(process, currentEntry->next); //verbesserbar?
	}
}

void addProcess(PCB_t* process) {
	addProcessRec(process, &firstListEntry);
}

//Prozess Loeschen
void removeProcess(PCB_t* process) {
	ListEntry_t* currentEntry = &firstListEntry;
	ListEntry_t* lastEntry = NULL;
	printf("----------------DELETE\n");

	//Ersmal den Prozess suchen
	while ((currentEntry->process == NULL || currentEntry->process->pid != process->pid) && currentEntry->next != NULL) { //prozess oder free space
		lastEntry = currentEntry;
		currentEntry = currentEntry->next;
	}

	if (currentEntry->process->pid == process->pid) {
		currentEntry->free = true;
		currentEntry->process = NULL;

		if (lastEntry != NULL && lastEntry->free) {
			lastEntry->size += currentEntry->size;
			if (currentEntry->next != NULL) {
				lastEntry->next = currentEntry->next;
			}
			else {
				lastEntry->next = NULL;
			}
			//und danach
			if (currentEntry->next != NULL && currentEntry->next->free) {
				lastEntry->size += currentEntry->next->size;
				if (currentEntry->next->next != NULL) {
					lastEntry->next = currentEntry->next->next;
				}
				else {
					lastEntry->next = NULL;
				}
			}
		}

		else if (currentEntry->next != NULL && currentEntry->next->free) {
			currentEntry->size += currentEntry->next->size;
			if (currentEntry->next->next != NULL) {
				currentEntry->next = currentEntry->next->next;
			}
			else {
				currentEntry->next = NULL;
			}
		}

	}
	else {
		//ERR PROCESS NICHT IN LISTE
	}
	speicherGraphischAusgeben();
}

void memoryCompaction(){
	ListEntry_t* currentEntry = &firstListEntry;
	ListEntry_t* lastEntry;
	printf("\n\n\nCOMPACTION--------------------\n\n\n");
	//Einmal die ganze Liste durch
	while (currentEntry->next != NULL) {

		if (!currentEntry->free && lastEntry != NULL && lastEntry->free) {
			removeProcess(currentEntry->process);
			addProcess(currentEntry->process);
		}

		lastEntry = currentEntry;
	}
}


void speicherGraphischAusgeben() {

	ListEntry_t* currentEntry = &firstListEntry;
	printf("+++++++++++++Speicher++++++++++++\n");

	//Einmal die ganze Liste durch
	do {

		if (currentEntry->free) {
			printf("+\t\tFREE SPACE\n");
			printf("+\t\tSIZE: %d\n", currentEntry->size);
			printf("+++++++++++++++++++++++++++++++++\n");
		}
		else {
			printf("+\t\tALLOCATED SPACE\n");
			printf("+\t\tSIZE: %d\n", currentEntry->process->size);
			printf("+\t\tPID: %d\n", currentEntry->process->pid);
			printf("+++++++++++++++++++++++++++++++++\n");
		}

		fflush(stdout);
		
		if (currentEntry->next != NULL) {
			currentEntry = currentEntry->next;
		}
		else {
			break;
		}
		
	} while (true);
}

