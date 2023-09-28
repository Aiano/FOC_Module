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
//float FOC_pwm_max       = 8400; // PWM最大值计数值
float FOC_pwm_max       = 800; // PWM最大值计数值

/* 电流采样参数 */
//float FOC_sample_duty_cycle = 8200;
float FOC_sample_duty_cycle = 750;

/* 电源参数 */
float FOC_supply_voltage = 15; // 供电电压，单位：V

/* 电机参数 */
float FOC_pole_pairs = 7; // 极对数

/* 角度编码器 */
FOC_ENCODER_TYPE FOC_encoder_type = FOC_ENCODER_AS5600; // 角度编码器型号
float FOC_mechanical_angle_offset = 0; // 机械角度偏移