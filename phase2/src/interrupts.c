#include "interrupts.h"
#include "pcb.h"
#include "asl.h"
#include "tools.h"
#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>
#include "scheduler.h"
#include "exceptions.h"

int getDeviceNumber(int IntLineNo) {
    int bitMap = 0x10000040;
    int i;

    switch(IntLineNo) {
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

    for(i = 0; i < 8; i++) {
        if(bitMap+i == 1) {
            return i;
        }
    }

    return -1;
}

void deviceInterruptHandler(int IntLineNo) {
    /* Calcolo ddell'indirizzo del registro del device */
    int DevNo = getDeviceNumber(IntLineNo);
    dtpreg_t *devRegister = (dtpreg_t *) (0x10000054 + ((IntLineNo - 3) * 0x80) + (DevNo * 0x10));
    int devStatus = devRegister->status;
    devRegister->command = ACK;
    pcb_t *unblockedProc = headBlocked(sem[(IntLineNo-3)*DevNo+1]->s_semAdd);
    unblockedProc->p_s.status = devStatus;
    verhogen(sem[(IntLineNo-3)*DevNo+1]->s_semAdd);
    LDST(&(currentProc->p_s));
}

void interruptsHandler() {
    /* Calcolo della linea di interrupt del processo che ha sollevato l'eccezione */
    state_t *exceptionState = (state_t*) BIOSDATAPAGE;
    int bits[REGISTERLENGTH];
    int i;
    for(i = 0; i < REGISTERLENGTH; i++) {
        bits[i] = 0;
    }
    decToBin(bits, exceptionState->cause);
    /* In base al suo valore, solleva uno specifico gestore */
    if(bits[1] == 1) {
        /* PLT */
        setTIMER(5000);
        copyProcessorState(&(currentProc->p_s), (state_t*) BIOSDATAPAGE);
        insertProcQ(&readyQueue, currentProc);
        scheduler();
    } else if(bits[2] == 1) {
        /* IT */
        LDIT(100000);
        while(!emptyProcQ(sem[0]->s_procQ)) {
            /* unblock all pcb blocked on pseudo-clock semaphore */
            verhogen(sem[0]->s_semAdd);
        }
        sem[0] = 0;
        LDST((state_t *) BIOSDATAPAGE);

    } else if(bits[3] == 1) {
        /* Disk Device */
        deviceInterruptHandler(3);
    } else if(bits[4] == 1) {
        /* Flash Device */
        deviceInterruptHandler(4);
    } else if(bits[5] == 1) {
        /* Network Device */
        deviceInterruptHandler(5);
    } else if(bits[6] == 1) {
        /* Printer Device */
        deviceInterruptHandler(6);
    } else if(bits[7] == 1) {
        /* Terminal Device */
        deviceInterruptHandler(7);
    }
}