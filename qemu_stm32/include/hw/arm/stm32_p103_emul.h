/*
 * REPTAR Spartan6 FPGA emulation
 * Emulation "logic" part. Gateway between the emulation code and the backend.
 *
 * Copyright (c) 2013 HEIG-VD / REDS
 * Written by Romain Bornet
 *
 * This code is licensed under the GPL.
 */

// christopher.metrailler@hevs.ch - 2014/10/24
// STM32 P103 Olimex board emulation

#ifndef STM32_P103_EMUL_H_
#define STM32_P103_EMUL_H_

typedef enum {
	DIGITAL_OUT	= 0x00,	//!< Value written to a digital output
	C_EVENT 	= 0x10,	//!< Event sent from the C code
} EventId;


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



int stm32p103_emul_init(void);

int stm32p103_emul_exit(void);


#endif /* STM32_P103_EMUL_H_ */
