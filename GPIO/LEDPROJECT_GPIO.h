//Header file for setting GPIO pins to output.

#ifndef PROJECTLEDGPIO_h
#define PROJECTLEDGPIO_h

#include <Arduino.h>

#define Pin_Base (0x700)
#define Port0_Base (0x50000000)
#define Port1_Base (0x50000300)

// uint32_t Pin_Offset;
void Port0Output(int);
void Port1Output(int);
void Port0Input(int);
void Port1Input(int);

#endif