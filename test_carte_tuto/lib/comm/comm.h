#ifndef COMM_H
#define COMM_H
#include <cstdint>
#include <initializer_list>

enum Register{
    POT_L = 0x00,
    POT_H = 0x01,  // C'est vrai que c'est...
    LIGHT_L = 0x02,
    LIGHT_H = 0x03,
    LEDS_L = 0x04,
    LEDS_H = 0x05,
    MODE = 0x06
};

enum RxMessageType{
    PONG = 0x11,
    REG = 0x12,
    CONTINUOUS = 0x13
};
struct RxMessage{
    uint8_t instruction;
    uint8_t dataLength;
    uint8_t data[255];
};

class ATTinyCommunication{

public:
ATTinyCommunication(): rxState(RxState::IDLE), expectedRxLength(0){};

bool sendPing();
bool sendRead(const uint8_t reg, const uint8_t readLength);
bool sendWrite(const uint8_t startRegister, const uint8_t* values, const uint8_t writeLength);

bool getMessage(RxMessage& message);

private:
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

struct __attribute__((__packed__)) TxWriteMessage{
    uint8_t instruction = TxMessageType::WRITE;
    uint8_t startRegister;
    uint8_t values[250];
};

enum RxState{
    IDLE,
    HEADER_RECEIVED
};

bool checkHeader();
bool checkPayload(RxMessage& rxMessage);

RxState rxState;
uint8_t expectedRxLength;
uint8_t rxMessage[256];
};


#endif /* COMM_H */
