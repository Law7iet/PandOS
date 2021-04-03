#include "interrupts.h"
#include "pcb.h"
#include "asl.h"
#include "tools.h"
#include <umps3/umps/types.h>
#include <umps3/umps/libumps.h>
#include "scheduler.h"
#include "exceptions.h"

int getDeviceNumber(int intLineNo) {
    int bitMap = 0x10000040;
    int i;

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

    for(i = 0; i < 8; i++) {
        if(bitMap+i == 1) {
            return i;
        }
    }

    return -1;
}

void deviceInterruptHandler(int intLineNo) {
    /* Calcolo ddell'indirizzo del registro del device */
    int devNo = getDeviceNumber(intLineNo);
    dtpreg_t *devRegister = (dtpreg_t *) (0x10000054 + ((intLineNo - 3) * 0x80) + (devNo * 0x10));
    /* Registro status del device */
    int devStatus = devRegister->status;
    /* Invio dell'ACK */
    devRegister->command = ACK;
    /* Processo da liberare */
    pcb_t *unblockedProc = headBlocked(sem[(intLineNo - 3) * devNo + 1]);
    /* Liberazione del processo */
    verhogen(sem[(intLineNo - 3) * devNo + 1]);
    /* Salvataggio del registro nel processo da liberare */
    unblockedProc->p_s.gpr[0] = devStatus;
    /* Inserimento della coda dei processi ready */
    insertProcQ(&(readyQueue), unblockedProc);
    /* Ritorno ddel controllo al processo corrente */
    LDST(&(currentProc->p_s));
}

void interruptsHandler() {
    /* Calcolo della linea di interrupt del processo che ha sollevato l'eccezione */
    int bits[REGISTERLENGTH];
    int i;
    for(i = 0; i < REGISTERLENGTH; i++) {
        bits[i] = 0;
    }
    decToBin(bits, currentProc->p_s.cause);
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
        while(sem[0] >= 0) {
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