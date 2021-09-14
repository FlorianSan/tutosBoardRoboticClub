#pragma once

#include <inttypes.h>

//communication protocol
// tx packet: STX  LEN  INSTRUCTION  PARAMS  CHK
// STX: 0xFF
// LEN: byte size of instruction + params + chk
// 
// INSTRUCTION: 
//     PING: 0x01  --> pong
//     READ: 0x02  --> register
//     WRITE: 0x03 --> N/A
//
// instructions details
// PING: no params
// READ : addr, nb reg to be read
// WRITE: addr, values avec auto increment des reg


// registers
// potarL, potarH
// lightL, lightH
// LEDS[0:7], LEDS[8]
// mode:
//   1 bit potar continuous
//   1 bit light continuous
//   1 bit leds continuous
//   2/3 bits: modes leds: ring, ring full, .., MANUAL
// 
//

// rx packet: STX LEN ID DATA CHK
// IDs:
// PONG: 0x01
// register: 0x02
//   DATA: reg | values
// continuous: 0x03
//   DATA: data_id[1:0], xxxx,  data[9:8] | data[7:0]
//   data_id: 0: potar, 1: light, 2:leds


#define STX 0xFF

#define RCV_BUFFER_MAX 10 //should be 8

enum RcvState {
    WAIT_STX,
    WAIT_LENGHT,
    GET_PAYLOAD,
    GET_CHK,
};

enum RcvCode {
    RCV_NONE,
    RCV_MESSAGE,
    RCV_ERROR,
};

struct Receiver {
    int (*char_available)();
    uint8_t (*read)();
    uint8_t buffer[RCV_BUFFER_MAX];
    uint8_t buffer_len;
    RcvState state;
    uint8_t chk;
    uint8_t bytes_left;
};

enum TXType {
    TX_PING,
    TX_READ,
    TX_WRITE,
};

struct Sender {
    void (*transmit)(uint8_t*, int size);
};

void init_receiver(Receiver* rcv, int (*char_available)(), uint8_t (*read)());

RcvCode parse_byte(struct Receiver* rcv, uint8_t b);

/**
 *  Sends <size> bytes of <data> throught the sender.
 */
void sendMessage(Sender* sender, uint8_t* data, int size);

/**
 *  Return type of the received message
 */
TXType getTxType(Receiver* rcv);


