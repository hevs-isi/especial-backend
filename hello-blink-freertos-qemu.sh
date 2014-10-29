#!/bin/bash
 
echo ""
echo "Running 'hello-blink-freertos' on QEMU."
echo ""

qemu_stm32/arm-softmmu/qemu-system-arm \
 	-M stm32-p103 \
	-kernel demos/hello-blink-freertos/target-qemu/hello-blink-freertos.elf \
	-serial stdio \
	-nographic \
	-monitor null