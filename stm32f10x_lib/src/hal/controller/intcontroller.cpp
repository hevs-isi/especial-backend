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

void IntController::registerInt(Interrupt* isr, Gpio::Pin pin) {
	// Configure the GPIO for this interrupt

	// GPIO_PortSourceGPIOA is 0x00
	const uint8_t portSrc = pin.port - 'A';
	GPIO_EXTILineConfig(portSrc, pin.pinNumber);

	// Configure external interrupt for PC6
	EXTI_InitTypeDef extiInitStructure;
	extiInitStructure.EXTI_Line = pin.pin;
	extiInitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	extiInitStructure.EXTI_LineCmd = ENABLE;
	EXTI_ClearITPendingBit(pin.pin);
	EXTI_Init(&extiInitStructure);

	// Enable EXTI interrupt
	// EXTI9_5_IRQn   = External Line[9:5] Interrupts
	// EXTI15_10_IRQn = External Line[15:10] Interrupts
	const uint8_t channel =
			(pin.pinNumber < 10) ? EXTI9_5_IRQn : EXTI15_10_IRQn;

	NVIC_InitTypeDef nvicInitStructure;
	nvicInitStructure.NVIC_IRQChannel = channel;
	nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	nvicInitStructure.NVIC_IRQChannelSubPriority = 0x00;
	nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStructure);

	// The pin number correspond to the EXTI_Line0 to 15 parameter
	_isrVector[pin.pin] = isr;
}

bool IntController::unregisterInt(Gpio::Pin pin) {
	map<uint32_t, Interrupt*>::iterator it = _isrVector.find(pin.pin);
	if (it != _isrVector.end()) {
		_isrVector.erase(it);
		return true; // Correctly removed
	}
	return false; // Error: not found
}

bool IntController::callIsr(uint8_t pinNumber) {
	// Do not used exception here
	map<uint32_t, Interrupt*>::iterator it = _isrVector.find(pinNumber);
	if (it != _isrVector.end()) {
		it->second->irq();
		return true; // IRQ function called
	}
	return false; // Error: not found
}

/* C ISR routines */

static IntController* ctrl = IntCtrl::instance();

static void checkLineFlag(uint32_t exti_Line) {
	if (EXTI_GetITStatus(exti_Line) != RESET) {
		// Call the ISR
		bool valid = ctrl->callIsr(exti_Line);
		assert(valid);
		EXTI_ClearITPendingBit(exti_Line);
	}
}

extern "C" void EXTI9_5_IRQHandler(void) {
	// EXTI9_5_IRQn   = External Line[9:5] Interrupts
	checkLineFlag(EXTI_Line5);
	checkLineFlag(EXTI_Line6);
	checkLineFlag(EXTI_Line7);
	checkLineFlag(EXTI_Line8);
	checkLineFlag(EXTI_Line9);
}

extern "C" void EXTI15_10_IRQHandler(void) {
	// EXTI15_10_IRQn = External Line[15:10] Interrupts
	checkLineFlag(EXTI_Line10);
	checkLineFlag(EXTI_Line11);
	checkLineFlag(EXTI_Line12);
	checkLineFlag(EXTI_Line13);
	checkLineFlag(EXTI_Line14);
	checkLineFlag(EXTI_Line15);
}
