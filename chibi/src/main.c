/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "printf.h"
#include "math.h"
#include "buzzer.h"
#include "potar.h"
#include "servo_led.h"
#include "tca.h"


static const SerialConfig serialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS | USART_CR2_LINEN,
  0
};

static I2CConfig i2c1Config = {
    .op_mode = OPMODE_I2C,
    .clock_speed = 100000,
    .duty_cycle = STD_DUTY_CYCLE
};



/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(500);
    palSetLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}


/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  tca_t tca = {
    .i2cd = &I2CD1,
    .addr = 0x27,
    .out_state = 0b00010101,
    .conf = 0b11000000
  };

  sdStart(&SD2, &serialConfig);
  i2cStart(&I2CD1, &i2c1Config);
  potar_init();
  buzzer_init();
  servo_led_init();
  tca_init(&tca);


  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 1, Thread1, NULL);


  while (true) {
    int32_t val = potar_get_value();
    int note = buzzer_get_note(val);
    buzzer_set_note(note);
    if (!palReadLine(LINE_BUTTON)) {
      chprintf((BaseSequentialStream *)&SD2, "pot: %ld  note:%d\r\n", val, note);
      servo_set_pos(SERVO1, 2000);
      servo_set_pos(SERVO2, 1000);
      led_set(LED1, 4096);
      led_set(LED1, 0);
    } else {
      led_set(LED1, 4058-val);
      led_set(LED2, val);
      servo_set_pos(SERVO1, 800 + 1600*val/4096);
      servo_set_pos(SERVO2, 2400 - 1600*val/4096);
      uint8_t tca_input = tca_read(&tca);
      chprintf((BaseSequentialStream *)&SD2, "tca input: %x\r\n", tca_input);
    }

    static int i = 0;
    tca_write(&tca, ~(1<<(i%6)));
    i++;

    chThdSleepMilliseconds(50);
  
  }
}
