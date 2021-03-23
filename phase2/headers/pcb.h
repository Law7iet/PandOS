#ifndef PCB
#define PCB

/* 1.
Inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati. */
void initPcbs();

/* 2.
Inserisce il PCB puntato da p nella lista dei PCB liberi, ossia pcbFree_h. */
void freePcb(pcb_t *p);

/* 3.
Restituisce NULL se la pcbFree_h è vuota.
Altrimenti rimuove un elemento dalla pcbFree, inizializza tutti i campi e restituisce l’elemento rimosso. */
pcb_t * allocPcb();

/* 4.
Crea una lista di PCB, inizializzando la sentinella a NULL.
Ritorna la sentinella. */
pcb_t * mkEmptyProcQ();

/* 5.
Restituisce TRUE se la lista puntata da tp è vuota, FALSE altrimenti. */
int emptyProcQ(pcb_t *tp);

/* 6.
inserisce l’elemento puntato da p nella coda dei processi tp.
La doppia indirezione su tp serve per poter inserire p come ultimo elemento della coda. */
void insertProcQ(pcb_t **tp, pcb_t *p);

/* 7.
Restituisce l’elemento in fondo alla coda dei processi tp, senza rimuoverlo.
Ritorna NULL se la coda non ha elementi. */
pcb_t * headProcQ(pcb_t *tp);

/* 8.
Rimuove l’elemento più vecchio dalla coda tp.
Ritorna NULL se la coda è vuota, altrimenti ritorna il puntatore all’elemento rimosso dalla lista. */
pcb_t * removeProcQ(pcb_t **tp);

/* 9.
Rimuove il PCB puntato da p dalla coda dei processi puntata da tp.
Se p non è presente nella coda, restituisce NULL (p può trovarsi in una posizione arbitraria della coda). */
pcb_t * outProcQ(pcb_t **tp, pcb_t *p);

/* 10.
Restituisce TRUE se il PCB puntato da p non ha figli, FALSE altrimenti. */
int emptyChild(pcb_t *p);

/* 11.
Inserisce il PCB puntato da p come figlio del PCB puntato da prnt. */
void insertChild(pcb_t *prnt, pcb_t *p);

/* 12.
Rimuove il primo figlio del PCB puntato da p.
Se p non ha figli, restituisce NULL. */
pcb_t * removeChild(pcb_t *p);

/* 13.
Rimuove il PCB puntato da p dalla lista dei figli del padre.
Se il PCB puntato da p non ha un padre, restituisce NULL, altrimenti restituisce l’elemento rimosso (cioè p).
A differenza della removeChild, p può trovarsi in una posizione arbitraria (ossia non è necessariamente il primo figlio del padre). */
pcb_t * outChild(pcb_t* p);

#endif