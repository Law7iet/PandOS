#include "scheduler.h"
#include "tools.h"
#include "pcb.h"
#include <umps3/umps/libumps.h>

void scheduler() {
    /* Controlli dello scheduler */
    if(emptyProcQ(readyQueue) == 1) {
        /* Non ci sono processi da eseguire */
        if(processCount == 0) {
            /* Si spegne la macchina */
            HALT();
        }
        /* Non ci sono processi pronti, il processore va in stato d'attesa */
        else if(processCount > 0 && softBlockCount > 0) {
            currentProc->p_s.status = STATUS_WAIT;
            /* Mette il processore in stato d'attesa */
            WAIT();
        }
        /* Presenza di un deadlock */
        else if(processCount > 0 && softBlockCount == 0) {
            /* Si va in panico */
            PANIC();
        }
    } else {
        /* Cambio del processo */
        pcb_t *readyProc = removeProcQ(&readyQueue);
        currentProc = readyProc;
        setTIMER(PLT_TIME);
        LDST(&(currentProc->p_s));
    }
}