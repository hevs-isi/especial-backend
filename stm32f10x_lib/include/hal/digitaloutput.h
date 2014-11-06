/*
 * DigitalOutput.h
 */
#pragma once

#include <digitalio.h>
#include <stdint.h>


class DigitalOutput: public DigitalIO {
public:

	DigitalOutput(uint8_t port, uint8_t pin);
	virtual ~DigitalOutput();

	virtual bool initialize();

	State state() const;
	void setState(State state);

	inline void set(bool state) {
		if (state)
			setState(On);
		else
			setState(Off);
	}

	inline const DigitalOutput & operator=(State state) {
		setState(state);
		return *this;
	}

	inline operator State() const {
		return state();
	}

	void toggle() {
		setState(static_cast<State>(-state()));
	}
};
