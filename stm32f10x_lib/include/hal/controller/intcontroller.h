/*
 * IntController.h
 */
#pragma once

#include "utils/singleton.h"
#include "interrupt.h"
#include "gpio.h"

#include <stdint.h>
#include <map>

/**
 * External interrupt controller.
 *
 * All GPIO which need external interrupts must be registered here.
 * This controller check all external interrupts lines and call the GPIO registered on this interrupt line.
 * Only one GPIO can be registered on each lines.
 *
 * @author	Christopher Métrailler (mei@hevs.ch)
 */
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
	 *
	 * This must be a public method, because it is called from extern C functions.
	 *
	 * @param[in] exti_Line the external line number where the interrupt come from
	 * @return `true` if success, `false` if not found (not registered)
	 */
	bool callIsr(uint32_t exti_Line);

private:
	// Store ISR callback for EXTI_Line0 to 15.
	// 16 external lines available for each pins (shared with all ports).
	// Only one GPIO can be registered to an EXT line.
	Interrupt* _isrVector[16];
};

// Use the Singleton template
typedef Singleton<IntController> IntCtrl;

