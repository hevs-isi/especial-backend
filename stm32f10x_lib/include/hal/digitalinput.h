/*
 * DigitalInput.h
 */
#pragma once

#include <stdint.h>

class DigitalInput {
public:

	typedef enum e_State {
		Off = -1, On = 1
	} State;

	DigitalInput(uint8_t pin);
	virtual ~DigitalInput();

	bool initialize();
	State state();

private:
	uint8_t _pin;
	State _state;
};
