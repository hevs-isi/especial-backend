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

using namespace std;

/**
 * @brief Control a digital input or output.
 */
class Gpio {
public:

	/**
	 * @brief The state of an input or output.
	 */
	typedef enum State_t {
		Off = -1, /*!< GPIO is OFF */
		On = 1, /*!< GPIO is ON */
		Invalid = 0 /*!< Invalid state (not initialized) */
	} State;

	/**
	 * @brief The GPIO port and pin used to initialize the GPIO.
	 */
	typedef struct Pin_t {
		uint8_t port; /*!< Port letter, from 'A' to 'G' (included) */
		uint8_t pinNumber; /*!< Pin number, from 0 to 15 */
		uint32_t pin; /*!< Shifted pin number (1 << number) */

		Pin_t(uint8_t port_, uint8_t pin_) :
				port(port_), pinNumber(pin_), pin(1 << pin_) {
		}
	} Pin;

public:

	// TODO: pass the Pin struct as argument
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

	/** Current value of the GPIO */
	State _state;

	// Conversions from Pin to registers
	const uint32_t gpio_port_rcc = ports_rcc.at(_pin.port);
	GPIO_TypeDef* gpio_port_base = ports_base.at(_pin.port);

public:
	//FIXME: optimize with an array ?
	static const map<uint8_t, uint32_t> ports_rcc;
	static const map<uint8_t, GPIO_TypeDef*> ports_base;
};
