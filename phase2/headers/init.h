#ifndef INIT
#define INIT

#include "asl.h"
#include "pcb.h"
#include "tools.h"
#include "p2test.h"
#include "scheduler.h"
#include "exceptions.h"

extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern int sem[SEMAPHORELENGTH];

#endif