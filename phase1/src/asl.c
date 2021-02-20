#include "pandos_const.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"

/* Array di semafori di PandOS */ 
semd_t semd_table[MAXPROC];
/* Lista dei semafori libero o inutilizzati */
semd_t *semdFree_h = NULL;
/* Lista dei semafori utilizzati */
semd_t *semd_h = NULL;

int insertBlocked(int *semAdd, pcb_t *p) {
    /* Puntatore-iteratore */
    semd_t *tmp = semd_h;
    /* Usando tmp, itera su tutti gli elementi di semd_h */   
    while(tmp != NULL) {
        /* tmp punta al semaforo semAddd */
        if(tmp->s_semAdd == semAdd) {
            p->p_semAdd = tmp->s_semAdd;
            ((tmp->s_procQ)->p_next)->p_prev = p;
            p->p_next = (tmp->s_procQ)->p_next;
            p->p_prev = tmp->s_procQ;
            (tmp->s_procQ)->p_next = p;
            tmp->s_procQ = p;
            return 0;
        }
        tmp = tmp->s_next;
    }
    /* semAdd non si trova in semd_h */
    /* semdFree è vuota */
    /* Non si può allorare un nuovo semaforo */
    if(semdFree_h == NULL) {
        return 1;
    /* Si prende il primo semaforo dalla lista dei semafori liberi */
    } else {    
        tmp = semdFree_h;
        /* Ridefinisce la sentinella di semdFree_h */
        semdFree_h = semdFree_h->s_next;
        /* Inizializzazione dei campi del nuovo semaforo */
        tmp->s_semAdd = semAdd;
        tmp->s_procQ = p;
        p->p_semAdd = semAdd;
        p->p_next = p;
        p->p_prev = p;
        /* Aggiunge alla lista dei semafori attivi il semaforo tmp */
        /* La lista dei semafori attivi è vuota */
        if(semd_h == NULL) {
            semd_h = tmp;
            tmp->s_next = NULL;
        /* La lista dei semafori attivi non è vuota */
        } else {
            tmp->s_next = semd_h;
            semd_h = tmp;
        }
        return 0;
    }
}

pcb_t * removeBlocked(int *semAdd) {
    /* Puntatore al semaforo precedente di tmp */
    semd_t *pre = NULL;
    /* Puntatore-iteratore */
    semd_t *tmp = semd_h;
    /* Usando tmp, itera su tutti gli elementi di semd_h */
    while(tmp != NULL) {
        /* tmp è il semaforo ricercato */
        if(tmp->s_semAdd == semAdd) {
            /* Puntatore al processo rimosso */
            pcb_t *removed = removeProcQ(&(tmp->s_procQ));
            /* Controlla il semaforo */
            /* Il semaforo non ha più processi */
            /* Cancella il semaforo dalla lista dei semafori attivi */
            if(tmp->s_procQ == NULL) {
                /* E' il primo semaforo puntato da semd_h */
                if(pre == NULL) {
                    semd_h = tmp->s_next;
                /* Non è il primo semaforo puntato da semd_h */
                } else {
                    pre->s_next = tmp->s_next;
                }
                /* Il semaforo cancellato torna a semdFree */
                tmp->s_next = semdFree_h;
                semdFree_h = tmp;
            }
            return removed;
        }
        pre = tmp;
        tmp = tmp->s_next;
    }
    /* semAdd non si trova all'interno dei semafori attivi */
    return NULL;
}

pcb_t* outBlocked(pcb_t *p) {
    /* Puntatore al semaforo di p */
    int *semAdd = p->p_semAdd;
    /* Puntatore al semaforo precedente di tmp */
    semd_t *pre = NULL;
    /* Puntatore-iteratore */
    semd_t *tmp = semd_h;
    /* Usando tmp, itera su tutti gli elementi di semd_h */
    while (tmp != NULL) {
        /* tmp è il semaforo ricercato */
        if(tmp->s_semAdd == semAdd) {
            /* Puntatore al processo rimosso */
            pcb_t *removed = outProcQ(&(tmp->s_procQ), p);
            /* Non è stato eliminato p - condizione di errore */
            if(removed == NULL) {
                return NULL;
            /* E' stato eliminato p */
            } else {
                /* Controlla il semaforo */
                /* Il semaforo non ha più processi */
                /* Cancella il semaforo dalla lista dei semafori attivi */
                if(tmp->s_procQ == NULL) {
                    /* E' il primo semaforo puntato da semd_h */
                    if(pre == NULL) {
                            semd_h = semd_h->s_next;
                    /* Non è il primo semaforo puntato da semd_h */
                    } else {
                        pre->s_next = tmp->s_next;
                    }
                    /* Il semaforo cancellato torna a semdFree */
                    tmp->s_next = semdFree_h;
                    semdFree_h = tmp;
                }
                return removed;
            }
        }
        pre = tmp;
        tmp = tmp->s_next;
    }
    /* Il semaforo di p non si trova all'interno della lista dei semafori attivi - condizione di errore */
    return NULL;
}

pcb_t * headBlocked(int *semAdd) {
    /* Puntatore-iteratore */
    semd_t *tmp = semd_h;
    /* Usando tmp, itera su tutti gli elementi di semd_h */
    while(tmp != NULL) {
        /* tmp è il semaforo ricercato */
        if(tmp->s_semAdd == semAdd) {
            return (tmp->s_procQ)->p_next;
        }
        tmp = tmp->s_next;
    }
    /* semAdd non è all'interno dei semafori attivi */
    return NULL;
}

void initASL() {
    int i = 0;
    /* Inizializzazione del primo elemento */
    semdFree_h = &semd_table[0];
    /* Inizializzazione degli elementi intermedi */
    for(i = 0; i < MAXPROC - 1; i++) {
        if(i != MAXPROC - 1) {
            semd_table[i].s_next = &(semd_table[i + 1]);
        }
    }
    /* Inizializzazione dell'ultimo elemento */
    semd_table[MAXPROC - 1].s_next = NULL;
}