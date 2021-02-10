#include "listx.h"
#include "pandos_const.h"
#include "pandos_types.h"
#include "pcb.h"

pcb_t pcbFree_table[MAXPROC];
struct list_head pcbFree;
struct list_head * pcbFree_h;

void initPcbs() {
    INITI_LIST_HEAD(&pcbFree_h);
    pcbFree_h = &pcbFree;
    for(int i = 0; i < MAXPROC; i++) {
        struct pcb_t *pcb = &(pcbFree_table[i]);
        list_add(&pcb, pcbFree_h);
    }
}

void freePcb(pcb_t *p) {
    list_add_tail(&p, pcbFree_h);
}

pcb_t * allocPcb() {
    if (list_empty(pcbFree_h) == TRUE) {
        return NULL;
    } else {
        /* immagazzina e rimuove il primo elemento in coda */
        struct list_head *elem = pcbFree_h->next;
        list_del(elem);
        /* cattura il pcb di tale elemento */
        struct pcb_t *pcb = container_of(elem, pcb_t, p_next);
        /* inizializza i campi del pcb */
        INIT_LIST_HEAD(&(pcb->p_next));
        INIT_LIST_HEAD(&(pcb->p_prev));
        INIT_LIST_HEAD(&(pcb->p_prnt));
        INIT_LIST_HEAD(&(pcb->p_child));
        INIT_LIST_HEAD(&(pcb->p_next_sib));
        INIT_LIST_HEAD(&(pcb->p_prev_sib));

        pcb->p_s.entry_hi = 0;
        pcb->p_s.cause = 0;
        pcb->p_s.status = 0;
        pcb->p_s.pc_epc = 0;
        for(int i = 0; i < STATE_GPR_LEN; i++) {
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