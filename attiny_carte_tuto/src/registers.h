#pragma once
#include <inttypes.h>

#define MAX_ADDR 6

enum Registers {
    POT_L,
    POT_H,
    LIGHT_L,
    LIGHT_H,
    LEDS_L,
    LEDS_H,
    MODE
};


#define MODE_Continuous_Mask (0b111 << 5)
#define MODE_POTC_Mask (1<<7)
#define MODE_LIGHTC_Mask (1<<6)
#define MODE_LEDSC_Mask (1<<5)
#define MODE_CFREQ_Mask (0b11<<3)
#define MODE_LEDSMODE_Mask (0b111)

enum MODE_CFREQ_enum {
    MODE_CFREQ_50MS =  (0x00<<3),  /* 50ms  (20Hz) */
    MODE_CFREQ_100MS = (0x01<<3),  /* 100ms (10Hz) */
    MODE_CFREQ_200MS = (0x02<<3),  /* 200ms (5Hz ) */
    MODE_CFREQ_500MS = (0x03<<3),  /* 500ms (2Hz ) */
};

enum MODE_LEDSMODE_enum {
  MODE_LEDSMODE_MANUAL      = (0x00 << 0), /* apply LEDS register */
  MODE_LEDSMODE_POT         = (0x01 << 0), /* potentiometer scale */
  MODE_LEDSMODE_LIGHT       = (0x02 << 0), /* light scale */
  MODE_LEDSMODE_POT_SPEED   = (0x03 << 0), /* potentiometer speed */
  MODE_LEDSMODE_LIGHT_SPEED = (0x04 << 0), /* light speed */
};


void registers_init();
void writeRegister(Registers addr, uint8_t value);
uint8_t getRegister(Registers addr);
uint8_t* getRegisters();
