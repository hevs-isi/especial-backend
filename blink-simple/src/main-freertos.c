#include "helper.h"

#include "FreeRTOS.h"
#include "task.h"

int i = 0;

#pragma GCC diagnostic ignored "-Wunused-parameter"
void led_blink_task(void *pvParameters) {
	while (1) {
		char str[80];
		send_string(str);
		sprintf(str, "Blink[%03d]\r\n", i++);
		//led_toggle();
		vTaskDelay(100); // 1 second delay
	}
}

int main(void) {
	led_init();

	init_rs232();
	enable_rs232_interrupts();
	enable_rs232();

	send_string("\nHello World !\n--\n");

	// 512 is stack size
	xTaskCreate(led_blink_task, (signed portCHAR*)"LED Flash", 512, NULL, tskIDLE_PRIORITY + 5, NULL);

	vTaskStartScheduler(); // Start running the task
	return 0;
}
