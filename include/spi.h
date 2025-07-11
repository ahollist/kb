#ifndef _SPI_H
#define _SPI_H

#include "pico/stdlib.h"

typedef struct spi_task_notification {
    uint32_t gpio_expander_0 : 1;
    uint32_t rsvd : 31;
} SPI_Task_Notification_t;

int system_spi_init();

#endif // _SPI_H