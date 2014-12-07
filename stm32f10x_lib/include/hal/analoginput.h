/*
 * AnalogInput.h
 */
#pragma once

#include "gpio.h"
#include <stdint.h>

class AnalogInput {
public:

	AnalogInput(uint8_t port, uint8_t pin, uint8_t channel);
	virtual ~AnalogInput();

	bool initialize();

	// Current cached value
	inline bool get() const {
		return _lastValue;
	}

	// Start a conversion to read the current analog value from the ADC
	uint16_t read();

protected:

	/** Pin of the GPIO */
	Gpio::Pin _pin;

	uint8_t _channel;
	uint16_t _lastValue;

	// Conversions from Pin to registers
	const uint32_t gpio_port_rcc = Gpio::ports_rcc.at(_pin.port);
	GPIO_TypeDef* gpio_port_base = Gpio::ports_base.at(_pin.port);
};
