#include "digitalinput.h"
#include "digitaloutput.h"
#include "analoginput.h"

// Main board
DigitalOutput led0('C', 12);
DigitalInput btn0('C', 6);
AnalogInput adc0('C', 5, 15);

// Extension board
//DigitalInput btn1('B', 1);
DigitalInput btn2('C', 0);
DigitalInput btn3('C', 1);
DigitalInput btn4('C', 2);

AnalogInput adc1('B', 0, 8);

DigitalOutput led1('C', 3);
DigitalOutput led2('C', 4);
DigitalOutput led3('B', 8);
DigitalOutput led4('B', 9);

/**
 * Demo application used to test the extension board.
 *
 * Each button is used to control the corresponding led.
 * See the I/O extension board documentation for more details.
 */
int main() {

	// -- Main board --
	led0.initialize();	// Active low
	btn0.initialize();
	adc0.initialize();

	// -- Extension board --
	adc1.initialize();

	btn2.initialize();
	btn3.initialize();
	btn4.initialize();

	led1.initialize();	// Active high
	led2.initialize();
	led3.initialize();
	led4.initialize();

	while (1) {
		led0.set(btn0.get());

		uint16_t value = adc1.read();
		led1.set((value > 2048) ? true : false);

		led2.set(btn2.get());
		led3.set(btn3.get());
		led4.set(btn4.get());

		// Joystick directions values (from A/D)
		// 0000 .. 0000  MIDDLE
		// 0936 .. 0937	 UP
		// 0464 .. 0465	 RIGHT
		// 0170 .. 0171	 DOWN
		// 1966 .. 1668  LEFT
	}
}
// END of file 'Sch1aCode.cpp'
