#include "exceptions.h"
#include "pcb.h"
#include "asl.h"
#include "interrupts.h"
#include <umps3/umps/libumps.h>
#include "scheduler.h"
#include "tools.h"

#define SEMAPHORELENGTH 49
#define REGISTERLENGTH  32

extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern semd_t *sem[SEMAPHORELENGTH];

void createProcess(state_t *statep, support_t *supportp) {
    /* Nuovo processo */
    pcb_t *newProcess = allocPcb();
    /* Il nuovo processo è vuoto */
    if(newProcess == NULL) {
        /* Salvataggio del valore di ritorno */
        currentProc->p_s.gpr[1] = -1;
    }
    /* Il nuovo processo non è vuoto */
    else {
        insertProcQ(&(readyQueue), newProcess);
        processCount++;
        
        /* Inizializzazione dei campi */
        insertChild(currentProc, newProcess);
        
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

        /* Salvataggio del valore di ritorno */
        currentProc->p_s.gpr[1] = 0;
    }
}

void terminateProcess(pcb_t *proc) {
    if(proc != NULL) {        
        /* Se il processo ha figli, si chiama la funzione ricorsivamente sui figli */
        while(!emptyChild(proc)) {
            pcb_t *tmp = outChild(proc);
            terminateProcess(tmp);
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

void passeren(int *semaddr) {
    *semaddr = *semaddr - 1;
    /* Il processo viene bloccato */
    if(*semaddr < 0) {
        /* TOD clock */
        unsigned int tmp;

        currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 0x4;
        currentProc->p_s = *((state_t *) BIOSDATAPAGE);
        currentProc->p_time = currentProc->p_time + STCK(tmp);
        insertBlocked(semaddr, currentProc);

        currentProc = NULL;
        scheduler();
    }
}

void verhogen(int *semaddr) {
    (*semaddr)++;
    pcb_t *process = removeBlocked(semaddr);
    if(process != NULL) {
        insertProcQ(&readyQueue, process);
    }
}

void ioWait(int intlNo, int dnum, int termRead) {
    /* Indice del semaforo */    
    int index = intlNo - 3;
    if(termRead == TRUE) {
        index++;
    }
    softBlockCount++;
    currentProc->p_s.gpr[1] = 0;
    dtpreg_t *devRegister = (dtpreg_t *) (0x10000054 + ((intlNo - 3) * 0x80) + (dnum * 0x10));
    currentProc->p_s.gpr[1] = devRegister->status;
    passeren(sem[(index * dnum) + 1]->s_semAdd);
}

void getTime() {
    currentProc->p_s.gpr[1] = currentProc->p_time;
}

void clockWait() {
    softBlockCount++;
    passeren(sem[0]->s_semAdd);
}

void getSupportPtr() {
    currentProc->p_s.gpr[1] = (unsigned int) currentProc->p_supportStruct;
}

void passUpOrDie(int i) {
    if(currentProc->p_supportStruct == NULL) {
        terminateProcess(currentProc);
    } else {
        currentProc->p_supportStruct->sup_exceptState[i] = *((state_t *) BIOSDATAPAGE);
        LDCXT(currentProc->p_s.gpr[26], currentProc->p_s.status, currentProc->p_s.pc_epc);
        *((state_t *) BIOSDATAPAGE) = currentProc->p_s;
        exceptionsHandler();
    }

}

/* Gestore delle SYSCALL */
void systemCallsHandler() {
    /* Controllo che sia in kernel mode */
    int currentProcessStatus = currentProc->p_s.status;
    int bits[REGISTERLENGTH] = {0};
    decToBin(bits, currentProcessStatus);
    /* Il processo corrente è in user mode */
    if(bits[3] == 1) {
        /* Si uccide il processo */
        SYSCALL(2, 0, 0, 0);
    }
    /* Il processo corrente è in kernel mode */
    else {
        switch(currentProc->p_s.gpr[3]) {
        /* currentProc->p_s.gpr[4], currentProc->p_s.gpr[5], currentProc->p_s.gpr[6]); */
            case 1:
                createProcess((state_t *) currentProc->p_s.gpr[4], (support_t *) currentProc->p_s.gpr[5]);
                break;
            case 2:
                if(currentProc != NULL) {
                    terminateProcess(currentProc);
                }
                scheduler();
                break;
            case 3:
                passeren((int *) currentProc->p_s.gpr[4]);
                break;
            case 4:
                verhogen((int *) currentProc->p_s.gpr[4]);
                break;
            case 5:
                ioWait(currentProc->p_s.gpr[4], currentProc->p_s.gpr[5], currentProc->p_s.gpr[6]);
                break;
            case 6:
                getTime();
                break;
            case 7:
                clockWait();
                break;
            case 8:
                getSupportPtr();
                break;
            default:
                passUpOrDie(1);
        }
    }
}

/* Gestore delle eccezioni */
void exceptionsHandler() {
    /* Calcolo della causa dello stato del processo che ha sollevato l'eccezione */
    state_t *exceptionState = (state_t*) BIOSDATAPAGE;
    int bits[REGISTERLENGTH] = {0};
    decToBin(bits, exceptionState->cause);
    int exceptionCauseCode = binToDec(bits, 2, 6);

    /* In base al suo valore, solleva uno specifico gestore */
    if(exceptionCauseCode == 0) {
        interruptsHandler();
    }
    else if(exceptionCauseCode == 8) {
        systemCallsHandler();
    }
    else if((exceptionCauseCode >= 1 && exceptionCauseCode <= 3) || (exceptionCauseCode >= 9 && exceptionCauseCode <= 12)) {
        passUpOrDie(0);
    }
}