#ifndef __MEMORYMAN__
#define __MEMORYMAN__

#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"


void initList();
void addProcess(PCB_t* process);
void removeProcess(PCB_t* process);
void speicherGraphischAusgeben();

#endif /* __MEMORYMAN__ */

//finaler Test.