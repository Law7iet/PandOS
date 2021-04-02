#ifdef EXCEPTIONS
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
extern semd_t *sem[SEMAPHORELENGTH];

void exceptionsHandler();
void systemcallsHandler();
void passUpOrDie();
int SYSCALL(CREATEPROCESS, state_t *statep, support_t *supportp, 0);
void SYSCALL(TERMINATEPROCESS, 0, 0, 0);
void SYSCALL(PASSEREN, int *semaddr, 0, 0);
void SYSCALL(VERHOGEN, int *semaddr, 0, 0);
int SYSCALL(IOCOMMAND, int intlNo, int dnum, int termRead);
int SYSCALL(GETCPUTIME, 0, 0, 0);
int SYSCALL(WAITCLOCK, 0, 0, 0);
support_t* SYSCALL(GETSUPPORTPTR, 0, 0, 0);

#endif