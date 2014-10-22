# Sample application to run on QEMU

This application use the `stm32f1-stdperiph` version 3.5.0. This sample project based is based on [stm32_p103_demos]. The GNU Tools for ARM Embedded Processors (`arm-none-eabi-gcc`) toolchain is used to compile the project. FreeRTOS version 7.1.1 is ready to use.

The application run on a modified version on QEMU ([qemu_stm32]) and use the `stm32-p103` machine. This QEMU version can emulate the [STM32-P103] board. Only some devices are emulated for now. Interrupts, SysTick, GPIOs and UART works.

## How to run

 * Launch the sample project using the following script:
    
```
$ cd blink-simple/eclipse/
$ ./run-qemu.sh # run from the eclipse folder
```

* Or simply import the project in Eclipse and use the `qemu-run` launch configuration available in the `eclipse` folder to compile the project and launch qemu. If necessary, change the path of [qemu_stm32].

The Led (GPIOC, pin 12) will blink each second using a Freertos task. The program output should be the following (using `-serial stdio`):
```
[14:43:34]  GPIO_Pin_12 off

Hello World !
--
[14:43:34]  GPIO_Pin_12 off
[14:43:35]  GPIO_Pin_12 on
[14:43:36]  GPIO_Pin_12 off
[14:43:37]  GPIO_Pin_12 on
[14:43:38]  GPIO_Pin_12 off
```
When a value is set to the GPIOC pin 12, a print is displayed with the current time (only for the simulation in QEMU). The `UART2` of the processor is redirected to the output console.

[stm32_p103_demos]: https://github.com/beckus/stm32_p103_demos
[qemu_stm32]: https://github.com/beckus/qemu_stm32
[STM32-P103]: https://www.olimex.com/Products/ARM/ST/STM32-P103/