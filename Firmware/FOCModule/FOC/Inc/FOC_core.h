#ifndef FOCMODULE_FOC_CORE_H
#define FOCMODULE_FOC_CORE_H

void FOC_calibrate();
void FOC_SVPWM(float Uq, float Ud, float angle);
void FOC_voltage_loop(float target_voltage);

#endif //FOCMODULE_FOC_CORE_H
