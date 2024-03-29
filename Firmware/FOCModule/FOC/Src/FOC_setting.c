/**
 * @file FOC_setting.c
 * @brief FOC算法参数设置
 * @details 这里的参数在电机运行过程中保持长时间不变，通过控制总线或上位机修改
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_setting.h"
#include "FOC_utility.h"

/* PWM 参数 */
float FOC_pwm_frequency = 20 * 000 * 000; // PWM频率，单位：Hz
float FOC_pwm_max       = 4000; // PWM最大值计数值
//float FOC_pwm_max       = 400; // PWM最大值计数值

/* 电流采样参数 */
float FOC_sample_direction   = -1; // 由于PCB布局布线限制，读到的电流可能需要软件取反
float FOC_sample_duty_cycle  = 3950; // PWM触发ADC采样的计数值
//float FOC_sample_duty_cycle = 350;
float FOC_sample_max         = 4096; // ADC读数的最大值
float FOC_sample_offset[3]   = {2040, 2040, 2040}; // 电流直流偏置
float FOC_sample_mid         = 2048;
float FOC_sample_ref_voltage = 3.3f;
float FOC_INA_gain           = 20;
float FOC_shunt_resistance   = 0.005f;

/* 温度监测 */
float FOC_temp_B          = 3380;
float FOC_temp_R2         = 10000;
float FOC_temp_T2         = 25;
float FOC_temp_upper_R    = 10000;
float FOC_temp_sample_max = 4096;
float FOC_PCB_temp        = 25;

/* 电源参数 */
float FOC_supply_voltage = 12; // 供电电压，单位：V

/* 电机参数 */
float FOC_pole_pairs = 7; // 极对数

/* 角度编码器 */
FOC_ENCODER_TYPE FOC_encoder_type            = FOC_ENCODER_AS5600; // 角度编码器型号
float            FOC_mechanical_angle_offset = 3.91318512f; // 机械角度偏移
float            FOC_mechanical_angle_zero   = -0.23f; // 机械角度零点
float            FOC_encoder_direction       = -1; // 编码器方向，只能为1或-1

/* 三相线序 */
float FOC_line_direction = 1; // 三相线序，修改线序后需要重新校准

/* PID参数 */
FOC_PID_TYPE pid_current_d = {0.15f, 0.001f, 0, 0, 0, 2000, -2000, 6, -6, 0};
FOC_PID_TYPE pid_current_q = {0.15f, 0.001f, 0, 0, 0, 2000, -2000, 6, -6, 0};
FOC_PID_TYPE pid_velocity  = {0.6f, 0.006f, 0, 0, 0, 800, -800, 8.0f, -8.0f, 0.0f};
FOC_PID_TYPE pid_position  = {20.0f, 0, 0, 0, 0, 100, -100, 3.0f, -3.0f, 0};

/* LPF参数 */
FOC_LPF_TYPE lpf_velocity = {0.2f, 0};

/* 运行模式 */
FOC_MODE_TYPE FOC_mode = FOC_MODE_VOLTAGE;

/* 目标参数 */
float FOC_target_voltage  = 0; // 目标电压
float FOC_target_current  = 0; // 目标电流
float FOC_target_velocity = 0.0f; // 目标速度
float FOC_target_position = 0.0f; // 目标位置

/* 范围限制 */
float FOC_target_velocity_upper_limit = 200.0f; // 目标速度上限
float FOC_target_velocity_lower_limit = -200.0f; // 目标速度下限
float FOC_target_current_upper_limit  = 10.0f; // 目标电流上限
float FOC_target_current_lower_limit  = -10.0f; // 目标电流下限
float FOC_target_voltage_upper_limit  = 10; // 目标电压上限
float FOC_target_voltage_lower_limit  = -10; // 目标电压下限

/* 运行参数 */
float Ia, Ib, Ic;
float Id, Iq;
float Uq, Ud;

/* 通信参数 */
uint8_t FOC_CAN_driver_ID = 1; // 驱动器编号 1~4