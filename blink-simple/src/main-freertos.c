#include "helper.h"

#include "FreeRTOS.h"
#include "task.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
void led_blink_task(void *pvParameters) {
	while (1) {
		led_toggle();
		vTaskDelay(100); // 1 second delay

	}
}

int main(void) {
	init_rs232();
	enable_rs232_interrupts();
	enable_rs232();

	send_string("\nHello World !\n--\n");

	led_init();

	// 512 is stack size
	xTaskCreate(led_blink_task, (signed portCHAR*)"LED Flash", 512, NULL, tskIDLE_PRIORITY + 5, NULL);

	vTaskStartScheduler(); // Start running the task
	return 0;
}
