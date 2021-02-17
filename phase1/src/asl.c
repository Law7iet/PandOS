#include "pandos_const.h"
#include "pandos_types.h"
#include "asl.h"
#include "pcb.h"

semd_t semd_table[MAXPROC];
semd_t *semdFree_h = NULL;
semd_t *semd_h = NULL;

int insertBlocked(int *semAdd, pcb_t *p) {
    semd_t *tmp;
    tmp = semd_h;
    while(tmp != NULL) {
        /* Cicla sui semafori attivi */
        if(tmp->s_semAdd == semAdd) {
            /* Si aggiunge p nella coda dei processi bloccati da semAdd */
            /* Aggiungiamo in coda */
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
    if(semdFree_h == NULL) {
        /* semdFree è vuota */
        /* Non si può allorare un nuovo semaforo */
        return 1;
    } else {
        /* Si prende il primo semaforo dalla lista dei semafori liberi */
        tmp = semdFree_h;
        semdFree_h = semdFree_h->s_next;
        /* Inizializzazione dei campi */
        tmp->s_semAdd = semAdd;
        tmp->s_procQ = p;
        p->p_semAdd = semAdd;
        p->p_next = p;
        p->p_prev = p;
        /* Aggiunge alla lista dei semafori attivi il semaforo tmp */
        if(semd_h == NULL) {
            /* La lista dei semafori attivi è vuota */
            semd_h = tmp;
            tmp->s_next = NULL;
        } else {
            /* La lista dei semafori attivi non è vuota */
            /* Si aggiunge in testa */
            tmp->s_next = semd_h;
            semd_h = tmp;
        }
        return 0;
    }
}

pcb_t * removeBlocked(int *semAdd) {
    semd_t *pre = NULL;
    semd_t *tmp = semd_h;
    while(tmp != NULL) {
        /* Cicla sui semafori attivi */
        if(tmp->s_semAdd == semAdd) {
            /* tmp è il semaforo ricercato */
            /* In teoria tmp ha almeno un processo figlio */
            /* Prende il processo in testa */
            pcb_t *p = (tmp->s_procQ)->p_next;
            /* Cancella il processo */
            if(p == p->p_next && p == p->p_prev) {
                /* Il semaforo ha solo un processo */
                tmp->s_procQ = NULL;
            } else {
                /* Il semaforo ha più processi in coda */
                (((tmp->s_procQ)->p_next)->p_next)->p_prev = tmp->s_procQ;
                (tmp->s_procQ)->p_next = p->p_next;
            }
            if(tmp->s_procQ == NULL) {
                /* Cancella il semaforo */
                if(pre == NULL) {
                    /* E' il primo semaforo */
                    semd_h = tmp->s_next;
                } else {
                    /* Non è il primo semaforo */
                    pre->s_next = tmp->s_next;
                }
                /* Il semaforo cancellato torna al semdFree */
                tmp->s_next = semdFree_h;
                semdFree_h = tmp;
            }
            return p;
        }
        pre = tmp;
        tmp = tmp->s_next;
    }
    /* semAdd non si trova all'interno dei semafori attivi */
    return NULL;
}

pcb_t* outBlocked(pcb_t *p) {
    int *semAdd = p->p_semAdd;
    semd_t *pre = NULL;
    semd_t *tmp = semd_h;
    while (tmp != NULL) {
        /* Cicla sui semafori attivi */
        if(tmp->s_semAdd == semAdd) {
            /* tmp è il semaforo di p */
            /* Si rimuove p */
            pcb_t *removed = outProcQ(&(tmp->s_procQ), p);
            if(removed == NULL) {
                /* Non è stato eliminato p */
                return NULL;
            } else {
                /* E' stato eliminato p */
                if(tmp->s_procQ == NULL) {
                    /* Non ci sono più processi dentro tmp */
                    /* Si cancella il semaforo */
                    if(pre == NULL) {
                        /* E' il primo semaforo */
                            semd_h = semd_h->s_next;
                    } else {
                        /* Non è il primo semaforo */
                        pre->s_next = tmp->s_next;
                    }
                    /* Il semaforo cancellato torna al semdFree */
                    tmp->s_next = semdFree_h;
                    semdFree_h = tmp;
                }
                return removed;
            }
        }
        pre = tmp;
        tmp = tmp->s_next;
    }
    return NULL;
}

pcb_t * headBlocked(int *semAdd) {
    semd_t *tmp = semd_h;
    while(tmp != NULL) {
        if(tmp->s_semAdd == semAdd) {
            return (tmp->s_procQ)->p_next;
        }
        tmp = tmp->s_next;
    }
    return NULL;
}

void initASL() {
    int i = 0;
    semdFree_h = &semd_table[0];
    for(i = 0; i < MAXPROC - 1; i++) {
        if(i != MAXPROC - 1) {
            semd_table[i].s_next = &(semd_table[i + 1]);
        }
    }
    semd_table[MAXPROC - 1].s_next = NULL;
}