/*
 * DigitalOutput.h
 */
#pragma once

#include <stdint.h>

class DigitalOutput {
public:

	DigitalOutput(uint8_t pin);
	virtual ~DigitalOutput();

	typedef enum e_State {
		Off = -1, On = 1, Invalid = 0
	} State;

	bool initialize();
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

private:
	uint8_t _pin;
	State _state;
};
