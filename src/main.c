#include <stdio.h>
#include "limits.h"

#include "pico/stdlib.h"
#include "pico/platform.h"

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "tusb.h"

#include "pin_defines.h"
#include "task_defines.h"
#include "system_defines.h"

#include "led.h"
#include "mcp23s18.h"
#include "utils.h"


typedef struct spi_task_notification {
    uint32_t gpio_expander_0 : 1;
    uint32_t rsvd : 31;
} SPI_Task_Notification_t;

int system_spi_init();

static uint8_t spi_data[NUM_GPIO_EXPANDERS][2] = {0};
static bool data_updated = false;

SemaphoreHandle_t keymap_updated;

TaskHandle_t usb_device_task_handle;
TaskHandle_t spi_task_handle;
TaskHandle_t hid_task_handle;

void usb_device_task();
void spi_task();
void hid_task();

int64_t spi_wakeup_alarm(alarm_id_t id, void* user_data);
void gpio_callback(uint gpio, uint32_t events);

int main() {
    // Initialize uart stdio printing, TX=4, RX=5
    stdio_uart_init_full(uart1, 115200, UART_TX, UART_RX);
    printf("hello, world!\n");

    int ret = 0;

    ret = led_init();
    hard_assert(ret == PICO_OK);

    ret = system_spi_init();
    hard_assert(ret == PICO_OK);

    ret = mcp23s18_init();
    hard_assert(ret == PICO_OK);

    printf("drivers and peripherals initialized\n");

    xTaskCreate(usb_device_task, "USB Device", USB_DEVICE_TASK_STACK_SIZE, NULL, USB_DEVICE_TASK_PRIORITY, &usb_device_task_handle);
    xTaskCreate(spi_task, "SPI", SPI_TASK_STACK_SIZE, NULL, SPI_TASK_PRIORITY, &spi_task_handle);
    xTaskCreate(hid_task, "HID", HID_TASK_STACK_SIZE, NULL, HID_TASK_PRIORITY, &hid_task_handle);

    // Enable GPIO7 level low interrupt
    gpio_set_irq_enabled_with_callback(7,GPIO_IRQ_LEVEL_LOW, true, &gpio_callback);

    vTaskStartScheduler();

    printf("closing\n");
}

void usb_device_task() {

    // Started after scheduler to avoid issues with USB IRQ
    tusb_init();

    while(true){
        // Task waits here for an event, then proceeds
        tud_task();
    }
}

void spi_task() {
    while(true){
        uint32_t notification_value = 0;
        if (pdTRUE == xTaskNotifyWait(0x0UL, ULONG_MAX, &notification_value, portMAX_DELAY)){
            SPI_Task_Notification_t notification = {notification_value};
            // read designated expander and store values into specified buffer
            uint8_t data[2] = {0};
            if (notification.gpio_expander_0) {
                uint8_t bytes_read = mcp23s18_read_2_sequential_bytes(GPIOA, data); // eventually choose CS as well
                (void) bytes_read;
                if (0 == data[0] && 0 == data[1]) {
                    printf("GOT ALL ZEROES\n");
                }
                if (data[0] != spi_data[0][0]) {
                    spi_data[0][0] = data[0];
                    data_updated = true;
                }
                if (data[1] != spi_data[0][1]) {
                    spi_data[0][1] = data[1];
                    data_updated = true;
                }
                // Re-enable associated that was IRQ disabled in the GPIO Callback Function
                gpio_set_irq_enabled(GPIO_EXPANDER_0_INT_PIN, GPIO_IRQ_LEVEL_LOW, true);
            }
        } else {
            printf("Timed out waiting for notification\n");
            // don't do stuff
        }
        if (data_updated){
            // Notify HID task that we have new data to handle
            xTaskNotify(hid_task_handle, 0, eSetBits);
        }
    }
}

void hid_task() {
    while(true) {
        uint32_t notification_value = 0; // We don't use this but maybe in the future
        if (pdTRUE == xTaskNotifyWait(0x0UL, ULONG_MAX, &notification_value, portMAX_DELAY)) {
            // Do stuff with the new data

            if ((spi_data[0][0] | 0b11101111) == 0b11101111) {
                set_led(LED_R, on);
            } else {
                set_led(LED_R, off);
            }
            if ((spi_data[0][0] | 0b11011111) == 0b11011111 ) {
                set_led(LED_G, on);
            } else {
                set_led(LED_G, off);
            }
            if ((spi_data[0][0] | 0b10111111) == 0b10111111 ) {
                set_led(LED_B, on);
            } else {
                set_led(LED_B, off);
            }
            if ((spi_data[0][0] | 0b01111111) == 0b01111111) {
                enable_mcp23s18_logging(true);
            } else {
                enable_mcp23s18_logging(false);
            }

        // Send HID report here
        }
    }
}

int64_t spi_wakeup_alarm(alarm_id_t id, void* user_data){
    BaseType_t task_woken = pdFALSE;
    uint32_t wakeup_value = *(uint32_t*)user_data;
    xTaskNotifyFromISR(spi_task_handle, wakeup_value, eSetBits, &task_woken);
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {
    // Determine from GPIO pin which interrupt has occurred
    static SPI_Task_Notification_t which_expander = {0};
    switch(gpio){
        case GPIO_EXPANDER_0_INT_PIN:
            gpio_set_irq_enabled(GPIO_EXPANDER_0_INT_PIN, GPIO_IRQ_LEVEL_LOW, false);
            which_expander.gpio_expander_0 = true;
            break;
        default:
            printf("No associated expander for %d\n", gpio);
            break;
    }
    // Start 200us Alarm, calling the wakeup directly if the timeout expires before setting
    add_alarm_in_us(SW_DEBOUNCE_US, spi_wakeup_alarm, (void*)&which_expander, true);
}

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

//--------------------------------------------------------------------+
// HID Report Callbacks
//--------------------------------------------------------------------+


// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    // TODO not Implemented
    (void) instance;
    (void) len;
    (void) report;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    // TODO not Implemented

    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;

}


//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    // TODO not Implemented

}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    // TODO not Implemented
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    // TODO not Implemented
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    // TODO not Implemented
}
