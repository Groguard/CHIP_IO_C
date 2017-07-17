# CHIP_IO Arduino Style for C
A CHIP IO library for C: IO+PWM+SPWM+ADC+Utilities

Arduino Style GPIO control for C.H.I.P and C.H.I.P Pro

# Installation and first build
Attach an LED to XIO-P2 with a resistor to run the blink example::

    sudo apt-get install git build-essential -y
    git clone https://github.com/Groguard/CHIP_IO.git
    cd CHIP_IO/examples/
    arm-linux-gnueabihf-gcc -pthread blink.c ../chip_io_c.c ../source/event_gpio.c ../source/common.c -o blink -static
    sudo ./blink


# Examples

**GPIO Setup**

    #include "chip_io_c.h"
    pinMode("CSID0", OUTPUT);

You can also refer to the pin number:

    pinMode("U14_31", OUTPUT);

You can also refer to the bin based upon its alternate name:

    pinMode("GPIO1", INPUT);

Debug can be enabled/disabled by the following command:

    // Enable Debug
    toggle_debug();

**GPIO Output**

Setup the pin for output, and write HIGH or LOW. Or you can use 1 or 0:

    #include "chip_io_c.h"
    pinMode("CSID0", OUTPUT);
    digitalWrite("CSID0", HIGH);

**GPIO Input**

Inputs work similarly to outputs.:

    #include "chip_io_c.h"
    pinMode("CSID0", INPUT);

Other options when setting up pins:

    // Specify pull up/pull down settings on a pin
    pinMode("CSID0", INPUT_PULLUP);
    pinMode("CSID0", INPUT_PULLDOWN);
    
Pull Up/Down values are only for pins that are provided by the R8, the XIO are not capable of this.  The allowable values are: INPUT_PULLOFF, INPUT_PULLUP, and INPUT_PULLDOWN.

Polling inputs:

    int state = digitalRead("CSID0");
    if(state)
        printf("HIGH\n");
    else
        printf("LOW\n");

**GPIO Cleanup**

To clean up the GPIO when done, do the following:

    // Clean up a single pin (keeping everything else intact)
    cleanup("XIO-P0");
    
    // Clean up all 
    cleanup("ALL");
    
**PWM** 

Hardware PWM requires a DTB Overlay loaded on the CHIP to allow the kernel to know there is a PWM device available to use. Works by defualt on CHIPPro

    //startPwm(channel, duty, polarity);
    
    #inlcude "chip_io_c_pwm.h"
    startPwm("PWM0", 50, 0);
    //duty values are valid 0 (off) to 100 (on)
    setPwmDutyCycle("PWM0", 20.5);
    setPwmFrequency("PWM0", 50);
    
    // to Stop PWM
    stopPwm("PWM0");
    cleanupPwm("PWM0");
    
