#include "FOC_PID.h"
#include "math.h"

/**
 * @brief 获取pid的控制量u
 * @param pid
 * @param target
 * @param real
 * @return u
 */
float FOC_PID_get_u(FOC_PID_TYPE *pid, float target, float real) {
    float error = target - real; // 误差

    if (fabs(error) < pid->dead_zone) error = 0; // 死区

    pid->integral += error; // 积分误差
//    if (fabs(pid->integral) < pid->dead_zone) pid->integral = 0; // 死区

    if (pid->integral > pid->max_integral) { // 设置积分误差的上下限，防止过大或过小
        pid->integral = pid->max_integral;
    } else if (pid->integral < pid->min_integral) {
        pid->integral = pid->min_integral;
    }

    double differential = error - pid->last_error; // 差分误差
//    if (fabs(differential) < pid->dead_zone) differential = 0; // 死区

    double u = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * differential;  // 控制量
    if (u > pid->max_u) { // 设置控制量u的上下限，防止过大或过小
        u = pid->max_u;
    } else if (u < pid->min_u) {
        u = pid->min_u;
    }

    pid->last_error = error; // 记录下本轮误差供下次差分使用

    return u;
}