#pragma once
#include "hal.h"

typedef struct {
    I2CDriver* i2cd;
    i2caddr_t addr;
    uint8_t out_state;
    uint8_t conf;
    // read only
    uint8_t input_port;
} tca_t;

void tca_init(tca_t* tca);
void tca_write(tca_t* tca, uint8_t out);
void tca_set_bit(tca_t* tca, uint8_t n, bool value);
uint8_t tca_read(tca_t* tca);
