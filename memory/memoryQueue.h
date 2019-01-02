#ifndef __MEMORYQUEUE__
#define __MEMORYQUEUE__

#include <stdio.h>
#include <stdbool.h>
#include "bs_types.h"
#include "globals.h"
#include "memorysidekick.h"
#include "memorysidekick.h"


bool emptyQueue();

void initWaitList();
void putt(PCB_t process);
PCB_t* pull();
int sizeToPull();

#endif /* __MEMORYQUEUE__ */