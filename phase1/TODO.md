# Implementare le strutture dati utilizzate dal kernel

- Process Control Block PCB
    - Allocazione e deallocazione
    - Gestione delle code
    - Gestione dell'albero
    - Gestione dell'active semaphore list
- Semafori
- ASL

## Vincoli
- `pcbFree_h`: lista dei PCB che sono liberi o inutilizzati.
- `pcbFree_table[MAX_PROC]`: array di PCB con dimensione massima di MAX_PROC.
- Ogni genitore contiene un puntatore alla lista dei figli (p_child).
- Ogni figlio ha un puntatore al padre (p_parent) ed un puntatore che collega tra loro i fratelli.
- `semd_table[MAX_PROC]`: array di SEMD con dimensione massima di MAX_PROC.
- `semdFree_h`: Lista dei SEMD liberi o inutilizzati.
- `semd_h`: Lista dei semafori attivi (Active Semaphore List – ASL)

## Consigli
- Non esiste un metodo univoco per l’implementazione delle strutture dati di Fase1.
- Suggerimento: Creare due moduli separati, uno per la gestione dei PCB ed uno per la gestione dei SEMD.
- Usare ove possibile metodi/variabili static.