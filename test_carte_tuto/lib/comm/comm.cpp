#include "comm.h"
#include <Arduino.h>
#include <cstddef>



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
        Serial.print("Send message: payload size too big.");
        return -1;
    }
    message[1] = (payloadSize + 1) % 256;  // +1 for checksum; %256 to be sure
    memcpy(&(message[2]), payload, payloadSize);
    message[2+payloadSize] = computeChecksum(message, payloadSize);
    Serial6.write(message, 3 + payloadSize);
    return 0;
}

bool ATTinyCommunication::sendPing(){
    TxPingMessage pingMessage;
    if (sendTxMessage(reinterpret_cast<uint8_t*>(&pingMessage), 1)){
        Serial.print("Sending ping failed");
        return false;
    }
    return true;
}

bool ATTinyCommunication::sendRead(const uint8_t startRegister, const uint8_t readLength){
    TxReadMessage readMessage;
    readMessage.startRegister = startRegister;
    readMessage.readLength = readLength;
    if (sendTxMessage(reinterpret_cast<uint8_t*>(&readMessage), 3)){
        return false;
    }
    return true;
}

bool ATTinyCommunication::sendWrite(const uint8_t startRegister, const uint8_t* values, const uint8_t writeLength){
    if (writeLength >= 250)
    {
        return false;
    }
    TxWriteMessage writeMessage;
    writeMessage.startRegister = startRegister;
    memcpy(writeMessage.values, values, writeLength);
    if (sendTxMessage(reinterpret_cast<uint8_t*>(&writeMessage), 2+writeLength)){
        return false;
    }
    return true;
}

bool ATTinyCommunication::getMessage(RxMessage& message){
    switch (rxState)
    {
    case RxState::IDLE:
        if (checkHeader()){
            return checkPayload(message);
        }
        return false;
        break;
    case RxState::HEADER_RECEIVED:
        return checkPayload(message);
    }
    return false;
}

bool ATTinyCommunication::checkHeader(){
    Serial.println("Receive: checking header");
    if (Serial6.available() >= 2){
        if (Serial6.read() == 0xFF){
            Serial.println("Receive: start byte received");
            expectedRxLength = Serial6.read();
            if (expectedRxLength < 250){
                Serial.print("Receive: expected message length: ");
                Serial.println(expectedRxLength);
                rxMessage[0] = 0xFF;
                rxMessage[1] = expectedRxLength;
                rxState = RxState::HEADER_RECEIVED;
                return true;
            }
        }
    }
    rxState = RxState::IDLE;
    return false;
}

bool ATTinyCommunication::checkPayload(RxMessage& message){
    if (Serial6.available() >= expectedRxLength){
        Serial.println("Receive: enough bytes received, extracting payload");
        Serial6.readBytes(&(rxMessage[2]), expectedRxLength);
        rxState = RxState::IDLE;
    }
    uint8_t checksum = computeChecksum(rxMessage, expectedRxLength - 1);
    Serial.println("Receive: checking checksum");
    if (checksum == rxMessage[2 + expectedRxLength - 1]){  // 2 for header, -1 for the checksum (included in expectedRxLength)
        Serial.println("Receive: Checksum matched");
        message.instruction = rxMessage[2];
        message.dataLength = expectedRxLength - 2;  // -1 for the instuction and -1 for the checksum
        Serial.print("Receive: instruction:");
        Serial.println(message.instruction);
        Serial.print("Receive: data length");
        Serial.println(message.dataLength);
        memcpy(message.data, &(rxMessage[3]), message.dataLength);  
        return true;
    }
    return false;
}

