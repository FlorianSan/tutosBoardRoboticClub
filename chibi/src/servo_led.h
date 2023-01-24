#pragma once

typedef enum {
    SERVO1 = 0,
    SERVO2 = 1
} servo_t;

typedef enum {
    LED1 = 3,
    LED2 = 2
} led_t;


void servo_led_init(void);

void servo_set_pos(servo_t servo, int microseconds);

/**
 * value: [0-4096]
*/
void led_set(led_t led, int value);

