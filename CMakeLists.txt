cmake_minimum_required(VERSION 3.12)

# TODO: Verify version of SDKs are sufficient

# Configure SDK (must be before project)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH} CACHE PATH "Path to the Raspberry Pi Pico SDK" FORCE)
message("Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")

# Using a tiny2040 for development, comment out to build for pico or define own
set(PICO_BOARD "pimoroni_tiny2040")

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH} CACHE PATH "Path to the FreeRTOS_KERNEL" FORCE)
message("Using FREERTOS_KERNEL_PATH from environment ('${FREERTOS_KERNEL_PATH}')")
set(FREERTOS_KERNEL_RP2040_RELATIVE_PATH "portable/ThirdParty/GCC")

add_subdirectory(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040 FREERTOS_KERNEL)

# Begin project
project(keyboard C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

add_subdirectory(src)