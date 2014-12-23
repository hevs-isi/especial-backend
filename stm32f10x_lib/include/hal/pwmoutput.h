/*
 * PwmOutput.h
 */
#pragma once

#include "gpio.h"

#include <stdint.h>

/**
 * Configure an output as PWM mode.
 *
 * Available only for PB8 (TIM4_CH3) and PB9 (TIM4_CH4), respectively led3 and led4.
 * Configure only the timer4 for now. Cannot be used with any PWM output.
 * Configure the PWM output as push-pull pin with a frequency of 8.77kHz (maximum).
 */
class PwmOutput: public Gpio {
public:

	PwmOutput(uint8_t port, uint8_t pin);
	virtual ~PwmOutput();

	/**
	 * @brief Configure the pin as PWM output and set it to `OFF`.
	 * @return `true` when configured.
	 */
	bool initialize();

	/**
	 * @brief Set the `ON` period of the output.
	 * @param[in] period Period when the output is `ON`. Duty of 100% is 4096.
	 */
	void setPeriod(uint16_t period) const;

	inline const PwmOutput & operator=(uint16_t period) {
		setPeriod(period);
		return *this;
	}
};
