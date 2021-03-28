#include "exceptions.h"
#include "pcb.h"
#include "init.h"

int SYSCALL(CREATEPROCESS, state_t *statep, support_t *supportp, 0) {
    pcb_t *newProcess = allocPcb();
    if(newProcess == NULL) {
        return -1;
    } else {
        insertProcQ(&(readyQueue), newProcess);
        insertChild(&(currentProc), newProcess);
        processCount++;
        
        newProcess->p_s.entry_hi = statep->entry_hi;
        newProcess->p_s.cause = statep->cause;
        newProcess->p_s.status = statep->status;
        newProcess->p_s.pc_epc = statep->pc_epc;
        int i;
        for(i = 0; i < STATE_GPR_LEN; i++) {
            newProcess->p_s.gpr[i] = statep->gpr[i];
        }
        newProcess->p_s.hi = statep->hi;
        newProcess->p_s.lo = statep->lo;
        
        newProcess->p_supportStruct = supportp;

        newProcess->p_time = 0;
        
        newProcess->p_semAdd = NULL;

        return 0;
    }
}


void SYSCALL(TERMINATEPROCESS, 0, 0, 0) {
    if(currentProc != NULL) {
        pcb_t *tmp = currentProc;
        freePcb(currentProc);
        processCount--;
        currentProc = currentProc->p_child;
        while(currentProc->p_next_sib != currentProc) {
            SYSCALL(TERMINATEPROCESS, 0, 0, 0);
            currentProc = currentProc->p_next_sib;
        }
        currentProc = NULL;
    }
}

void SYSCALL(PASSEREN, int *semaddr, 0, 0) {
    (*semaddr)--;
    if(*semaddr < 0) {
        currentProcess->p_semAdd = sem;
        insertBlocked(sem, currentProcess);
        currentProcess = NULL;
    }    
}

void SYSCALL(VERHOGEN, int *semaddr, 0, 0) {
    (*semaddr)++;
    pcb_t *process = removeBlocked(semaddr);
    if(process != NULL) {
        insertProcQ(&ReadyQueue, process);
    }
}

int SYSCALL(IOCOMMAND, int intlNo, int dnum, int termRead) {    
    int index=intlNo-2;
    if(termRead==true)
        index++;    
    softBlockCount++;    
    SYSCALL(PASSEREN,sem[index*dnum+1],0,0);
    scheduler();

}

int SYSCALL(GETCPUTIME, 0, 0, 0) {    
    return currentProc->p_time;
}

int SYSCALL(WAITCLOCK, 0, 0, 0) {
    softBlockCount++;
    SYSCALL(PASSEREN,sem[0],0,0);
    scheduler();
}

support_t* SYSCALL(GETSUPPORTPTR, 0, 0, 0) {
    return currentProc->p_supportStruct;
}