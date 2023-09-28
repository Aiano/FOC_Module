/**
 * @file FOC_core.c
 * @brief FOC核心算法
 * @details
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_core.h"
#include "FOC_utility.h"
#include "FOC_setting.h"
#include "FOC_PWM.h"
#include "main.h"
#include "math.h"
#include "arm_math.h"

/**
 * @brief 校准
 * @details 获取机械零度角等
 */
void FOC_calibrate()
{
    FOC_SVPWM(0, 0.5f, 0);
    HAL_Delay(400);
    FOC_mechanical_angle_offset = FOC_encoder_read_mechanical_angle();
    HAL_Delay(100);
    FOC_SVPWM(0, 0, 0);
}

/**
 * @brief 空间矢量脉冲宽度调制（Space Vector Pulse Width Modulation）
 * @details 包含了Park逆变换，dq轴定义参考：https://zhuanlan.zhihu.com/p/641738156
 * @param Uq 交轴（Quadrature Axis, 与转子磁矩垂直的轴）电压
 * @param Ud 直轴（Direct Axis, 与转子磁矩平行的轴）电压
 * @param angle 转子dq坐标系相对于定子αβ坐标系的弧度角
 */
void FOC_SVPWM(float Uq, float Ud, float angle) {
    uint8_t      sector; // 电压矢量所在扇区
    static float Uout;
    static float T0, T1, T2;
    static float Ta, Tb, Tc;

    arm_sqrt_f32(Ud * Ud + Uq * Uq, &Uout); // 合成电压幅值
    Uout /= FOC_supply_voltage; // 归一化

    angle = _normalizeAngle(angle + atan2f(Uq, Ud)); // 转子角度＋电压矢量角度，这里相当于逆Park变换

    sector = (uint8_t) floorf(angle / _PI_3) + 1; // 计算合成电压矢量所在扇区

    // 计算两相邻基础矢量的作用时间
    T1 = _SQRT3 * arm_sin_f32((float) sector * _PI_3 - angle) * Uout;
    T2 = _SQRT3 * arm_sin_f32(angle - ((float) sector - 1.0f) * _PI_3) * Uout;
    if (T1 + T2 > 1) { // 当合成矢量落在基础矢量六边形内切圆之外时，将会发生过调制，此时波形会失真
        // 等比例缩小
        T1 = T1 / (T1 + T2);
        T2 = 1 - T1;
        T0 = 0;
    } else { // 输出的最大不失真电压为(√3/3)*Usupply
        T0 = 1 - T1 - T2;
    }

    switch (sector) {
        case 1:
            Ta = T1 + T2 + T0 / 2;
            Tb = T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 2:
            Ta = T1 + T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 3:
            Ta = T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T2 + T0 / 2;
            break;
        case 4:
            Ta = T0 / 2;
            Tb = T1 + T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 5:
            Ta = T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 6:
            Ta = T1 + T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T0 / 2;
            break;
        default:
            // 错误状态
            Ta = 0;
            Tb = 0;
            Tc = 0;
    }

    FOC_set_duty_cycle(Ta, Tb, Tc);
}

/**
 * @brief 电压环
 * @details 控制SVPWM的Uq电压
 * @param target_voltage
 */
void FOC_voltage_loop(float target_voltage) {
    // 读取角度
    FOC_encoder_compute_electrical_angle();
    FOC_SVPWM(target_voltage, 0, FOC_electrical_angle);
}
