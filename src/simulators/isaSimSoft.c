#include <stdio.h>
#include <assert.h>
#include "isaSimSoft.h"
#include "rv32i.h"

#define ERROR_MESSAGE_MAX_LENGTH (100)

typedef struct inputRegs_t
{
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
} inputRegs_t;

/*** Static function prototypes ***/
static int32_t instructionFetch(uint32_t pc, const uint8_t* const prog);
static void readInputRegisters(int32_t instruct, inputRegs_t* inputRegs);
static uint32_t instructionExecute(enum rv32i_instruct_t instrType, inputRegs_t* inputRegs, int32_t regFile[32], int32_t imm, uint32_t pc);

int8_t isaSimSoftRun(uint8_t *prog, uint32_t progSize, int32_t regFile[32], int8_t verbosity)
{
    uint32_t pc = 0;
    int32_t instruction = 0;
    int32_t imm = 0;
    inputRegs_t inputRegs = {};
    enum rv32i_instruct_t instructType;
    //char errMess[ERROR_MESSAGE_MAX_LENGTH] = {};

    while (pc < progSize)
    {
        /* IF: Instruction Fetch */
        instruction = instructionFetch(pc, prog);
        pc += 4;

        /* ID: Instruction Decode*/
        instructType = rv32iDecodeInstructType(instruction);
        if (instructType == RV32I_NOT_SUPPORTED)
        {
            fprintf(stderr, "SoftSim error: Decoder encountered unsuported instruction 0x%08x at PC = %d\n", instruction, (pc-4));
            return -1; // TODO: Reconsider error handling at unsuported instruction
        }
        readInputRegisters(instruction, &inputRegs);
        imm = rv32iGenerateImmediate(instruction);

        /* EX, MEM, WB: Execute, Memory, Write back */
        pc = instructionExecute(instructType, &inputRegs, regFile, imm, pc);

    }

    return 0;
}

int32_t instructionFetch(uint32_t pc, const uint8_t *const prog)
{
    // TODO: Consider evaluating endianess in the pre-processor and compile the appropiate variant code
    // Big-endian target architecture
    //return (prog[pc] << 24 | prog[pc+1] << 16 | prog[pc+2] << 8 | prog[pc+3]);

    // Little-endian target architecture
    return (prog[pc] | prog[pc+1] << 8 | prog[pc+2] << 16 | prog[pc+3] << 24);
}

void readInputRegisters(int32_t instruct, inputRegs_t* inputRegs)
{
    inputRegs->rs1 = rv32iGetRs1(instruct);
    inputRegs->rs2 = rv32iGetRs2(instruct);
    inputRegs->rd  = rv32iGetRd( instruct);
}

uint32_t instructionExecute(enum rv32i_instruct_t instrType, inputRegs_t* inputRegs, int32_t regFile[32], int32_t imm, uint32_t pc)
{
    uint8_t rd  = inputRegs->rd;
    uint8_t rs1 = inputRegs->rs1;
    uint8_t rs2 = inputRegs->rs2;
    switch (instrType)
    {
    case RV32I_ADD:
        regFile[rd] = regFile[rs1] + regFile[rs2];
        break;
    case RV32I_SUB:
        regFile[rd] = regFile[rs1] - regFile[rs2];
        break;
    case RV32I_SLL:
        regFile[rd] = regFile[rs1] << regFile[rs2];
        break;
    case RV32I_SLT:
        regFile[rd] = (regFile[rs1] < regFile[rs2]) ? 1 : 0;
        break;
    case RV32I_SLTU:
        regFile[rd] = ( (uint32_t) regFile[rs1] < (uint32_t) regFile[rs2]) ? 1 : 0;
        break;
    case RV32I_XOR:
        regFile[rd] = regFile[rs1] ^ regFile[rs2];
    case RV32I_SRL:
        regFile[rd] = regFile[rs1] >> regFile[rs2];
        break;
    default:
        // TODO: How to handle unknown instruction? NOP? Emulate exception by exiting?
        break;
    };

    regFile[0] = 0; // x0 is hardwired to 0. Instructions changing it are effectively NOPs.

    return pc;
}
