#pragma once

#include <Arduino.h>

void ADC1_init(ADC_MUXPOS_t ain_init);

uint16_t ADC1_read(ADC_MUXPOS_t ain);
