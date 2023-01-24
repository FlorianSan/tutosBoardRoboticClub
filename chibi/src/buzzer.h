#pragma once

void buzzer_init(void);

/**
 * val: [0 - 4096]
*/

int buzzer_get_note(int val);

void buzzer_set_note(int val);
