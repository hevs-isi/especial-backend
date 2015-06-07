/************************************************************************//**
 * \file	heivs/delay.h
 * \brief	simple active delays
 * \author	marc dot pignat at hevs dot ch
 * \author  christopher dot metrailler at hevs dot ch
 *
 * Delays with busy loops (active waiting, no timer).
 *
 * Code adapted from the "libheivs_stm32" library.
 * See <a href="http://armebs4.hevs.ch">Armebs4</a>.
 *
 * ChangeLog:
 * - code updated for the STM32F103-STK board.
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
