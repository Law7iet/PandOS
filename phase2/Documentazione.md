### Semafori
I semafori utilizzati sono 49; più precisamente:
- 1 semaforo per l'interval timer
- 1 semaforo per sub-device; ci sono 5 sub-device:
    - Disk
    - Flash
    - Network
    - Printer
    - Terminal (ha bisogno di due semafori)
- PandOS può supportare fino ad 8 istanze
Quindi: 1 + (5 + 1) * 8 = 49

Sono implementati tramite un array di 49 elementi, indicati con la variabile `sem`.
- `sem[0]` è il semaforo dell'interval timer;
- `sem[1] ... sem[8]` sono i semafori per il Disk Device
- `sem[9] ... sem[16]`
- ...to complete