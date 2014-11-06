/*
 * digitalio.h
 */
#ifndef INCLUDE_HAL_DIGITALIO_H_
#define INCLUDE_HAL_DIGITALIO_H_

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
class DigitalIO {
public:

	/**
	 * @brief The state of an input or output.
	 */
	typedef enum State_t {
		Off = -1, On = 1, Invalid = 0
	} State;

	/**
	 * @brief The GPIO port and pin to initialize a GPIO.
	 */
	typedef struct Pin_t {
		uint8_t port; // from 'A' to 'G' (included)
		uint32_t pin; // 1 << pin

		Pin_t(uint8_t port_, uint8_t pin_) :
				port(port_), pin(1 << pin_) {
		}
	} Pin;

public:

	// TODO: pass the Pin struct as argument
	DigitalIO(uint8_t port, uint8_t pin);
	virtual ~DigitalIO();

	/**
	 * @brief Initialization of the GPIO.
	 * Configure the pin as input or output, enable clock, etc.
	 */
	virtual bool initialize() = 0;

protected:
	Pin _pin;
	State _state;

	// Conversions from Pin to Cortex registers
	const uint32_t gpio_port_rcc = _ports_rcc.at(_pin.port);
	GPIO_TypeDef* gpio_port_base = _ports_base.at(_pin.port);

private:
	static const map<uint8_t, uint32_t> _ports_rcc;
	static const map<uint8_t, GPIO_TypeDef*> _ports_base;
};

#endif /* INCLUDE_HAL_DIGITALIO_H_ */
