#include "pandos_const.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"

semd_t semd_table[MAXPROC];
semd_t *semdFree_h;
semd_t *semd_h;

int insertBlocked(int *semAdd, pcb_t *p) {
    semd_t *tmp;
    tmp = semd_h;
    while(tmp->s_next == NULL) {
        if(tmp->s_semAdd == semAdd) {
            /* Si aggiunge p nella coda dei processi bloccati da semAdd */
            /* Aggiungiamo in coda */
            (tmp->s_procQ)->p_next = p;
            p->p_prev = tmp->s_procQ;
            /* s_procQ punta alla coda */
            tmp->s_procQ = p;
            return 0;
        }
        tmp = tmp->s_next;
    }
    if(tmp->s_semAdd == semAdd) {
        /* Si aggiunge p nella coda dei processi bloccati da semAdd */
        /* Aggiungiamo in coda */
        (tmp->s_procQ)->p_next = p;
        p->p_prev = tmp->s_procQ;
        /* s_procQ punta alla coda */
        tmp->s_procQ = p;
        return 0;
    } else {
        /* semAdd non si trova in semd_h */
        if(semdFree_h == NULL) {
            /* semdFree è vuota */
            return 1;
        } else {
            /* Si prende il primo semaforo */
            tmp = semdFree_h;
            semdFree_h = semdFree_h->s_next;
            /* Inizializzazione dei campi */
            tmp->s_next = semdFree_h;
            tmp->s_semAdd = semAdd;
            tmp->s_procQ = p;
            return 0;
        }
    }
}

pcb_t * removeBlocked(int *semAdd) {
    semd_t *pre = NULL;
    semd_t *tmp = semd_h;
    do {
        if(tmp->s_semAdd == semAdd) {
            pcb_t *p;
            p = tmp->s_procQ;
            if(tmp->s_procQ == (tmp->s_procQ)->p_prev) {
                /* C'è solo un processo bloccato */
                /* Si cancella il semaforo */
                if(pre == NULL) {
                    /* C'è solo un semaforo */
                    semd_h = NULL;
                } else {
                    pre->s_next = tmp->s_next;
                    tmp->s_next = semdFree_h;
                    semdFree_h = tmp;
                }
            } else {
                tmp->s_procQ = (tmp->s_procQ)->p_prev;
            }        
            return p;
        }
        pre = tmp;
        tmp = tmp->s_next;
    } while(tmp->s_next != NULL);
    return NULL;
}

pcb_t* outBlocked(pcb_t *p) {
    int *semAdd = p->p_semAdd;
    semd_t *pre = NULL;
    semd_t *tmp = semd_h;
    do {
        if(tmp->s_semAdd == semAdd) {
            pcb_t *tp = outProcQ(&(tmp->s_procQ), p);
            if(tp == NULL) {
                /* C'è solo un processo bloccato o non c'era nessun processo bloccato*/
                /* Si cancella il semaforo */
                if(pre == NULL) {
                    /* C'è solo un semaforo */
                    semd_h = NULL;
                } else {
                    pre->s_next = tmp->s_next;
                    tmp->s_next = semdFree_h;
                    semdFree_h = tmp;
                }
            } else {
                tmp->s_procQ = (tmp->s_procQ)->p_prev;
            }        
            return p;
        }
        pre = tmp;
        tmp = tmp->s_next;
    } while(tmp->s_next != NULL);
    return p;
}

pcb_t * headBlocked(int *semAdd) {
    semd_t *tmp = semd_h;
    do {
        if(tmp->s_semAdd == semAdd) {
            return (tmp->s_procQ)->p_prev;
        }
        tmp = tmp->s_next;
    } while(tmp->s_next != NULL);
    return NULL;
}

void initASL() {
    int i = 0;
    semd_t *tmp;
    tmp = semdFree_h;
    for(i = 0; i < MAXPROC; i++) {
        tmp = &(semd_table[i]);
        tmp = tmp->s_next;
    }
    tmp->s_next = NULL;
}