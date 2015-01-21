/*
 * Olimex STM32 P103 Development Board
 *
 * Copyright (C) 2010 Andre Beckus
 *
 * Implementation based on
 * Olimex "STM-P103 Development Board Users Manual Rev. A, April 2008"
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

// Time
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "hw/arm/stm32.h"
#include "hw/sysbus.h"
#include "hw/arm/arm.h"
#include "hw/devices.h"
#include "ui/console.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"

// Emulation add-on. Add TCP clients to monitor events and GPIOs.
#include "hw/arm/stm32_p103_emul.h"

/**
 * Called when a new value is written to a GPIO (digital output).
 *
 * This function simulates a write callback. Events are posted through TCP/IP when an output state change.
 *
 * IRQ behavior is defined in the `stm32_gpio.c` file.
 * IRQ are registered in the `stm32_p103_init` function.
 */
static void output_write_handler(void *opaque, int n, int level) {
	// Map the IRQ number with digital outputs pins
	switch (n) {
	case 0:
		post_event_digital_out('C', 12, level);	// LED0 - LED on `PC.12`
		break;
	case 1:
		post_event_digital_out('C', 3, level);	// LED1 - Yellow LED on `PC.3`
		break;
	case 2:
		post_event_digital_out('C', 4, level);	// LED2 - Yellow LED on `PC.4`
		break;
	case 3:
		post_event_digital_out('B', 8, level);	// LED3 - Red LED on `PB.8`
		break;
	case 4:
		post_event_digital_out('B', 9, level);	// LED4 - Red LED on `PB.9`
		break;
	default:
		// Unknown GPIO number or not registered.
		break;
	}
}

/*
 * Example on how to use the QEMU monitor to send command from a terminal.
 * Inside the monitor, you can use the "sendkey" command.
 */
//	static void stm32_p103_key_event(void *opaque, int keycode) {
//		Stm32P103 *s = (Stm32P103 *) opaque;
//		bool make;
//		int core_keycode;
//
//		if ((keycode & 0x80) == 0) {
//			make = true;
//			core_keycode = keycode;
//		} else {
//			make = false;
//			core_keycode = keycode & 0x7f;
//		}
//
//		if (core_keycode == 0x30) {
//			// Responds when a "B" key press is received.
//			// Inside the monitor, you can type "sendkey b"
//		}
//		return;
//	}


static void stm32_p103_init(MachineState *machine) {
	const char* kernel_filename = machine->kernel_filename;
	qemu_irq *outputs_irq;
	Stm32P103 *s;

	s = (Stm32P103 *) g_malloc0(sizeof(Stm32P103));

	stm32_init(/*flash_size*/0x0001ffff, /*ram_size*/0x00004fff, kernel_filename, 8000000, 32768);

	DeviceState *gpio_a = DEVICE(object_resolve_path("/machine/stm32/gpio[a]", NULL));
	DeviceState *gpio_b = DEVICE(object_resolve_path("/machine/stm32/gpio[b]", NULL));
	DeviceState *gpio_c = DEVICE(object_resolve_path("/machine/stm32/gpio[c]", NULL));
	DeviceState *uart2 = DEVICE(object_resolve_path("/machine/stm32/uart[2]", NULL));

	assert(gpio_a);
	assert(gpio_b);
	assert(gpio_c);
	assert(uart2);

	/* Outputs IRQs */

	// IRQs for all digital outputs of the main and extension board.
	// Write callback is `output_write_handler`.
	outputs_irq = qemu_allocate_irqs(output_write_handler, NULL, 5); // 5 LEDs

	// Main red led
	qdev_connect_gpio_out(gpio_c, 12, outputs_irq[0]);	// LED0 on GPIO C pin 12

	// Digital outputs of the extension board
	qdev_connect_gpio_out(gpio_c, 3, outputs_irq[1]);	// LED1 - Yellow LED on `PC.3`
	qdev_connect_gpio_out(gpio_c, 4, outputs_irq[2]);	// LED2 - Yellow LED on `PC.4`
	qdev_connect_gpio_out(gpio_b, 8, outputs_irq[3]);	// LED3 - Red LED on `PB.8`
	qdev_connect_gpio_out(gpio_b, 9, outputs_irq[4]);	// LED4 - Red LED on `PB.9`

	/* Inputs IRQs */

	/**
	 * Simulate and external line interrupt.
	 * Each external lines are registered in the `Stm32P103` structure.
	 * The keycode is used to identify buttons 0 to 3.
	 *
	 * To set the input 0 to '1': qemu_irq_raise(state->btn0_irq);
	 * To set the input 0 to '0': qemu_irq_lower(state->btn0_irq);
	 */

	// Joystick center button
	s->btn0_irq = qdev_get_gpio_in(gpio_c, 6); // Btn0

	// Extension board buttons inputs
	s->btn3_irq = qdev_get_gpio_in(gpio_c, 2); // Btn3
	s->btn2_irq = qdev_get_gpio_in(gpio_c, 1); // Btn2
	s->btn1_irq = qdev_get_gpio_in(gpio_c, 0); // Btn1

	// Debug only. Connect the QEMU monitor here if necessary.
	// qemu_add_kbd_event_handler(stm32_p103_key_event, s);

	/* Connect RS232 to UART */
	stm32_uart_connect((Stm32Uart *) uart2, serial_hds[0], STM32_USART2_NO_REMAP);

	/* QEMU TCP gateway */

	stm32p103_emul_init(s); // TCP clients initialization

	// UART5 used to read/write debug informations from/to QEMU.
	// UART5 cannot be used on the real target. This is a "fake" peripheral that can be used in QEMU only.
	DeviceState *uart5 = DEVICE(object_resolve_path("/machine/stm32/uart[5]", NULL));
	assert(uart5);
	stm32_uart_connect((Stm32Uart *) uart5, NULL, STM32_UART5);

	DBG(": stm32_p103_init done.\n");
}

static QEMUMachine stm32_p103_machine = {
		.name = "stm32-p103",
		.desc = "Olimex STM32 p103 Dev Board",
		.init = stm32_p103_init,
};

static void stm32_p103_machine_init(void) {
	qemu_register_machine(&stm32_p103_machine);
}

machine_init(stm32_p103_machine_init);
