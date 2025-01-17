add_executable(kb
    main.c
    spi_helpers.c
    led_helpers.c
    )

# pull in common dependencies
target_link_libraries(kb 
    pico_stdlib 
    hardware_spi
    pico_async_context_freertos
    FreeRTOS-Kernel-Heap4
    )

target_compile_definitions(kb PRIVATE
    configNUMBER_OF_CORES=1
    )

target_include_directories(kb PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
    )

# create map/bin/hex file etc.
pico_add_extra_outputs(kb)