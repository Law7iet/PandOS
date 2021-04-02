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
        while(0) {
            // unblock all pcb blocked on pseudo-clock semaphore 
        }
        sem[0] = 0;
        LDST((state_t *) BIOSDATAPAGE);

    } else if(bits[3] == 1) {
        /* Disk Device */
    } else if(bits[4] == 1) {
        /* Flash Device */
    } else if(bits[5] == 1) {
        /* Network Device */
    } else if(bits[6] == 1) {
        /* Printer Device */
    } else if(bits[7] == 1) {
        /* Terminal Device */
    }
}

void deviceInterruptHandler(int IntLineNo) {
    /* Calcolo ddell'indirizzo del registro del device */
    dtpreg *devRegister;
    int devStatus = devRegister->status;
    devRegister->command = ACK;
}