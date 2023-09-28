#ifndef FOCMODULE_FOC_CURRENT_SENSE_H
#define FOCMODULE_FOC_CURRENT_SENSE_H

void FOC_cs_init();
void FOC_cs_read_current(float *Ia, float *Ib, float *Ic);

#endif //FOCMODULE_FOC_CURRENT_SENSE_H
