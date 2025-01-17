#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/platform.h"

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

#include "FreeRTOS.h"
#include "task.h"
#include "task_defines.h"

#include "spi_helpers.h"
#include "led_helpers.h"
#include "mcp23s18.h"
#include "utils.h"

void main_task();

int main() {
    stdio_uart_init_full(uart1, 115200, 4, 5);
    int ret = pico_led_init();
    hard_assert(ret == PICO_OK);

    printf("hello, world!\n");

    ret = system_spi_init();
    hard_assert(ret == PICO_OK);

    ret = mcp23s18_init();
    hard_assert(ret == PICO_OK);

    printf("drivers and peripherals initialized\n");
    TaskHandle_t main_handle;
    xTaskCreate(main_task, "Main", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &main_handle);

    vTaskStartScheduler();

    printf("closing\n");
}

void main_task() {
    while(true);
}