#include "tools.h"
#include "pandos_const.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"
#include <umps3/umps/libumps.h>

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
    for(i = first; i <= last; i++) {
        num = num + (bits[i] * mul);
        mul = mul * 2;
    }
    return num;
}

int checkBlockedOnSemDev(int* semAdd) {
    int i;
    for(i = 0; i < SEMAPHORE_LENGTH; i++) {
        if(sem[i] == semAdd) {
            return 1;
        }
    }
    return 0;
}

void copyProcessorState(state_t *dst, state_t *src) {
    dst->cause = src->cause;
    dst->entry_hi = src->entry_hi;
    dst->hi = src->hi;
    dst->lo = src->lo;
    dst->pc_epc = src->pc_epc;
    dst->status = src->status;
    int i;
    for(i = 0; i < STATE_GPR_LEN; i++) {
        dst->gpr[i] = src->gpr[i];
    }
}

void recTerminateProcess(pcb_t *proc) {
    if(proc != NULL) {        
        /* Se il processo ha figli, si chiama la funzione ricorsivamente sui figli */
        while(!emptyChild(proc)) {
            pcb_t *tmp = outChild(proc);
            recTerminateProcess(tmp);
        }
        /* Il processo non ha figli */
        /* Aggiornamento dei semafori */
        if(proc->p_semAdd != NULL) {
            /* Flag che indica se il processso è bloccato su un semaforo device */
            int BlockedOnSemDev = checkBlockedOnSemDev(proc->p_semAdd);
            /* Il processo non è bloccato su un semaforo */
            if (!BlockedOnSemDev) {
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