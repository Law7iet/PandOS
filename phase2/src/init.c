#include "asl.h"
#include "pcb.h"
#include "tools.h"
#include "scheduler.h"
#include "exceptions.h"

extern void test();
extern void uTLB_RefillHandler();
extern void exceptionsHandler();

/* Sezione 3.1.1 - dichiarazione delle variabili globali */
int processCount;
int softBlockCount;
pcb_t *readyQueue;
pcb_t *currentProc;
int *sem[SEMAPHORELENGTH];

int main() {
    /* Sezione 3.1.2 - popolazione del Pass Up Vector */
    *((memaddr*) 0x0FFFF900) = (memaddr) uTLB_RefillHandler;
    *((memaddr*) 0x0FFFF904) = 0x20001000;
    *((memaddr*) 0x0FFFF908) = (memaddr) exceptionsHandler;
    *((memaddr*) 0x0FFFF90c) = 0x20001000;

    /* Sezione 3.1.3 - inizializzazione delle strutture di dati */
    initPcbs();
    initASL();

    /* Sezione 3.1.4 - inizializzazione delle variabili del kernel */
    processCount = 0;
    softBlockCount = 0;
    readyQueue = mkEmptyProcQ();
    currentProc = NULL;
    int i;
    for(i = 0; i < SEMAPHORELENGTH; i++) {
        sem[i] = 0;
    }

    /* Sezione 3.1.5 - caricare l'Interval Timer */
    LDIT(100000);

    /* Sezione 3.1.6 - instanza del processo test */
    /* Inizializzazione del processo */
    pcb_t *procTest = allocPcb();
    procTest->p_prnt = NULL;
    procTest->p_child = NULL;
    procTest->p_next_sib = NULL;
    procTest->p_prev_sib = NULL;
    procTest->p_time = 0;
    procTest->p_semAdd = NULL;
    procTest->p_supportStruct = NULL;
    /* Attivazione degli interrupt, PLT e kernel-mode */
    procTest->p_s.status = 0b00001000000000001111111100000100;
    /* SP impostato con RAMTOP */
    RAMTOP(procTest->p_s.gpr[26]); 
    /* PC impostato con test */
    procTest->p_s.pc_epc = (memaddr) test;
    procTest->p_s.gpr[24] = (memaddr) test;
    /* Inserimento del processo nella coda ready */
    insertProcQ(&readyQueue, procTest);
    /* Aumento del contatore dei processi in coda */
    processCount++;

    /* Sezione 3.1.7 - chiamata dello scheduler */
    scheduler();

    return 0;
}