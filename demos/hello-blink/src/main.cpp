#include "digitalinput.h"
#include "digitaloutput.h"

DigitalInput btn2('C', 06);
DigitalOutput led('C', 12);

void init() {
	btn2.initialize();
	btn2.registerInterrupt();
	led.initialize();
}

/**
 * Read the main button value and update the main red LED (no extension board).
 * The button value is updated using an external interrupt line.
 */
int main() {
	init();

	while (1) {
		bool r = btn2.get();
		led.set(!r); // Led0 is active low
	}
}
