#ifndef FOCMODULE_FOC_CORE_H
#define FOCMODULE_FOC_CORE_H

void FOC_calibrate();
void FOC_SVPWM(float Uq, float Ud, float angle);
void FOC_Clarke_Transform(float Ia, float Ib, float Ic, float *Ialpha, float *Ibeta);
void FOC_Park_Transform(float Ialpha, float Ibeta, float angle, float *Id, float *Iq);
void FOC_voltage_loop(float target_voltage);
void FOC_current_loop(float target_current);
void FOC_main_loop();
void FOC_current_callback();

#endif //FOCMODULE_FOC_CORE_H
