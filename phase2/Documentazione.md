# PandOS
PandOS è un sistema operativo a scopo didattico.
Deve essere realizzato su architettura **uMPS3**, che può essere descritta con **6 livelli** di astrazione.

## Fase 2
La fase 2 del progetto di PandOS si occupa del terzo livello di astrazione, ossia l'**implementazione di un kernel minimale** per il sistema operativo.\
Il kernel si occupa di inizializzare le strutture dati necessarie, caricare il primo processo fornito dal docente, gestire lo scheduling dei processi (*preempitve round robin*) e gestire le eccezioni.

### Semafori
I semafori necessari sono 49; più precisamente: un semaforo per l'interval timer, un semaforo per ogni sub-device.
I sub-device sono 5, di cui il terminale ha bisogno di due semafori; ogni sub-device può avere 8 istanze, quindi: 1 + (5 + 1) * 8 = 49.\
La loro implementazione avviene tramite un array di 49 elementi, indicati con la variabile `sem`, dove:
- `sem[0]` è il semaforo dell'interval timer;
- `sem[i * 1] ... sem[i * 8]` sono gli 8 semafori ordinati dell'`i`-esimo sub-device (ogni semaforo rappresentza un'istanza);
- i device numero 5 e 6 sono i semafori per il terminale (trasmittente e ricevente).

### Eccezioni
Le eccezioni vengono catturate dal *PassUpVector* che attivano la funzione `exceptionHandler()`, che chiama uno specifico gestore in base al codice della causa dell'eccezione.
Il codice viene identificato convertendo con le funzioni `decToBin` e `binToDec` l'intero raprresentante il campo *causa* dell'eccezione: viene inizialmente convertito in un array di bit, e successivvamente si converte in intero la parte dell'array che rappresenta il codice.
Le eccezioni possono essere gestire dal:
- `interruptHandler`;
- `systemCallHandler`;
- `passUpOrDie`.

Gli interrupt a loro volta vengono suddivise in 8 tipologie, identificabili tramite la linea di interrupt nella causa dell'eccezione.
Anche gli interrupt quindi utilizzando la funzione `decToBin` per convertire l'intero in un binario e successivamente analizzare i bit della linea di interrupt.