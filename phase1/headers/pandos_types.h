#ifndef PANDOS_TYPES_H_INCLUDED
#define PANDOS_TYPES_H_INCLUDED

/**************************************************************************** 
 *
 * This header file contains utility types definitions.
 * 
 ****************************************************************************/

#include <umps3/umps/types.h>
#include "pandos_const.h"

typedef signed int cpu_t;
typedef unsigned int memaddr;

typedef int support_t;

/* process table entry type */
typedef struct pcb_t {

	/* process queue fields */
    struct pcb_t   *p_next;							/* ptr to next entry										*/
    struct pcb_t   *p_prev; 						/* ptr to previous entry									*/

	/* process tree fields */
	struct pcb_t	*p_prnt, 						/* ptr to parent											*/
					*p_child,						/* ptr to 1st child											*/
					*p_next_sib,					/* ptr to next sibling 										*/
					*p_prev_sib;					/* ptr to prev. sibling										*/
	
	/* process status information */
    state_t p_s;									/* processor state 											*/
    cpu_t p_time;									/* cpu time used by proc 									*/
    int *p_semAdd;									/* ptr to semaphore on which proc is blocked				*/

    /* support layer information */
    support_t *p_supportStruct;

}  pcb_t, *pcb_PTR;

typedef struct semd_t {

    struct semd_t *s_next;							/* ptr to next element on queue 							*/
    int *s_semAdd;    								/* ptr to the semaphore										*/
    pcb_PTR s_procQ;    							/* ptr to tail of the queue of procs. blocked on this sem.	*/

} semd_t, *semd_PTR;

#endif
