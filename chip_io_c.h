#ifndef CHIP_IO_C_H
#define CHIP_IO_C_H

#include <stdint.h>

#define NO_EDGE      0
#define RISING_EDGE  1
#define FALLING_EDGE 2
#define BOTH_EDGE    3

#define INPUT  0
#define INPUT_PULLUP  1
#define INPUT_PULLDOWN  2
#define INPUT_PULLOFF  3
#define OUTPUT 4
#define ALT0   8

#define HIGH 1
#define LOW  0

int pinMode(char *channel, uint8_t mode);
int digitalWrite(char *channel, uint8_t value);
int digitalRead(char *channel);
void clean_up(char *channel);

#endif
