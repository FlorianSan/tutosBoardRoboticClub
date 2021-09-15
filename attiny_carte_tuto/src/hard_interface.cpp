#include "hard_interface.h"
#include "registers.h"
#include "adc.h"
#include <Arduino.h>

#define NB_LEDS 9
pin_size_t leds[NB_LEDS] = {PIN_PC0, PIN_PB1, PIN_PB4, PIN_PB5, PIN_PA7, PIN_PA6, PIN_PA5, PIN_PA4, PIN_PB0};

#define POTAR_AIN ADC_MUXPOS_AIN7_gc
#define LIGHT_AIN ADC_MUXPOS_AIN8_gc

uint32_t adc_time;
uint32_t led_time;

#define LED_UPDATE_PERIOD 20
#define ADC_UPDATE_PERIOD 25


enum ADCChannel {
  ADC_POTAR,
  ADC_LIGHT,
};

void leds_init() {
  for (int i = 0; i < NB_LEDS; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH); // turn leds OFF
  }

  // startup LEDs sequence
  for (int i = 0; i < NB_LEDS; i++) {
    digitalWrite(leds[i], LOW);
    delay(100);
  }
  for (int i = 0; i < NB_LEDS; i++) {
    digitalWrite(leds[i], HIGH);
    delay(100);
  }
}

static uint16_t getUint16(Registers base_addr) {
  Registers reg_high = static_cast<Registers>(static_cast<int>(base_addr) + 1);
  uint16_t value = getRegister(reg_high);
  value <<= 8;
  value |= getRegister(base_addr);
  return value;
}

/**
 *  value between 0 and 1023
 */
static void led_scale(uint16_t value) {
  value = min(max(value, 0), 1023);
  int led_max = (value*NB_LEDS)/1023;
  uint16_t leds = 0;
  for(int i=0; i<led_max; i++) {
    leds |= (1<<i);
  }
  writeRegister(LEDS_L, leds & 0xFF);
  writeRegister(LEDS_H, leds >> 8);
}

// void leds_animation() {
//   static uint32_t led_time = 0;
//   static int led_step = 0;

//   if(millis() - led_time > 200) {
//     if(led_step < NB_LEDS) {
//       digitalWrite(leds[led_step], LOW);  // turn led ON
//     }
//     else if(led_step < 2*NB_LEDS) {
//       digitalWrite(leds[led_step - NB_LEDS], HIGH);  // turn led OFF
//     } else if(led_step >= 2*NB_LEDS + 5) {
//       led_step = -1;
//     }
//     led_step += 1;
//     led_time = millis();
//   }
// }


void periodic_tasks()
{
  static ADCChannel channel = ADC_POTAR;

  // Update poter and light values
  if (millis() - adc_time > ADC_UPDATE_PERIOD)
  {
    if (channel == ADC_POTAR)
    {
      uint16_t val_potar = ADC1_read(POTAR_AIN);
      writeRegister(POT_L, val_potar & 0xFF);
      writeRegister(POT_H, val_potar >> 8);
      channel = ADC_LIGHT;
    }
    else
    {
      uint16_t val_light = 1023 - ADC1_read(LIGHT_AIN);
      writeRegister(LIGHT_L, val_light & 0xFF);
      writeRegister(LIGHT_H, val_light >> 8);
      channel = ADC_POTAR;
    }
    adc_time = millis();
  }

  if (millis() - led_time > LED_UPDATE_PERIOD) {
    // Run led animation, if any
    uint8_t mode = getRegister(MODE) & MODE_LEDSMODE_Mask;
    switch (mode) 
    {
    case MODE_LEDSMODE_MANUAL:
      // do nothing
      break;
    case MODE_LEDSMODE_POT:
    {
      uint16_t pot = getUint16(POT_L);
      led_scale(pot);
      break;
    }
    case MODE_LEDSMODE_LIGHT:
    {
      uint16_t light = getUint16(LIGHT_L);
      led_scale(light);
      break;
    }
    case MODE_LEDSMODE_POT_SPEED:
    {
      //TODO
      break;
    }
    case MODE_LEDSMODE_LIGHT_SPEED:
    {
      //TODO
      break;
    }
    default:
      break;
    }

    update_leds();
    led_time = millis();
  }
}

void update_leds() {
  uint16_t leds_state = getUint16(LEDS_L);
  for(int i=0; i< NB_LEDS; i++) {
    int led_state = !(leds_state & (1<<i));
    digitalWrite(leds[i], led_state);
  }
}
