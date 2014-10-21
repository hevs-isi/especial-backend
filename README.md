
# QEMU for stm32

For the first init (already done):

	$ git submodule update --init dtc

Delete the `.git` folder of the `qemu_stm32` repository because it is included in a new repository.

## 1) Configure and make qemu

	$ ./configure --enable-debug --target-list="arm-softmmu"
	$ make -j4

## 2) Run qemu

The modified version of qemu for ARM is available in `arm-softmmu/qemu-system-arm`.

	$ ./arm-softmmu/qemu-system-arm --version
	QEMU emulator version 2.1.1, Copyright (c) 2003-2008 Fabrice Bellard

Two new machines are available:

	$ ./arm-softmmu/qemu-system-arm -M help
	Supported machines are:
	stm32-maple          OPEN SOURCE HARDWARE MAPLE / ARDUINO LIKE DEVELOPMENT BOARD
	stm32-p103           Olimex STM32 p103 Dev Board
	[...]

The `cortex-m3` processor is already available in `qemu` :

	$ ./arm-softmmu/qemu-system-arm -M stm32-p103 -cpu help
	Available CPUs:
	cortex-m3
	[...]

See `blink-simple/README` to run a demo application on `qemu`.

# QEMU modifications

The two machines `stm32-maple` and `stm32-p103` have been added to `qemu` (based on version 2.1.1).

The `stm32-p103` machine is an emulation of the STM32-P103 dev kit of Olimex (see https://www.olimex.com/Products/ARM/ST/STM32-P103/).

	MCU: STM32F103RBT6 ARM 32 bit CORTEX M3™ with 128K Bytes Program Flash, 20K Bytes RAM, USB, CAN, x2 I2C, x2 ADC 12 bit, x3 UART, x2 SPI, x3 TIMERS, up to 72Mhz operation

For now LEDs, buttons DTC and UART are emulated and available.

Modified files (`$ grep -r "Andre Beckus" ./`) are:

* ./hw/arm/stm32_p103.c

* ./hw/arm/stm32.c and ./include/hw/arm/stm32.h
* ./hw/arm/stm32_clktree.c and ./include/hw/arm/stm32_clktree.h

* ./hw/arm/stm32_rcc.c
* ./hw/gpio/stm32_exti.c
* ./hw/gpio/stm32_afio.c
* ./hw/gpio/stm32_gpio.c
* ./hw/char/stm32_uart.c

* ./tests/test-stm32.c

Other machine added by `Marius Vlad`:
* ./hw/arm/stm32_maple.c
