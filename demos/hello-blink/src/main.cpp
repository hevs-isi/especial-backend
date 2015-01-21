#include "digitalinput.h"
#include "digitaloutput.h"

DigitalInput btn2('C', 1);
DigitalOutput led('C', 12);

void init() {
	btn2.initialize();
	btn2.registerInterrupt();
	led.initialize();
}

/**
 * Read the button value and update the LED.
 * The button value is updated using an external interrupt line.
 */
int main() {
	init();

	while (1) {
		bool r = btn2.get();
		led.set(r);
	}
}
