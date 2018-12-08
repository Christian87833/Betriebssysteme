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
ListEntry_t* processList;

void initilList();
void addProcessRec(PCB_t* process, ListEntry_t* currentEntry);
void addProcess(PCB_t* process);
void memoryCompaction();



void initilList() {
	firstListEntry.free = true;
	firstListEntry.size = MEMORY_SIZE;
	firstListEntry.next = NULL;
	processList = (ListEntry_t*)malloc(MAX_PROCESSES * sizeof(ListEntry_t));
}

static unsigned processCount = 0;

void addProcessRec(PCB_t* process, ListEntry_t* currentEntry) {

	

	if (currentEntry->free && currentEntry->size >= process->size){
		processCount++;
		//Luecke und Einfuege Prozess gleich gross
		if (currentEntry->size == process->size){
			currentEntry->process = process;
			currentEntry->free = false;
			currentEntry->size = process->size;
			process->baseRegister = //vorheriger eintrag
				process->limitRegister = process->baseRegister + process->size;
		} else {
			//Neuer Eintrag
			(processList + (processCount * sizeof(ListEntry_t)))->free = false;
			(processList + (processCount * sizeof(ListEntry_t)))->size = process->size;
			(processList + (processCount * sizeof(ListEntry_t)))->process = process;
			

			//Lueck zum Prozess umgestalten
			currentEntry->free = true;
			currentEntry->size = currentEntry->size - process->size;

			//Pointer neu setzen
			if (currentEntry->next == NULL) {

				currentEntry->next = (processList + (processCount * sizeof(ListEntry_t)));
				(processList + (processCount * sizeof(ListEntry_t)))->next = NULL;
			}
			else {
				ListEntry_t* temp = currentEntry->next; 
				currentEntry->next = (processList + (processCount * sizeof(ListEntry_t)));
				(processList + (processCount * sizeof(ListEntry_t)))->next = temp;
			}

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
	ListEntry_t* lastEntry;
	printf("----------------DELETE\n");
	//Ersmal den Prozess suchen
	while (currentEntry->process->pid != process->pid && currentEntry->next != NULL) {
		lastEntry = currentEntry;
		currentEntry = currentEntry->next;
	}
	if (currentEntry->process->pid == process->pid) {
		
		if (currentEntry->next != NULL && currentEntry->next->free) {	//Folgt eine Freie Luecke? dann ist mein next pointer der der Luecke.
			currentEntry->next = currentEntry->next->next; 
		}
		if (lastEntry != NULL && lastEntry->free) { //Wenn vorher eine Frei Luecke war, nimmt diese jetzt den gesammten bereich ein
			lastEntry->next = currentEntry->next;
		}
	}
	else {
		//ERR PROCESS NICHT IN LISTE
	}
}

void memoryCompaction(){
	ListEntry_t* currentEntry = &firstListEntry;
	ListEntry_t* lastEntry;

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
			printf("+\t\tFREE SPACE\t+\n");
			printf("+\t\tSIZE: %d\t+\n", currentEntry->size);
			printf("+++++++++++++++++++++++++++++++++\n");
		}
		else {
			printf("+\t\tALLOCATED SPACE\t+\n");
			printf("+\t\tSIZE: %d\t+\n", currentEntry->process->size);
			printf("+\t\tPID: %d\t+\n", currentEntry->process->pid);
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

