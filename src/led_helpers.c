#include "led_helpers.h"

int pico_led_init(void) {
    gpio_init(TINY2040_LED_R_PIN);
    gpio_set_dir(TINY2040_LED_R_PIN, GPIO_OUT);
    gpio_init(TINY2040_LED_G_PIN);
    gpio_set_dir(TINY2040_LED_G_PIN, GPIO_OUT);
    gpio_init(TINY2040_LED_B_PIN);
    gpio_set_dir(TINY2040_LED_B_PIN, GPIO_OUT);

    // Turn off all LEDs
    gpio_put(TINY2040_LED_R_PIN, true);
    gpio_put(TINY2040_LED_G_PIN, true);
    gpio_put(TINY2040_LED_B_PIN, true);

    return PICO_OK;
}

// Turn the led on or off
void set_led(uint pin, bool led_on) {
    // set to 0 to sink led, set to 1 to turn off
    gpio_put(pin, !led_on);
}