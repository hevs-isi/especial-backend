/*
 * AnalogInput.cpp
 */
#include "analoginput.h"

#include "stm32f10x_adc.h"

AnalogInput::AnalogInput(uint8_t port, uint8_t pin, uint8_t channel) :
		_pin(port, pin), _channel(channel) {
	_lastValue = 0;
}

AnalogInput::~AnalogInput() {

}

/**
 * Initializes ADC15 used to measure the joystick button state (left, right, up, down).
 * ADC1 is used for the conversion.
 */
bool AnalogInput::initialize() {
	assert(_pin.port >= 'A' && _pin.port <= 'G');

	GPIO_InitTypeDef gpioInitStructure;
	ADC_InitTypeDef adcInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | gpio_port_rcc, ENABLE);
	ADC_DeInit(ADC1);

	// Set RC5 as analog input
	gpioInitStructure.GPIO_Pin = _pin.pin; //GPIO_Pin_5;
	gpioInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	gpioInitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &gpioInitStructure);

	// ADC Structure Initialization
	ADC_StructInit(&adcInitStructure);
	// Preinit
	adcInitStructure.ADC_Mode = ADC_Mode_Independent;
	adcInitStructure.ADC_ScanConvMode = DISABLE;
	adcInitStructure.ADC_ContinuousConvMode = DISABLE;
	adcInitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adcInitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	adcInitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &adcInitStructure);

	// Enable the ADC
	ADC_Cmd(ADC1, ENABLE);
	return true;
}

uint16_t AnalogInput::read() {
	// Configure channel
	ADC_RegularChannelConfig(ADC1, _channel, 1, ADC_SampleTime_55Cycles5);

	// Start the conversion
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	// Wait until conversion completion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	// Get the conversion value and save it as cached value
	_lastValue =  ADC_GetConversionValue(ADC1);
	return _lastValue;
}
