#include "exceptions.h"
#include "tools.h"
#include "pandos_const.h"
#include "asl.h"
#include "pcb.h"
#include <umps3/umps/libumps.h>
#include "scheduler.h"
#include "interrupts.h"

void createProcess(state_t *statep, support_t *supportp) {
    /* Nuovo stato */
    state_t *newState = (state_t *) BIOSDATAPAGE;
    /* Nuovo processo */
    pcb_t *newProcess = allocPcb();
    /* Valore di ritorno */
    int returnValue;

    /* Non è possibile istanziare un nuovo processo */
    if(newProcess == NULL) {
        returnValue = -1;
    }
    /* E' possibile istanziare un nuovo processo */
    else {
        returnValue = 0;

        /* Inizializzazione dei campi */
        insertProcQ(&(readyQueue), newProcess);
        processCount++;
        insertChild(currentProc, newProcess);
        copyProcessorState(&(newProcess->p_s), statep);
        newProcess->p_supportStruct = supportp;
        newProcess->p_time = 0;
        newProcess->p_semAdd = NULL;
    }
    
    newState->pc_epc = newState->pc_epc + 4;
    newState->reg_v0 = returnValue;
    LDST(newState);
}

void terminateProcess() {
    recTerminateProcess(currentProc);
    scheduler();
}

void passeren(int *semaddr) {
    if(semaddr != NULL) {
        /* Nuovo stato */
        state_t *newState = (state_t *) BIOSDATAPAGE;
        /* Viene richiesto una risorsa */
        *semaddr = *semaddr - 1;
        /* Se la risorsa non è disponibile, il processo viene bloccato */
        if(*semaddr < 0) {
            unsigned int clock;
            softBlockCount++;
            copyProcessorState(&(currentProc->p_s), newState);
            currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 4;
            currentProc->p_time = currentProc->p_time + STCK(clock);
            insertBlocked(semaddr, currentProc);
            scheduler();
        } else {
            newState->pc_epc = newState->pc_epc + 4;
            LDST(newState);
        }
    }
}

void verhogen(int *semaddr) {
    if(semaddr != NULL) {
        /* Nuovo stato */
        state_t *newState = (state_t *) BIOSDATAPAGE;
        /* Viene rilasciato una risorsa */
        *semaddr = *semaddr + 1;
        /* Se presente, si libera l'ultimo processo bloccato */
        pcb_t *removedProcess = removeBlocked(semaddr);
        if(removedProcess != NULL) {
            insertProcQ(&readyQueue, removedProcess);
        }
        newState->pc_epc = newState->pc_epc + 4;
        LDST(newState);
    }
}

void ioWait(int intLineNo, int devNo, int termRead) {
    /* Indice del semaforo */    
    int index = intLineNo - 3;
    if(intLineNo == 7 && termRead == TRUE) {
        index++;
    }
    index = (index * 8) + devNo + 1;

    /* Nuovo stato */
    state_t *newState = (state_t *) BIOSDATAPAGE;
    /* Registro del semaforo che ha bloccato il processo corrente */
    dtpreg_t *devRegister = (dtpreg_t *) (0x10000054 + ((intLineNo - 3) * 0x80) + (devNo * 0x10));
    /* Salvataggio del valore di ritorno */
    newState->reg_v0 = devRegister->status;
    /* Viene richiesto una risorsa */
    *sem[index] = *sem[index] - 1;
    /* Se la risorsa non è disponibile, il processo viene bloccato */
    if(*sem[index] < 0) {
        unsigned int clock;
        softBlockCount++;
        copyProcessorState(&(currentProc->p_s), newState);
        currentProc->p_s.pc_epc = currentProc->p_s.pc_epc + 4;
        currentProc->p_time = currentProc->p_time + STCK(clock);
        insertBlocked(sem[index], currentProc);
        processCount--;
        scheduler();
    } else {
        newState->pc_epc = newState->pc_epc + 4;
        LDST(newState);
    }
}

void getTime() {
    state_t *newState = (state_t *) BIOSDATAPAGE;
    newState->pc_epc = newState->pc_epc + 4;
    newState->reg_v0 = currentProc->p_time;
    LDST(newState);    
}

void clockWait() {
    softBlockCount++;
    passeren(sem[0]);
}

void getSupportPtr() {
    state_t *newState = (state_t *) BIOSDATAPAGE;
    newState->pc_epc = newState->pc_epc + 4;
    newState->reg_v0 = (unsigned int) currentProc->p_supportStruct;
    LDST(newState);
}

void passUpOrDie(int i) {
    if(currentProc->p_supportStruct == NULL) {
        terminateProcess();
    } else {
        copyProcessorState(&(currentProc->p_supportStruct->sup_exceptState[i]), (state_t *) BIOSDATAPAGE);
        LDCXT(currentProc->p_supportStruct->sup_exceptState[i].reg_sp, currentProc->p_supportStruct->sup_exceptState[i].status, currentProc->p_supportStruct->sup_exceptState[i].pc_epc);
    }
}

void systemCallHandler() {
    /* Stato del processo che ha sollevato l'eccezione */
    state_t *excState = (state_t *) BIOSDATAPAGE;
    /* Array di bits rappresentante la forma binaria di status */
    int bits[REGISTER_LENGTH];
    int i;
    for(i = 0; i < REGISTER_LENGTH; i++) {
        bits[i] = 0;
    }
    decToBin(bits, excState->status);
    /* Il processo corrente è in user mode */
    if(bits[3] == 1) {
        terminateProcess();
    }
    /* Il processo corrente è in kernel mode */
    else {
        switch(excState->reg_a0) {
            case 1:
                createProcess((state_t *) excState->reg_a1, (support_t *) excState->reg_a2);
                break;
            case 2:
                terminateProcess();
                break;
            case 3:
                passeren((int *) excState->reg_a1);
                break;
            case 4:
                verhogen((int *) excState->reg_a1);
                break;
            case 5:
                ioWait((int) excState->reg_a1, (int) excState->reg_a2, (int) excState->reg_a3);
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
                passUpOrDie(GENERALEXCEPT);
        }
    }
}

void exceptionHandler() {
    /* Stato del processo che ha sollevato l'eccezione */
    state_t * excState = (state_t *) BIOSDATAPAGE;
    /* Array di bits rappresentante la forma binaria di status */
    int bits[REGISTER_LENGTH];
    int i;
    for(i = 0; i < REGISTER_LENGTH; i++) {
        bits[i] = 0;
    }
    decToBin(bits, excState->cause);
    /* Causa dell'eccezione */
    int exceptionCauseCode = binToDec(bits, 2, 6);

    /* In base al suo valore, solleva uno specifico gestore */
    if(exceptionCauseCode == 0) {
        interruptsHandler();
    }
    else if(exceptionCauseCode == 8) {
        systemCallHandler();
    }
    else if(exceptionCauseCode >= 1 && exceptionCauseCode <= 3) {
        passUpOrDie(PGFAULTEXCEPT);
    }
    else if((exceptionCauseCode >= 4 && exceptionCauseCode <= 7) || (exceptionCauseCode >= 9 && exceptionCauseCode <= 12)) {
        passUpOrDie(GENERALEXCEPT); 
    }
}