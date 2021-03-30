#include "init.h"

/* Sezione 3.1 - dichiarazione delle variabili globali */
int processCount;
int softBlockCount;
pcb_t *readyQueue;
pcb_t *currentProc;
int sem[SEMAPHORELENGTH];

int main() {
    /* Sezione 3.2 - popolazione del Pass Up Vector */
    *((memaddr*) 0x0FFFF900) = (memaddr) uTLB_RefillHandler;
    *((memaddr*) 0x0FFFF904) = 0x20001000;
    *((memaddr*) 0x0FFFF908) = (memaddr) systemcallHandler;
    *((memaddr*) 0x0FFFF90c) = 0x20001000;

    /* Sezione 3.3 - inizializzazione delle strutture di dati */
    initPcbs();
    initASL();

    /* Sezione 3.4 - inizializzazione delle variabili del kernel */
    processCount = 0;
    softBlockCount = 0;
    readyQueue = mkEmptyProcQ();
    currentProc = NULL;
    int i;
    for(i = 0; i < SEMAPHORELENGTH; i++) {
        sem[i] = 0;
    }

    /* Sezione 3.5 - caricare l'Interval Timer */
    LDIT(100000);

    /* Sezione 3.6 -inizializzazione del processo test */
    pcb_t *procTest = allocPcb();
    procTest->p_prnt = NULL;
    procTest->p_child = NULL;
    procTest->p_next_sib = NULL;
    procTest->p_prev_sib = NULL;
    procTest->p_time = 0;
    procTest->p_semAdd = NULL;
    procTest->p_supportStruct = NULL;
    /* Attivazione interrupt */
    procTest->p_s.gpr[2] = 1;
    procTest->p_s.gpr[8] = 1;
    procTest->p_s.gpr[9] = 1;
    procTest->p_s.gpr[10] = 1;
    procTest->p_s.gpr[11] = 1;
    procTest->p_s.gpr[12] = 1;
    procTest->p_s.gpr[13] = 1;
    procTest->p_s.gpr[14] = 1;
    procTest->p_s.gpr[15] = 1;
    /* Attivazione PLT */
    procTest->p_s.gpr[27] = 1;
    /* Attivazione Kernel-mode */
    procTest->p_s.gpr[3] = 0;
    /* SP impostato con RAMTOP */
    int tmp = RAMTOP(tmp);
    procTest->p_s.gpr[26] = tmp;
    /* PC impostato con test */
    procTest->p_s.pc_epc = (memaddr) test;
    /* Inserimento del processo nella coda ready */
    insertProcQ(&readyQueue, procTest);
    /* Aumento del contatore dei processi in coda */
    processCount++;

    /* Sezione 3.7 - chiamata dello scheduler */
    scheduler();
   return 0;
}