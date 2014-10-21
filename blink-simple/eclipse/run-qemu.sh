#!/bin/bash


echo "Starting QEMU ARM..."

/home/chm/Downloads/arm/qemu_stm32/arm-softmmu/qemu-system-arm \
	-M stm32-p103 \
	-kernel /home/chm/workspace-cpp/blink-simple/target-qemu/blink-simple.elf \
	-monitor stdio
	# 	-clock unix \
	#-serial stdio

	# -monitor stdio
	# -qmp tcp:localhost:4444,server 

#-kernel  \

