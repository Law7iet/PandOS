#include "tools.h"
#include "pcb.h"
#include "asl.h"

#define SEMAPHORELENGTH 49
#define REGISTERLENGTH  32
extern int processCount;
extern int softBlockCount;
extern pcb_t *readyQueue;
extern pcb_t *currentProc;
extern semd_t *sem[SEMAPHORELENGTH];

void decToBin(int bits[], int n) {
    int i = 0;
    while (n > 0) {
        bits[i] = n % 2;
        n = n / 2;
        i++;
    }
}

int binToDec(int bits[], int first, int last) {
    int num = 0;
    int mul = 1;
    int i;
    for(i = 0; i < first; i++) {
      mul = mul * 2;
    }
    for(i = first; i < last; i++) {
        num = num + (bits[i] * mul);
        mul = mul * 2;
    }
    return num;
}

int checkBlockedOnSemDev(int* semaphore) {
    int i;
    for(i = 0; i < SEMAPHORELENGTH; i++) {
        if(sem[i]->s_semAdd == semaphore) {
            return 1;
        }
    }
    return 0;
}

void copyProcessorState(state_t *dst, state_t *src) {
    dst->cause = src->cause;
    dst->entry_hi = src->entry_hi;
    dst->hi = src->hi;
    dst->lo = src->lo;
    dst->pc_epc = src->pc_epc;
    dst->status = src->status;
    int i;
    for(i = 0; i < STATE_GPR_LEN; i++) {
        dst->gpr[i] = src->gpr[i];
    }
}