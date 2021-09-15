#include "registers.h"

uint8_t registers[7];

void registers_init() {
    registers[POT_L] = 0;
    registers[POT_H] = 0;
    registers[LIGHT_L] = 0;
    registers[LIGHT_H] = 0;
    registers[LEDS_L] = 0;
    registers[LEDS_H] = 0;
    registers[MODE] = 1;
}

void writeRegister(Registers addr, uint8_t value) {
    registers[addr] = value;
}

uint8_t getRegister(Registers addr) {
    return registers[addr];
}

uint8_t* getRegisters() {
    return registers;
}
