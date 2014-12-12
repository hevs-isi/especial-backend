#include "digitaloutput.h"
#include "digitalinput.h"
#include "analoginput.h"
#include "pwmoutput.h"

PwmOutput pwm4('B', 9);		// led4
PwmOutput pwm3('B', 8);		// led3
DigitalOutput led2('C', 4);	// led2
DigitalOutput led1('C', 3);	// led1

AnalogInput adc1('B', 0, 8);
DigitalInput btn2('C', 0);	// btn2

/**
 * Test application for the I/O extension board.
 * 	- led1 is ON when running
 * 	- led2 is controlled by btn2
 * 	- led3 and led4 are controlled by the potentiometer
 */
int main_io_ad_btn() {

	btn2.initialize();
	adc1.initialize();

	pwm3.initialize();
	pwm4.initialize();
	led2.initialize();
	led1.initialize();

	led1 = true;

	while (1) {
		bool val = btn2.get();
		led2.set(val);

		uint16_t period = adc1.read();
		pwm3.setPeriod(period);
		pwm4.setPeriod(4096 - period);
	}

	return 0;
}
