#ifndef RPM_CALC_H
#define RPM_CALC_H
#include "stdint.h"


volatile uint16_t t2_overruns;
volatile uint16_t goto_overruns;
volatile uint8_t goto_timer;
volatile bool goto_flag;
void check_goto();
void set_goto(uint16_t overruns, uint8_t timer);
void init_rpm_calculation();

int16_t exact;                 // real rotation speed
int16_t e_sum;
int16_t e_old;

#endif
