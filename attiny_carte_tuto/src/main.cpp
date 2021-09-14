#include <Arduino.h>
#include "adc.h"

#define NB_LEDS 9

pin_size_t leds[NB_LEDS] = {PIN_PC0, PIN_PB1, PIN_PB4, PIN_PB5, PIN_PA7, PIN_PA6, PIN_PA5, PIN_PA4, PIN_PB0};

uint32_t adc_time;
uint16_t val_potar;
uint16_t val_light;

enum ADCChannel {
  POTAR,
  LIGHT,
};

#define POTAR_AIN ADC_MUXPOS_AIN7_gc
#define LIGHT_AIN ADC_MUXPOS_AIN8_gc

/**
 *  value between 0 and 1023
 */
void led_scale(uint16_t value) {
  int led_max = (value*NB_LEDS)/1023;
  for(int i=0; i<led_max; i++) {
    digitalWrite(leds[i], LOW);
  }
  for(int i=led_max; i<NB_LEDS; i++) {
    digitalWrite(leds[i], HIGH);
  }
}

void setup() {
  
  Serial.begin(115200);

  for(int i=0; i<NB_LEDS; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH);    // turn leds OFF
  }

  // startup LEDs sequence
  for(int i=0; i<NB_LEDS; i++) {
    digitalWrite(leds[i], LOW);
    delay(100);
  }
  for(int i=0; i<NB_LEDS; i++) {
    digitalWrite(leds[i], HIGH);
    delay(100);
  }  


  ADC1_init(ADC_MUXPOS_AIN7_gc);

}

void leds_animation() {
  static uint32_t led_time = 0;
  static int led_step = 0;

  if(millis() - led_time > 200) {
    if(led_step < NB_LEDS) {
      digitalWrite(leds[led_step], LOW);  // turn led ON
    }
    else if(led_step < 2*NB_LEDS) {
      digitalWrite(leds[led_step - NB_LEDS], HIGH);  // turn led OFF
    } else if(led_step >= 2*NB_LEDS + 5) {
      led_step = -1;
    }
    led_step += 1;
    led_time = millis();
  }
}


void loop() {
  static ADCChannel channel = POTAR;

  // Update potar and light sensor values
  if(millis() - adc_time > 25) {
    if(channel == POTAR) {
      val_potar = ADC1_read(POTAR_AIN);
      channel = LIGHT;
    } else {
      val_light  = ADC1_read(LIGHT_AIN);
      channel = POTAR;
    }
    adc_time = millis();
  }



  led_scale(1023-val_light);
      Serial.println(val_light);






  

}