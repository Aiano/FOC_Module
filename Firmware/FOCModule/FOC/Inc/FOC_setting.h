#ifndef FOCMODULE_FOC_SETTINGS_H
#define FOCMODULE_FOC_SETTINGS_H

#include "FOC_encoder.h"
#include "FOC_PID.h"
#include "FOC_LPF.h"

typedef enum{
    FOC_MODE_VOLTAGE = 0,
    FOC_MODE_CURRENT,
    FOC_MODE_VELOCITY,
    FOC_MODE_POSITION
}FOC_MODE_TYPE;

/* PWM 参数 */
extern float FOC_pwm_frequency;
extern float FOC_pwm_max;

/* 电流采样参数 */
extern float FOC_sample_direction;
extern float FOC_sample_duty_cycle;
extern float FOC_sample_max;
extern float FOC_sample_offset[3];
extern float FOC_sample_mid;
extern float FOC_sample_ref_voltage;
extern float FOC_INA_gain;
extern float FOC_shunt_resistance;

/* 电源参数 */
extern float FOC_supply_voltage;

/* 电机参数 */
extern float FOC_pole_pairs;

/* 角度编码器 */
extern FOC_ENCODER_TYPE FOC_encoder_type;
extern float FOC_mechanical_angle_offset;
extern float FOC_encoder_direction;

/* PID参数 */
extern FOC_PID_TYPE pid_current_q;
extern FOC_PID_TYPE pid_current_d;
extern FOC_PID_TYPE pid_velocity;
extern FOC_PID_TYPE pid_position;

/* LPF参数 */
extern FOC_LPF_TYPE lpf_velocity;

/* 运行模式 */
extern FOC_MODE_TYPE FOC_mode;

/* 目标参数 */
extern float FOC_target_voltage;
extern float FOC_target_current;
extern float FOC_target_velocity;
extern float FOC_target_position;

/* 范围限制 */
extern float FOC_target_velocity_upper_limit; // 目标速度上限
extern float FOC_target_velocity_lower_limit; // 目标速度下限
extern float FOC_target_current_upper_limit; // 目标电流上限
extern float FOC_target_current_lower_limit; // 目标电流下限

/* 运行参数 */
extern float Ia, Ib, Ic;
extern float Id, Iq;
extern float Uq, Ud;

#endif //FOCMODULE_FOC_SETTINGS_H
