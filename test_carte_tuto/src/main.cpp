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
    

    motor_time = millis();
  }

  //display potentiometer value.
  if(millis() - display_time >= 100) {
    tm.display(pot_filtered);
    display_time = millis();
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
