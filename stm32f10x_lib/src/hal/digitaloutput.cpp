/*
 * DigitalOutput.cpp
 */
#include "digitaloutput.h"

DigitalOutput::DigitalOutput(uint8_t port, uint8_t pin) :
		Gpio(port, pin) {
	_state = Invalid;
}

DigitalOutput::~DigitalOutput() {
}

bool DigitalOutput::initialize() {
	// Enable needed clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | gpio_port_rcc, ENABLE);

	// Configure the pin as push-pull output.
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = _pin.pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(gpio_port_base, &GPIO_InitStructure);

	set(false);	// Default output state is OFF
	return true;
}

void DigitalOutput::setState(State state) {
	_state = state;

	if (_state == State(Off))
		GPIO_ResetBits(gpio_port_base, _pin.pin); // clear
	else
		GPIO_SetBits(gpio_port_base, _pin.pin); // set
}

State DigitalOutput::state() const {
	return _state;
}
