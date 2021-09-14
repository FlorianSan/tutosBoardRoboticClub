#pragma once

#include <Arduino.h>

#define TCA_ADDR_LSB 1

#define BTN1 PA0
#define BTN2 PA1
#define BUZZ PA4
#define ENC1A PA8
#define ENC1B PA9
#define DISPLAY_CLK PA10
#define UART6_TX PA11
#define UART6_RX PA12
#define MOTB PA15
#define POT PB0
#define LED1 PB1
#define SERVO1 PB4
#define SERVO2 PB5
#define ENC2A PB6
#define ENC2B PB7
#define SCL1 PB8
#define SDA1 PB9
#define MOTA PB10
#define ATTINY_LINK PB12
#define DISPLAY_DIO PC4
#define LED2 PC8
#define INT_TCA PC9


#define TCA_ADDR (0x26 + TCA_ADDR_LSB)
#define TCA_INPUT_PORT 0x00
#define TCA_OUTPUT_PORT 0x01
#define TCA_POL_INV 0x02
#define TCA_CONFIG 0x03