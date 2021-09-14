#pragma once

//communication protocol
// tx packet: STX  LEN  INSTRUCTION  PARAMS  CHK
// STX: 0xFF 0xFF
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
//   data_id: 1: potar, 2: light, 3:leds



enum RcvState {
    IDLE,
    WAIT_LENGHT,
    WAIT_PAYLOAD,
    WAIT_CHK,
};

