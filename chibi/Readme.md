
# ChibiOS example


Install the compiler, the debugger, and the stlink tools:

`sudo apt-get install gcc-arm-none-eabi gdb-multiarch stlink-tools`


Fetch ChibiOS and the STM32 pin data:

`git submodule update --init --recursive`


Build the firmware:

`make`

Flash the MCU:

`make flash`


The `ext/boardGen.pl` tool is extracted from https://github.com/alex31/chibios_enac_various_common.git.
It uses data from https://github.com/STMicroelectronics/STM32_open_pin_data to generate the `board.h` file from a short `board.cfg`.



HAL doc: http://chibiforge.org/doc/21.11/hal/
