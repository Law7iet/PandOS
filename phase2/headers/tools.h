#ifndef TOOLS
#define TOOLS

#include "pandos_types.h"

#define SEMAPHORE_LENGTH    49
#define REGISTER_LENGTH     32
#define STATUS_INIT         0b00001000000000001111111100000100
#define STATUS_WAIT         0b00001000000000001111111100000001
#define PLT_TIME            5000
#define IT_TIME             100000
#define BITMAP_ADDR         0x10000040

int processCount;
int softBlockCount;
pcb_t *readyQueue;
pcb_t *currentProc;
int *sem[SEMAPHORE_LENGTH];
passupvector_t *passUpVector;

void decToBin(int bits[], int n);
int binToDec(int bits[], int first, int last);
int checkBlockedOnSemDev(int *semaphore);
void copyProcessorState(state_t *dst, state_t *src);
void recTerminateProcess(pcb_t *proc);
void breakPoint();

#endif