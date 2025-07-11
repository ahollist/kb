add_executable(kb
    main.c
    led.c
    mcp23s18.c
    spi.c
    usb_descriptors.c
    )

# pull in common dependencies
target_link_libraries(kb 
    pico_stdlib 
    hardware_spi
    pico_async_context_freertos
    tinyusb_device
    FreeRTOS-Kernel-Heap4
    )

target_compile_definitions(kb PRIVATE
    configNUMBER_OF_CORES=1
    PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
    )

target_include_directories(kb PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
    ${PICO_TINYUSB_PATH}/
    )

# create map/bin/hex file etc.
pico_add_extra_outputs(kb)