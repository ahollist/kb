#include <stdio.h>

#include "pico/stdlib.h"

#include "hardware/spi.h"
#include "hardware/clocks.h"

#include "pin_defines.h"

#include "mcp23s18.h"
#include "utils.h"

static bool print_logs = false;

void enable_mcp23s18_logging(bool enable){
    print_logs = enable;
}

uint8_t mcp23s18_write_byte(uint8_t addr, uint8_t byte){
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

uint8_t mcp23s18_read_byte(uint8_t addr, uint8_t *data){
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

uint8_t mcp23s18_write_2_sequential_bytes(uint8_t addr, uint8_t msg1, uint8_t msg2) {
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

uint8_t mcp23s18_read_2_sequential_bytes(uint8_t addr, uint8_t *data) {
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
    mcp23s18_write_byte(IOCON, msg);
    // Set all GPIOs to be internally pulled up
    mcp23s18_write_2_sequential_bytes(GPPUA, UINT8_MAX, UINT8_MAX);
    // Enable interrupt-on-change for each pin
    mcp23s18_write_2_sequential_bytes(GPINTENA, UINT8_MAX, UINT8_MAX);

    return PICO_OK;
}