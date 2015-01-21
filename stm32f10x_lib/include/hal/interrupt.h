/*
 * Interrupt.h
 */
#pragma once

/**
 * Interface used by GPIO that needs interrupt (ISR).
 *
 * @author	Christopher Métrailler (mei@hevs.ch)
 */
class Interrupt {
public:
	virtual ~Interrupt() {
	}

	/**
	 * @brief Called when an Interrupt ReQuest is available for this GPIO.
	 * Must be registered in the `IntController`.
	 */
	virtual void irq() = 0;

	/**
	 * @brief Helper function used to register a GPIO as an interrupt.
	 */
	virtual bool registerInterrupt() = 0;

	/**
	 * @brief Helper function used to unregister a GPIO as an interrupt.
	 */
	virtual void unregisterInterrupt() = 0;
};
