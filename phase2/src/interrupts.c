#include "interrupts.h"
#include "pcb.h"
#include "asl.h"
#include "tools.h"
#include <umps3/umps/types.h>

void interruptsHandler() {
    /* Calcolo della linea di interrupt del processo che ha sollevato l'eccezione */
    state_t *exceptionState = (state_t*) BIOSDATAPAGE;
    int bits[REGISTERLENGTH] = {0};
    DecToBin(bits, exceptionState->cause);
    /* In base al suo valore, solleva uno specifico gestore */
    if(bits[1] == 1) {
        /* PLT */
        setTIMER(5000)
        currentProc->p_s = (state_t *) BIOSDATAPAGE;
        insertProcQ(&readyQueue, currentProc);
        scheduler();
    } else if(bits[2] == 1) {
        /* IT */
        LDIT(100000);
        while(!emptyProcQ(sem[0]->s_procQ)) {
            // unblock all pcb blocked on pseudo-clock semaphore
            verhogen(sem[0]);
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

void deviceInterruptHandler(int IntLineNo) {
    /* Calcolo ddell'indirizzo del registro del device */
    int DevNo = getDeviceNumber(IntLineNo);
    dtpreg *devRegister = 0x1000.0054 + ((IntLineNo - 3) * 0x80) + (DevNo * 0x10);
    int devStatus = devRegister->status;
    devRegister->command = ACK;
    pcb_t *unblockedProc =headBlocked(sem[(IntLineNo-3)*DevNo+1]);
    unblockedProc->p_s.status = devStatus;
    verhogen(sem[(IntLineNo-3)*DevNo+1]);
    LDST(&(currentProc->p_s));
}

int getDeviceNumber(int IntLineNo){
    int bitMap;
    if(IntLineNo == 3){
        bitMap = 0x1000.0040;
        for(int i=0; i<8; i++){
            if(bitMap+i == 1)
                return i;
        }
    }
    if(IntLineNo == 4){
        bitMap = 0x1000.0040 + 0x04;
        for(int i=0; i<8; i++){
            if(bitMap+i == 1)
                return i;
        }
    }
    if(IntLineNo == 5){
        bitMap = 0x1000.0040 + 0x08;
        for(int i=0; i<8; i++){
            if(bitMap+i == 1)
                return i;
        }
    }
    if(IntLineNo == 6){
        bitMap = 0x1000.0040 + 0x0C;
        for(int i=0; i<8; i++){
            if(bitMap+i == 1)
                return i;
        }
    }
    if(IntLineNo == 7){
        bitMap = 0x1000.0040 + 0x10;
        for(int i=0; i<8; i++){
            if(bitMap+i == 1)
                return i;
        }
    }
}
