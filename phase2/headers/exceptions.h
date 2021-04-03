#ifndef EXCEPTIONS
#define EXCEPTIONS

#include "pandos_types.h"

void createProcess(state_t *statep, support_t *supportp);
void terminateProcess();
void passeren(int *semaddr);
void verhogen(int *semaddr);
void ioWait(int intLineNo, int devNo, int termRead);
void getTime();
void clockWait();
void getSupportPtr();

void passUpOrDie(int i);
void systemcallHandler();
void exceptionHandler();

#endif