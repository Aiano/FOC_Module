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
#include "FOC_current_sense.h"
#include "FOC_encoder.h"
#include "FOC_PWM.h"
#include "FOC_scope.h"
#include "FOC_PID.h"
#include "FOC_LPF.h"
#include "main.h"
#include "math.h"
#include "arm_math.h"
#include "usbd_cdc_if.h"

/**
 * @brief 校准
 * @details 获取机械零度角等
 */
void FOC_calibrate() {
//    FOC_SVPWM(0, 0.5f, 0);
//    HAL_Delay(400);
//    FOC_mechanical_angle_offset = FOC_encoder_read_mechanical_angle();
//    HAL_Delay(100);
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
 * @brief Clarke变换
 * @param Ia
 * @param Ib
 * @param Ic
 * @param Ialpha
 * @param Ibeta
 */
void FOC_Clarke_Transform(float Ia, float Ib, float Ic, float *Ialpha, float *Ibeta) {
    static float mid, a, b;

    mid = (1.f / 3) * (Ia + Ib + Ic);
    a   = Ia - mid;
    b   = Ib - mid;
    *Ialpha = a;
    *Ibeta  = _1_SQRT3 * a + _2_SQRT3 * b;
}

/**
 * @brief Park变换
 * @param Ialpha
 * @param Ibeta
 * @param angle
 * @param _Id
 * @param _Iq
 */
void FOC_Park_Transform(float Ialpha, float Ibeta, float angle, float *_Id, float *_Iq) {
    static float ct, st;

    ct = arm_cos_f32(angle);
    st = arm_sin_f32(angle);
    *_Id = Ialpha * ct + Ibeta * st;
    *_Iq = Ibeta * ct - Ialpha * st;
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

/**
 * @brief 电流环
 * @details 在ADC转换完成中断中调用本函数
 * @param target_current
 */
void FOC_current_loop(float target_current) {
    static float Ialpha, Ibeta;

//    // 读取角度
//    FOC_encoder_compute_electrical_angle();

    // 读取电流
    FOC_cs_read_current(&Ia, &Ib, &Ic);
    FOC_Clarke_Transform(Ia, Ib, Ic, &Ialpha, &Ibeta);
    FOC_Park_Transform(Ialpha, Ibeta, FOC_electrical_angle, &Id, &Iq);

    // PID
    Ud = FOC_PID_get_u(&pid_current_d, 0, Id);
    Uq = FOC_PID_get_u(&pid_current_q, target_current, Iq);

    // SVPWM
    FOC_SVPWM(Uq, Ud, FOC_electrical_angle);

    // 探测变量
//    FOC_scope_probe_float(0.1f * Id + 0.5f, FOC_SCOPE_DAC1);
//    FOC_scope_probe_float(0.1f * Iq + 0.5f, FOC_SCOPE_DAC2);
}

/**
 * @brief 速度环
 * @param target_velocity
 */
void FOC_velocity_loop(float target_velocity) {
    // TODO: 提高速度环频率，频率越高振动越小
    FOC_encoder_compute_velocity(); // 计算速度
    FOC_velocity = FOC_LPF_output(&lpf_velocity, FOC_velocity);

    FOC_target_current = FOC_PID_get_u(&pid_velocity, target_velocity, FOC_velocity);

    //CDC_printf("%.3f,%.3f,%.3f\n", FOC_velocity, target_velocity, FOC_target_current);
}

/**
 * @brief 位置环
 * @param target_position
 */
void FOC_position_loop(float target_position) {
    static float target_velocity;
    static float angle_error;

    FOC_encoder_compute_velocity(); // 计算速度，同时获得机械角度和电角度
    FOC_velocity = FOC_LPF_output(&lpf_velocity, FOC_velocity);

    FOC_mechanical_angle = _normalizeAngle(FOC_mechanical_angle);

    angle_error = target_position - FOC_mechanical_angle;
    if (angle_error < -_PI) target_position += _2PI;
    else if (angle_error > _PI) target_position -= _2PI;

    target_velocity = FOC_PID_get_u(&pid_position, target_position, FOC_mechanical_angle);
    FOC_target_current = FOC_PID_get_u(&pid_velocity, target_velocity, FOC_velocity);

    //CDC_printf("%.3f,%.3f,%.3f\n", FOC_velocity, target_velocity, FOC_target_current);
}

/**
 * @brief 主循环
 * @details 在主循环中调用本函数
 */
void FOC_main_loop() {
    // 模式选择
    switch (FOC_mode) {
        case FOC_MODE_VOLTAGE:
            FOC_voltage_loop(FOC_target_voltage);
            break;
        case FOC_MODE_CURRENT:
            FOC_encoder_compute_electrical_angle();
            break;
        case FOC_MODE_VELOCITY:
            FOC_velocity_loop(FOC_target_velocity);
            break;
        case FOC_MODE_POSITION:
            FOC_position_loop(FOC_target_position);
            break;
    }

    // 参数输出
    // Ia, Ib, Ic, Id, Iq, target_Id, target_Iq, Ud, Uq, velocity, target_velocity, position, target_position
    //CDC_printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", Ia, Ib, Ic, Id, Iq, 0.0f, FOC_target_current);
    //CDC_printf("%.2f,%.2f\n", FOC_velocity ,FOC_target_velocity);
}

/**
 * @brief 电流中断回调函数
 * @details 在ADC转换完成回调函数中调用本函数
 */
void FOC_current_callback() {
    switch (FOC_mode) {
        case FOC_MODE_VOLTAGE: // 电压环无需采样电流
            break;
        case FOC_MODE_CURRENT: // 电流环、速度环和位置环都基于电流环
        case FOC_MODE_VELOCITY:
        case FOC_MODE_POSITION:
            FOC_current_loop(FOC_target_current);
            break;
    }
}