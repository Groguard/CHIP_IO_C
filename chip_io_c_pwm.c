/*
Copyright (c) 2017 Christopher Alessandro

Copyright (c) 2016 Robert Wolterman

Original BBIO Author Justin Cooper
Modified for CHIP_IO Author Robert Wolterman

This file incorporates work covered by the following copyright and
permission notice, all modified code adopts the original license:

Copyright (c) 2013 Adafruit
Author: Justin Cooper

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
#include <string.h>
#include "source/common.h"
#include "source/c_pwm.h"
#include "chip_io_c_pwm.h"


int cleanupPwm(char *channel)
{
    char key[8];

    clear_error_msg();

    // The !channel fixes issues #50
    if (channel == NULL || strcmp(channel, "\0") == 0) {
        pwm_cleanup();
    } else {
        if (!get_pwm_key(channel, key)) {
            pwm_cleanup();
        }
        pwm_disable(key);
    }
}

static int init_module(void)
{
    clear_error_msg();

    // If we make it here, we're good to go
    if (DEBUG)
        printf(" ** init_module: setup complete **\n");
    module_setup = 1;

    return 0;
}


int startPwm(char *channel, float duty_cycle, int polarity)
{
    char key[8];
    float frequency = 2000.0;
    int allowed = -1;

    clear_error_msg();

    if (!module_setup) {
        init_module();
    }

    if (!get_pwm_key(channel, key)) {
        printf("Invalid PWM key or name.");
        return 0;
    }

    // Check to see if PWM is allowed on the hardware
    // A 1 means we're good to go
    allowed = pwm_allowed(key);
    if (allowed == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error determining hardware. (%s)", get_error_msg());
        return 0;
    } else if (allowed == 0) {
        char err[2000];
        snprintf(err, sizeof(err), "PWM %s not available on current Hardware", key);
        return 0;
    }

    if (duty_cycle < 0.0 || duty_cycle > 100.0) {
        printf("duty_cycle must have a value from 0.0 to 100.0");
        return 0;
    }

    if (frequency <= 0.0) {
        printf("frequency must be greater than 0.0");
        return 0;
    }

    if (polarity < 0 || polarity > 1) {
        printf("polarity must be either 0 or 1");
        return 0;
    }

    if (pwm_start(key, duty_cycle, frequency, polarity) < 0) {
        char err[2000];
        snprintf(err, sizeof(err), "Unable to start PWM: %s (%s)", channel, get_error_msg());
        return 0;
    }
}

int setPwmDutyCycle(char *channel, float duty_cycle)
{
    char key[8];
    int allowed = -1;

    clear_error_msg();

    if (duty_cycle < 0.0 || duty_cycle > 100.0) {
        printf("duty_cycle must have a value from 0.0 to 100.0");
        return 0;
    }

    if (!get_pwm_key(channel, key)) {
        printf("Invalid PWM key or name.");
        return 0;
    }

    // Check to see if PWM is allowed on the hardware
    // A 1 means we're good to go
    allowed = pwm_allowed(key);
    if (allowed == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error determining hardware. (%s)", get_error_msg());
        return 0;
    } else if (allowed == 0) {
        char err[2000];
        snprintf(err, sizeof(err), "PWM %s not available on current Hardware", key);
        return 0;
    }

    if (pwm_set_duty_cycle(key, duty_cycle) == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "PWM: %s issue: (%s)", channel, get_error_msg());
        return 0;
    }
}

int setPwmFrequency(char *channel, float frequency) 
{
    char key[8];
    int allowed = -1;

    clear_error_msg();

    if (frequency <= 0.0) {
        printf("frequency must be greater than 0.0");
        return 0;
    }

    if (!get_pwm_key(channel, key)) {
        printf("Invalid PWM key or name.");
        return 0;
    }

    // Check to see if PWM is allowed on the hardware
    // A 1 means we're good to go
    allowed = pwm_allowed(key);
    if (allowed == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error determining hardware. (%s)", get_error_msg());
        return 0;
    } else if (allowed == 0) {
        char err[2000];
        snprintf(err, sizeof(err), "PWM %s not available on current Hardware", key);
        return 0;
    }

    if (pwm_set_frequency(key, frequency) < 0) {
        char err[2000];
        snprintf(err, sizeof(err), "PWM: %s issue: (%s)", channel, get_error_msg());
        return 0;
    }
}

int stopPwm(char *channel)
{
    char key[8];
    int allowed = -1;

    clear_error_msg();

    if (!get_pwm_key(channel, key)) {
        printf("Invalid PWM key or name.");
        return 0;
    }

    // Check to see if PWM is allowed on the hardware
    // A 1 means we're good to go
    allowed = pwm_allowed(key);
    if (allowed == -1) {
        char err[2000];
        snprintf(err, sizeof(err), "Error determining hardware. (%s)", get_error_msg());
        return 0;
    } else if (allowed == 0) {
        char err[2000];
        snprintf(err, sizeof(err), "PWM %s not available on current Hardware", key);
        return 0;
    }

    if (pwm_disable(key) < 0) {
        char err[2000];
        snprintf(err, sizeof(err), "PWM: %s issue: (%s)", channel, get_error_msg());
        return 0;
    }
}
    
