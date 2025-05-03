#ifndef _LED_H
#define _LED_H

#include "pico/stdlib.h"

#include "pin_defines.h"

// Turn the led on or off
void set_led(uint pin, bool led_on) {
    // set to 0 to sink led, set to 1 to turn off
    gpio_put(pin, !led_on);
}

int led_init(void) {
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);

    // Turn off all LEDs
    set_led(LED_R, false);
    set_led(LED_G, false);
    set_led(LED_B, false);

    return PICO_OK;
}


#endif // _LED_H