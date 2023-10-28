#include "FOC_LPF.h"

float FOC_LPF_output(FOC_LPF_TYPE *lpf, float input) {
    float output = lpf->alpha * input + (1 - lpf->alpha) * lpf->last_output;
    lpf->last_output = output;

    return output;
}