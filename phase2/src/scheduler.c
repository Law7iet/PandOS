#include "pandos_const.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"
#include "init.h"

void scheduler() {
    insertProcQ(&readyQueue, currentProc);
    pcb_t * readyProc = removeProcQ(readyQueue);
    currentProc =readyProc;
    LDIT(5000);
    LDST(&(currentProc->p_s));
    if(emptyProcQ(readyQueue) == 1) {
        if(processCount == 0)
            HALT();
        else if(processCount > 0 && softBlockCount > 0) {
            currentProc->p_s.gpr[0] = 1;
            procTest->p_s.gpr[0] = 1;
            procTest->p_s.gpr[8] = 1;
            procTest->p_s.gpr[9] = 1;
            procTest->p_s.gpr[10] = 1;
            procTest->p_s.gpr[11] = 1;
            procTest->p_s.gpr[12] = 1;
            procTest->p_s.gpr[13] = 1;
            procTest->p_s.gpr[14] = 1;
            procTest->p_s.gpr[15] = 1;
            procTest->p_s.gpr[27] = 0;
            WAIT();
        }
        else if(processCount > 0 && softBlockCount > 0)
            PANIC();
    }
}
