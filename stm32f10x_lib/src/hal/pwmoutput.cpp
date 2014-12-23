/*
 * PwmOutput.cpp
 */
#include "pwmoutput.h"

#include "stm32f10x_tim.h"

PwmOutput::PwmOutput(uint8_t port, uint8_t pin) :
		Gpio(port, pin) {
	// Only available for PB8 and PB9 (LEDs 3 and 4)
	assert(port == 'B');
	assert(pin == 8 || pin == 9);
}

PwmOutput::~PwmOutput() {
}

bool PwmOutput::initialize() {
	// Enable needed clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | gpio_port_rcc, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Configure the pin as push-pull PWM output. Timer is alternative function. */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = _pin.pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(gpio_port_base, &GPIO_InitStructure);

	/* Configure timer 4. */
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 0xFFF; // 4096 - 1;

	// pre = 71 => 244 Hz
	// pre = 02 => 5,87 kHz
	// pre = 01 => 8.77 kHz
	TIM_TimeBase_InitStructure.TIM_Prescaler = 1;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBase_InitStructure);

	/* Configure PWM channel output. */
	TIM_OCInitTypeDef TIM_OC_InitStructure;
	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OC_InitStructure.TIM_Pulse = 0x00; // Default is OFF

	// Enable the PWM channel of the timer 4
	if (_pin.pinNumber == 8)
		TIM_OC3Init(TIM4, &TIM_OC_InitStructure); // Timer4, channel 3
	else if (_pin.pinNumber == 9)
		TIM_OC4Init(TIM4, &TIM_OC_InitStructure); // Timer4, channel 4
	else
		assert(false);

	TIM_Cmd(TIM4, ENABLE);
	return true;
}

void PwmOutput::setPeriod(uint16_t period) const {

	if (period > 0xFFF)
		period = 0xFFF;

	// Change the period for each PWM channels
	if (_pin.pinNumber == 8)
		TIM4->CCR3 = period; // Timer4, channel 3
	else if (_pin.pinNumber == 9)
		TIM4->CCR4 = period; // Timer4, channel 4
	else
		assert(false);
}

