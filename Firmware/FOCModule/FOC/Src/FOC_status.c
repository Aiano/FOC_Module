#include "FOC_status.h"
#include "FOC_utility.h"
#include "FOC_setting.h"
#include "adc.h"
#include "tim.h"

uint16_t adc_buffer[2];


void FOC_status_init()
{
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_buffer, 1);
    HAL_TIM_Base_Start(&htim3);
}

void FOC_status_process()
{
    float R1;

    // 获取PCB温度
    R1 = FOC_temp_upper_R / ( FOC_temp_sample_max / (float)adc_buffer[0] - 1);
    FOC_PCB_temp = _NTCTemp(FOC_temp_B, R1, FOC_temp_R2, FOC_temp_T2);
}