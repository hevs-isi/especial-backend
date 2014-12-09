/*
 * IntController.cpp
 */
#include "controller/intcontroller.h"

IntController::IntController() {
}

IntController::~IntController() {
}

bool IntController::registerInt(Interrupt* isr, Gpio::Pin pin) {
	// Configure the GPIO for this interrupt
	// GPIO_PortSourceGPIOA is 0x00
	const uint8_t portSrc = pin.port - 'A';
	GPIO_EXTILineConfig(portSrc, pin.pinNumber);

	// The first 16 external interrupt lines are available for GPIO
	// Configure external interrupt
	EXTI_InitTypeDef extiInitStructure;
	extiInitStructure.EXTI_Line = pin.pin;
	extiInitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	extiInitStructure.EXTI_LineCmd = ENABLE;
	EXTI_ClearITPendingBit(pin.pin);
	EXTI_Init(&extiInitStructure);

	// Enable EXTI interrupt
	// EXTI0 to EXTI4 for the first 5 lines
	// EXTI9_5_IRQn   = External Line[9:5] Interrupts
	// EXTI15_10_IRQn = External Line[15:10] Interrupts
	uint8_t channel = 0;
	switch (pin.pinNumber) {
	case 0:
		channel = EXTI0_IRQn;
		break;
	case 1:
		channel = EXTI1_IRQn;
		break;
	case 2:
		channel = EXTI2_IRQn;
		break;
	case 3:
		channel = EXTI3_IRQn;
		break;
	case 4:
		channel = EXTI4_IRQn;
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		channel = EXTI9_5_IRQn;
		break;
	default:
		channel = EXTI15_10_IRQn;
		break;
	}

	NVIC_InitTypeDef nvicInitStructure;
	nvicInitStructure.NVIC_IRQChannel = channel;
	nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	nvicInitStructure.NVIC_IRQChannelSubPriority = 0x00;
	nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStructure);

	// Register the interrupt callback with an EXT line number
	if (_isrVector[pin.pinNumber] != NULL) {
		// Already registered for this pin number. The pin number MUST be different.
		// The EXT line is shared for all ports with the same pin number.
		return false;
	}

	_isrVector[pin.pinNumber] = isr;
	return true;
}

void IntController::unregisterInt(Gpio::Pin pin) {
	_isrVector[pin.pinNumber] = NULL;
}

bool IntController::callIsr(uint32_t exti_Line) {
	// Find the index from the EXT line number.
	// Example: EXTI_Line6 = 0x40, i is 6
	for (uint8_t i = 0; i < 16; i++) {
		uint32_t lNbr = (1 << i);
		if (lNbr == exti_Line) {
			Interrupt * isr = _isrVector[i];
			if (isr == NULL)
				return false; // Not registered ?
			isr->irq();
			return true;
		}
	}
	return false; // Wrong EXT line number
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

extern "C" void EXTI0_IRQHandler(void) {
	checkLineFlag(EXTI_Line0); // EXTI0_IRQn
}

extern "C" void EXTI1_IRQHandler(void) {
	checkLineFlag(EXTI_Line1); // EXTI1_IRQn
}

extern "C" void EXTI2_IRQHandler(void) {
	checkLineFlag(EXTI_Line2); // EXTI2_IRQn
}

extern "C" void EXTI3_IRQHandler(void) {
	checkLineFlag(EXTI_Line3); // EXTI3_IRQn
}

extern "C" void EXTI4_IRQHandler(void) {
	checkLineFlag(EXTI_Line4); // EXTI4_IRQn
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
