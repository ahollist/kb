#include <stdio.h>
#include "limits.h"

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

#define NUM_GPIO_EXPANDERS 1
#define SW_DEBOUNCE_US 200

typedef enum {
    FIRST = 0x1,
} GPIOExpander_index_t;

static uint8_t spi_data[NUM_GPIO_EXPANDERS][2] = {0};
static bool data_updated = false;

TaskHandle_t main_task_handle;
TaskHandle_t spi_task_handle;

void main_task();
void spi_task();

int64_t spi_wakeup_alarm(alarm_id_t id, void* user_data);
void gpio_callback(uint gpio, uint32_t events);

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
    xTaskCreate(main_task, "Main", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &main_task_handle);
    xTaskCreate(spi_task, "SPI", SPI_TASK_STACK_SIZE, NULL, SPI_TASK_PRIORITY, &spi_task_handle);

    // Enable GPIO7 level low interrupt
    gpio_set_irq_enabled_with_callback(7,GPIO_IRQ_LEVEL_LOW, true, &gpio_callback);


    vTaskStartScheduler();

    printf("closing\n");
}

void main_task() {
    while(true){
        if (data_updated) {
            // Do stuff with the new data
            if ((spi_data[0][0] | 0b11101111) == 0b11101111) {
                set_led(TINY2040_LED_R_PIN, on);
            } else {
                set_led(TINY2040_LED_R_PIN, off);
            }
            if ((spi_data[0][0] | 0b11011111) == 0b11011111 ) {
                set_led(TINY2040_LED_G_PIN, on);
            } else {
                set_led(TINY2040_LED_G_PIN, off);
            }
            if ((spi_data[0][0] | 0b10111111) == 0b10111111 ) {
                set_led(TINY2040_LED_B_PIN, on);
            } else {
                set_led(TINY2040_LED_B_PIN, off);
            }
            if ((spi_data[0][0] | 0b01111111) == 0b01111111) {
                print_logs = true;
            } else {
                print_logs = false;
            }
        }
    }
}

void spi_task() {
    while(true){
        uint32_t notification_value = 0;
        if (pdTRUE == xTaskNotifyWait(0x0UL, ULONG_MAX, &notification_value, portMAX_DELAY)){
            // read designated expander and store values into specified buffer
            GPIOExpander_index_t gpio_expander_num = FIRST;
            uint8_t data[2] = {0};
            if (notification_value & FIRST) {
                gpio_expander_num = FIRST;
                uint8_t bytes_read = spi_read_2_sequential_bytes(GPIOA, data); // eventually choose CS as well
                if (MSG_TWO_BYTE_LEN != bytes_read){
                    printf("INCORRECT READ: %D INSTEAD OF %D\n", bytes_read, MSG_TWO_BYTE_LEN);
                }
                if (0 == data[0] || 0 == data[1]) {
                    printf("GOT ALL ZEROES\n");
                    continue;
                }
                if (data[0] != spi_data[gpio_expander_num-1][0]) {
                    spi_data[gpio_expander_num-1][0] = data[0];
                    data_updated = true;
                }
                if (data[1] != spi_data[gpio_expander_num-1][1]) {
                    spi_data[gpio_expander_num-1][1] = data[1];
                    data_updated = true;
                }
            }
            gpio_set_irq_enabled(GPIO_EXPANDER_1_INT_PIN, GPIO_IRQ_LEVEL_LOW, true);
        } else {
            printf("Timed out waiting for notification\n");
            // don't do stuff
        }
    }
}

int64_t spi_wakeup_alarm(alarm_id_t id, void* user_data){
    BaseType_t task_woken = pdFALSE;
    xTaskNotifyFromISR(spi_task_handle, *(uint32_t*)user_data, eSetBits, &task_woken);
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {
    gpio_set_irq_enabled(GPIO_EXPANDER_1_INT_PIN, GPIO_IRQ_LEVEL_LOW, false);
    // Determine from GPIO pin which interrupt has occurred
    uint32_t which_expander = 0;
    switch(gpio){
        case GPIO_EXPANDER_1_INT_PIN:
            which_expander = (uint32_t)FIRST;
            break;
        default:
            printf("No associated expander for %d\n", gpio);
            break;
    }
    // Start 200us Alarm, calling the wakeup directly if the timeout expires before setting
    add_alarm_in_us(SW_DEBOUNCE_US, spi_wakeup_alarm, (void*)&which_expander, true);
}