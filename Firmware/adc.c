/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.08.05
 * ADC data reading and processing functions
 *****************************************************************************/

#include <avr\io.h>
#include <avr\interrupt.h>
#include "adc.h"

#ifndef ADMUX_REFSOURCE
	#error ADMUX_REFSOURCE must be defined in configuration file
#endif
#ifndef ADMUX_CHANNELV
	#error ADMUX_CHANNELV must be defined in configuration file
#endif
#ifndef ADMUX_CHANNELC
	#error ADMUX_CHANNELC must be defined in configuration file
#endif

uint32_t g_adcsumv = 0, g_adcsumc = 0;  // ADC resulting summs from READINGS_TO_ACCUMULATE readings
uint8_t g_adcdeliveries = 0; // ADC unprocessed deliveries counter

void ADC_Initialize() {
    // ADC interupt, free mode, ADC enabled, prescaler CK/128
    // Tconv = F_CPU / PRESCALE / 13 (4000000 / 128 / 13 ~ 2404 reads per sec)
	ADCSRA = _BV(ADIE) | _BV(ADFR) | _BV(ADEN) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);
	// reference 2.56V, input CHANNELV
    ADMUX = ADMUX_REFSOURCE | ADMUX_CHANNELV;
	// start conversion
    ADCSRA |= (_BV(ADSC));
}

ISR(ADC_vect) {
	static uint32_t sumv = 0, sumc = 0; // ADC readings accumulators
	static uint16_t count = 0; // ADC accumulate readings counter
	switch (ADMUX & (ADMUX_CHANNELV | ADMUX_CHANNELC)) {
		case ADMUX_CHANNELV:
			ADMUX = ADMUX_REFSOURCE | ADMUX_CHANNELC;
			sumc += ADC;
			break;

		case ADMUX_CHANNELC:
			ADMUX = ADMUX_REFSOURCE | ADMUX_CHANNELV;
			sumv += ADC;
			break;
	}

	if (++count == 2 * READINGS_TO_ACCUMULATE) {
		g_adcsumv = sumv;
		g_adcsumc = sumc;
		g_adcdeliveries++;

		count = 0x0000;
		sumv = sumc = 0x00000000;
	}
}