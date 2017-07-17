#include <unistd.h>
#include <stdio.h>
#include "../chip_io_c_pwm.h"

#define ledPin "PWM0"           // LED

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

void setup(void) {
    startPwm(ledPin, 50, 0);
}

void main(void) {
    setup();
    while(1) {
        // set the brightness of pin PWM0:
      setPwmDutyCycle(ledPin, brightness);

      // change the brightness for next time through the loop:
      brightness = brightness + fadeAmount;

      // reverse the direction of the fading at the ends of the fade:
      if (brightness <= 0 || brightness >= 100) {
        fadeAmount = -fadeAmount;
      }
      // wait for 30 milliseconds to see the dimming effect
      usleep(30000);
  }
}
