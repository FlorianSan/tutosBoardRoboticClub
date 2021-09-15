#include <Arduino.h>
#include "adc.h"
#include "tutrocol.h"
#include "hard_interface.h"
#include "registers.h"

uint32_t continuous_time;

void setup() {
  leds_init();
  ADC1_init(ADC_MUXPOS_AIN7_gc);
  registers_init();
  Serial.begin(115200);
  tutrocol_init();
}

void loop() {
  tutrocol_recv();
  periodic_tasks();

  uint8_t cfreq = getRegister(MODE) & MODE_CFREQ_Mask;
  uint32_t continuous_period;
  switch (cfreq) {
    case MODE_CFREQ_50MS:
      continuous_period = 50;
      break;
    case MODE_CFREQ_100MS:
      continuous_period = 100;
      break;
    case MODE_CFREQ_200MS:
      continuous_period = 200;
      break;
    case MODE_CFREQ_500MS:
      continuous_period = 500;
      break;
    default:
      continuous_period = 1000;
  }

  if(millis() - continuous_time > continuous_period) {
    sendContinuous();
    continuous_time = millis();
  }
}