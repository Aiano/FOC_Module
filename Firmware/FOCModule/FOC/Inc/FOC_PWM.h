#ifndef FOCMODULE_FOC_PWM_H
#define FOCMODULE_FOC_PWM_H

#include "main.h"
#include "FOC_utility.h"

void FOC_tim_init();
FOC_STATE FOC_set_duty_cycle(float duty_cycle_A, float duty_cycle_B, float duty_cycle_C);
#endif //FOCMODULE_FOC_PWM_H
