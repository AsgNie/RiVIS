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

typedef enum execute_return_values_t
{
    EXECUTE_UNKNOWN = -1, EXECUTE_OK = 0, EXECUTE_ECALL_EXIT, EXECUTE_ECALL_UNSUPORTED,
} execute_return_values_t;

/*** Static function prototypes ***/
static int32_t instructionFetch(uint32_t pc, const uint8_t* const prog);
static void readInputRegisters(int32_t instruct, inputRegs_t* inputRegs);
static enum execute_return_values_t instructionExecute(enum rv32i_instruct_t instrType, inputRegs_t* inputRegs, int32_t regFile[32], uint8_t *prog, int32_t imm, uint32_t* pcPtr);
static void printRegisterFile(int32_t regFile[32]);

int8_t isaSimSoftRun(uint8_t *prog, uint32_t progSize, int32_t regFile[32], int8_t verbosity)
{
    uint32_t pc = 0;
    int32_t instruction = 0;
    int32_t imm = 0;
    inputRegs_t inputRegs = {};
    enum rv32i_instruct_t instructType;
    enum execute_return_values_t executeReturnVal;

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

        if (verbosity)
        {
            fprintf(stderr, ">>>SoftSim: Instruction type %d with value 0x%08x at PC = %d\n",(uint8_t) instructType, instruction, (pc-4));
            fprintf(stderr, "imm = %d\n", imm);
        }

        /* EX, MEM, WB: Execute, Memory, Write back */
        executeReturnVal = instructionExecute(instructType, &inputRegs, regFile, prog, imm, &pc);
        switch (executeReturnVal)
        {
        case EXECUTE_OK:
            break;
        case EXECUTE_ECALL_EXIT:
            if (verbosity)
            {
                fprintf(stderr, "SoftSim: ECALL exit at PC = %d\n", (pc-4));
            }
            return 0;
        case EXECUTE_ECALL_UNSUPORTED:
            fprintf(stderr, "SoftSim error: Unsuported ECALL with argument a7 = %d at PC = %d\n", regFile[17], (pc-4));
            return -1;
        case EXECUTE_UNKNOWN: // Fallthrough
        default:
            fprintf(stderr, "SoftSim error: Unknown instructExecute command at PC = %d\n", (pc-4));
            assert(0); // Should not exist
            return -1;
        }
        if (verbosity)
        {
            printRegisterFile(regFile);
        }
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

// TODO: Consider making regFile static variable in this file and have a copy function to return it to caller
enum execute_return_values_t instructionExecute(enum rv32i_instruct_t instrType, inputRegs_t* inputRegs, int32_t regFile[32], uint8_t *prog, int32_t imm, uint32_t* pcPtr)
{
    uint8_t rd  = inputRegs->rd;
    uint8_t rs1 = inputRegs->rs1;
    uint8_t rs2 = inputRegs->rs2;
    enum execute_return_values_t returnVal = EXECUTE_OK;
    switch (instrType)
    {
    // ALU operations
    case RV32I_ADD:
        regFile[rd] = regFile[rs1] + regFile[rs2];
        break;
    case RV32I_SUB:
        regFile[rd] = regFile[rs1] - regFile[rs2];
        break;
    case RV32I_SLL: // Shift with lower 5 bits of rs2
        regFile[rd] = regFile[rs1] << (regFile[rs2] & 0b00000000000000000000000000011111);
        break;
    case RV32I_SLT:
        regFile[rd] = (regFile[rs1] < regFile[rs2]) ? 1 : 0;
        break;
    case RV32I_SLTU:
        regFile[rd] = ( (uint32_t) regFile[rs1] < (uint32_t) regFile[rs2]) ? 1 : 0;
        break;
    case RV32I_XOR:
        regFile[rd] = regFile[rs1] ^ regFile[rs2];
        break;
    case RV32I_SRL: // Shift with lower 5 bits of rs2
        regFile[rd] = (uint32_t) regFile[rs1] >> (regFile[rs2] & 0b00000000000000000000000000011111);
        break;
    case RV32I_SRA: // Shift with lower 5 bits of rs2
        regFile[rd] = regFile[rs1] >> (regFile[rs2] & 0b00000000000000000000000000011111);
        break;
    case RV32I_OR:
        regFile[rd] = regFile[rs1] | regFile[rs2];
        break;
    case RV32I_AND:
        regFile[rd] = regFile[rs1] & regFile[rs2];
        break;
    // ALU immediate operations
    case RV32I_ADDI:
        regFile[rd] = regFile[rs1] + imm;
        break;
    case RV32I_SLTI:
        regFile[rd] = (regFile[rs1] < imm) ? 1 : 0;
        break;
    case RV32I_SLTIU:
        regFile[rd] = ( (uint32_t) regFile[rs1] < (uint32_t) imm) ? 1 : 0;
        break;
    case RV32I_XORI:
        regFile[rd] = regFile[rs1] ^ imm;
        break;
    case RV32I_ORI:
        regFile[rd] = regFile[rs1] | imm;
        break;
    case RV32I_ANDI:
        regFile[rd] = regFile[rs1] & imm;
        break;
    case RV32I_SLLI: // Shift with lower 5 bits of imm
        regFile[rd] = regFile[rs1] << (imm & 0b00000000000000000000000000011111);
        break;
    case RV32I_SRLI: // Shift with lower 5 bits of imm
        regFile[rd] = (uint32_t) regFile[rs1] >> (imm & 0b00000000000000000000000000011111);
        break;
    case RV32I_SRAI: // Shift with lower 5 bits of imm
        regFile[rd] = regFile[rs1] >> (imm & 0b00000000000000000000000000011111);
        break;
    // Branch operations
    case RV32I_BEQ:
        if (regFile[rs1] == regFile[rs2])
        {
            *pcPtr = (*pcPtr - 4) + imm;
        }
        break;
    case RV32I_BNE:
        if (regFile[rs1] != regFile[rs2])
        {
            *pcPtr = (*pcPtr - 4) + imm;
        }
        break;
    case RV32I_BLT: // TODO: Double check this
        if (regFile[rs1] < regFile[rs2])
        {
            *pcPtr = (*pcPtr - 4) + imm;
        }
        break;
    case RV32I_BGE:
        if (regFile[rs1] >= regFile[rs2])
        {
            *pcPtr = (*pcPtr - 4) + imm;
        }
        break;
    case RV32I_BLTU:
        if ( (uint32_t) regFile[rs1] < (uint32_t) regFile[rs2])
        {
            *pcPtr = (*pcPtr - 4) + imm;
        }
        break;
    case RV32I_BGEU:
        if ((uint32_t) regFile[rs1] >= (uint32_t) regFile[rs2])
        {
            *pcPtr = (*pcPtr - 4) + imm;
        }
        break;
    // Environment operations
    case RV32I_ECALL:
        if (regFile[17] == 10) // ECALL exit at a7 = 10 defined in assignment specification
        {
            returnVal = EXECUTE_ECALL_EXIT;
        }
        else
        {
            returnVal = EXECUTE_ECALL_UNSUPORTED;
        }
        break;
    // Jump operations
    case RV32I_JAL:
        regFile[rd] = *pcPtr; // Current instruction + 4
        *pcPtr = (*pcPtr - 4) + imm;
        break;
    case RV32I_JALR:
        regFile[rd] = *pcPtr; // Current instruction + 4
        *pcPtr = (regFile[rs1] + imm) & 0b11111111111111111111111111111110; // Mask away LS bit as specified in RISC-V Instruction Set Manual
        break;
    // Load operations
    case RV32I_LB:
        regFile[rd] = rv32iSignExtentByte( rv32iLoadByte(prog + regFile[rs1] + imm));
        break;
    case RV32I_LH:
        // TODO: Account for endianess - For now little endian is assumed
        regFile[rd] = rv32iSignExtentHalfWord( rv32iLoadHalfWord(prog + regFile[rs1] + imm) );
        break;
    case RV32I_LW:
        regFile[rd] = rv32iLoadWord(prog + regFile[rs1] + imm);
        break;
    case RV32I_LBU:
        regFile[rd] = rv32iLoadByte(prog + regFile[rs1] + imm);
        break;
    case RV32I_LHU:
        regFile[rd] = rv32iLoadHalfWord(prog + regFile[rs1] + imm);
        break;
    // Upper immediates operations
    case RV32I_LUI:
        regFile[rd] = imm;
        break;
    case RV32I_AUIPC:
        regFile[rd] = (*pcPtr-4) + imm;
        break;
    // Store operations
    case RV32I_SB:
        rv32iStoreByte(prog + regFile[rs1] + imm, regFile[rs2]);
        break;
    case RV32I_SH:
        rv32iStoreHalfWord(prog+regFile[rs1]+imm, regFile[rs2]);
        break;
    case RV32I_SW:
        rv32iStoreWord(prog+regFile[rs1]+imm, regFile[rs2]);
        break;
    default:
        returnVal = EXECUTE_UNKNOWN;
        break;
    };

    regFile[0] = 0; // x0 is hardwired to 0. Instructions changing it are effectively NOPs.

    return returnVal;
}

void printRegisterFile(int32_t regFile[32])
{
    for (size_t i = 0; i < 31; i++)
    {
        fprintf(stderr, "x%ld=%d, ", i, regFile[i]);
    }
    fprintf(stderr, "x%d=%d\n", 31, regFile[31]);
}
