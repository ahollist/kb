add_executable(kb
    main.c
    )

# pull in common dependencies
target_link_libraries(kb pico_stdlib hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(kb)