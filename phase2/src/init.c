#include "pandos_const.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"
#include "p2test.h"
#include "scheduler.h"

/* Sezione 3.1 - dichiarazione delle variabili globali */
int processCount;
int softBlockCount;
pcb_t *readyQueue;
pcb_t *currentProc;
int sem[SEMAPHORELENGTH];

int main() {
    /* Sezione 3.2 - popolazione del Pass Up Vector */
    passupvector_t *passUpVector = 0x0FFFF900;
    passUpVector->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    passUpVector->tlb_refill_stackPtr = 0x20001000;
    passUpVector->exception_handler = NULL;
    passUpVector->exception_stackPtr = 0x20001000;

    /* Sezione 3.3 - inizializzazione delle strutture di dati */
    initPcbs();
    initSemd();

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
    procTest->p_s.gpr[0] = 1;
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
    procTest->p_s.gpr[3] = 1;
    /* SP impostato con RAMTOP */
    int tmp = RAMTOP(tmp);
    procTest->p_s.gpr[26] = tmp;
    /* PC impostato con test */
    procTest->p_s.s_pc = (memaddr) test;
    /* Chiamata dello scheduler */
    scheduler();
    /* A che serve?
    setTIMER(1);
    */
}