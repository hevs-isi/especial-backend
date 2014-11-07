/*
 * Interrupt.h
 */
#pragma once

/**
 * @brief Interface used by inputs that use interrupt (ISR).
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
	virtual void registerInterrupt() = 0;

	/**
	 * @brief Helper function used to unregister a GPIO as an interrupt.
	 */
	virtual void unregisterInterrupt() = 0;
};
