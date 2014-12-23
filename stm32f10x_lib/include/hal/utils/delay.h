/************************************************************************//**
 * \file	heivs/delay.h
 * \brief	simple active delays
 * \author	marc dot pignat at hevs dot ch
 *
 * Code updated for the STM32F103-STK board.
 * Delays with busy loops (active waiting, no timer).
 ***************************************************************************/
#pragma once

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdint.h>

/************************************************************************//**
 * \brief Wait for at least that time
 *
 * \param ms The millisecond time to wait
 ***************************************************************************/
void delay_wait_ms(uint32_t ms);

/************************************************************************//**
 * \brief Wait for at least that time
 *
 * \param us The microsecond time to wait
 ***************************************************************************/
void delay_wait_us(uint32_t us);

#ifdef __cplusplus
	}
#endif
