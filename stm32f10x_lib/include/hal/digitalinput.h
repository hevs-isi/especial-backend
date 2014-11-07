/*
 * DigitalInput.h
 */
#pragma once

#include "gpio.h"
#include "interrupt.h"
#include "controller/intcontroller.h"

class DigitalInput: public Gpio, public Interrupt {
public:

	DigitalInput(uint8_t port, uint8_t pin);
	virtual ~DigitalInput();

	virtual bool initialize();

	// Current cached state
	inline State state() const {
		return _state;
	}

	// Current cached state as boolean value
	inline bool get() const {
		if (state() == State(On))
			return true;
		else
			return false;
	}

	// Current cached state
	inline operator State() const {
		return state();
	}



	// Read the input GPIO value and update the cached state
	inline operator bool() {
		return read();
	}

	bool read();

	// Interrupt

	virtual inline void registerInterrupt() {
		IntCtrl::instance()->registerInt(this, _pin);
	}

	virtual inline void unregisterInterrupt() {
		IntCtrl::instance()->unregisterInt(_pin);
	}

	virtual void irq();
};
