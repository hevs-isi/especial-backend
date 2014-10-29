/*
 * DigitalOutput.cpp
 */
#include "digitaloutput.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#include <assert.h>

DigitalOutput::DigitalOutput(uint8_t pin) {
	_pin = pin;
	_state = Invalid;
}

DigitalOutput::~DigitalOutput() {
	// Not used
}

bool DigitalOutput::initialize() {
	// FIXME: remove this
	assert(_pin == 12);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO C clock. */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* Set the LED pin state such that the LED is off.  The LED is connected
	 * between power and the microcontroller pin, which makes it turn on when
	 * the pin is low.
	 */
	GPIO_WriteBit(GPIOC, 1 << _pin , Bit_SET);

	/* Configure the LED pin as push-pull output. */
	GPIO_InitStructure.GPIO_Pin = 1 << _pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	return true;
}

void DigitalOutput::setState(State state) {
	_state = state;

	if (_state == State(On))
		GPIO_ResetBits(GPIOC, 1 << _pin); // clear
	else
		GPIO_SetBits(GPIOC, 1 << _pin); // set
}

DigitalOutput::State DigitalOutput::state() const {
	return _state;
}
