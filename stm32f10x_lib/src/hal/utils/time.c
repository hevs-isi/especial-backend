/**
 * \file	heivs/time.c
 * \author	marc dot pignat at hevs dot ch
 *
 * Uses timer 2 (STM32F10X_MD).
 */

#include "utils/time.h"
#include "utils/delay.h"

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

timeout_t time_set_timeout_ms(uint32_t ms) {
	timeout_t time = time_get();
	time.value += ms;

	/* The timeout should be at least ms */
	time.value += 1;
	return time;
}

volatile uint32_t gTimeCounter = 0;

void TIM2_IRQHandler(void) {
	// 1kHz tick (each 1ms)
	if (TIM2->SR & TIM_SR_UIF) {
		gTimeCounter++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void time_suspend() {
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
}

void time_resume() {
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void time_init(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	gTimeCounter = 0;

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* From clock to 1 MHz */
	// hclk = SystemCoreClock (SystemClock.hclk)
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) / 2 - 1; // 72MHz -> 35

	/* From 1 MHz to 1 KHz*/
	TIM_TimeBaseStructure.TIM_Period = 2000 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	time_resume();

	delay_wait_ms(500);
}

uint32_t time_diff_ms(timeout_t now, timeout_t before) {
	return now.value - before.value;
}
