#include <stdint.h>

#ifndef HELPER_H
#define HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the main red LED on the development board.
 */
void init_led(void);

/**
 * Toggle the state of the main red LED.
 */
void led_toggle(void);

/**
 * Configure and enable the RS232 serial port (USART2) using the following settings :
 *   155200 Baud
 *   8 bits + 1 stop bit
 *   No parity bit
 *   No hardware flow control
 */
void init_usart2(void);

/**
 * Send one byte through the USART2 port.
 * @see println
 */
void send_byte(uint8_t b);

/**
 * Send a String through the USART2 port.
 */
void println(const char* s);

/**
 * Wait some cycle (busy waiting).
 */
void busy_loop(uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif /* HELPER_H */
