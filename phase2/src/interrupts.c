#include "interrupts.h"
#include "init.h"

void interruptsHandler() {
    /* Calcolo della linea di interrupt del processo che ha sollevato l'eccezione */
    state_t *exceptionState = (state_t*) BIOSDATAPAGE;
    int bits[REGISTERLENGTH] = {0};
    DecToBin(exceptionState->cause);

    /* In base al suo valore, solleva uno specifico gestore */
    if(bits[0] == 1) {
        /* Interrupt */
    } else if(bits[1] == 1) {
        /* PLT */
    } else if(bits[2] == 1) {
        /* IT */
    } else if(bits[3] == 1) {
        /* Disk Device */
    } else if(bits[4] == 1) {
        /* Flash Device */
    } else if(bits[5] == 1) {
        /* Network Device */
    } else if(bits[6] == 1) {
        /* Printer Device */
    } else if(bits[7] == 1) {
        /* Terminal Device */
    }
}