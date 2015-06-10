# ESPeciaL - back-end

This repository contains the C++ Hardware Abstraction Layer (HAL) for the [STM32-103STK development board](https://www.olimex.com/Products/ARM/ST/STM32-103STK/) with sample applications and a modified version of the Qemu emulator.

## Qemu for STM32

Based on a modified version of Qemu with an STM32 microcontroller implementation.
Basic STM32F1 microcontroller with the following peripherals. See the [qemu_stm32](http://beckus.github.io/qemu_stm32/) project for more details.
* RCC
* GPIO
* EXTI
* AFIO
* UART
* TIMER (work in progress)

### Configure and make Qemu

	$ ./configure --enable-debug --target-list="arm-softmmu"
	$ make -j4

### Run Qemu

The modified version of Qemu for ARM is available in `arm-softmmu/qemu-system-arm`.

	$ ./arm-softmmu/qemu-system-arm --version
	QEMU emulator version 2.1.1, Copyright (c) 2003-2008 Fabrice Bellard

A base project and Eclipse run configurations are available to emulate STM32 sotware in Qemu.

## License

ESPeciaL is licensed under the [MIT License](https://raw.githubusercontent.com/hevs-isi/especial-frontend/master/LICENSE).