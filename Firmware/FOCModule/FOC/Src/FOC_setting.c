/**
 * @file FOC_setting.c
 * @brief FOC算法参数设置
 * @details 这里的参数在电机运行过程中保持长时间不变，通过控制总线或上位机修改
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_setting.h"

/* PWM 参数 */
float FOC_pwm_frequency = 20 * 000 * 000; // PWM频率，单位：Hz
float FOC_pwm_max       = 4000; // PWM最大值计数值
//float FOC_pwm_max       = 400; // PWM最大值计数值

/* 电流采样参数 */
float FOC_sample_duty_cycle = 3950;
//float FOC_sample_duty_cycle = 350;
float FOC_sample_max = 4096;
float FOC_sample_mid = 2048;
float FOC_sample_ref_voltage = 3.3f;
float FOC_INA_gain = 20;
float FOC_shunt_resistance = 0.005f;

/* 电源参数 */
float FOC_supply_voltage = 15; // 供电电压，单位：V

/* 电机参数 */
float FOC_pole_pairs = 7; // 极对数

/* 角度编码器 */
FOC_ENCODER_TYPE FOC_encoder_type = FOC_ENCODER_AS5600; // 角度编码器型号
float FOC_mechanical_angle_offset = 0; // 机械角度偏移

/* PID参数 */
FOC_PID_TYPE pid_current_d = {0.15f, 0.0005f, 0, 0, 0, 200, -200, 5, -5, 0};
FOC_PID_TYPE pid_current_q = {0.15f, 0.0005f, 0, 0, 0, 200, -200, 5, -5, 0};