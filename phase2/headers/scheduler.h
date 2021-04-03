#ifndef SCHEDULER
#define SCHEDULER

#include "asl.h"
#include "pcb.h"
#include "tools.h"

#define SEMAPHORELENGTH 49
#define REGISTERLENGTH  32

extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern int *sem[SEMAPHORELENGTH];

void scheduler();

#endif