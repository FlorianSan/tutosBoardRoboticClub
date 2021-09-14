#include "comm.h"
#include <Arduino.h>
#include <cstdint>
#include <cstddef>

enum TxMessageType{
    PING = 0x01,
    READ = 0x02,
    WRITE = 0x03
};

struct __attribute__((__packed__)) TxPingMessage{
    uint8_t instruction = TxMessageType::PING;
};

struct __attribute__((__packed__)) TxReadMessage{
    uint8_t instruction = TxMessageType::READ;
    uint8_t startRegister;
    uint8_t readLength;
};

uint8_t computeChecksum(uint8_t* message, uint8_t payloadSize){
    /* Checksum for the packet.
        CHK = ~(LEN + PAYLOAD[0] + … + PAYLOAD[N]) */
    uint8_t checksum = 0;
    for (size_t i=1; i < payloadSize + 2; i++){  // After STX, from LEN to the end of payload
        checksum = (checksum + message[i]) % 256;
    }
    return ~checksum;
}

int8_t sendTxMessage(uint8_t* payload, uint8_t payloadSize){
    uint8_t message[256];
    message[0] = 0xFF;  // STX
    if (payloadSize >= 254){
        return -1;
    }
    message[1] = (payloadSize + 1) % 256;  // +1 for checksum; %256 to be sure
    memcpy(&(message[2]), payload, payloadSize);
    message[2+payloadSize] = computeChecksum(message, payloadSize);
    Serial6.write(message, 3 + payloadSize);
    return 0;
}

bool sendPing(){
    TxPingMessage pingMessage;
    if (sendTxMessage(reinterpret_cast<uint8_t*>(&pingMessage), 1)){
        return false;
    }
    return true;
}

bool sendRead(const uint8_t startRegister, const uint8_t readLength){
    TxReadMessage readMessage;
    readMessage.startRegister = startRegister;
    readMessage.readLength = readLength;
        if (encapsulateTxMessage(pingMessage, 3)){
        return false;
    }
}

