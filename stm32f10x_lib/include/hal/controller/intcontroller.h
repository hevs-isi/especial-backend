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
	 * @brief Register an External Interrupt (EXTI) for this pin. Select one available external line (up to 16).
	 * @param[in] isr the ISR callback for the input
	 * @param[in] pin the gpio pin
	 * @return `true` if success, `false` if the EXT line is already used by another port
	 */
	bool registerInt(Interrupt* isr, Gpio::Pin pin);

	/**
	 * @brief Unregister an Interrupt.
	 * @param[in] pin the gpio pin
	 */
	void unregisterInt(Gpio::Pin pin);

	/**
	 * @brief Call the ISR function of the corresponding input.
	 * @param[in] exti_Line the external line number where the interrupt come from
	 * @return `true` if success, `false` if not found (not registered)
	 */
	bool callIsr(uint32_t exti_Line);

private:
	// Store ISR callback for EXTI_Line0 to 15.
	// 16 external lines available for each pins (shared with all ports).
	Interrupt* _isrVector[16];
};

// Use the Singleton template
typedef Singleton<IntController> IntCtrl;

