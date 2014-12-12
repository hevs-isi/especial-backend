/*
 * DigitalInput.cpp
 */
#include "digitalinput.h"

DigitalInput::DigitalInput(uint8_t port, uint8_t pin) :
		Gpio(port, pin) {
	_state = Invalid;
}

DigitalInput::~DigitalInput() {
}

bool DigitalInput::initialize() {
	// Enable needed clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | gpio_port_rcc, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = _pin.pin;
	GPIO_Init(gpio_port_base, &GPIO_InitStructure);

	// Register as interrupt and configure the pin
	return registerInterrupt();
}

bool DigitalInput::read() {
	if ((GPIO_ReadInputDataBit(gpio_port_base, _pin.pin)) == Bit_SET)
		_state = On;
	else
		_state = Off;

	return (_state == State(On)); // Boolean value
}

void DigitalInput::irq() {
	read();	// Read the button value and save the new state
}
