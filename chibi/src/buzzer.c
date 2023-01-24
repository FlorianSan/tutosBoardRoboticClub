#include "buzzer.h"
#include "hal.h"

static void buz(PWMDriver * pwmd) {
  (void)pwmd;
  if(!palReadLine(LINE_BUTTON)) {
    palToggleLine(LINE_BUZZER);
  }
}


#define PERIOD_TICK(resolution) (1<<(resolution))
#define F_TICK(resolution, fpwm) (PERIOD_TICK(resolution) * (fpwm))

static const PWMConfig pwmBuzConfig = {
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

int notes[] = { 4096, 3649, 3251, 3069, 2734, 2435, 2170,
                2048, 1825, 1625, 1534, 1367, 1218, 1085,
                1024, 912, 813, 767, 683, 609, 542,
                512, 456, 406, 384, 342, 304, 271,
                256, 228, 203, 192, 171, 152, 136};

//#define abs(x) ( (x) > 0 ? (x) : -(x))

int abs(int x) {
  if(x>= 0) { return x; }
  else { return -x; }
}


int nearest_note(int input) {
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


void buzzer_init(void) {
  pwmStart(&PWMD5, &pwmBuzConfig);
  pwmEnablePeriodicNotification(&PWMD5);
}

int buzzer_get_note(int val) {
  return nearest_note(val);
}

void buzzer_set_note(int val) {
  int note = nearest_note(val);
  pwmChangePeriod(&PWMD5, note);
}
