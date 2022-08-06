/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.08.05
 * ADC data reading and processing functions
 *****************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <avr\io.h>
#include "config.h"

extern uint32_t g_adcsumv, g_adcsumc;
extern uint8_t g_adcdeliveries;

void ADC_Initialize();

#endif