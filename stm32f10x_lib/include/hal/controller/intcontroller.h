/*
 * IntController.h
 */
#pragma once

#include "utils/singleton.h"
#include "interrupt.h"
#include "gpio.h"

#include <stdint.h>
#include <map>

using namespace std;

class IntController {
public:
	IntController();
	virtual ~IntController();

	/**
	 * @brief Register an Interrupt interface and enable interrupt for the pin.
	 * @param[in] isr ISR calback
	 * @param[in] pin the gpio pin
	 */
	void registerInt(Interrupt* isr, Gpio::Pin pin);

	/**
	 * @brief Unregister an Interrupt.
	 * @param[in] pin the gpio pin
	 * @return `true` if success, `false` if not already registered
	 */
	bool unregisterInt(Gpio::Pin pin);

	/**
	 * @brief Call the ISR function of the GPIO.
	 * @param[in] pinNumber the pin number of the registered ISR
	 * @return `true` if success, `false` if not found (not registered)
	 */
	bool callIsr(uint8_t pinNumber);

public:
	//FIXME: optimize with an array ?
	// EXTI_Line0 to 15 corresponds to the pin number
	static map<uint32_t, Interrupt*> _isrVector;
};

// Use the Singleton template
typedef Singleton<IntController> IntCtrl;

