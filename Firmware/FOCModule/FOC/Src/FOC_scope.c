/**
 * @file FOC_scope.c
 * @brief 观察运行状态
 * @details 包含了用于监测变量的函数
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_scope.h"
#include "FOC_utility.h"
#include "dac.h"


void FOC_scope_init() {
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
}

void FOC_scope_probe(uint16_t variable, FOC_SCOPE_CHANNEL channel) {
    switch (channel) {
        case FOC_SCOPE_DAC1:
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, variable);
            break;
        case FOC_SCOPE_DAC2:
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, variable);
            break;
        case FOC_SCOPE_UART:
            break;
    }
}

void FOC_scope_probe_float(float variable, FOC_SCOPE_CHANNEL channel) {
    _checkRange(variable, 1.0f, 0.0f);
    switch (channel) {
        case FOC_SCOPE_DAC1:
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(variable * 4096.0f));
            break;
        case FOC_SCOPE_DAC2:
            HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t)(variable * 4096.0f));
            break;
        case FOC_SCOPE_UART:
            break;
    }
}
