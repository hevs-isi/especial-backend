/*
 * Control the execution of the MCU code running in QEMU over TCP/IP.
 *
 * Events are sent to monitor the code execution (see QemuLogger).
 * Commands can be sent from the Scala side to set input values (EXT lines only).
 * When an output value change, its value is sent to the Scala side automatically.
 *
 * STM32 P103 Olimex board emulation.
 * Code adapted from the "REPTAR Spartan6 FPGA emulation".
 *
 * Copyright (c) 2013 HEIG-VD / REDS
 * Written by Romain Bornet
 *
 * This code is licensed under the GPL.
 */

#ifndef STM32_P103_EMUL_H_
#define STM32_P103_EMUL_H_

#include "hw/arm/stm32.h"

typedef enum {
	// To the Scala side
	DIGITAL_OUT	= 0x00,	//!< -> New digital output value. Event sent to the Scala side.
	C_EVENT 	= 0x10,	//!< -> Event sent from the C code to the Scala side.

	// From the Scala side
	SCALA_DIGITAL_IN = 'V',	//!> <- Event received from the Scala side to set digital (boolean) input value.
	SCALA_ACK_EVT = 'A',	//!> <- Event received from the Scala side to ack an event.
} EventId;

// Value written by the monitor to ack an event
static const uint16_t MONITOR_ACK_READ = 0x1FF;

/**
 * Helper function used to post an event when a value has been written to a digital output.
 * Contains the output port, pin and the new value of the output.
 */
inline void post_event_digital_out(uint8_t port, uint8_t pin, uint32_t value);

/**
 * Sent an event to the Scala monitor server when the C program report an event.
 * Contains the event ID.
 */
inline void post_event_c(uint8_t eventId);


/**
 * Wait until the event has been confirmed by the monitor server.
 */
void event_wait_ack(void);


int stm32p103_emul_init(Stm32P103* state);

int stm32p103_emul_exit(void);


#endif /* STM32_P103_EMUL_H_ */
