#include "digitaloutput.h"

extern "C" {
	#include "helper.h"
}

void busy_loop(uint32_t delay) {
	// FIXME: not the same behavior on QEMU and on the real target
	while (delay)
		delay--;
}

int main(void) {

	DigitalOutput led(12);
	led.initialize();

	init_rs232();

	send_string("Hello World !\nStart blinking...\n");

	while (1) {
		led.toggle();
		//busy_loop(4000000); // ~ 1 sec. on the real target
		busy_loop(50000000);
	}

	return 0;
}
