/*
 * AnalogInput.h
 */
#pragma once

#include "gpio.h"
#include <stdint.h>

/**
 * Configure a pin as analog input.
 *
 * Use the `ADC1` for the analog to digital conversion.
 */
class AnalogInput: public Gpio {
public:

	AnalogInput(uint8_t port, uint8_t pin, uint8_t channel);
	virtual ~AnalogInput();

	/**
	 * @brief Configure the pin as analog input.
	 * @return `true` when configured
	 */
	virtual bool initialize();

	/**
	 * @brief Current cached value
	 */
	inline bool get() const {
		return _lastValue;
	}

	/**
	 * @brief Start a conversion to read the current analog value from the ADC. Save the last value as cached value.
	 */
	uint16_t read();

protected:
	/* Analog input channel corresponding to the pin number. */
	uint8_t _channel;

	/* Last read value. */
	uint16_t _lastValue;
};
