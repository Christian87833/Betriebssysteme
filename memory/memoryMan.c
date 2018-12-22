#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"
#include "memorysidekick.h"
#include "memorysidekick.h"

/*Struct fuer Liste. Einfach Verkettet mit Informationen zu 
 Belegt? Groesse, Prozess und natuerlich der Naechste Eintrag*/
typedef struct List{
	bool free;
	unsigned size;
	PCB_t* process;
	struct List* next;
} ListEntry_t;


ListEntry_t firstListEntry; //Anker Punkt, immer der erste Eintrag der Liste

void initilList();
void addProcessRec(PCB_t* process, ListEntry_t* currentEntry);
void addProcess(PCB_t* process);
void memoryCompaction();
void speicherGraphischAusgeben();


/*Liste iniziieren. Dabei wird ein Freier Eintrag erstellt, 
* mit der Groesse MEMORY_SIZE
*/
void initilList() {
	firstListEntry.free = true;
	firstListEntry.size = MEMORY_SIZE;
	firstListEntry.next = NULL;
}

/*Rekursieve Methode die einen Prozess in die Erste Luecke steckt, in die er rein Passt*/
void addProcessRec(PCB_t* process, ListEntry_t* currentEntry) {

	if (currentEntry->free && currentEntry->size >= process->size){	//Plaz frei und gross genug?
		printf("--ADD\n");
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
		speicherGraphischAusgeben();
	}
	else if (currentEntry->next == NULL){	//Am Ende und Keine Luecke gefunden die Grossgenug ist, aber theoretisch genug Speicher...
	
		memoryCompaction();					//also memoryCompaction(), um freie Luecken zusammen zu fuegen.
		
		addProcessRec(process, &firstListEntry);	//Und erneut versuchen den Prozess unter zu Bringen(Suche beginnt von vorne)
	}
	else { //Noch nicht gefunden aber weitere Eintraege noch zu checken, also rekursiev aufruf mit dem naechsten Eintrag als kandidat
		addProcessRec(process, currentEntry->next); //verbesserbar?
	}
}

/*Diese Methode wird aus core aufgerufen und startet die Rekursieve Methode*/
void addProcess(PCB_t* process) {
	addProcessRec(process, &firstListEntry);
}

//Prozess Loeschen
void removeProcess(PCB_t* process) {
	ListEntry_t* currentEntry = &firstListEntry;
	ListEntry_t* lastEntry = NULL;
	printf("--DELETE\n");

	//Ersmal den Prozess suchen
	while ((currentEntry->free || (currentEntry->process->pid != process->pid)) && (currentEntry->next != NULL)) { //prozess oder free space
		lastEntry = currentEntry;
		currentEntry = currentEntry->next;
	}

	//hier angekommen ist der current entry definietiv der gesuchte Prozess...

	if (currentEntry->process->pid == process->pid) {	//Siehe obigen kommentar, eigentlich unnoetige Abfrage aber so schutz gegen missbrauch der Methode
		//Bereich frei geben.
		currentEntry->free = true;
		currentEntry->process = NULL;

		//Jetzt noch benachbarte Luecken zusammen Legen bzw verschmelzen
		if (lastEntry != NULL && lastEntry->free) { //Gab es eine Luecke davor
			lastEntry->size += currentEntry->size;
			if (currentEntry->next != NULL) {		
				lastEntry->next = currentEntry->next;
			}
			else {
				lastEntry->next = NULL;
			}
			
			if (currentEntry->next != NULL && currentEntry->next->free) {	//und zusaetzlich noch danach
				lastEntry->size += currentEntry->next->size;
				if (currentEntry->next->next != NULL) {						
					lastEntry->next = currentEntry->next->next;
				}
				else {
					lastEntry->next = NULL;
				}
			}
		}
		
		else if (currentEntry->next != NULL && currentEntry->next->free) {	//oder nur danach
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


//kompaktierung des Speichers
void memoryCompaction(){
	ListEntry_t* currentEntry = &firstListEntry;
	ListEntry_t* lastEntry;
	printf("--COMP\n");
	int removedFree = 0;	//diese Variable speichert die Groesse aller 'free Spaces' die wir nach hinten schicben wollen

	//Einmal die ganze Liste durch
	while (currentEntry->next != NULL) {
		
		/*Suche Freien Speicher der nicht ganz am Ende ist.
		bei fund: Vorerigen Eintrag auf folgenden Eintrag zeigen lassen, so dass Luecke aus Liste entfaellt
		Groesse der entfallenen Luecke Speichern um ganz am Ande anzuhaengen*/
		if (currentEntry->free && currentEntry->next != NULL && !currentEntry->next->free) {
			if (lastEntry == NULL) {		//gibt es keinen Vorherigen Eintrag muss der Anker neu gesetzt werden
				removedFree += currentEntry->size;
				firstListEntry = *currentEntry->next;
			} else {
				removedFree += currentEntry->size;
				lastEntry->next = currentEntry->next;
			}
		}
		lastEntry = currentEntry;
		currentEntry = currentEntry->next;
	}
	if (currentEntry->free) {				//Wenn am ende schon ein freier Eintrag ist, diesen einfach vergroessern
		currentEntry->size += removedFree; 
	}
	else {									//Sonnst neuen Freien Eintrag anhaengen
		ListEntry_t* newSpace = (ListEntry_t*)malloc(sizeof(ListEntry_t));
		newSpace->free = true;
		newSpace->process = NULL;
		newSpace->size = removedFree;
		newSpace->next = NULL;
		currentEntry->next = newSpace;
	}
	speicherGraphischAusgeben();
}

/*Diese Methode Orientiert sich an der Darstellungsweise des Speichers in Grafiken - wie in der Vorlesung verwendet
Mit ihrer Hilfe kann man schnell und uebersichtlich erkennen wie der Speicher gerade aufgeteilt ist*/
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

