#ifndef INTERRUPTS
#define INTERRUPTS

#include "pcb.h"
#include "asl.h"
#include "tools.h"

#define SEMAPHORELENGTH 49
#define REGISTERLENGTH  32

extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern int *sem[SEMAPHORELENGTH];

void interruptsHandler();

#endif