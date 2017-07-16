#include <unistd.h>
#include <stdio.h>
#include "../chip_io_c.h"

#define ledPin "XIO-P2"           // LED

void setup(void) 
{
    pinMode(ledPin, OUTPUT);      // sets the digital pin as output
}

void main(void)
{
    setup();
    while(1) {
        digitalWrite(ledPin, HIGH);   // sets the LED on
        sleep(1);                  // waits for a second
        digitalWrite(ledPin, LOW);    // sets the LED off
        sleep(1);                  // waits for a second
    }
}
