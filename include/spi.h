#ifndef _SPI_H
#define _SPI_H

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"

#include "pin_defines.h"
#include "system_defines.h"

typedef struct spi_task_notification {
    uint32_t gpio_expander_0 : 1;
    uint32_t rsvd : 31;
} SPI_Task_Notification_t;


int system_spi_init() {
    uint real_baud = spi_init(spi_default, SPI_BAUDRATE);
    
    printf("set spi baudrate at: %d\n", real_baud);
    printf("system clock: %d\n", clock_get_hz(clk_peri));

    gpio_init(SPI_MOSI_PIN);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_MISO_PIN);
    gpio_set_function(SPI_MISO_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_SCLK_PIN);
    gpio_set_function(SPI_SCLK_PIN, GPIO_FUNC_SPI);

    // Manual CS control
    gpio_init(GPIO_EXPANDER_0_CS_PIN);
    gpio_set_dir(GPIO_EXPANDER_0_CS_PIN, GPIO_OUT);
    gpio_put(GPIO_EXPANDER_0_CS_PIN, 1);

    // !RST
    gpio_init(GPIO_EXPANDER_0_RST_PIN);
    gpio_set_dir(GPIO_EXPANDER_0_RST_PIN, GPIO_OUT);
    gpio_put(GPIO_EXPANDER_0_RST_PIN, 1); // Must be high or MCP23S18 will stay off

    // Interrupt Pin
    gpio_init(GPIO_EXPANDER_0_INT_PIN);
    gpio_set_dir(GPIO_EXPANDER_0_INT_PIN, GPIO_IN);

    spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    return PICO_OK;
}  

static uint8_t spi_data[NUM_GPIO_EXPANDERS][2] = {0};

#endif // _SPI_H