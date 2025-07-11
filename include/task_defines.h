#ifndef _TASK_DEFINES_H
#define _TASK_DEFINES_H

#include "FreeRTOSConfig.h"
#include "task.h"

#define USB_DEVICE_TASK_PRIORITY ( tskIDLE_PRIORITY + 1UL )
#define SPI_TASK_PRIORITY ( tskIDLE_PRIORITY + 2UL )
#define HID_TASK_PRIORITY ( tskIDLE_PRIORITY + 2UL ) // Priority?

#define USB_DEVICE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define SPI_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define HID_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

TaskHandle_t usb_device_task_handle;
TaskHandle_t spi_task_handle;
TaskHandle_t hid_task_handle;

void usb_device_task();
void spi_task();
void hid_task();


#endif // _TASK_DEFINES_H