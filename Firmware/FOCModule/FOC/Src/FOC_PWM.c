/**
 * @file FOC_PWM.c
 * @brief 产生PWM波的底层文件
 * @details
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_PWM.h"
#include "FOC_setting.h"
#include "tim.h"

/**
 * @brief TIM初始化
 */
void FOC_tim_init()
{
    // 初始化TIM互补通道产生PWM
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // 通道四用于触发ADC注入组采样

    HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);

    // 设置默认的占空比
    FOC_set_duty_cycle(0.5f, 0.5f, 0.5f);
}

/**
 * @brief 设置PWM占空比
 * @details 范围 [0.0 ~ 1.0],如果超过范围会返回FOC_ERROR
 * @param duty_cycle_A
 * @param duty_cycle_B
 * @param duty_cycle_C
 * @return FOC_STATE
 */
FOC_STATE FOC_set_duty_cycle(float duty_cycle_A, float duty_cycle_B, float duty_cycle_C) {
    // 检查范围
    if (!_checkRange(duty_cycle_A, 1.0f, 0.0f) ||
        !_checkRange(duty_cycle_B, 1.0f, 0.0f) ||
        !_checkRange(duty_cycle_C, 1.0f, 0.0f))
        return FOC_ERROR;

    // 设置PWM占空比
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty_cycle_A * FOC_pwm_max);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty_cycle_B * FOC_pwm_max);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty_cycle_C * FOC_pwm_max);
    return FOC_OK;
}