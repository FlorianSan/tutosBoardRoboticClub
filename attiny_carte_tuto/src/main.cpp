#include <Arduino.h>
#include "adc.h"
#include "tutrocol.h"
#include "hard_interface.h"
#include "registers.h"

uint32_t continuous_time;
#define CONTINUOUS_PERIOD 100

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

  if(millis() - continuous_time > CONTINUOUS_PERIOD) {
    sendContinuous();
    continuous_time = millis();
  }
}