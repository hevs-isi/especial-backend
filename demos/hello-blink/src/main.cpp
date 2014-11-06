#include "digitalinput.h"
#include "digitaloutput.h"

// Button on the GPIOC port, pin 06 (external interrupt line 6)
DigitalInput bt1('C', 6);

// Led on the GPIOC port, pin 12
DigitalOutput led('C', 12);

int main() {

	led.initialize();
	bt1.initialize();

	while (1) {
		// Read the button value and updated the led.
		// The cached value of the button is updated in the interrupt function.
		led.set(bt1.get());
	}
}

//TODO: how to managed this in the DigitalIO class file ??

extern "C" void EXTI9_5_IRQHandler(void) {
	// EXTI9_5_IRQn   = External Line[9:5] Interrupts

	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		bt1.read();
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}

extern "C" void EXTI15_10_IRQHandler(void) {
	// EXTI15_10_IRQn = External Line[15:10] Interrupts

	// Not used
}

// END of file 'Sch1Code.cpp'
