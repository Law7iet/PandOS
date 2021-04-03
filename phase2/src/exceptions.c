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
extern int *sem[SEMAPHORELENGTH];

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
        /* Inizializzazione dei campi */
        insertProcQ(&(readyQueue), newProcess);
        processCount++;
        insertChild(currentProc, newProcess);
        copyProcessorState(&(newProcess->p_s), statep);
        newProcess->p_supportStruct = supportp;
        newProcess->p_time = 0;
        newProcess->p_semAdd = NULL;

        /* Incremento del PC */
        currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 0x4;

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
        /* Il processo non ha figli */
        /* Aggiornamento dei semafori */
        if(proc->p_semAdd != NULL) {
            /* Flag che indica se il processso è bloccato su un semaforo device */
            int BlockedOnSemDev = checkBlockedOnSemDev(proc->p_semAdd);
            /* Il processo non è bloccato su un semaforo */
            if (!BlockedOnSemDev) {
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
    if(semaddr != NULL) {
        /* Viene richiesto una risorsa */
        *semaddr = *semaddr - 1;
        /* Se la risorsa non è disponibile, il processo viene bloccato */
        if(*semaddr < 0) {
            /* Aggiornamento dello stato del processo bloccato */
            copyProcessorState(&(currentProc->p_s), (state_t *) BIOSDATAPAGE);
            /* TOD clock */
            unsigned int tmp;
            currentProc->p_time = currentProc->p_time + STCK(tmp);
            insertBlocked(semaddr, currentProc);
            scheduler();
        }
    }
}

void verhogen(int *semaddr) {
    if(semaddr != NULL) {
        /* Viene rilasciato una risorsa */
        (*semaddr)++;
        /* Il processo bloccato, se presente, viene liberato */
        pcb_t *process = removeBlocked(semaddr);
        if(process != NULL) {
            insertProcQ(&readyQueue, process);
        }
    }
}

void ioWait(int intLineNo, int devNo, int termRead) {
    /* Calcolo dell'indice del semaforo */    
    int index = intLineNo - 3;
    if(termRead == TRUE) {
        index++;
    }
    index = (index * devNo) + 1;

    /* Blocco del processo corrente */
    softBlockCount++;

    /* Registro del semaforo che ha bloccato il processo corrente */
    dtpreg_t *devRegister = (dtpreg_t *) (0x10000054 + ((intLineNo - 3) * 0x80) + (devNo * 0x10));

    /* Incremento del PC */
    currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 0x4;
    
    /* Salvataggio del valore di ritorno */
    currentProc->p_s.gpr[1] = devRegister->status;
    passeren(sem[index]);
}

void getTime() {
    /* Incremento del PC */
    currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 0x4;

    /* Salvataggio del valore di ritorno */
    currentProc->p_s.gpr[1] = currentProc->p_time;
}

void clockWait() {
    softBlockCount++;
    passeren(sem[0]);
}

void getSupportPtr() {
    /* Incremento del PC */
    currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 0x4;

    /* Salvataggio del valore di ritorno */
    currentProc->p_s.gpr[1] = (unsigned int) currentProc->p_supportStruct;
}

void passUpOrDie(int i) {
    if(currentProc->p_supportStruct == NULL) {
        terminateProcess(currentProc);
        scheduler();
    } else {
        copyProcessorState(&(currentProc->p_supportStruct->sup_exceptState[i]), (state_t *) BIOSDATAPAGE);
        LDCXT(currentProc->p_supportStruct->sup_exceptState[i].gpr[26], currentProc->p_supportStruct->sup_exceptState[i].status, currentProc->p_supportStruct->sup_exceptState[i].pc_epc);
        exceptionsHandler();
    }
}

/* Gestore delle SYSCALL */
void systemCallsHandler() {
    /* Controllo che sia in kernel mode */
    int currentProcessStatus = currentProc->p_s.status;
    int bits[REGISTERLENGTH];
    int i;
    for(i = 0; i < REGISTERLENGTH; i++) {
        bits[i] = 0;
    }
    decToBin(bits, currentProcessStatus);
    /* Il processo corrente è in user mode */
    if(bits[3] == 1) {
        /* Si uccide il processo */
        terminateProcess(currentProc);
        scheduler();
    }
    /* Il processo corrente è in kernel mode */
    else {
        switch(currentProc->p_s.gpr[3]) {
            case 1:
                createProcess((state_t *) currentProc->p_s.gpr[4], (support_t *) currentProc->p_s.gpr[5]);
                break;
            case 2:
                terminateProcess(currentProc);
                scheduler();
                break;
            case 3:
                passeren((int *) currentProc->p_s.gpr[4]);
                break;
            case 4:
                verhogen((int *) currentProc->p_s.gpr[4]);
                break;
            case 5:
                ioWait((int) currentProc->p_s.gpr[4], (int) currentProc->p_s.gpr[5], (int) currentProc->p_s.gpr[6]);
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
    /* Copio lo stato dell'eccezione nel processo corrente */
    copyProcessorState(&(currentProc->p_s), (state_t *) BIOSDATAPAGE);
    /* Calcolo della causa dello stato del processo che ha sollevato l'eccezione */
    int bits[REGISTERLENGTH];
    int i;
    for(i = 0; i < REGISTERLENGTH; i++) {
        bits[i] = 0;
    }
    decToBin(bits, currentProc->p_s.cause);
    int exceptionCauseCode = binToDec(bits, 2, 6);

    /* In base al suo valore, solleva uno specifico gestore */
    if(exceptionCauseCode == 0) {
        interruptsHandler();
    }
    else if(exceptionCauseCode == 8) {
        systemCallsHandler();
    }
    else if(exceptionCauseCode >= 1 && exceptionCauseCode <= 3) {
        passUpOrDie(0);
    }
    else if((exceptionCauseCode >= 4 && exceptionCauseCode <= 7) || (exceptionCauseCode >= 9 && exceptionCauseCode <= 12)) {
        passUpOrDie(1); 
    }
}