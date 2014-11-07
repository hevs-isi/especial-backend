/*
 * IntController.h
 */
#pragma once

#include "utils/singleton.h"
#include "api/interrupt.h"

#include <stdint.h>
#include <map>

using namespace std;

class IntController {
public:
	IntController();
	virtual ~IntController();

	/**
	 * Register an Interrupt interface.
	 */
	void registerInt(Interrupt* isr, uint8_t pinNumber);

	void unregisterInt(uint8_t pinNumber);

	bool callIsr(uint8_t pinNumber);

private:
	static const uint8_t MAX_INT = 8;

	//FIXME: optimize with an array ?
public:
	// EXTI_Line0 to 15 corresponds to the pin number
	static map<uint32_t, Interrupt*> _isrVector;
};

typedef Singleton<IntController> IntCtrl;

