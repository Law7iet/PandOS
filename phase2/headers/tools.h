#ifndef TOOLS
#define TOOLS

#include "asl.h"
#include "pcb.h"

#define SEMAPHORELENGTH 49
#define REGISTERLENGTH  32

extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern int *sem[SEMAPHORELENGTH];

void decToBin(int bits[], int n);
int binToDec(int bits[], int first, int last);
int checkBlockedOnSemDev(int* semaphore);
void copyProcessorState(state_t* dst, state_t* src);

#endif