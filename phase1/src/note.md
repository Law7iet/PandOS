# listx.h
E' una libreria per gestire liste generiche.\n
Le liste generate da tale libreria:
- hanno una sentinella
- sono bidirezionali
- sono circolari

L'elemento di una lista è la struttura `list_head`, dotata di un puntatore al precedente ed al successivo elemento `prec` e `next`.
La sentinella è ti tipo `list_head` e ha i due puntatori che puntavo verso il primo elemento della lista.
Essendo circolari, l'ultimo elemento della lista ha come `next` il primo elemento della lista che ha come `prec` l'ultimo elemento.\n
Tale struttura deve essere implementata in una struttura poiché non contiene dati ma solo puntatori.

## Metodi
- `void INIT_LIST_HEAD(&list)` inizializza la lista gestista dalla sentinella `list`
- `int list_empty(struct list_head *head)` ritorna `1` o `TRUE` se la lista è vuota, `0` o `FALSE` altrimenti.
- `void list_add(struct list_head *new_elem, struct list_head *head)` aggiunge in **testa** alla sentinella `head` l'elemento `new_elem` 
- `void list_add_tail(struct list_head *new_elem, struct list_head *head)` aggiunge in **coda** alla sentinella `head` l'elemento `new_elem`
- `void list_del(struct list_head *entry)` rimuove l'elemento `entry` dalla lista in cui è contenuto

## Macro
- `container_of(ptr, type, member)` ritorna una variabile di tipo `type`, che è l'elemento di nome `member` che ha come puntatore (?) `ptr`