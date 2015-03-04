/*
 * PulseInput.cpp
 */
#include "pulseinput.h"

PulseInput::PulseInput(uint8_t port, uint8_t pin) :
		DigitalInput(port, pin) {
	pulseCounter = 0;
	lastPulseTime = 1000; // Fan is "OFF" at start
}

PulseInput::~PulseInput() {
}

bool PulseInput::initialize() {
	bool res = DigitalInput::initialize();
	time_init();
	time_resume();

	lastPulseTimestamp = time_get();

	return res;
}

void PulseInput::irq() {
	if (pulseCounter == 0) {
		startTime = time_get();
	}

	// Measure the elapsed time after 4 pulses (8 edges)
	else if (pulseCounter == 4) {
		timeout_t now = time_get();
		lastPulseTime = time_diff_ms(now, startTime);
		lastPulseTimestamp = now;

		// Reset the counter and count the next 8 edges
		startTime = time_get();
		pulseCounter = 0;
	}

	// Software glitch filter. Fix a minimum of ms between two edges.
	timeout_t n = time_get();
	uint32_t pulseWidth = time_diff_ms(n, currentTime);
	if (pulseWidth >= 12) {
		// Count pulse if not a glitch (> 12ms between edges)
		currentTime = time_get();
		pulseCounter += 1;
	}
}
