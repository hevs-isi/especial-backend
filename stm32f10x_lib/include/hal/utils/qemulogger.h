/*
 * QemuLogger.h
 */
#pragma once

#include "stm32f10x_usart.h"

/** Events ID (uint8_t) */
typedef enum EventId_t {
	SECTION_START = '0', /*!< Start of the main program. */
	SECTION_INIT_END = 'A', /*!< Components initialization will be called. */
	SECTION_LOOP_START = 'B', /*!< While loop going to start. */
	SECTION_LOOP_TICK = 'C', /*!< While loop step. */
	SECTION_END = 'F' /*!< Last program line. Terminated. */
} EventId;

class QemuLogger {
private:
	static const uint16_t MONITOR_ACK_READ = 0x1FF;
public:

	/**
	 * @brief Send an event to the code monitor
	 *
	 * An event is a `byte` identifier used by the monitor Server to check the code execution.
	 * The event is written in the `UART5 DR` register, read by QEMU and forwarded by TCP to the monitor server.
	 *
	 * @param[in] id the predefined event id
	 * @param[in] ack wait for the monitor confirmation or not (default is `false`)
	 */
	static void send_event(EventId id, bool ack = false) {
		USART_SendData(UART5, (uint8_t) id); // Write to the DR register of UART5

		if (ack) {
			// Wait until previous character got transfered
			while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);

			// Wait until the value has been validated by the monitor.
			// UART5 DR read register is blocking
			while (USART_ReceiveData(UART5) != MONITOR_ACK_READ);
		}
	}
};
