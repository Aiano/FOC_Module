/**
 * @file FOC_utility.c
 * @brief FOC工具函数
 * @details
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_utility.h"
#include "math.h"
#include "tim.h"

void FOC_utility_init()
{
    HAL_TIM_Base_Start(&htim2);
}

/**
 * @brief 检查范围
 * @details 检查变量是否在范围内（包含上界和下界）
 * @param value 被检查的变量
 * @param upper_limit 上界
 * @param lower_limit 下界
 * @return FOC_STATE
 */
FOC_STATE _checkRange(float value, float upper_limit, float lower_limit) {
    if (value > upper_limit || value < lower_limit) return FOC_ERROR;
    return FOC_OK;
}

/**
 * @brief 快速平方根算法
 * @details square root approximation function using
 * @details https://reprap.org/forum/read.php?147,219210
 * @details https://en.wikipedia.org/wiki/Fast_inverse_square_root
 * @param number 被开方数
 * @return 开方结果
 */
float _sqrtApprox(float number) {//low in fat
    long  i;
    float y;
    // float x;
    // const float f = 1.5F; // better precision

    // x = number * 0.5F;
    y = number;
    i = *(long *) &y;
    i = 0x5f375a86 - (i >> 1);
    y = *(float *) &i;
    // y = y * ( f - ( x * y * y ) ); // better precision
    return number * y;
}

/**
 * @brief 标准化角度为[0, 2PI]
 * @param angle
 * @return
 */
float _normalizeAngle(float angle) {
    float a = fmodf(angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}

/**
 * @brief 获取两次调用之间的时间跨度，单位微秒
 * @return 时间跨度，32位无符号
 */
uint32_t _spanMicro() {
    static uint32_t count;
    count = __HAL_TIM_GET_COUNTER(&htim2);
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    return count;
}

/**
 * @brief 获取NTC热敏电阻温度
 * @param B
 * @param R1 Ω
 * @param R2 Ω
 * @param T2 ℃
 * @return T1 ℃
 */
float _NTCTemp(float B, float R1, float R2, float T2){
    return 1.0f / ( 1.0f / B * logf( R1 / R2 ) + 1.0f / ( T2 + 273.15f)) - 273.15f;
}