#include "interrupts.h"
#include "tools.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"
#include <umps3/umps/libumps.h>
#include "scheduler.h"
#include "exceptions.h"

int getDeviceNumber(int intLineNo) {
    int bitMap = BITMAP_ADDR;
    switch(intLineNo) {
        case 4:
        bitMap = bitMap + 0x04;
        break;
        case 5:
        bitMap = bitMap + 0x08;
        break;
        case 6:
        bitMap = bitMap + 0x0C;
        break;
        case 7:
        bitMap = bitMap + 0x10;
        break;
    }

    int i;
    for(i = 0; i < 8; i++) {
        if(*((int *) (bitMap + i)) == 1) {
            return i;
        }
    }
    return -1;
}

void deviceInterruptHandler(int intLineNo) {
    /* Stato del processo che ha sollevato l'eccezione */
    state_t *excState = (state_t *) BIOSDATAPAGE;
    /* Calcolo ddell'indirizzo del registro del device */
    int devNo = getDeviceNumber(intLineNo);
    if (devNo != -1) {
        /* Registro del device */
        dtpreg_t *devRegister = (dtpreg_t *) (0x10000054 + ((intLineNo - 3) * 0x80) + (devNo * 0x10));
        /* Registro status del device */
        int devStatus = devRegister->status;
        /* Indice del semaforo */
        int index = ((intLineNo - 3) * devNo) + 1;

        devRegister->command = ACK;
        pcb_t *unblockedProc = headBlocked(sem[index]);
        if(unblockedProc == NULL) {
            unblockedProc->p_s.reg_v0  = devStatus;
            verhogen(sem[index]);
            insertProcQ(&(readyQueue), unblockedProc);
        }
        LDST(excState);
    }
}

void interruptsHandler() {
    /* Stato del processo che ha sollevato l'eccezione */
    state_t *excState = (state_t *) BIOSDATAPAGE;
    /* Array di bits rappresentante la forma binaria di status */
    int bits[REGISTER_LENGTH];
    int i;
    for(i = 0; i < REGISTER_LENGTH; i++) {
        bits[i] = 0;
    }
    decToBin(bits, excState->cause);
    /* In base al suo valore, solleva uno specifico gestore */
    if(bits[9] == 1) {
        /* PLT */
        breakPoint();
        setTIMER(PLT_TIME);
        copyProcessorState(&(currentProc->p_s), excState);
        insertProcQ(&readyQueue, currentProc);
        scheduler();
    } else if(bits[10] == 1) {
        /* IT */
        breakPoint();
        LDIT(IT_TIME);
        while(headBlocked(sem[0]) != NULL) {
            pcb_t *tmp = removeBlocked(sem[0]);
            insertProcQ(&readyQueue, tmp);
            softBlockCount--;
        }
        sem[0] = 0;
        LDST(excState);
    } else if(bits[11] == 1) {
        /* Disk Device */
        breakPoint();
        deviceInterruptHandler(3);
    } else if(bits[12] == 1) {
        /* Flash Device */
        breakPoint();
        deviceInterruptHandler(4);
    } else if(bits[13] == 1) {
        /* Network Device */
        breakPoint();
        deviceInterruptHandler(5);
    } else if(bits[14] == 1) {
        /* Printer Device */
        breakPoint();
        deviceInterruptHandler(6);
    } else if(bits[15] == 1) {
        /* Terminal Device */
        breakPoint();
        deviceInterruptHandler(7);
    }
}