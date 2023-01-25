#include "tca.h"
#include "hal.h"
#include "printf.h"

const i2caddr_t TCA_ADDR = 0x27;

enum TCARegisters {
    TCA_INPUT_PORT =            0x00,
    TCA_OUTPUT_PORT =           0x01,
    TCA_POLARITY_INVERSION =    0x02,
    TCA_CONFIGURATION =         0x03
};

static void _tca_write(tca_t* tca) {
    uint8_t tx_out[] = {TCA_OUTPUT_PORT, tca->out_state};
    i2cMasterTransmitTimeout(tca->i2cd, tca->addr, tx_out, 2, NULL, 0, TIME_INFINITE);
}

static void _tca_read(tca_t* tca) {
    uint8_t tx_out[] = {TCA_INPUT_PORT};
    uint8_t input;
    i2cMasterTransmitTimeout(tca->i2cd, tca->addr, tx_out, 1, &input, 1, TIME_INFINITE);
    tca->input_port = input & tca->conf;
}

void tca_init(tca_t* tca) {
    uint8_t tx_conf_buf[] = {TCA_CONFIGURATION, tca->conf};
    msg_t ret = i2cMasterTransmitTimeout(tca->i2cd, tca->addr, tx_conf_buf, 2, NULL, 0, TIME_INFINITE);
    if(ret != MSG_OK) {
        chprintf((BaseSequentialStream *)&SD2, "init I2C Error %ld\r\n", ret);
        return;
    }
    
    uint8_t tx_out[] = {TCA_OUTPUT_PORT, tca->out_state};
    ret = i2cMasterTransmitTimeout(tca->i2cd, tca->addr, tx_out, 2, NULL, 0, TIME_INFINITE);
    if(ret != MSG_OK) {
        chprintf((BaseSequentialStream *)&SD2, "configure I2C Error %ld\r\n", ret);
        return;
    }
}

void tca_write(tca_t* tca, uint8_t out) {
    tca->out_state = out;
    _tca_write(tca);
}

void tca_set_bit(tca_t* tca, uint8_t n, bool value) {
    if(value) {
        tca->out_state |= 1<<n;
    } else {
        tca->out_state &= ~(1<<n);
    }
    _tca_write(tca);
}

uint8_t tca_read(tca_t* tca) {
    // TODO if using the interrupt, read only if the interrupt occured
    _tca_read(tca);
    return tca->input_port;
}
