#ifndef FOCMODULE_FOC_ENCODER_H
#define FOCMODULE_FOC_ENCODER_H

typedef enum {
    FOC_ENCODER_NONE,
    FOC_ENCODER_AS5600,
    FOC_ENCODER_AS5047,
    FOC_ENCODER_AS5048
} FOC_ENCODER_TYPE;

extern float FOC_mechanical_angle; // 机械角度
extern float FOC_last_mechanical_angle; // 上一次的机械角度
extern float FOC_electrical_angle; // 电角度
extern float FOC_velocity; // 速度

void FOC_encoder_init();

float FOC_encoder_read_raw_data();

float FOC_encoder_read_mechanical_angle();

void FOC_encoder_compute_electrical_angle();

void FOC_encoder_compute_velocity();

#endif //FOCMODULE_FOC_ENCODER_H
