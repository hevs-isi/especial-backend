#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED_PERIPH	RCC_APB2Periph_GPIOC
#define LED_PORT	GPIOC
#define LED_PIN		GPIO_Pin_12

void led_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIO C clock
	RCC_APB2PeriphClockCmd(LED_PERIPH, ENABLE);

	/* Set the LED pin state such that the LED is off.  The LED is connected
	 * between power and the microcontroller pin, which makes it turn on when
	 * the pin is low.
	 */
	GPIO_WriteBit(LED_PORT, LED_PIN, Bit_SET);

	// Configure the LED pin as push-pull output
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void led_toggle() {
	uint8_t led_bit = GPIO_ReadOutputDataBit(LED_PORT, LED_PIN);

	if (led_bit == (uint8_t) Bit_SET)
		GPIO_ResetBits(LED_PORT, LED_PIN); // clear
	else
		GPIO_SetBits(LED_PORT, LED_PIN); // set
}

void led_blink_task(void *pvParameters) {
	while (1) {
		led_toggle();
		vTaskDelay(100); // 1 second delay
	}
}

int main(void) {
	led_init();
	led_toggle();

	// 512 is stack size
	xTaskCreate(led_blink_task, (signed portCHAR*)"LED Flash", 512, NULL, tskIDLE_PRIORITY + 5, NULL);

	vTaskStartScheduler(); // Start running the task
	return 0;
}
