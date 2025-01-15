add_executable(kb
    main.c
    )

# pull in common dependencies
target_link_libraries(kb pico_stdlib hardware_spi)

# enable usb serial port and disable uart
pico_enable_stdio_usb(kb 1)
pico_enable_stdio_uart(kb 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(kb)