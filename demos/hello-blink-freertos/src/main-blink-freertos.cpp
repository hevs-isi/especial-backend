#include <stdio.h>
#include <string.h>

#include "digitaloutput.h"
#include "helper.h"

#include "FreeRTOS.h"
#include "task.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
void led_blink_task(void *pvParameters) {

	// Led on the GPIOC port, pin 12
	DigitalOutput led('C', 12);
	led.initialize();
	// led = 0; // Done automatically

	while (1) {
		// Toggle the led state each second
		led.toggle();
		vTaskDelay(100);
	}
}

int main(void) {
	init_rs232();
	println("Hello World !\nStart blinking using FreeRTOS...\n");

	// 512 is stack size
	xTaskCreate(led_blink_task, (signed portCHAR*)"LED Flash", 512, NULL, tskIDLE_PRIORITY + 5, NULL);

	vTaskStartScheduler(); // Start running the task
	return 0;
}
