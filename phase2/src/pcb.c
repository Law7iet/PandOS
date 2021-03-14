#include "pandos_const.h"
#include "pandos_types.h"
#include "pcb.h"

/* Array contenente tutti i processi di PandOS */
pcb_t pcbFree_table[MAXPROC];
/* Lista dei processi liberi o inutilizzati */
pcb_t *pcbFree_h = NULL;

void initPcbs() {
    int i;
    /* Inizializza il primo pcb */
    pcbFree_h = &pcbFree_table[0];
    pcbFree_table[0].p_prev = NULL;
    pcbFree_table[0].p_next = &(pcbFree_table[1]);
    /* Inizializza i pcb intermedi concatenandoli*/
    for(i = 1; i < MAXPROC - 1; i++) {
        pcbFree_table[i].p_prev = &pcbFree_table[i - 1];
        pcbFree_table[i].p_next = &pcbFree_table[i + 1];
    }
    /* Inizializza l'ultimo pcb */
    pcbFree_table[MAXPROC - 1].p_prev = &(pcbFree_table[MAXPROC - 2]);
    pcbFree_table[MAXPROC - 1].p_next = NULL;
}

void freePcb(pcb_t *p) {
    /* Aggiunge in coda */
    p->p_next = pcbFree_h;
    p->p_prev = NULL;
    pcbFree_h = p;
}

pcb_t * allocPcb() {
    /* La lista dei processi liberi è vuota */
    if (pcbFree_h == NULL) {
        return NULL;
    /* La lista dei processi liberi non è vuota */
    /* Immagazzina e rimuove il primo elemento in coda */
    } else {
        /* Puntatore rimossso dalla lista dei processi liberi */
        pcb_t *tmp = pcbFree_h;
        /* Dopo aver rimosso un elemento, la sentinella punta all'elemento successivo */
        /* pcbFree_h ha un solo elemento */
        if(pcbFree_h->p_next == NULL && pcbFree_h->p_prev == NULL) {
            pcbFree_h = NULL;
        /* pcbFree_h ha più di un elemento */
        } else {
            pcbFree_h = pcbFree_h->p_next;
            pcbFree_h->p_prev = NULL;
        }
        /* Inizializza i campi del pcb */
        tmp->p_next = NULL;
        tmp->p_prev = NULL;
        tmp->p_prnt = NULL;
        tmp->p_child = NULL;
        tmp->p_next_sib = NULL;
        tmp->p_prev_sib = NULL;
        tmp->p_s.entry_hi = 0;
        tmp->p_s.cause = 0;
        tmp->p_s.status = 0;
        tmp->p_s.pc_epc = 0;
        int i;
        for(i = 0; i < STATE_GPR_LEN; i++) {
            tmp->p_s.gpr[i] = 0;
        }
        tmp->p_s.hi = 0;
        tmp->p_s.lo = 0;
        tmp->p_time = 0;
        tmp->p_semAdd = NULL;
        tmp->p_supportStruct = NULL;
        /* Ritorna il processo rimosso */
        return tmp;
    }
}

pcb_t * mkEmptyProcQ() {
    /* Sentinella della nuova coda dei processi */
    pcb_t *tmp = NULL;
    return tmp;
}

int emptyProcQ(pcb_t *tp) {
    /* La lista puntata da tp è vuota */
    if(tp == NULL) {
        return 1;
    /* La lista puntata da tp non è vuota */
    } else {
        return 0;
    }
}

void insertProcQ(pcb_t **tp, pcb_t *p) {
    /* La lista puntata da *tp è vuota */    
    if(emptyProcQ(*tp)) {
        *tp = p;
        p->p_next = p;
        p->p_prev = p;
    /* La lista puntata da *tp non è vuota */
    } else {
        p->p_next = (*tp)->p_next;
        ((*tp)->p_next)->p_prev = p;
        p->p_prev = *tp;
        (*tp)->p_next = p;
        /* La sentinella che punta alla coda cambia puntatore */
        *tp = p;
    }
}

pcb_t * headProcQ(pcb_t *tp) {
    /* La lista puntata da tp è vuota */
    if(emptyProcQ(tp)) {
        return NULL;
    /* La lista puntata da tp non è vuota */
    } else {
        /* Essendo la coda circolare, l'elemento più a fondo è p_next della sentinella */
        return tp->p_next;
    }
}

pcb_t * removeProcQ(pcb_t **tp) {
    /* La lista puntata da *tp è vuota */
    if(emptyProcQ(*tp)) {
        return NULL;
    /* La lista puntata da *tp non è vuota */
    } else {
        /* Elemento in testa da rimuovere di *tp */
        pcb_t *tmp = (*tp)->p_next;
        /* La lista ha un solo elemento */
        if(tmp == (*tp)) {
            *tp = NULL;
        /* La lista non ha un solo elemento */
        } else {
            (tmp->p_prev)->p_next = tmp->p_next;
            (tmp->p_next)->p_prev = *tp;
        }
        return tmp;
    }
}

pcb_t * outProcQ(pcb_t **tp, pcb_t *p) {
    /* La lista puntata da *tp è vuota */
    if(emptyProcQ(*tp)) {
        return NULL;
    /* La lista puntata da *tp non è vuota */
    } else {
        /* La lista puntata da *tp ha un solo elemento */
        if((*tp) == (*tp)->p_next && (*tp) == (*tp)->p_prev) {
            /* L'unico elemento di *tp coincide con p */
            if(*tp == p) {
                *tp = NULL;
                return p;
            /* L'unico elemento di *tp non coincide con p */
            } else {
                return NULL;
            }
        /* La lista puntata da *tp ha più di un elemento */
        } else {
            /* Puntatore-indice */
            pcb_t *tmp = *tp;
            /* Usando tmp, itera su tutti gli elementi di *tp */
            do {
                /* L'iterato è l'elemento da eliminare */
                if(tmp == p) {
                    (tmp->p_prev)->p_next = tmp->p_next;
                    (tmp->p_next)->p_prev = tmp->p_prev;
                    return tmp;
                }
                tmp = tmp->p_next;
            } while (tmp != *tp);
        }
        /* p non si trova dentro a *tp */
        return NULL;
    }
}

int emptyChild(pcb_t *p) {
    return emptyProcQ(p->p_child);
}

void insertChild(pcb_t *prn_t, pcb_t *p) {
    /* prn_t non ha figli */
    if(emptyChild(prn_t)) {
        prn_t->p_child = p;
        p->p_next_sib = p;
        p->p_prev_sib = p;
        p->p_prnt = prn_t;
    /* prn_t ha almeno un figlio */
    } else {
        p->p_prnt = prn_t;
        ((prn_t->p_child)->p_next_sib)->p_prev_sib = p;
        p->p_next_sib = (prn_t->p_child)->p_next_sib;
        p->p_prev_sib = prn_t->p_child;
        (prn_t->p_child)->p_next_sib = p;
        prn_t->p_child = p;
    }
}

pcb_t * removeChild(pcb_t *p) {
    /* p non ha figli */
    if(emptyChild(p)) {
        return NULL;
    /* p ha almeno un figlio */
    } else {
        /* L'elemento in testa di p da rimuovere */
        pcb_t *tmp = (p->p_child)->p_next_sib;
        /* p ha solo un figlio */
        if(tmp == tmp->p_next_sib && tmp == tmp->p_prev_sib) {
            p->p_child = NULL;
            return tmp;
        /* p ha almeno un figlio */
        } else {
            (p->p_child)->p_next_sib = tmp->p_next_sib;
            (tmp->p_next_sib)->p_prev_sib = p->p_child;
            return tmp;
        }
    }
}

pcb_t *outChild(pcb_t *p) {
    /* Puntatore al genitore di p */
    pcb_t *prnt = p->p_prnt;
    /* prnt non esiste */
    if(emptyProcQ(prnt)) {
        return NULL;
    /* prnt esiste */
    } else {
        /* prnt non ha figli */
        if(emptyProcQ(prnt->p_child)) {
            return NULL;
        /* prnt ha almeno un figlio */
        } else {
            /* prnt ha un solo un figlio */
            if(prnt->p_child == (prnt->p_child)->p_next && prnt->p_child == (prnt->p_child)->p_prev) {
                /* L'unico figlio di prnt coincide con p */
                if(prnt->p_child == p) {
                    prnt->p_child = NULL;
                    return p;
                /* L'unico figlio di prnt non coincide con p */
                } else {
                    return NULL;
                }
            /* prnt ha più di un figlio */
            } else {
                /* Puntatore-indice */
                pcb_t *tmp = prnt->p_child;
                /* Usando tmp, itera su tutti i figli di *tp */
                do {
                    /* L'iterato tmp è l'elemento da eliminare */
                    if(tmp == p) {
                        (tmp->p_prev_sib)->p_next_sib = tmp->p_next_sib;
                        (tmp->p_next_sib)->p_prev_sib = tmp->p_prev_sib;
                        return tmp;
                    }
                    tmp = tmp->p_next_sib;
                } while (tmp != prnt->p_child);
            }
            /* p non si trova dentro a prnt */
            return NULL;
        }
    }
}