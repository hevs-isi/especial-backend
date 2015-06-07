/**
 * \file	heivs/delay.c
 * \author	marc dot pignat at hevs dot ch
 * \author  christopher dot metrailler at hevs dot ch
 *
 * Original file modified, see "delay.h".
 */
 
#include "utils/delay.h"

#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "cmsis/core_cm3.h"

void delay_wait_ms(uint32_t ms)
{
	uint32_t i;
	for (i = 0 ; i < ms ; i++)
	{
		delay_wait_us(1000);
	}
}

void _delay_loop(uint32_t n);
/* while (--i > 0){} in assembler */
asm
(
	"_delay_loop:"
	"1:						\n"
	"	subs.n	r0, #1		\n" /* This is one clock */
	"	bne.n	1b			\n" /* 1+P here is 2 clocks (destination is know and a 16-bit instruction) */
	"	bx		lr			\n"
);

void delay_wait_us(uint32_t us)
{
	uint32_t hz;
	uint32_t mhz;
	uint32_t i;

	if (us > 1000)
	{
		/**
		 * What a looooong wait, call the milisecond version after rounding up
		 */
		delay_wait_ms((us+999)/1000);
		return;
	}

	/**
	 * Split clock between Hz and MHz
	 */
	mhz = SystemCoreClock / (1000*1000); // SystemClock.hclk = SystemCoreClock
	hz = SystemCoreClock % (1000*1000);

	/* Multiply by us, can't overflow because us <= 1000 */
	hz *= us;
	hz /= (1000*1000);

	/* Convert to the number of iteration */
	i = mhz * us;
	i += (hz * us) / (1000 * 1000);

	/* Divide by the number of clocks in a loop, see: the loop ;) */
	i /= 7; // 3 for CortexM4 (ARMEBS4)

	/**
	 * The wait loop is in the form (--i > 0){}, so if i is zero, this loop will
	 * be far too long.
	 * There is only two way of falling here, clock or us are zero.
	 */
	if (i == 0)
	{
		return;
	}

	_delay_loop(i);
}


