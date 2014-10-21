#include <stdint.h>

#include "xhw_types.h"
#include "xhw_memmap.h"
#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xgpio.h"


void busyLoop(unsigned long delay) {
	while (delay) {
		delay--;	// -O0 optimization
	}
}

#pragma GCC diagnostic ignored "-Wunused-parameter"

int main(int argc, char* argv[]) {

	// Not implemented in qemu...
	// xSysCtlClockSet(16000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_25MHZ);

	xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOC);
	xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOA);

	// Set GPIO port C pin 12 as output
	xGPIODirModeSet( xGPIO_PORTC_BASE, xGPIO_PIN_12, xGPIO_DIR_MODE_OUT);

	// Set GPIO port A pin 0 as input
	xGPIODirModeSet( xGPIO_PORTA_BASE, xGPIO_PIN_0, xGPIO_DIR_MODE_IN);

	while (1) {
		xGPIOPinWrite(xGPIO_PORTC_BASE, xGPIO_PIN_12, 1);

		// Wait on button
		while(!xGPIOPinRead(xGPIO_PORTA_BASE, xGPIO_PIN_0));

		while(1) {
			busyLoop(50000000);
			xGPIOPinWrite(xGPIO_PORTC_BASE, xGPIO_PIN_12, 0);
			busyLoop(50000000);
			xGPIOPinWrite(xGPIO_PORTC_BASE, xGPIO_PIN_12, 1);
		}
	}

}
