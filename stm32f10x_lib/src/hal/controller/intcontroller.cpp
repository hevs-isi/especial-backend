/*
 * IntController.cpp
 */
#include "controller/intcontroller.h"

#include <assert.h>

map<uint32_t, Interrupt*> IntController::_isrVector;

IntController::IntController() {
}

IntController::~IntController() {
}

void IntController::registerInt(Interrupt* isr, uint8_t pinNumber) {
	// The pin number correspond to the EXTI_Line0 to 15 parameter
	_isrVector[pinNumber] = isr;
}

void IntController::unregisterInt(uint8_t pinNumber) {
	assert(pinNumber == 255); // TODO
}

bool IntController::callIsr(uint8_t pinNumber) {
	// Do not used exception here
	map<uint32_t, Interrupt*>::iterator it = _isrVector.find(pinNumber);
	if (it != _isrVector.end()) {
		it->second->irq();
		return true; // Found
	}
	return false; // Not found
}

/* C ISR routines */

#include "stm32f10x_exti.h"

extern "C" void EXTI9_5_IRQHandler(void) {
	// EXTI9_5_IRQn   = External Line[9:5] Interrupts
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		// Call the ISR
		bool valid = IntCtrl::instance()->callIsr(EXTI_Line6);
		assert(valid);
		// assert(valid == true);
		EXTI_ClearITPendingBit(EXTI_Line6);
	}

	// TODO: other lines
}

extern "C" void EXTI15_10_IRQHandler(void) {
	// EXTI15_10_IRQn = External Line[15:10] Interrupts

	// Not used
}
