#ifndef _LED_HELPERS_H
#define _LED_HELPERS_H

#include "pico/stdlib.h"

int pico_led_init(void);

// Turn the led on or off
void set_led(uint pin, bool led_on);

#endif // _LED_HELPERS_H