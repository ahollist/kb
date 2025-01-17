#ifndef _SPI_HELPERS_H
#define _SPI_HELPERS_H

#include "pico/stdlib.h"

#define SPI_BAUDRATE 10000000

#define SPI_MISO_PIN 0
#define SPI_MOSI_PIN 3
#define SPI_SCLK_PIN 2
#define GPIO_EXPANDER_RST_PIN 6
#define GPIO_EXPANDER_1_INT_PIN 7
#define GPIO_EXPANDER_1_CS_PIN_1 1

extern bool print_logs;
extern uint8_t val[2];

int system_spi_init();

uint8_t spi_write_byte(uint8_t addr, uint8_t byte);

uint8_t spi_read_byte(uint8_t addr, uint8_t *data);

uint8_t spi_write_2_sequential_bytes(uint8_t addr, uint8_t msg1, uint8_t msg2);

uint8_t spi_read_2_sequential_bytes(uint8_t addr, uint8_t *data);

int mcp23s18_init();

#endif // _SPI_HELPERS_H