#ifndef EXCEPTIONS
#define EXCEPTIONS

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

void exceptionsHandler();
void systemcallsHandler();
void passUpOrDie(int i);

void createProcess(state_t *statep, support_t *supportp);
void terminateProcess();
void passeren(int *semaddr);
void verhogen(int *semaddr);
void ioWait(int intlNo, int dnum, int termRead);
void getTime();
void clockWait();
void getSupportPtr();

#endif