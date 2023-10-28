#ifndef FOCMODULE_FOC_UTILITY_H
#define FOCMODULE_FOC_UTILITY_H

#include "main.h"

// utility defines
#define _2_SQRT3 1.15470053838f
#define _SQRT3 1.73205080757f
#define _1_SQRT3 0.57735026919f
#define _SQRT3_2 0.86602540378f
#define _SQRT2 1.41421356237f
#define _120_D2R 2.09439510239f
#define _PI 3.14159265359f
#define _PI_2 1.57079632679f
#define _PI_3 1.0471975512f
#define _2PI 6.28318530718f
#define _3PI_2 4.71238898038f
#define _PI_6 0.52359877559f
#define _RPM_TO_RADS 0.10471975512f
#define _RADIAN_TO_DEGREE 57.295779513f


/**
 * @brief 返回状态
 */
typedef enum{
    FOC_ERROR = 0,
    FOC_OK
}FOC_STATE;

void FOC_utility_init();
FOC_STATE _checkRange(float value, float upper_limit, float lower_limit);
float _normalizeAngle(float angle);
uint32_t _spanMicro();

#endif //FOCMODULE_FOC_UTILITY_H
