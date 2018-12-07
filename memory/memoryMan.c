#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"
#include "memorysidekick.h"

typedef struct List{
	bool free;
	unsigned size;
	PCB_t* process;
	struct List* next;
} ListEntry_t;


ListEntry_t firstListEntry;



void initList();
void addProcessRec(PCB_t* process, ListEntry_t* currentEntry);
void addProcess(PCB_t* process);
void memoryCompaction();



void initList() {
	firstListEntry.free = true;
	firstListEntry.size = MEMORY_SIZE;
	firstListEntry.next = NULL;
}


void addProcessRec(PCB_t* process, ListEntry_t* currentEntry) {

	
	if (currentEntry->free && currentEntry->size >= process->size){

		//Luecke und Einfuege Prozess gleich gross
		if (currentEntry->size == process->size){
			currentEntry->process = process;
			currentEntry->free = false;
			process->baseRegister = //vorheriger eintrag
				process->limitRegister = process->baseRegister + process->size;
		}
		else {
			//Leerer Eintrag
			ListEntry_t newSpace;
			newSpace.free = true;
			newSpace.size = currentEntry->size - process->size;
			newSpace.process = NULL;

			//Lueck zum Prozess umgestalten
			currentEntry->process = process;
			currentEntry->free = false;
			

			//Pointer neu setzen
			ListEntry_t* temp = currentEntry->next; //Pointer von freier Stelle auf naechste Stelle
			currentEntry->size = process->size;
			currentEntry->next = &newSpace;

			newSpace.next = temp;

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


	printf("+++++++++++++Speicher+++++++++++++\n");
	//Einmal die ganze Liste durch
	while (currentEntry->next != NULL) {
		if (currentEntry->free) {
			printf("+\t\tFREE SPACE\t\t+\n");
			printf("+\t\tSIZE: %d\t\t+\n", currentEntry->size);
			printf("++++++++++++++++++++++++++++++++++++\n");
		}
		else {
			printf("+\t\tALLOCATED SPACE\t\t+\n");
			printf("+\t\tSIZE: %d\t\t+\n", currentEntry->size);
			printf("+\t\tPID: %d\t\t+\n", currentEntry->process->pid);
			printf("++++++++++++++++++++++++++++++++++++\n");
		}
		
	}
}

