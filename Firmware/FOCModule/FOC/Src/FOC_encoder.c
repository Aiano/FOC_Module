/**
 * @file FOC_encoder.c
 * @brief 编码器函数接口
 * @details 对不同型号的编码器抽象出统一的接口
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_encoder.h"
#include "FOC_setting.h"
#include "FOC_utility.h"
#include "AS5600.h"

float FOC_mechanical_angle; // 机械角度
float FOC_electrical_angle; // 电角度

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

float FOC_encoder_read_mechanical_angle() {
    switch (FOC_encoder_type) {
        case FOC_ENCODER_NONE:
            break;
        case FOC_ENCODER_AS5600:
            return FOC_encoder_read_raw_data()  / 4096 * _2PI;
        case FOC_ENCODER_AS5047:
            break;
        case FOC_ENCODER_AS5048:
            break;
    }
}

void FOC_encoder_compute_electrical_angle() {
    FOC_mechanical_angle = FOC_encoder_read_mechanical_angle();
    FOC_electrical_angle = _normalizeAngle((FOC_mechanical_angle - FOC_mechanical_angle_offset) * FOC_pole_pairs);
}