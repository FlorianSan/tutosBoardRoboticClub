#include "tutrocol.h"
#include <Arduino.h>

Receiver rcv;

int rand() {
    return 4;
}

int char_available() {
    return Serial.available();
}

uint8_t read() {
    (uint8_t)Serial.read();
}

void tutrocol_init() {
    init_receiver(&rcv, char_available, read);
}

void tutrocol_recv() {

}
