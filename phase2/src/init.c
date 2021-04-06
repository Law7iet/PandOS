#include "asl.h"
#include "pcb.h"
#include "tools.h"
#include "pandos_const.h"
#include "pandos_types.h"
#include "scheduler.h"

extern void test();
extern void uTLB_RefillHandler();
extern void exceptionHandler();

int main() {
    /* Sezione 3.1.2 - popolazione del Pass Up Vector */
    passUpVector = (passupvector_t *) (memaddr) PASSUPVECTOR;
    passUpVector->exception_handler = (memaddr) exceptionHandler;
    passUpVector->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    passUpVector->exception_stackPtr = (memaddr) KERNELSTACK;
    passUpVector->tlb_refill_stackPtr = (memaddr) KERNELSTACK;

    /* Sezione 3.1.3 - inizializzazione delle strutture di dati */
    initPcbs();
    initASL();

    /* Sezione 3.1.4 - inizializzazione delle variabili del kernel */
    processCount = 0;
    softBlockCount = 0;
    readyQueue = mkEmptyProcQ();
    currentProc = NULL;
    int i;
    static int semValue[SEMAPHORE_LENGTH];
    for(i = 0; i < SEMAPHORE_LENGTH; i++) {
        semValue[i] = 0;
        sem[i] = &semValue[i];
    }

    /* Sezione 3.1.5 - caricare l'Interval Timer */
    LDIT(IT_TIME);

    /* Sezione 3.1.6 - instanza del processo test */
    pcb_t *procTest = allocPcb();
    procTest->p_prnt = NULL;
    procTest->p_child = NULL;
    procTest->p_next_sib = NULL;
    procTest->p_prev_sib = NULL;
    procTest->p_time = 0;
    procTest->p_semAdd = NULL;
    procTest->p_supportStruct = NULL;
    procTest->p_s.status = STATUS_INIT;
    RAMTOP(procTest->p_s.reg_sp); 
    procTest->p_s.pc_epc = (memaddr) test;
    procTest->p_s.reg_t9 = (memaddr) test;
    insertProcQ(&readyQueue, procTest);
    processCount++;

    /* Sezione 3.1.7 - chiamata dello scheduler */
    scheduler();

    return 0;
}