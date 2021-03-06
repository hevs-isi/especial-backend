#include "helper.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "misc.h"

void init_led(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIO C clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	// Set the LED off (connected as negative logic)
	GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_SET);

	/* Configure the LED pin as push-pull output. */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void enable_rs232(void) {
	// Enable the RS232 port (USART2)
	USART_Cmd(USART2, ENABLE);
}

void init_usart2(void) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	// Enable peripheral clocks.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// Configure USART2 Rx pin as floating input.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure USART2 Tx as alternate function push-pull.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Init clock for USART
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART2, &USART_ClockInitStructure);

	// Configure the USART2
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx; // USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	// In order to output USART2_CK, the SSOE bit in the SPI1_CR2
	// register must be set to configure the pin in output mode (hack).
	SPI1->CR2 |= SPI_CR2_SSOE;
	enable_rs232();
}

void enable_rs232_interrupts(void) {
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable transmit and receive interrupts for the USART2.
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	// Enable the USART2 IRQ in the NVIC module (so that the USART2 interrupt handler is enabled).
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


#define LED_PORT	GPIOC
#define LED_PIN		GPIO_Pin_12

void led_toggle(void) {
	uint8_t led_bit = GPIO_ReadOutputDataBit(LED_PORT, LED_PIN);

	if (led_bit == (uint8_t) Bit_SET)
		GPIO_ResetBits(LED_PORT, LED_PIN); // clear
	else
		GPIO_SetBits(LED_PORT, LED_PIN); // set
}

void send_byte(uint8_t b) {
	// Send next character
	USART_SendData(USART2, (b & 0xFF));

	// Wait until previous character got transfered
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;
}

void println(const char* s) {
	uint8_t i = 0;
	while (s[i] != 0x00) {
		send_byte((uint8_t) s[i]);
		i++;
	}
}

void busy_loop(uint32_t delay) {
	while (delay)
		delay--;
}
