#ifndef INIT
#define INIT

#include "asl.h"
#include "pcb.h"
#include "p2test.h"
#include "scheduler.h"

extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern int sem[SEMAPHORELENGTH];

#endif