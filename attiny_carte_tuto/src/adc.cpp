#include "adc.h"

void ADC1_init(ADC_MUXPOS_t ain_init) {
  /* Disable digital input buffer */
//   PORTC.PIN1CTRL &= ~PORT_ISC_gm;
//   PORTC.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;

  /* Disable pull-up resistor */
//   PORTC.PIN1CTRL &= ~PORT_PULLUPEN_bm;

  ADC1.CTRLC = ADC_PRESC_DIV32_gc     /* CLK_PER divided by 4 */
             | ADC_REFSEL_VDDREF_gc;  //ADC_REFSEL_INTREF_gc;  /* Internal reference */
  
  ADC1.MUXPOS = ain_init;   /* Select ADC channel */

  ADC1.CTRLA = ADC_ENABLE_bm          /* ADC Enable: enabled */
             | ADC_RESSEL_10BIT_gc;   /* 10-bit mode */
}

uint16_t ADC1_read(ADC_MUXPOS_t ain)
{
  // select channel
  ADC1.MUXPOS = ain;

  /* Start ADC conversion */
  ADC1.COMMAND = ADC_STCONV_bm;
  
  /* Wait until ADC conversion done */
  while ( !(ADC1.INTFLAGS & ADC_RESRDY_bm) )
  {}
  
  /* Clear the interrupt flag by writing 1: */
  ADC1.INTFLAGS = ADC_RESRDY_bm;
  
  return ADC1.RES;
}
