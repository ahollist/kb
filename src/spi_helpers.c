#include <stdio.h>

#include "pico/stdlib.h"

#include "hardware/spi.h"
#include "hardware/clocks.h"

#include "spi_helpers.h"
#include "led_helpers.h"
#include "mcp23s18.h"
#include "utils.h"

bool print_logs = false;

int system_spi_init() {
    int real_baud = spi_init(spi_default, SPI_BAUDRATE);
    if (print_logs) {
        printf("set spi baudrate at: %d\n", real_baud);
        printf("system clock: %d\n", clock_get_hz(clk_peri));
    }

    gpio_init(SPI_MOSI_PIN);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_MISO_PIN);
    gpio_set_function(SPI_MISO_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_SCLK_PIN);
    gpio_set_function(SPI_SCLK_PIN, GPIO_FUNC_SPI);

    // Manual CS control
    gpio_init(GPIO_EXPANDER_1_CS_PIN_1);
    gpio_set_dir(GPIO_EXPANDER_1_CS_PIN_1, GPIO_OUT);
    gpio_put(GPIO_EXPANDER_1_CS_PIN_1, 1);

    // !RST
    gpio_init(GPIO_EXPANDER_RST_PIN);
    gpio_set_dir(GPIO_EXPANDER_RST_PIN, GPIO_OUT);
    gpio_put(GPIO_EXPANDER_RST_PIN, 1); // Must be high or MCP23S18 will stay off

    // Interrupt Pin
    gpio_init(GPIO_EXPANDER_1_INT_PIN);
    gpio_set_dir(GPIO_EXPANDER_1_INT_PIN, GPIO_IN);

    spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    return PICO_OK;
}   

uint8_t spi_write_byte(uint8_t addr, uint8_t byte){
    uint8_t written = 0;
    const uint8_t msg[MSG_ONE_BYTE_LEN] = {MCP23S18_WRITE, addr, byte};
    uint8_t res[MSG_ONE_BYTE_LEN] = {0,0,0};
    gpio_put(1, 0);
    written = spi_write_read_blocking(spi_default, msg, res, MSG_ONE_BYTE_LEN);
    gpio_put(1, 1);
    if (print_logs) {
        printf("Wrote to %02X: %02X\n", addr, byte);
    }

    return written;
}

uint8_t spi_read_byte(uint8_t addr, uint8_t *data){
    uint8_t written = 0;
    uint8_t msg[MSG_ONE_BYTE_LEN] = {MCP23S18_READ, addr, 0};
    uint8_t res[MSG_ONE_BYTE_LEN] = {0, 0, 0};
    gpio_put(1, 0);
    written = spi_write_read_blocking(spi_default, msg, res, MSG_ONE_BYTE_LEN);
    gpio_put(1, 1);
    *data = res[2];
    if (print_logs) {
        printf("Read from %02X:"BYTE_TO_BINARY_PATTERN"\n", addr, BYTE_TO_BINARY(*data));
    }

    return written;
}

uint8_t spi_write_2_sequential_bytes(uint8_t addr, uint8_t msg1, uint8_t msg2) {
    uint8_t written = 0;
    const uint8_t msg[MSG_TWO_BYTE_LEN] = {MCP23S18_WRITE, addr, msg1, msg2};
    uint8_t res[MSG_TWO_BYTE_LEN] = {0,0,0,0};
    gpio_put(1, 0);
    written = spi_write_read_blocking(spi_default, msg, res, MSG_TWO_BYTE_LEN);
    gpio_put(1, 1);
    if (print_logs) {
        printf("Wrote to %02X and %02X: %02X %02X\n", addr, addr+1, msg1, msg2);
    }

    return written;
}

uint8_t spi_read_2_sequential_bytes(uint8_t addr, uint8_t *data) {
    uint8_t written = 0;
    const uint8_t msg[MSG_TWO_BYTE_LEN] = {MCP23S18_READ, addr, 0, 0};
    uint8_t res[MSG_TWO_BYTE_LEN] = {0,0,0,0};
    gpio_put(1, 0);
    written = spi_write_read_blocking(spi_default, msg, res, MSG_TWO_BYTE_LEN);
    gpio_put(1, 1);
    data[0] = res[2];
    data[1] = res[3];
    if (print_logs) {
        printf("Read from %02X and %02X: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", addr, addr+1, BYTE_TO_BINARY(res[2]), BYTE_TO_BINARY(res[3]));
    }

    return written;
}

int mcp23s18_init(){
    // Bind A/B interrupts to output from both interrupt pins
    uint8_t msg =(1 << IOCON_MIRROR_SHIFT);
    spi_write_byte(IOCON, msg);
    // Set all GPIOs to be internally pulled up
    spi_write_2_sequential_bytes(GPPUA, ENABLE_ALL_BITS, ENABLE_ALL_BITS);
    // Enable interrupt-on-change for each pin
    spi_write_2_sequential_bytes(GPINTENA, ENABLE_ALL_BITS, ENABLE_ALL_BITS);

    return PICO_OK;
}