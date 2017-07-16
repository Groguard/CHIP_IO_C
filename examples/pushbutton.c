#include <unistd.h>
#include <stdio.h>
#include "../chip_io_c.h"

#define ledPin "LCD-D4"           // LED
#define butPin "XIO-P2"           // BUTTON

void setup(void) 
{
    pinMode(ledPin, OUTPUT);      // sets the digital pin as output
    pinMode(butPin, INPUT);      // sets the digital pin as input
}

void main(void)
{
    setup();
    while(1) {
        int butState = digitalRead(butPin);
        if(butState)
            digitalWrite(ledPin, HIGH);    // sets the LED off 
        else
            digitalWrite(ledPin, LOW);    // sets the LED on
        sleep(0.5);
    }
}
