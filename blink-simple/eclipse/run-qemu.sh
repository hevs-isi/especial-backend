#!/bin/bash

echo "Run 'blink-simple' on QEMU."
echo ""

../../qemu_stm32/arm-softmmu/qemu-system-arm \
	-M stm32-p103 \
	-kernel ../target-qemu/blink-simple.elf \
	-monitor stdio