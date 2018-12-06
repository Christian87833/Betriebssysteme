#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"

typedef struct List{
	bool free;
	unsigned size;
	PCB_t* process;
	//struct List* last;
	struct List* next;
} ListEntry_t;


ListEntry_t firstListEntry;


static void initList() {
	firstListEntry.free = true;
	firstListEntry.size = MEMORY_SIZE;
	firstListEntry.next = NULL;
	//list.last = NULL;
}


void addProcess(PCB_t* process){
	addProcessRec(process, &firstListEntry);
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
		addProcess(process, currentEntry->next); //verbesserbar?
	}
}

void memoryCompaction(){
	//TODO//

	//test comment// 

	// was nun
	//test

}

