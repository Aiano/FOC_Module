/**
 * @file FOC_encoder.c
 * @brief 编码器函数接口
 * @details 对不同型号的编码器抽象出统一的接口
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include <math.h>
#include "FOC_encoder.h"
#include "FOC_setting.h"
#include "FOC_utility.h"
#include "AS5600.h"
#include "usbd_cdc_if.h"

float FOC_mechanical_angle; // 机械角度
float FOC_last_mechanical_angle; // 上一次的机械角度
float FOC_electrical_angle; // 电角度
float FOC_velocity; // 速度

void FOC_encoder_init()
{
    FOC_last_mechanical_angle = FOC_encoder_read_mechanical_angle() - FOC_mechanical_angle_offset;
}

/**
 * @brief 读取编码器的原始数据
 * @return
 */
float FOC_encoder_read_raw_data()
{
    switch (FOC_encoder_type) {
        case FOC_ENCODER_NONE:
            break;
        case FOC_ENCODER_AS5600:
            return AS5600_ReadSensorRawData();
        case FOC_ENCODER_AS5047:
            break;
        case FOC_ENCODER_AS5048:
            break;
    }
}

/**
 * @brief 读取机械角度
 * @details 根据编码器位数，将编码器原始数据转换为弧度制
 * @return
 */
float FOC_encoder_read_mechanical_angle() {
    switch (FOC_encoder_type) {
        case FOC_ENCODER_NONE:
            break;
        case FOC_ENCODER_AS5600:
            return FOC_encoder_read_raw_data() * _2PI / 4096 ;
        case FOC_ENCODER_AS5047:
            break;
        case FOC_ENCODER_AS5048:
            break;
    }
}

/**
 * @brief 计算电角度
 */
void FOC_encoder_compute_electrical_angle() {
    FOC_mechanical_angle = FOC_encoder_read_mechanical_angle() - FOC_mechanical_angle_offset;
    FOC_electrical_angle = _normalizeAngle(FOC_encoder_direction * FOC_mechanical_angle * FOC_pole_pairs);
}

/**
 * @brief 计算速度，同时获得电角度和机械角度
 */
void FOC_encoder_compute_velocity() {
    static float dt;
    static float delta_angle;
    FOC_encoder_compute_electrical_angle(); // 计算电角度（同时获取机械角度）

    dt = (float)_spanMicro() * 0.000001f; // 跨度时间，单位秒
    delta_angle = FOC_mechanical_angle - FOC_last_mechanical_angle;
    FOC_last_mechanical_angle = FOC_mechanical_angle; // 记录角度，供下次使用

    if (fabsf(delta_angle) > _PI)
    {
        if (delta_angle > 0)
        {
            delta_angle -= _2PI;
        }
        else
        {
            delta_angle += _2PI;
        }
    }

    FOC_velocity = - 0.5f * delta_angle / dt; // 计算速度

//    CDC_printf("%.4f,%.4f,%.4f\n",dt,delta_angle,FOC_velocity);
}
