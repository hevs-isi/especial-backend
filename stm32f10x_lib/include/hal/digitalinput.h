/*
 * DigitalInput.h
 */
#pragma once

#include "gpio.h"
#include "state.h"
#include "interrupt.h"
#include "controller/intcontroller.h"

/**
 * Configure the pin as a digital input.
 *
 * Works with all pin and ports. Use interruptions to read the pin value when it change.
 * Use `get` to poll the input or `get` to read the current cached value.
 * If interrupts are used, the cached value is updated automatically.
 *
 * @author	Christopher Métrailler (mei@hevs.ch)
 */
class DigitalInput: public Gpio, public Interrupt {
public:

	DigitalInput(uint8_t port, uint8_t pin);
	virtual ~DigitalInput();

	/**
	 * Configure the pin as input with interrupts.
	 * @return `true` if success, `false` if the interrupt registration fails
	 */
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

	// Interrupt related

	// Called automatically when initialized
	virtual inline bool registerInterrupt() {
		return IntCtrl::instance()->registerInt(this, _pin);
	}

	virtual inline void unregisterInterrupt() {
		IntCtrl::instance()->unregisterInt(_pin);
	}

	virtual void irq();

protected:

	/** Current value of the GPIO */
	State _state;
};
