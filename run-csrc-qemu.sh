#!/bin/bash
 
echo ""
echo "Running 'csrc.elf' on QEMU."
echo ""

# To remove the monitor: -monitor null
qemu_stm32/arm-softmmu/qemu-system-arm \
 	-M stm32-p103 \
	-kernel ../ScalaTest/especial/csrc/target-qemu/csrc.elf \
	-serial stdio \
	-nographic
