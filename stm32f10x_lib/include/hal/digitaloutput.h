/*
 * DigitalOutput.h
 */
#pragma once

#include "gpio.h"

#include <stdint.h>

class DigitalOutput: public Gpio {
public:

	DigitalOutput(uint8_t port, uint8_t pin);
	virtual ~DigitalOutput();

	/**
	 * @brief Configure the pin as output and set it to `OFF`.
	 */
	virtual bool initialize();

	State state() const;
	void setState(State state);

	inline void set(bool state) {
		if (state)
			setState(Off);
		else
			setState(On);
	}

	inline const DigitalOutput & operator=(State state) {
		setState(state);
		return *this;
	}

	inline const DigitalOutput & operator=(bool value) {
		set(value);
		return *this;
	}

	inline operator State() const {
		return state();
	}

	void toggle() {
		setState(static_cast<State>(-state()));
	}
};
