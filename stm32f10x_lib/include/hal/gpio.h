/*
 * Gpio.h
 */
#pragma once

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#include <stdint.h>
#include <assert.h>
#include <map>

/**
 * Define a GPIO with a port and a pin number.
 *
 * Base class used for all I/O to access to a port and a pin.
 *
 * @author	Christopher Métrailler (mei@hevs.ch)
 */
class Gpio {

public:
	/**
	 * @brief The GPIO port and pin used to initialize the GPIO.
	 */
	typedef struct Pin_t {
		uint8_t port; /*!< Port letter, from 'A' to 'G' (included) */
		uint8_t pinNumber; /*!< Pin number, from 0 to 15 */
		uint32_t pin; /*!< Shifted pin number (1 << number) */

		Pin_t(uint8_t port_, uint8_t pin_) :
				port(port_), pinNumber(pin_), pin(1 << pin_) {
			assert(port >= 'A' && port <= 'G');
		}
	} Pin;

	Gpio(uint8_t port, uint8_t pin);

	virtual ~Gpio();

	/**
	 * @brief Initialization of the GPIO.
	 * Configure the pin as input or output, enable clock, etc.
	 */
	virtual bool initialize() = 0;

protected:
	/** Pin of the GPIO */
	Pin _pin;

	// Conversions from Pin to registers
	const uint32_t gpio_port_rcc = ports_rcc.at(_pin.port);
	GPIO_TypeDef* gpio_port_base = ports_base.at(_pin.port);

public:
	//FIXME: Can be optimized with an array instead of the map
	static const std::map<uint8_t, uint32_t> ports_rcc;
	static const std::map<uint8_t, GPIO_TypeDef*> ports_base;
};
