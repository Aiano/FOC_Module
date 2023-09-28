#ifndef FOCMODULE_FOC_SCOPE_H
#define FOCMODULE_FOC_SCOPE_H

#include "main.h"

typedef enum{
    FOC_SCOPE_DAC1 = 1,
    FOC_SCOPE_DAC2,
    FOC_SCOPE_UART
}FOC_SCOPE_CHANNEL;

void FOC_scope_init();
void FOC_scope_probe(uint16_t variable, FOC_SCOPE_CHANNEL channel);
void FOC_scope_probe_float(float variable, FOC_SCOPE_CHANNEL channel);

#endif //FOCMODULE_FOC_SCOPE_H
