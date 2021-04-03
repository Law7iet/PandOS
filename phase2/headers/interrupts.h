#ifndef INTERRUPTS
#define INTERRUPTS

int getDeviceNumber(int intLineNo);
void deviceInterruptHandler(int intLineNo);
void interruptsHandler();

#endif