#include "tutrocol.h"
#include <Arduino.h>
#include "registers.h"
#include "hard_interface.h"

enum TXType {
    TX_PING = 0x01,
    TX_READ = 0x02,
    TX_WRITE = 0x03,
};

enum RXType {
    RX_PONG = 0x11,
    RX_REG = 0x12,
    RX_CONTINOUS = 0x13,
};

enum ContinuousFlags {
    POT = 0,
    LIGHT = 1,
    LEDS = 2,
    NONE,
};

Receiver rcv;
Sender sdr;

int char_available() {
    return Serial.available();
}

uint8_t read() {
    (uint8_t)Serial.read();
}

void write(uint8_t* buffer, int size) {
    Serial.write(buffer, size);
}

static void send_message(RXType type, uint8_t* payload, int size) {
    sdr.buffer[0] = 0xFF;
    sdr.buffer[1] = size + 2;
    sdr.buffer[2] = static_cast<uint8_t>(type);
    memcpy(sdr.buffer + 3, payload, size);
    uint8_t chk = 0;
    for(int i=1; i<size+3; i++) {
        chk += sdr.buffer[i];
    }
    sdr.buffer[size+3] = ~chk;
    sdr.buffer_len = size + 4;

    sendMessage(&sdr);
}



static ContinuousFlags continuous_next() {
    static ContinuousFlags last_sent = POT;
    
    uint8_t mode = getRegister(Registers::MODE);
    // test if any flag is set
    if(!(mode & MODE_Continuous_Mask)) {
        return NONE;
    }
    ContinuousFlags to_send;
    switch (last_sent)
    {
    case POT:
        if(mode & MODE_LIGHTC_Mask) {
            to_send = LIGHT;
        } else if(mode & MODE_LEDSC_Mask) {
            to_send = LEDS;
        } else {
            to_send = POT;
        }
        break;
    case 1:
        if(mode & MODE_LEDSC_Mask) {
            to_send = LEDS;
        } else if(mode & MODE_POTC_Mask) {
            to_send = POT;
        } else {
            to_send = LIGHT;
        }
        break;
    case 2:
        if(mode & MODE_POTC_Mask) {
            to_send = POT;
        } else if(mode & MODE_LIGHTC_Mask) {
            to_send = LIGHT;
        } else {
            to_send = LEDS;
        }
        break;
    
    default:
        to_send = NONE;
        break;
    }
    return to_send;
}

void sendContinuous() {
    auto data_id = continuous_next();
    if(data_id == NONE) {
        return;
    }
    uint8_t data[2] = {0};
    data[1] = static_cast<uint8_t>(data_id) << 6;

    switch (data_id)
    {
    case POT:
    {
        auto pot_h = getRegister(POT_H);
        data[1] |= (pot_h & 0b11);  // get the 2 lower bits
        data[0] = getRegister(POT_L);
    }
    break;
    case LIGHT:
    {
        auto light_h = getRegister(LIGHT_H);
        data[1] |= (light_h & 0b11);  // get the 2 lower bits
        data[0] = getRegister(LIGHT_L);
    }
    break;
    case LEDS:
    {
        auto leds_h = getRegister(LEDS_H);
        data[1] |= (leds_h & 0b1);  // get the lower bits
        data[0] = getRegister(LEDS_L);
    }
    break;
    default:
        break;
    }
    
    send_message(RX_CONTINOUS, data, 2);
}

static void send_pong() {
    send_message(RX_PONG, NULL, 0);
}

static void handle_read() {
    uint8_t addr = rcv.buffer[1];
    uint8_t count = rcv.buffer[2];
    if(addr + count > MAX_ADDR+1) {return;}
    uint8_t* start = getRegisters() + addr;
    send_message(RX_REG, start, count);
}

static void handle_write() {
    uint8_t addr = rcv.buffer[1];
    if(addr < LEDS_L) {return;}     //registers before LEDS_L are read only.
    for(int i=0; i<rcv.buffer_len-2 && addr+i <= MAX_ADDR; i++) {
        auto reg = static_cast<Registers>(addr+i);
        writeRegister(reg, rcv.buffer[i+2]);
    }
}

void tutrocol_init() {
    init_receiver(&rcv, char_available, read);
    init_sender(&sdr, write);
}

void tutrocol_recv() {
    auto status = receive(&rcv);
    if(status == RcvCode::RCV_ERROR) {
        Serial.println("error!");
    } else if(status == RcvCode::RCV_MESSAGE) {
        if(rcv.buffer_len < 1) {return;}
        switch(rcv.buffer[0]) {
            case TX_PING:
                send_pong();
                break;
            case TX_READ:
                handle_read();
                break;
            case TX_WRITE:
                handle_write();
                // // Update leds state, in case LEDS registers were changed
                // update_leds();
                break;
            default:
                Serial.write(rcv.buffer[0]);
                break;
        }
    }
}
