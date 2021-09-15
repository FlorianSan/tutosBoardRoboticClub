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

void registers_init();
void writeRegister(Registers addr, uint8_t value);
uint8_t getRegister(Registers addr);
uint8_t* getRegisters();
