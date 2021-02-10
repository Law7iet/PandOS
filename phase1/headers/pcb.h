/* 1.
Initialize the pcbFree list to contain all the elements of the static array of MAXPROC pcbs.
This method will be called only once during data structure initialization. */
void initPcbs();

/* 2.
Insert the element pointed to by p onto the pcbFree list. */
void freePcb(pcb_t *p);

/* 3.
Return NULL if the pcbFree list is empty.
Otherwise, remove an element from the pcbFree list, provide initial values for ALL of the pcbs fields (i.e. NULL and/or 0) and then return a pointer to the removed element.
pcbs get reused, so it is important that no previous value persist in a pcb when it gets reallocated. */
pcb_t * allocPcb();

/* 4.
This method is used to initialize a variable to be tail pointer to a process queue.
Return a pointer to the tail of an empty process queue; i.e. NULL. */
pcb_t * mkEmptyProcQ();

/* 5.
Return TRUE if the queue whose tail is pointed to by tp is empty.
Return FALSE otherwise. */
int emptyProcQ(pcb_t *tp);

/* 6.
Insert the pcb pointed to by p into the process queue whose tail-pointer is pointed to by tp.
Note the double indirection throught p to allow for the possible updating of the tail pointer as well. */
void insertProcQ(pcb_t **tp, pcb_t *p);

/* 7.
Return a pointer to the first pcb from the process queue whose tail is pointed to by tp.
Do not remove this pcb from the process queue.
Return NULL if the process queue is empty. */
pcb_t * headProcQ(pcb_t **tp);

/* 8.
Remove the first (i.e. head) element from the process queue whose tail-pointer is pointed to by tp.
Return NULL if the process queue was initially empty; otherwise return the pointer to the removed element.
Update the process queue’s tail pointer if necessary. */
pcb_t * removeProcQ(pcb_t **tp);

/* 9.
Remove the pcb pointed to by p from the process queue whose tail-pointer is pointed to by tp.
Update the process queue’s tail pointer if necessary.
If the desired entry is not in the indicated queue (an error condition), return NULL; otherwise, return p.
Note that p can point to any element of the process queue. */
pcb_t * outProcQ(pcb_t **tp, pcb_t *p);

/* 10.
Return TRUE if the pcb pointed to by p has no children.
Return FALSE otherwise. */
int emptyChild(pcb_t *p);

/* 11.
Make the pcb pointed to by p a child of the pcb pointed to by prnt. */
void insertChild(pcb_t *prnt, pcb_t *p);

/* 12.
Make the first child of the pcb pointed to by p no longer a child of p.
Return NULL if initially there were no children of p.
Otherwise, return a pointer to this removed first child pcb. */
pcb_t * removeChild(pcb_t *p);

/* 13.
Make the pcb pointed to by p no longer the child of its parent.
If the pcb pointed to by p has no parent, return NULL; otherwise, return p.
Note that the element pointed to by p need not be the first child of its parent. */
pcb_t * outChild(pcb_t* p);