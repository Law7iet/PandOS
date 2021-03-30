#ifdef EXCEPTIONS
#define EXCEPTIONS

#include "pcb.h"
#include "asl.h"
#include "pandos_types.h"
#include "scheduler.h"

extern void exceptionHandler();
extern void systemcallHandler();

int SYSCALL(CREATEPROCESS, state_t *statep, support_t *supportp, 0);
void SYSCALL(TERMINATEPROCESS, 0, 0, 0);
void SYSCALL(PASSEREN, int *semaddr, 0, 0);
void SYSCALL(VERHOGEN, int *semaddr, 0, 0);
int SYSCALL(IOCOMMAND, int intlNo, int dnum, int termRead);
int SYSCALL(GETCPUTIME, 0, 0, 0);
int SYSCALL(WAITCLOCK, 0, 0, 0);
support_t* SYSCALL(GETSUPPORTPTR, 0, 0, 0);

#endif