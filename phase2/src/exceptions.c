#include "exceptions.h"
#include "pcb.h"
#include "asl.h"
#include "tools.h"
#include "interrupts.h"

#define SEMAPHORELENGTH 49
#define REGISTERLENGTH  32

extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern semd_t *sem[SEMAPHORELENGTH];

/* Gestore delle eccezioni */
void exceptionsHandler() {
    /* Calcolo della causa dello stato del processo che ha sollevato l'eccezione */
    state_t *exceptionState = (state_t*) BIOSDATAPAGE;
    int bits[REGISTERLENGTH] = {0};
    DecToBin(exceptionState->cause);
    int exceptionCauseCode = binToDec(bits, 2, 6);

    /* In base al suo valore, solleva uno specifico gestore */
    if(exceptionCauseCode == 0) {
        interruptsHandler();
    }
    else if(exceptionCauseCode == 8) {
        systemCallsHandler();
    }
    else if((exceptionCauseCode >= 1 && exceptionCauseCode <= 3) || (exceptionCauseCode >= 9 && exceptionCauseCode <= 12)) {
        passUpOrDie();
    }
}

/* Gestore delle SYSCALL */
void systemCallsHandler() {
    /* Controllo che sia in kernel mode */
    int currentProcessStatus = currentProc->p_s.status;
    int *bits = DecToBin(currentProcessStatus);
    /* Il processo corrente è in user mode */
    if(bits[3] == 1) {
        /* Si uccide il processo */
        SYSCALL(TERMPROCESS, 0, 0, 0);
    }
    /* Il processo corrente è in kernel mode */
    else {
        /* In base al valore di a0 si esegue una specifica systemcall */
        switch(currentProc->p_s.gpr[3]) {
            case 1:
            SYSCALL(CREATEPROCESS, currentProc->p_s.gpr[4], currentProc->p_s.gpr[5], currentProc->p_s.gpr[6]);
            break;
            case 2:
            SYSCALL(TERMPROCESS, 0, 0, 0);
            break;
            case 3:
            SYSCALL(PASSEREN, currentProc->p_s.gpr[4], 0, 0);
            break;
            case 4:
            SYSCALL(VERHOGEN, currentProc->p_s.gpr[4], 0, 0);
            break;
            case 5:
            SYSCALL(IOWAIT, currentProc->p_s.gpr[4], currentProc->p_s.gpr[5], currentProc->p_s.gpr[6]);
            break;
            case 6:
            SYSCALL(GETTIME, 0, 0, 0);
            break;
            case 7:
            SYSCALL(CLOCKWAIT, 0, 0, 0);
            break;
            case 8:
            SYSCALL(GETSUPPORTPTR, 0, 0, 0);
            break;
            /* Se a0 è maggiore di 8, si chiama il passUpOrDie */
            default:
            if(currentProc->p_s.gpr[3] >= 9) {
                passUpOrDie();
            }
        }  
    }
}

int SYSCALL(CREATEPROCESS, state_t *statep, support_t *supportp, 0) {
    /* Nuovo processo */
    pcb_t *newProcess = allocPcb();
    /* Il nuovo processo è vuoto */
    if(newProcess == NULL) {
        /* Salvataggio del valore di ritorno */
        currentProc->p_s.gpr[1] = -1;
        return -1;
    }
    /* Il nuovo processo non è vuoto */
    else {
        insertProcQ(&(readyQueue), newProcess);
        processCount++;
        
        /* Inizializzazione dei campi */
        insertChild(&(currentProc), newProcess);
        
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
        return 0;
    }
}

void SYSCALL(TERMPROCESS, 0, 0, 0) {
    /* Il processo corrente esiste */
    if(currentProc != NULL) {
        /* Coda dei processi da eliminare */
        pcb_t *queue = currentProc;
        /* Cancella il processo corrente e i suoi figli */
        void recTerminateProcess(pcb_t *currentProc);
        scheduler();
}

void SYSCALL(PASSEREN, int *semaddr, 0, 0) {
    *semaddr = *semaddr - 1;
    /* Il processo viene bloccato */
    if(*semaddr < 0) {
        /* TOD clock */
        unsigned int tmp;

        currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 0x4;
        currentProc->p_s = (state_t *) BIOSDATAPAGE;
        currentProc->p_time = current->p_time + STCK(tmp)
        insertBlocked(semaddr, currentProcess);

        currentProcess = NULL;
        scheduler();
    }
}

void SYSCALL(VERHOGEN, int *semaddr, 0, 0) {
    (*semaddr)++;
    pcb_t *process = removeBlocked(semaddr);
    if(process != NULL) {
        insertProcQ(&ReadyQueue, process);
    }
}

int SYSCALL(IOWAIT, int intlNo, int dnum, int termRead) {
    /* Indice del semaforo */    
    int index = intlNo - 3;
    if(termRead == true) {
        index++;
    }
    softBlockCount++;    
    SYSCALL(PASSEREN, sem[(index * dnum) + 1], 0, 0);
}

int SYSCALL(GETTIME, 0, 0, 0) {
    currentProc->p_s.gpr[1] = currentProc->p_time;
    return currentProc->p_time;
}

int SYSCALL(CLOCKWAIT, 0, 0, 0) {
    softBlockCount++;
    SYSCALL(PASSEREN, sem[0], 0, 0);
    scheduler();
}

support_t* SYSCALL(GETSUPPORTPTR, 0, 0, 0) {
    currentProc->p_s.gpr[1] = (support_t *) currentProc->p_supportStruct);
    return currentProc->p_supportStruct;
}

void passUpOrDie() {

}