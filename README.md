## Requirements
Pico SDK is installed to the home folder: `~/pico/pico-sdk` 

arm-none-eabi toolchain is installed to: `/opt/toolchains/arm-gnu-toolchain-14.2.rel1-darwin-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc`

## Building the target

Make sure the CMake extension is installed, and run the build task when you wish, or manually build the binary:
```
$ mkdir build && cd build
$ cmake ..
$ make
```

The resulting .uf2 in `build/src/` can be dragged and dropped into the mounted tiny2040 in bootloader mode. 