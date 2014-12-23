/*
 * AnalogInput.cpp
 */
#include "analoginput.h"

#include "stm32f10x_adc.h"

AnalogInput::AnalogInput(uint8_t port, uint8_t pin, uint8_t channel) :
		Gpio(port, pin) {
	_channel = channel;
	_lastValue = 0;
}

AnalogInput::~AnalogInput() {

}

void AnalogInput::adInit() {
	/* A/D initialization. */
	ADC_InitTypeDef adcInitStructure;
	ADC_DeInit(ADC1);
	ADC_StructInit(&adcInitStructure);
	adcInitStructure.ADC_Mode = ADC_Mode_Independent;
	adcInitStructure.ADC_ScanConvMode = DISABLE;
	adcInitStructure.ADC_ContinuousConvMode = DISABLE;
	adcInitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adcInitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	adcInitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &adcInitStructure);
	ADC_Cmd(ADC1, ENABLE); // Enable the ADC
}

bool AnalogInput::initialize() {
	GPIO_InitTypeDef gpioInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | gpio_port_rcc, ENABLE);

	/* Configure pin as output. */
	gpioInitStructure.GPIO_Pin = _pin.pin;
	gpioInitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(gpio_port_base, &gpioInitStructure);

	adInit();
	return true;
}

uint16_t AnalogInput::read() {

	// FIXME: before changing the channel, the A/D must be initialized again.
	adInit();

	// Configure channel
	ADC_RegularChannelConfig(ADC1, _channel, 1, ADC_SampleTime_55Cycles5);

	// Start the conversion
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	// Wait until conversion completion
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
		;

	// Get the conversion value and save it as cached value
	_lastValue = ADC_GetConversionValue(ADC1);
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	return _lastValue;
}
