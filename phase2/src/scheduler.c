#include "scheduler.h"
#include "pcb.h"
#include "asl.h"
#include "tools.h"
#include <umps3/umps/libumps.h>

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
            currentProc->p_s.status = 0b00010000000000001111111100000001;
            /* Mette il processore in stato d'attesa */
            WAIT();
        }
        /* Presenza di un deadlock */
        else if(processCount > 0 && softBlockCount == 0) {
            /* Si invoca PANIC() */
            PANIC();
        }
    }

    /* Rimuove il primo processo dalla coda dei processi ready e lo imposta come processo corrente */
    pcb_t *readyProc = removeProcQ(&readyQueue);
    currentProc = readyProc;
    /* Carica il Processor Local Timer di 5 millisecondi*/
    setTIMER(5000);
    /* Carica lo stato del processore trovato nel processo estratto */
    LDST(&(currentProc->p_s));
}
