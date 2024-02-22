#ifndef FOCMODULE_FOC_COMM_H
#define FOCMODULE_FOC_COMM_H

#include "main.h"

void FOC_comm_init();
void FOC_comm_parse_command(uint8_t *buf, uint32_t *len);

#endif //FOCMODULE_FOC_COMM_H
