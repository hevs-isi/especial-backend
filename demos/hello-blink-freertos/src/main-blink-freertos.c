#include "helper.h"

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

int i = 0;
char str[20] = {0};

#pragma GCC diagnostic ignored "-Wunused-parameter"
void led_blink_task(void *pvParameters) {
	while (1) {
		memset(&str[0], 0, sizeof(str));
		sprintf(str, "Blink[%03d]\n", i++);
		send_string(str);
		led_toggle();
		vTaskDelay(100); // 1 second delay
	}
}

int main(void) {
	init_led();

	init_rs232();
	enable_rs232_interrupts();
	enable_rs232();

	send_string("Hello World !\nStart blinking using FreeRTOS...\n");

	// 512 is stack size
	xTaskCreate(led_blink_task, (signed portCHAR*)"LED Flash", 512, NULL, tskIDLE_PRIORITY + 5, NULL);

	vTaskStartScheduler(); // Start running the task
	return 0;
}
