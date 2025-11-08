#ifndef ISA_SIM_SOFT_H
#define ISA_SIM_SOFT_H
#include <stdint.h>

int8_t isaSimSoftRun(uint8_t* prog, uint32_t progSize, int32_t regFile[32], int8_t verbosity);

/*** Instruction stages ***/
/*
Possible expansion:
Organise the simulation into 5 stages. This will result in a simulator architecture closer to that
of P&H Chapter 4, and simplify a later expansion into a 5 stage pipeline.
This will, however, be at the cost of implementing an actual data and control flow

stageIF
stageID
stageEX
stageMEM
stageWB
*/




#endif // ISA_SIM_SOFT_H