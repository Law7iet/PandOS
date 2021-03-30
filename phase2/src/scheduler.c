#include "scheduler.h"

void scheduler() {
    /* Controlli dello scheduler */
    /* La coda dei processi ready è vuota */
    if(emptyProcQ(readyQueue) == 1) {
        /* Non ci sono processi da eseguire */
        if(processCount == 0) {
            /* Si spegne la macchina */
            HALT();
        }
        /* Non ci sono processi pronti, il processore va in stato d'attesa */
        else if(processCount > 0 && softBlockCount > 0) {
            /* Salvataggio di alcuni registri */ 
            currentProc->p_s.gpr[0] = 1;
            currentProc->p_s.gpr[8] = 1;
            currentProc->p_s.gpr[9] = 1;
            currentProc->p_s.gpr[10] = 1;
            currentProc->p_s.gpr[11] = 1;
            currentProc->p_s.gpr[12] = 1;
            currentProc->p_s.gpr[13] = 1;
            currentProc->p_s.gpr[14] = 1;
            currentProc->p_s.gpr[15] = 1;
            currentProc->p_s.gpr[27] = 0;
            /* Mette il processore in stato d'attesa */
            WAIT();
        }
        /* Presenza di un deadlock */
        else if(processCount > 0 && softBlockCount > 0) {
            /* Si invoca PANIC() */
            PANIC();
        }
    }
    /* C'è almeno un processo in coda */
    else {
        /* Il processo corrente non è vuoto */
        if(currentProc != NULL) {
            /* Ha terminato il suo tempo, quindi viene inserito nella coda dei processi ready */
            insertProcQ(&readyQueue, currentProc);
        }
        /* Rimuove il primo processo dalla coda dei processi ready e lo imposta come processo corrente */
        pcb_t *readyProc = removeProcQ(&readyQueue);
        currentProc = readyProc;
        /* Carica il Processor Local Timer di 5 millisecondi*/
        LDIT(5000);
        /* Carica lo stato del processore trovato nel processo estratto */
        LDST(&(currentProc->p_s));
    }
}
