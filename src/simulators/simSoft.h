#ifndef SIM_SOFT_H
#define SIM_SOFT_H
#include <stdint.h>

int8_t simSoftRun(uint8_t* prog, uint32_t progSize, int32_t regFile[32], int8_t verbosity);

#endif // SIM_SOFT_H
