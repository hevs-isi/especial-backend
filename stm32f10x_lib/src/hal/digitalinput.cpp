/*
 * DigitalInput.cpp
 */
#include "digitalinput.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

DigitalInput::DigitalInput(uint8_t pin) {
	_pin = pin;
	state();
}

DigitalInput::~DigitalInput() {
	// Not used
}

bool DigitalInput::initialize() {
	return false;
}

DigitalInput::State DigitalInput::state() {
	_state = Off;
	return _state;
}
