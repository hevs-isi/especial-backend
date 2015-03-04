/*
 * PulseInput.h
 */
#pragma once

#include "digitalinput.h"
#include "utils/time.h"

/**
 * Configure the input as a digital input.
 * Registered as an external interrupt to count the number of edges.
 */
class PulseInput: public DigitalInput {
public:

	PulseInput(uint8_t port, uint8_t pin);
	virtual ~PulseInput();

	/**
	 * Time and input initialization.
	 *  @return `true` if success, `false` if the interrupt registration fails
	 */
	virtual bool initialize();

	/** Hardware interrupt for each rising and falling edge. */
	virtual void irq();

	/**
	 * Return the last valid time measured.
	 * Correspond to the time of 4 pulse captured from the fan.
	 *
	 * @return the time measured between 8 edges (4 pulses)
	 */
	inline uint32_t getSpeed() {
		return lastPulseTime;
	}

	inline uint32_t get() const {
		return lastPulseTime;
	}

	// Start time of the counter.
	timeout_t startTime;

	// Current time value when an edge is detected.
	timeout_t currentTime;

	// Number of valid pulse detected (pulse signal periods)
	uint32_t pulseCounter;

	// Time measure of the last valid pulse.
	uint32_t lastPulseTime;

	timeout_t lastPulseTimestamp;
};
