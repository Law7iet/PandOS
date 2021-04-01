void get_bits(int n) {
    int bits[32];
    int i = 0;
    while (n > 0) {
        bits[i] = n % 2;
        n = n / 2;
        i++;
    }
}

int get_num(int bits[]) {
    int num = 0, i, mult = 1;
    for(i = 0; i < 32; i++) {
        num = num + bits[i] * mult;
        mult = mult * 2;
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

void recTerminateProccess(pcb_t *proc) {
    if(proc != NULL) {        
        /* Se il processo ha figli, si chiama la funzione ricorsivamente sui figli */
        while(!emptyChild(proc)) {
            pcb_t *tmp = outchild(proc);
            recTerminateProcess(tmp);
        }

        /* Aggiornamento dei semafori */
        if(proc->p_semAddr != NULL) {
            /* Flag che indica se il processso è bloccato su un semaforo device */
            int BlockedOnSemDev = checkBlockedOnSemDev(proc->p_semAdd);
            /* Il processo non è bloccato su un semaforo */
            if (*(proc->p_semAdd) < 0 && !BlockedOnSemDev) {
                /* Si aggiorna il valore del semaforo */
                if(*(proc->p_semdAdd) < 0) {
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
        outProcQ(&readyQ, proc);
        freePcb(proc);
        processCount--;
    }
}