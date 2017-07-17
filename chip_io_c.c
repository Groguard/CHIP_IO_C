/*
Copyright (c) 2017 Christopher Alessandro
Copyright (c) 2016 Robert Wolterman
Original BBIO Author Justin Cooper
Modified for CHIP_IO Author Robert Wolterman
This file incorporates work covered by the following copyright and
permission notice, all modified code adopts the original license:
Copyright (c) 2013 Adafruit
Original RPi.GPIO Author Ben Croston
Modified for BBIO Author Justin Cooper
This file incorporates work covered by the following copyright and
permission notice, all modified code adopts the original license:
Copyright (c) 2013 Ben Croston
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "source/common.h"
#include "source/event_gpio.h"
#include "chip_io_c.h"


static int gpio_warnings = 1;
static int r8_mem_setup = 0;

int max_gpio = -1;
dyn_int_array_t *gpio_direction = NULL;

static int init_module(void)
{
    clear_error_msg();

    // If we make it here, we're good to go
    if (DEBUG)
        printf(" ** init_module: setup complete **\n");
    module_setup = 1;

    return 0;
}


static int init_r8_gpio_mem(void)
{
    clear_error_msg();

    if (DEBUG)
        printf(" ** init_r8_gpio_mem: mapping memory **\n");
    
    if (map_pio_memory() < 0) {
        char err[2000];
        snprintf(err, sizeof(err), "init_r8_gpio_mem error (%s)", get_error_msg());
        return 0;
    }

    // If we make it here, we're good to go
    if (DEBUG)
        printf(" ** init_r8_gpio_mem: setup complete **\n");
    r8_mem_setup = 1;
    
    return 0;
}

static void remember_gpio_direction(int gpio, int direction)
{
    dyn_int_array_set(&gpio_direction, gpio, direction, -1);
}


int pinMode(char *channel, uint8_t mode) 
{
    int gpio;
    int allowed = -1;
    int direction;
    int pud = PUD_OFF;
    int initial = 0;
    
    if (mode == INPUT)
        direction = INPUT;
    else if (mode == OUTPUT)
        direction = OUTPUT;
    else if (mode == INPUT_PULLUP) {
        direction = INPUT;
        pud = PUD_UP;
    }
    else if (mode == INPUT_PULLDOWN) {
        direction = INPUT;
        pud = PUD_DOWN;
    }
    else if (mode == INPUT_PULLOFF) {
        direction = INPUT;
        pud = PUD_OFF;
    }
 
    clear_error_msg();

 
    if (!module_setup) {
        init_module();
    }
 
    if (direction != INPUT && direction != OUTPUT) {
        printf("An invalid direction was passed to setup()");
        return 0;
    }
    
    // Force pud to be off if we're configured for output
    if (direction == OUTPUT) {
        pud = PUD_OFF;
    }
 
    if (pud != PUD_OFF && pud != PUD_DOWN && pud != PUD_UP) {
        printf("Invalid value for pull_up_down - should be either PUD_OFF, PUD_UP or PUD_DOWN");
        return 0;
    }
 
    if (get_gpio_number(channel, &gpio) < 0) {
        char err[2000];
        snprintf(err, sizeof(err), "Invalid channel %s. (%s)", channel, get_error_msg());
        return 0;
    }
    
    // Check to see if GPIO is allowed on the hardware
    // A 1 means we're good to go
    allowed = gpio_allowed(gpio);
    if (allowed == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error determining hardware. (%s)", get_error_msg());
        return 0;
    } else if (allowed == 0) {
        char err[2000];
        snprintf(err, sizeof(err), "GPIO %d not available on current Hardware", gpio);
        return 0;
    }
    
    // Only map /dev/mem if we're not an XIO
    if (!r8_mem_setup && !(gpio >= lookup_gpio_by_name("XIO-P0") && gpio <= lookup_gpio_by_name("XIO-P7"))) {
        init_r8_gpio_mem();
    }
    
    int exprtn = gpio_export(gpio);
    if (exprtn == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error setting up channel %s, maybe already exported? (%s)", channel, get_error_msg());
        return 0;
    } else if (exprtn == -2 && gpio_warnings) {
        char warn[2000];
        snprintf(warn, sizeof(warn), "Channel %s may already be exported, proceeding with rest of setup", channel);
    }
    if (gpio_set_direction(gpio, direction) < 0) {
        char err[2000];
        snprintf(err, sizeof(err), "Error setting direction %d on channel %s. (%s)", direction, channel, get_error_msg());
        return 0;
    }

    // Pull Up/Down
    // Only if the pin we want is able to use it (R8 Owned, no XIO)
    int port, pin;
    if (compute_port_pin(channel, gpio, &port, &pin) == 0) {
        // Set the PUD
        gpio_set_pud(port, pin, pud);
        // Check it was set properly
        int pudr = gpio_get_pud(port, pin);
        if (pudr != pud) {
            char err[2000];
            snprintf(err, sizeof(err), "Error setting pull up down %d on channel %s", pud, channel);
            return 0;
        }
    }

    if (direction == OUTPUT) {
        if (gpio_set_value(gpio, initial) < 0) {
            char err[2000];
            snprintf(err, sizeof(err), "Error setting initial value %d on channel %s. (%s)", initial, channel, get_error_msg());
            return 0;
        }
    }
    
    remember_gpio_direction(gpio, direction);

}

int digitalWrite(char *channel, uint8_t value) 
{
    int gpio;
    int allowed = -1;

    clear_error_msg();

    if (get_gpio_number(channel, &gpio)) {
        printf("Invalid channel");
        return 0;
    }

    // Check to see if GPIO is allowed on the hardware
    // A 1 means we're good to go
    allowed = gpio_allowed(gpio);
    if (allowed == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error determining hardware. (%s)", get_error_msg());
        return 0;
    } else if (allowed == 0) {
        char err[2000];
        snprintf(err, sizeof(err), "GPIO %d not available on current Hardware", gpio);
        return 0;
    }

    if (!module_setup || dyn_int_array_get(&gpio_direction, gpio, -1) != OUTPUT)
    {
        char err[2000];
        snprintf(err, sizeof(err), "Channel %s not set up or is not an output", channel);
        return 0;
    }

    int result = gpio_set_value(gpio, value);
    if (result < 0) {
        char err[2000];
        snprintf(err, sizeof(err), "Could not write %d on channel %s. (%s)", value, channel, get_error_msg());
        return 0;
    }
}

int digitalRead(char *channel) 
{
    int gpio;
    unsigned int value;
    int allowed = -1;

    clear_error_msg();

    if (get_gpio_number(channel, &gpio)) {
        printf("Invalid channel");
        return 0;
    }

    // Check to see if GPIO is allowed on the hardware
    // A 1 means we're good to go
    allowed = gpio_allowed(gpio);
    if (allowed == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error determining hardware. (%s)", get_error_msg());
        return 0;
    } else if (allowed == 0) {
        char err[2000];
        snprintf(err, sizeof(err), "GPIO %d not available on current Hardware", gpio);
        return 0;
    }

   // check channel is set up as an input or output
    if (!module_setup || (dyn_int_array_get(&gpio_direction, gpio, -1) == -1))
    {
        printf("You must setup() the GPIO channel first\n");
        return 0;
    }

    if (gpio_get_value(gpio, &value) < 0) {
        char err[1024];
        snprintf(err, sizeof(err), "Could not get value ('%s')", get_error_msg());
        return 0;
    }

    return value;
}

void cleanup(char *channel)
{
    int gpio;

    clear_error_msg();

    // The !channel fixes issues #50
    if (channel == "ALL" || strcmp(channel, "\0") == 0) {
        event_cleanup();
    } else {
        if (get_gpio_number(channel, &gpio) < 0) {
            event_cleanup();
        }
        gpio_unexport(gpio);
    }
}

