#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/platform.h"

#include "hardware/spi.h"
#include "hardware/clocks.h"

#include "../include/mcp23s18.h"
#include "../include/utils.h"

#define LED_DELAY_MS 500

#define SPI_BAUDRATE 10000000

/* LED STUFF */
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
/* END LED STUFF */

/* SPI STUFF */
uint8_t out_buf[MSG_ONE_BYTE_LEN], in_buf[MSG_ONE_BYTE_LEN];

int spi_gpio_init() {
    gpio_init(0);
    gpio_set_function(0, GPIO_FUNC_SPI);

    gpio_init(2);
    gpio_set_function(2, GPIO_FUNC_SPI);

    //gpio_init(1);
    //gpio_set_function(1, GPIO_FUNC_SPI);

    gpio_init(3);
    gpio_set_function(3, GPIO_FUNC_SPI);

    // Manual CS control
    gpio_init(1);
    gpio_set_dir(1, GPIO_OUT);
    gpio_put(1, 1);

    // !RST
    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);
    gpio_put(5, 1); // Must be high or MCP23S18 will stay off

    spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    return PICO_OK;
}   

uint8_t spi_write_byte(uint8_t addr, uint8_t byte){
    uint8_t written = 0;
    const uint8_t msg[3] = {MCP23S18_WRITE, addr, byte};
    uint8_t res[3] = {0,0,0};
    gpio_put(1, 0);
    written = spi_write_read_blocking(spi_default, msg, res, 3);
    gpio_put(1, 1);
    printf("Wrote to %02X: %04X\n", addr, byte);

    return written;
}

uint8_t spi_read_byte(uint8_t addr, uint8_t *data){
    uint8_t written = 0;
    uint8_t msg[3] = {MCP23S18_READ, addr, 0};
    uint8_t res[3] = {0, 0, 0};
    gpio_put(1, 0);
    written = spi_write_read_blocking(spi_default, msg, res, 3);
    gpio_put(1, 1);
    *data = res[2];
    printf("Read from %02X:"BYTE_TO_BINARY_PATTERN"\n", addr, BYTE_TO_BINARY(*data));

    return written;
}
/* END SPI STUFF */

int main() {
    stdio_init_all();
    int ret = pico_led_init();
    hard_assert(ret == PICO_OK);

    sleep_ms(2000);
    printf("hello, world!\n");

    int real_baud = spi_init(spi_default, SPI_BAUDRATE);
    printf("set spi baudrate at: %d\n", real_baud);
    printf("system clock: %d\n", clock_get_hz(clk_peri));
    ret = spi_gpio_init();
    hard_assert(ret == PICO_OK);
    printf("drivers initialized\n");

    // Enable Sequential Operations
    uint8_t msg = 1 << IOCON_SEQOP_SHIFT;
    spi_write_byte(IOCON, msg);
    // Set all GPIOs to be internally pulled up
    msg = ENABLE_ALL_BITS;
    spi_write_byte(GPPUA, msg);
    spi_write_byte(GPPUB, msg);
    
    uint8_t val = 0;
    while (true) {
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);

        spi_read_byte(GPIOA, &val);
        spi_read_byte(GPIOB, &val);

        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
    }
}
