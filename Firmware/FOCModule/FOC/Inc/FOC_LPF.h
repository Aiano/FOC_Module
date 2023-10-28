#ifndef FOCMODULE_FOC_LPF_H
#define FOCMODULE_FOC_LPF_H

typedef struct {
    float alpha;
    float last_output;
}FOC_LPF_TYPE;

float FOC_LPF_output(FOC_LPF_TYPE *lpf, float input);

#endif //FOCMODULE_FOC_LPF_H
