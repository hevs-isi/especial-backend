/*
 * QemuLogger.h
 */
#pragma once

#include "stm32f10x_usart.h"

/** Events ID (uint8_t) */
typedef enum EventId_t {
	SECTION_START = '0', /*!< Start of the main program */
	SECTION_END_INIT = 'A', /*!< Components initialization will be called */
	SECTION_LOOP_START = 'B', /*!< While loop going to start */
	SECTION_LOOP_TICK = 'C', /*!< While loop step */
	SECTION_LOOP_EXIT = 'F' /*!< While loop is terminated */
} EventId;

class QemuLogger {
public:

	static void send_event(EventId id) {
		USART_SendData(UART5, (uint8_t) id); // Write to the DR register of UART5

		// Wait until previous character got transfered
		while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
	}
};
