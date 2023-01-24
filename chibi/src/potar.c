#include "potar.h"
#include "hal.h"

#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      8
static adcsample_t samples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];


/*
 * ADC conversion group.
 * Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
 * Channels:    IN11.
 */
static const ADCConversionGroup adcgrpcfg1 = { 
  FALSE,
  ADC_GRP1_NUM_CHANNELS,
  NULL,
  NULL,
  0,                        /* CR1 */
  ADC_CR2_SWSTART,          /* CR2 */
  ADC_SMPR1_SMP_AN11(ADC_SAMPLE_3),
  0,                        /* SMPR2 */
  0,                        /* HTR */
  0,                        /* LTR */
  0,                        /* SQR1 */
  0,                        /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN8)
};



void potar_init(void) {
  adcStart(&ADCD1, NULL);
}

int32_t potar_get_value(void) {
  adcConvert(&ADCD1, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
  uint32_t acc = 0;
  for(int i=0; i<ADC_GRP1_BUF_DEPTH; i++) {
    acc += samples1[i];
  }
  acc/= ADC_GRP1_BUF_DEPTH;
  return acc;
}