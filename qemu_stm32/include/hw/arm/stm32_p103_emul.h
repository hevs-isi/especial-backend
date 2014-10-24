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
	DIGITAL_OUT = 0, DIGITAL_IN = 1
} P103PerId;

/**
 * Post an event when the value of a digital output has changed.
 */
inline void post_event_digital_out(uint32_t id, uint32_t data);




int stm32p103_emul_init(void);

void* stm32p103_emul_event_post(P103PerId periph, uint32_t id, uint32_t data);

int stm32p103_emul_exit(void);

#endif /* STM32_P103_EMUL_H_ */
