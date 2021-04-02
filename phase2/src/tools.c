#include "tools.h"
#include "pcb.h"
#include "asl.h"

#define SEMAPHORELENGTH 49
#define REGISTERLENGTH  32
extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern semd_t *sem[SEMAPHORELENGTH];

void decToBin(int bits[], int n) {
    int i = 0;
    while (n > 0) {
        bits[i] = n % 2;
        n = n / 2;
        i++;
    }
}

int binToDec(int bits[], int first, int last) {
    int num = 0;
    int mul = 1;
    int i;
    for(i = 0; i < first; i++) {
      mul = mul * 2;
    }
    for(i = first; i < last; i++) {
        num = num + (bits[i] * mul);
        mul = mul * 2;
    }
    return num;
}

int checkBlockedOnSemDev(int* semaphore) {
    int i;
    for(i = 0; i < SEMAPHORELENGTH; i++) {
        if(sem[i]->s_semAdd == semaphore) {
            return 1;
        }
    }
    return 0;
}

void recTerminateProcess(pcb_t *proc) {
    if(proc != NULL) {        
        /* Se il processo ha figli, si chiama la funzione ricorsivamente sui figli */
        while(!emptyChild(proc)) {
            pcb_t *tmp = outChild(proc);
            recTerminateProcess(tmp);
        }

        /* Aggiornamento dei semafori */
        if(proc->p_semAdd != NULL) {
            /* Flag che indica se il processso è bloccato su un semaforo device */
            int BlockedOnSemDev = checkBlockedOnSemDev(proc->p_semAdd);
            /* Il processo non è bloccato su un semaforo */
            if (*(proc->p_semAdd) < 0 && !BlockedOnSemDev) {
                /* Si aggiorna il valore del semaforo */
                if(*(proc->p_semAdd) < 0) {
                    *(proc->p_semAdd) = *(proc->p_semAdd) + 1;
                }
            }
            /* Il processo è bloccato su un semaforo */
            else {
                softBlockCount--;    
            }
            outBlocked(proc);
        }

        /* Cancellazione del processo */
        outProcQ(&readyQueue, proc);
        freePcb(proc);
        processCount--;
    }
}