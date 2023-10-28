#include "FOC_current_sense.h"
#include "FOC_setting.h"
#include "FOC_core.h"
#include "FOC_encoder.h"
#include "FOC_scope.h"
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "dac.h"

static uint16_t ADC_value[3];

/**
 * @brief 电流采样初始化
 */
void FOC_cs_init()
{
    HAL_ADCEx_InjectedStart_IT(&hadc1); // 启动ADC注入组，同时开启ADC中断

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, FOC_sample_duty_cycle); // 使用产生PWM的TIM的通道4的捕获比较事件作为ADC注入组的外部触发源，调节占空比，可以设置采样的位置
}

/**
 * @brief ADC注入组转换完成回调函数
 * @param hadc
 */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // 翻转IO便于调试
    HAL_GPIO_WritePin(ADC_INT_FLAG_GPIO_Port, ADC_INT_FLAG_Pin, GPIO_PIN_SET);

    // 读取ADC数据
    ADC_value[0] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
    ADC_value[1] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2);
    ADC_value[2] = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_3);

    // 探测变量
    FOC_scope_probe(ADC_value[0], FOC_SCOPE_DAC1);
    FOC_scope_probe(ADC_value[2], FOC_SCOPE_DAC2);

    // 电流环
//    FOC_current_loop(FOC_target_current);
    FOC_current_callback();

    // 翻转IO便于调试
    HAL_GPIO_WritePin(ADC_INT_FLAG_GPIO_Port, ADC_INT_FLAG_Pin, GPIO_PIN_RESET);
}


void FOC_cs_read_current(float *Ia, float *Ib, float *Ic)
{
    static float ratio;
    ratio = FOC_sample_ref_voltage / FOC_sample_max / FOC_INA_gain / FOC_shunt_resistance;
    FOC_sample_mid = (float)(ADC_value[0] + ADC_value[1] + ADC_value[2]) / 3;

    *Ia = ((float)ADC_value[0] - FOC_sample_offset[0]) * ratio;
    *Ib = ((float)ADC_value[1] - FOC_sample_offset[1]) * ratio;
    *Ic = ((float)ADC_value[2] - FOC_sample_offset[2]) * ratio;

//    *Ia = ((float)ADC_value[0] - FOC_sample_mid) * ratio;
//    *Ib = ((float)ADC_value[1] - FOC_sample_mid) * ratio;
//    *Ic = ((float)ADC_value[2] - FOC_sample_mid) * ratio;

//    *Ib = ((float)ADC_value[1] - FOC_sample_offset[1]) * ratio;
//    *Ic = ((float)ADC_value[2] - FOC_sample_offset[2]) * ratio;
//    *Ia = - *Ib - *Ic;
}