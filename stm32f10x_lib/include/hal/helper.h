#include <stdint.h>

#ifndef HELPER_H
#define HELPER_H

#ifdef __cplusplus
extern "C" {
#endif


#define LED_PERIPH	RCC_APB2Periph_GPIOC
#define LED_PORT	GPIOC
#define LED_PIN		GPIO_Pin_12

/* Initialize the LED (the board only has one). */
void init_led(void);

void led_toggle(void);

/* Configures the RS232 serial port using the following settings:
 *   155200 Baud
 *   8 bits + 1 stop bit
 *   No parity bit
 *   No hardware flow control
 */
void init_rs232(void);

void enable_rs232_interrupts(void);

void enable_rs232(void);

void send_byte(uint8_t b);

void send_string(const char* s);

void busy_loop(uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif /* HELPER_H */
