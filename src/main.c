#include "pico/stdlib.h"

#define LED_DELAY_MS 500

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
void pico_set_led(bool led_off) {
    gpio_put(PICO_DEFAULT_LED_PIN, led_off);
}

int main() {
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    while (true) {
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
    }
}
