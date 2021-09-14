#include "communication.h"

void init_receiver(Receiver* rcv, int (*char_available)(), uint8_t (*read)()) {
    rcv->state = WAIT_STX;
    rcv->read = read;
    rcv->char_available = char_available;
}

RcvCode parse_byte(struct Receiver* rcv, uint8_t b) {
    while(rcv->char_available()) {
        uint8_t b= rcv->read();
        switch(rcv->state) {
        case WAIT_STX:
            if(b == STX) {
                rcv->buffer_len = 0;
                rcv->chk = 0;
                rcv->state = WAIT_LENGHT;
            }
            break;
        case WAIT_LENGHT:
            if(b > RCV_BUFFER_MAX) {
                rcv->state = WAIT_STX;
                return RCV_ERROR;
            }
            rcv->bytes_left = b;
            rcv->chk += b;
            if(rcv->bytes_left == 1) {
                rcv->state = GET_CHK;
            } else {
                rcv->state = GET_PAYLOAD;
            }
            break;
        case GET_PAYLOAD:
            rcv->buffer[rcv->buffer_len++] = b;
            rcv->chk += b;
            rcv->bytes_left--;
            if(rcv->bytes_left == 1) {
                rcv->state = GET_CHK;
            }
            break;
        case GET_CHK:
            rcv->chk = ~rcv->chk;
            rcv->bytes_left--;
            rcv->state = WAIT_STX;
            if(rcv->chk == b) {
                //success
                return RCV_MESSAGE;
            } else {
                // checksum error
                return RCV_ERROR;
            }
            break;
        }
    }
    return RCV_NONE;
}

/**
 *  Sends <size> bytes of <data> throught the sender.
 */
void sendMessage(Sender* sender, uint8_t* data, int size) {
    sender->transmit(data, size);
}
