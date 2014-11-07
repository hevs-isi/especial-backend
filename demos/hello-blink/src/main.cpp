#include "digitalinput.h"
#include "digitaloutput.h"

// Button on the GPIOC port, pin 6 (external interrupt line 6)
DigitalInput bt1('C', 6);

// Led on the GPIOC port, pin 12
DigitalOutput led('C', 12);

int main() {

	led.initialize();
	bt1.initialize();
	// bt1.registerInterrupt(); // Done automatically
	// bt1.unregisterInterrupt();

	while (1) {
		// Read the button value and updated the led.
		// The cached value of the button is updated in the interrupt routine (ISR) -> black magic !

		//led.set(bt1.get());

		if(bt1)	// Boolean operator available
			led.set(true);
		else
			led.set(false);
	}
}

// END of file 'Sch1Code.cpp'
