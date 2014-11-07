/*
 * DigitalInput.cpp
 */
#include "digitalinput.h"
#include "controller/intcontroller.h"

DigitalInput::DigitalInput(uint8_t port, uint8_t pin) :
		Gpio(port, pin) {
}

DigitalInput::~DigitalInput() {
}

/**
 * Configure the pin as input with interrupts.
 */
bool DigitalInput::initialize() {
	assert(_pin.port >= 'A' && _pin.port <= 'G');

	// Enable needed clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | gpio_port_rcc, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = _pin.pin;
	GPIO_Init(GPIOC, &GPIO_InitStructure);



	// TODO: move to IntCtrl ?
	// FIXME: generic ?


	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);

	// Configure external interrupt for PC6
	EXTI_ClearITPendingBit(_pin.pin);
	EXTI_InitTypeDef extiInitStructure;
	extiInitStructure.EXTI_Line = _pin.pin;
	extiInitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	extiInitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&extiInitStructure);

	// Enable EXTI interrupt
	// EXTI9_5_IRQn   = External Line[9:5] Interrupts
	// EXTI15_10_IRQn = External Line[15:10] Interrupts
	const uint8_t channel =
			(_pin.pin < (1 << 10)) ? EXTI9_5_IRQn : EXTI15_10_IRQn;

	NVIC_InitTypeDef nvicInitStructure;
	nvicInitStructure.NVIC_IRQChannel = channel;
	nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	nvicInitStructure.NVIC_IRQChannelSubPriority = 0x00;
	nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStructure);

	// Register as interrupt
	IntCtrl::instance()->registerInt(this, _pin.pin);

	return true;
}

bool DigitalInput::read() {
	if ((GPIO_ReadInputDataBit(gpio_port_base, _pin.pin)) == Bit_SET)
		_state = On;
	else
		_state = Off;

	return (_state == State(On)); // Boolean value
}

void DigitalInput::irq() {
	read();	// Read the button value and save the new state
}
