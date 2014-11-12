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

// Emulation add-on. Add TCPclients to monitor events and GPIOs.
#include "hw/arm/stm32_p103_emul.h"

typedef struct {
    Stm32 *stm32;

    /* Joystick center button pressed or not */
    bool last_button_pressed;
    qemu_irq button_irq;

} Stm32P103;

static void btn_write(void *opaque, bool pressed)
{
	Stm32P103 *s = (Stm32P103 *)opaque;

	s->last_button_pressed = pressed;

	if(!s->last_button_pressed)
		qemu_irq_raise(s->button_irq);
	else
		qemu_irq_lower(s->button_irq);
}

inline static void btn_pressed(void *opaque) {
	btn_write(opaque, true);
}

inline static void btn_released(void *opaque) {
	btn_write(opaque, false);
}

/**
 * Called when a new value is written to a digital output.
 * IRQ function to simulate a write callback.
 *
 * IRQ behavior is defined in the `stm32_gpio.c` file.
 */
static void output_write_handler(void *opaque, int n, int level)
{

	uint32_t val = level == 0 ? 1 : 0;

	// Map the IRQ number with the GPIO by hand for now...
    switch(n) {
    // Interrupts 0,1 for GPIOC
    case 0:
    	post_event_digital_out('C', 12, val);
    	break;
    case 1:
    	post_event_digital_out('C', 13, val);
    	break;

    default:
    	// Unknown write
    	break;
    }
}

static void btn_irq_handler(void *opaque, int n, int level)
{
	Stm32P103 *s = (Stm32P103 *)opaque;

	DBG(": btn_irq_handler: n=%d, level=%d\n", n, level);
	DBG(": last_button_pressed=%d\n", s->last_button_pressed);
}

//static void stm32_p103_key_event(void *opaque, int keycode)
//{
//    Stm32P103 *s = (Stm32P103 *)opaque;
//    bool make;
//    int core_keycode;
//
//    if((keycode & 0x80) == 0) {
//        make = true;
//        core_keycode = keycode;
//    } else {
//        make = false;
//        core_keycode = keycode & 0x7f;
//    }
//
//    /* Responds when a "B" key press is received.
//     * Inside the monitor, you can type "sendkey b"
//     */
//    if(core_keycode == 0x30) {
//        if(make) {
//            if(!s->last_button_pressed) {
//                qemu_irq_raise(s->button_irq);
//                s->last_button_pressed = true;
//            }
//        } else {
//            if(s->last_button_pressed) {
//                qemu_irq_lower(s->button_irq);
//                s->last_button_pressed = false;
//            }
//        }
//    }
//    return;
//}


static void stm32_p103_init(MachineState *machine)
{
    const char* kernel_filename = machine->kernel_filename;
    qemu_irq *outputs_irq;
    Stm32P103 *s;

    s = (Stm32P103 *)g_malloc0(sizeof(Stm32P103));

    stm32_init(/*flash_size*/0x0001ffff,
               /*ram_size*/0x00004fff,
               kernel_filename,
               8000000,
               32768);

    DeviceState *gpio_a = DEVICE(object_resolve_path("/machine/stm32/gpio[a]", NULL));
    DeviceState *gpio_c = DEVICE(object_resolve_path("/machine/stm32/gpio[c]", NULL));
    DeviceState *uart2 = DEVICE(object_resolve_path("/machine/stm32/uart[2]", NULL));

    assert(gpio_a);
    assert(gpio_c);
    assert(uart2);

    /*
     * Outputs IRQs for all digital outputs.
     * Write callback is `output_write_handler`.
     */
    outputs_irq = qemu_allocate_irqs(output_write_handler, s, 2);

    qdev_connect_gpio_out(gpio_c, 12, outputs_irq[0]); // LED on GPIO C pin 12
    qdev_connect_gpio_out(gpio_c, 13, outputs_irq[1]); // GPIO C pin 13

    /* Inputs IRQs */

    /* Connect button to GPIO C pin 6 (Joystick button center) */
    s->button_irq = qemu_allocate_irq(btn_irq_handler, s, 1);
    qdev_connect_gpio_out(gpio_c, 6, s->button_irq);

    //s->button_irq = qdev_get_gpio_in(gpio_a, 0);
    //qemu_add_kbd_event_handler(stm32_p103_key_event, s);



    /* Connect RS232 to UART */
    stm32_uart_connect(
            (Stm32Uart *)uart2,
            serial_hds[0],
            STM32_USART2_NO_REMAP);


    stm32p103_emul_init();	// QEMU TCP gateway init
 }

static QEMUMachine stm32_p103_machine = {
    .name = "stm32-p103",
    .desc = "Olimex STM32 p103 Dev Board",
    .init = stm32_p103_init,
};


static void stm32_p103_machine_init(void)
{
    qemu_register_machine(&stm32_p103_machine);
}

machine_init(stm32_p103_machine_init);
