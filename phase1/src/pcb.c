#include "pandos_const.h"
#include "pandos_types.h"
#include "pcb.h"

pcb_t pcbFree_table[MAXPROC];
pcb_t *pcbFree_h;

void initPcbs() {
    int i;
    pcbFree_h = &pcbFree_table[0];
    for(i = 1; i < MAXPROC; i++) {
        pcbFree_table[i - 1].p_prev = NULL;
        pcbFree_table[i - 1].p_next = &pcbFree_table[i];
    }
    pcbFree_table[MAXPROC - 1].p_next = NULL;
}

void freePcb(pcb_t *p) {
    /* Aggiunge in testa */
    p->p_next = pcbFree_h;
    p->p_prev = NULL;
    pcbFree_h = p;
}

pcb_t * allocPcb() {
    if (pcbFree_h == NULL) {
        return NULL;
    } else {
        /* immagazzina e rimuove il primo elemento in coda */
        pcb_t *pcb = pcbFree_h;
        pcbFree_h = pcbFree_h->p_next;
        pcbFree_h = pcbFree_h->p_prev;
        /* inizializza i campi del pcb */
        pcb->p_next = NULL;
        pcb->p_prev = NULL;
        pcb->p_prnt = NULL;
        pcb->p_child = NULL;
        pcb->p_next_sib = NULL;
        pcb->p_prev_sib = NULL;
        pcb->p_s.entry_hi = 0;
        pcb->p_s.cause = 0;
        pcb->p_s.status = 0;
        pcb->p_s.pc_epc = 0;
        int i;
        for(i = 0; i < STATE_GPR_LEN; i++) {
            pcb->p_s.gpr[i] = 0;
        }
        pcb->p_s.hi = 0;
        pcb->p_s.lo = 0;
        pcb->p_time = 0;
        pcb->p_semAdd = NULL;
        pcb->p_supportStruct = NULL;

        return pcb;
    }
}

pcb_t * mkEmptyProcQ() {
    pcbFree_h = NULL;
    return pcbFree_h;
}

int emptyProcQ(pcb_t *tp) {
    if(tp == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void insertProcQ(pcb_t **tp, pcb_t *p) {
    if(*tp == NULL) {
        /* La lista puntata da tp è vuota, quindi si aggiunge in coda p */
        *tp = p;
        p->p_next = p;
        p->p_prev = p;
    } else {
        /* La lista non è vuota, quindi si aggiunge in coda */
        /* L'elemento viene aggiunto in testa */ 
        p->p_next = *tp;
        p->p_prev = (*tp)->p_prev;
        ((*tp)->p_prev)->p_next = p;
        (*tp)->p_prev = p;
        /* La sentinella cambia puntatore */
        *tp = p;
    }
}

pcb_t * headProcQ(pcb_t **tp) {
    if(*tp == NULL) {
        return NULL;
    } else {
        return (*tp)->p_prev;
    }
}

pcb_t * removeProcQ(pcb_t **tp) {
    if(*tp == NULL) {
        /* La lista è vuota */
        return NULL;
    } else {
        /* La lista non è vuota */
        /* Seleziona l'ultimo elemento */
        pcb_t *tmp;
        tmp = (*tp)->p_prev;
        /* Corregge il puntatore p_next del penultimo elemento*/
        (tmp->p_prev)->p_next = *tp;
        /* Corregge il puntatore p_prev del primo elemento */
        (*tp)->p_prev = tmp->p_prev;
        return tmp;
    }
}

pcb_t * outProcQ(pcb_t **tp, pcb_t *p) {
    if (*tp != NULL) {
        /* La lista non è vuota */
        if((*tp)->p_next == (*tp)->p_prev) {
            if(*tp == p) {
                /* La lista ha un solo elemento che coincide con p*/
                *tp = NULL;
                return p;
            } else {
                /* La lista ha un solo elemento e non coincide con p */
                return NULL;
            }
        } else {
            /* La lista ha più di un elemento */
            pcb_t *tmp;
            tmp = *tp;
            do {
                /* Itera su tutti gli elementi di tmp, alias di *tp */
                if(tmp == p) {
                    /* L'iterato è l'elemento da eliminare */
                    /* Modifica i puntatori degli altri elementi */
                    (tmp->p_prev)->p_next = tmp->p_next;
                    (tmp->p_next)->p_prev = tmp->p_prev;
                    return p;
                }
                tmp = tmp->p_next;
            } while (tmp != *tp);
        }
    }
    return NULL;
}