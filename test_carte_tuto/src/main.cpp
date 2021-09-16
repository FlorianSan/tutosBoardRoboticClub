#include <Arduino.h>
#include <Wire.h>
#include "board.h"
#include <TM1637.h>
#include <Servo.h>
#include <Tone.h>
//#define ENABLE_HWSERIAL6

TM1637 tm(DISPLAY_CLK, DISPLAY_DIO);

int light_status = 0;
uint8_t lightB;

Servo servos[2];
int current_servo = 0;

uint32_t pot_time;
uint32_t display_time;
uint32_t motor_time;
uint32_t enc_time;

bool tca_interrupt_occured = false;

void tca_isr() {
  tca_interrupt_occured = true;
}

void enc_isr() {
  enc_time = millis(); 
}

void b1_isr() {
  current_servo = 0;
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

void b2_isr() {
  current_servo = 1;
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
}

uint8_t tca_config() {
  Wire.beginTransmission(TCA_ADDR);
  uint8_t config[] = {TCA_CONFIG, 0b11000000};
  Wire.write(config, 2);
  return Wire.endTransmission();
}

uint8_t tca_output(uint8_t data) {
  Wire.beginTransmission(TCA_ADDR);
  uint8_t leds[] = {TCA_OUTPUT_PORT, data};
  Wire.write(leds, 2);
  return Wire.endTransmission();
}

uint8_t tca_input() {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(TCA_INPUT_PORT);
  Wire.endTransmission();
  Wire.requestFrom(TCA_ADDR, 1);
  uint8_t byte = Wire.read();
  return byte;
}

void send_buffer(uint8_t buffer[], int len_buffer) {
  buffer[1] = (uint8_t)(len_buffer - 2);
  uint8_t chk = 0;
  for(int i=1; i<len_buffer; i++) {
    chk += buffer[i];
  }
  buffer[len_buffer-1] = ~chk;
  Serial6.write(buffer, len_buffer);
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLDOWN);

  pinMode(ENC1A, INPUT_PULLUP);
  pinMode(ENC1B, INPUT_PULLUP);

  pinMode(MOTA, OUTPUT);
  pinMode(MOTB, OUTPUT);
  digitalWrite(MOTA, LOW);
  digitalWrite(MOTB, LOW);

  tm.begin();
  tm.setBrightness(4);

  servos[0].attach(SERVO1);
  servos[1].attach(SERVO2);

  attachInterrupt(INT_TCA, tca_isr, FALLING);
  attachInterrupt(ENC1A, enc_isr, FALLING);
  attachInterrupt(BTN1, b1_isr, FALLING);
  attachInterrupt(BTN2, b2_isr, RISING);

  Serial.begin(115200);
  Serial6.begin(115200);

  Wire.begin();//(SDA1, SCL1);

  auto ret = tca_config();
  if(ret) {
    Serial.printf("error TCA : %d\n", ret);
  }

  ret = tca_output(0b11111111);
  if(ret) {
    Serial.printf("error TCA : %d\n", ret);
  }

  int tempo = 90;
  int ddc = (250*30.0/tempo);

  tone(BUZZ, 523, 2*ddc);
  delay(4*ddc);
  tone(BUZZ, 392, ddc);
  delay(2*ddc);
  tone(BUZZ, 392, ddc);
  delay(2*ddc);
  tone(BUZZ, 440, 2*ddc);
  delay(4*ddc);
  tone(BUZZ, 392, 2*ddc);
  delay(8*ddc);
  tone(BUZZ, 492, 2*ddc);
  delay(4*ddc);
  tone(BUZZ, 523, 2*ddc);
}

#define PING 0x01
#define READ 0x02
#define WRITE 0x03

void loop() {
  // potentiometer initial value;
  static float pot_filtered = static_cast<float>(analogRead(POT));
  static bool btn3 = false;
  static bool btn4 = false;

  if(tca_interrupt_occured) {
    tca_interrupt_occured = false;
    auto tca_state = tca_input();
    btn3 = !(tca_state & 0b01000000);
    btn4 = !(tca_state & 0b10000000);

    if(btn3) {
      // attiny mode pot
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x06, 1, 0};
      send_buffer(buffer, 6);
    } else if(btn4) {
      // attiny mode manu
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x06, 0, 0};
      send_buffer(buffer, 6);
    }
  }

  if(enc_time && millis() - enc_time > 1) {
    enc_time = 0;
    if(!digitalRead(ENC1A)) {
      digitalWrite(LED1, current_servo);
      digitalWrite(LED2, current_servo);

      int servo_val = servos[current_servo].read();
      if(digitalRead(ENC1B)) {
        servo_val = min((servo_val + 10), 180);
      } else {
        servo_val = max((servo_val - 10), 0);
      }
      servos[current_servo].write(servo_val);
    }
  }

  // read and filter potentiometer
  if(millis() - pot_time >= 10) {
    auto val_pot = static_cast<float>(analogRead(POT));
    pot_filtered = 0.9*pot_filtered + 0.1*val_pot;
    pot_time = millis();
  }

  if(millis() - motor_time >= 50) {
    auto speed = map(pot_filtered, 0, 1024, -255, 255);

    // threshold
    if(abs(speed) < 50) {
      speed = 0;
    }

    // full speed left or right on BTN3 or BTN4
    if(btn4) {
      speed = 255;
    }
    if(btn3) {
      speed = -255;
    }

    // set motor command
    if(speed > 0) {
      analogWrite(MOTA, HIGH);
      analogWrite(MOTB, abs(speed));
    } else {
      analogWrite(MOTA, abs(speed));
      analogWrite(MOTB, HIGH);
    }

    // 
    uint8_t tca_leds = 0XFF;
    if(speed < -180) {
      tca_leds = 0b11000111;
    } else if(speed < -120) {
      tca_leds = 0b11100111;
    } else if(speed < -180) {
      tca_leds = 0b11110111;
    } else if(speed < 0) {
      tca_leds = 0b11110111;
    } else if(speed > 180) {
      tca_leds = 0b11111000;
    } else if(speed > 120) {
      tca_leds = 0b11111001;
    } else if(speed > 0) {
      tca_leds = 0b11111011;
    } else {
      tca_leds = 0b11111111;
    }
    tca_output(tca_leds);


    // attiny leds
    uint16_t leds_attiny = 0;
    for(int i=0; i<9*(int)pot_filtered/1000; i++) {
      leds_attiny |= 1<<i;
    }
    uint8_t leds_l = leds_attiny&0xFF;
    uint8_t leds_h = leds_attiny >> 8;
    uint8_t buffer[] = {0xFF, 0, WRITE, 0x04, leds_l, leds_h, 0};
    send_buffer(buffer, 7);
    
    

    motor_time = millis();
  }

  //display potentiometer value.
  if(millis() - display_time >= 100) {
    tm.display(pot_filtered);
    display_time = millis();
  }

  while(Serial.available()) {
    int b = Serial.read();
    if(b == 'p') {
      // ping
      Serial.println("-- ping --");
      uint8_t buffer[] = {0xFF, 0, PING, 0};
      send_buffer(buffer, 4);
    }
    if(b == 'o') {
      Serial.println("-- potar --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, READ, 0x00, 2, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'i') {
      Serial.println("-- light --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, READ, 0x02, 2, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'l') {
      Serial.println("-- leds --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, READ, 0x04, 2, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'm') {
      Serial.println("-- mode --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, READ, 0x06, 1, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'a') {
      Serial.println("-- mode manu --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x06, 0, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'w') {
      Serial.println("-- pot cont --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x06, 0b10000000, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'x') {
      Serial.println("-- light cont --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x06, 0b01000000, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'c') {
      Serial.println("-- leds cont 100 --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x06, 0b00101001, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'v') {
      Serial.println("-- leds cont 500 --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x06, 0b00111001, 0};
      send_buffer(buffer, 6);
    }
    if(b == 'f') {
      Serial.println("-- all leds --");
      // read leds
      uint8_t buffer[] = {0xFF, 0, WRITE, 0x04, 0xFF, 1, 0};
      send_buffer(buffer, 7);
    }
  }

  while(Serial6.available()) {
    Serial.println(Serial6.read(), HEX);
    //Serial.write(Serial6.read());
  }

  // while(Serial6.available()) {
  //   uint8_t b = Serial6.read();
  //   Serial.println(b);
  //   if(b == '\n') {
  //     light_status = 0;
  //   }

  //   if(light_status == 0) {
  //     lightB = b;
  //     light_status++;
  //   } else if(light_status == 1) {
  //     int light = lightB | (b << 8);
  //     Serial.println(light);
  //     light_status++;
  //   }
  // }

}
