#include "servo_led.h"
#include "hal.h"

static const PWMConfig servoLedPwmConfig = {
  .frequency = 1000000,  // 1Mz
  .period = 20000,       // 20 ms, 50Hz
  .callback = NULL,
  .channels = {
    {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL}, // SERVO1
    {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL}, // SERVO2
    {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL}, // LED2
    {.mode = PWM_OUTPUT_ACTIVE_LOW,  .callback = NULL}  // LED1
  }
};


void servo_led_init(void) {
    pwmStart(&PWMD3, &servoLedPwmConfig);
}

void servo_set_pos(servo_t servo, int microseconds) {
    pwmEnableChannel(&PWMD3, servo, microseconds);
}

void led_set(led_t led, int value) {
    pwmEnableChannel(&PWMD3, led, PWM_FRACTION_TO_WIDTH(&PWMD3, 4096, value));
}
