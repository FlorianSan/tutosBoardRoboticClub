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

#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      8
static adcsample_t samples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];


static const SerialConfig serialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS | USART_CR2_LINEN,
  0
};


void buz(PWMDriver * pwmd) {
  (void)pwmd;
  if(!palReadLine(LINE_BUTTON)) {
    palToggleLine(LINE_BUZZER);
  }
}


#define PERIOD_TICK(resolution) (1<<(resolution))
#define F_TICK(resolution, fpwm) (PERIOD_TICK(resolution) * (fpwm))

static const PWMConfig pwm3_config = {
  .frequency = F_TICK(12, 262),
  .period    = PERIOD_TICK(12),
  .callback  = buz,
  .channels = {
    {PWM_OUTPUT_DISABLED,    NULL},
    {PWM_OUTPUT_DISABLED,    NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_DISABLED,    NULL},
  }  
};

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
 * Channels:    IN11.
 */
static const ADCConversionGroup adcgrpcfg1 = { 
  FALSE,
  ADC_GRP1_NUM_CHANNELS,
  NULL,
  NULL,
  0,                        /* CR1 */
  ADC_CR2_SWSTART,          /* CR2 */
  ADC_SMPR1_SMP_AN11(ADC_SAMPLE_3),
  0,                        /* SMPR2 */
  0,                        /* HTR */
  0,                        /* LTR */
  0,                        /* SQR1 */
  0,                        /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN8)
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


int notes[] = { 4096, 3649, 3251, 3069, 2734, 2435, 2170,
                2048, 1825, 1625, 1534, 1367, 1218, 1085,
                1024, 912, 813, 767, 683, 609, 542,
                512, 456, 406, 384, 342, 304, 271,
                256, 228, 203, 192, 171, 152, 136};

//#define abs(x) ( (x) > 0 ? (x) : -(x))

int abs(int x) {
  if(x>= 0) {
    return x;
  } else {
    return -x;
  }
}

int nearest(int input) {
  int old_error = notes[0] - input;
  size_t i = 0;
  size_t nb_notes = 5*7;  //sizeof(notes)/sizeof(int);
  while(i < nb_notes) {
    int new_err = abs(notes[i+1] - input);
    if(new_err > old_error) {
      return notes[i];
    }
    old_error = new_err;
    i+=1;
  }
  return notes[nb_notes-1];
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

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, &serialConfig);

  adcStart(&ADCD1, NULL);
  pwmStart(&PWMD3, &pwm3_config);
  pwmEnablePeriodicNotification(&PWMD3);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 1, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    
      
    adcConvert(&ADCD1, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
    uint32_t acc = 0;
    for(int i=0; i<ADC_GRP1_BUF_DEPTH; i++) {
      acc += samples1[i];
    }
    acc/= ADC_GRP1_BUF_DEPTH;
    int note = nearest(acc);
    pwmChangePeriod(&PWMD3, note);
    pwmEnableChannel(&PWMD3, 2, note*(4058-acc)/4096);
    if (!palReadLine(LINE_BUTTON)) {
      chprintf((BaseSequentialStream *)&SD2, "pot: %ld  note:%d\r\n", acc, note);

      //test_execute((BaseSequentialStream *)&SD2, &rt_test_suite);
      //test_execute((BaseSequentialStream *)&SD2, &oslib_test_suite);
    }
    chThdSleepMilliseconds(50);
  
  }
}
