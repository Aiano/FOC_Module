#ifndef FOCMODULE_FOC_SETTINGS_H
#define FOCMODULE_FOC_SETTINGS_H

#include "FOC_encoder.h"

/* PWM 参数 */
extern float FOC_pwm_frequency;
extern float FOC_pwm_max;

/* 电流采样参数 */
extern float FOC_sample_duty_cycle;

/* 电源参数 */
extern float FOC_supply_voltage;

/* 电机参数 */
extern float FOC_pole_pairs;

/* 角度编码器 */
extern FOC_ENCODER_TYPE FOC_encoder_type;
extern float FOC_mechanical_angle_offset;

#endif //FOCMODULE_FOC_SETTINGS_H
