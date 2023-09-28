#ifndef FOCMODULE_FOC_PID_H
#define FOCMODULE_FOC_PID_H

typedef struct {
    float Kp;
    float Ki;
    float Kd;

    float integral;
    float last_error;

    float max_integral;
    float min_integral;

    float max_u;
    float min_u;

    float dead_zone;
} FOC_PID_TYPE;

extern float FOC_PID_get_u(FOC_PID_TYPE *pid, float target, float real);

#endif //FOCMODULE_FOC_PID_H
