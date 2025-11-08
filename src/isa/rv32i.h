#ifndef RV32I_H
#define RV32I_H
#include <stdint.h>

/*
Collection of defines and functionalities generally useable across multiple types of RISC-V RV32I instruction set implementations.
*/

/*
RV32I Base Instruction Set opcodes as given in The RISC-V Instruction Set Manual
Volume I, Version 20250508, Chapter 35: RV32/64G Instruction Set Listings.
*/
#define RV32I_OPCODE_BRANCH     (0b1100011)
#define RV32I_OPCODE_LUI        (0b0110111)
#define RV32I_OPCODE_JAL        (0b1101111)
#define RV32I_OPCODE_JALR       (0b1100111)
#define RV32I_OPCODE_LOAD       (0b0000011)
#define RV32I_OPCODE_STORE      (0b0100011)
#define RV32I_OPCODE_ALU_IMM    (0b0010011)
#define RV32I_OPCODE_ALU        (0b0110011)
#define RV32I_OPCODE_FENCE      (0b0110011)
#define RV32I_OPCODE_ENV        (0b1110011)

/* All supported rv32i instructions including 3 formats for unsupported instructions to differentiate between instructions where funct3 and funct7
should be evaluated for error handling. */
typedef enum rv32i_instruct_t
{
    RV32I_NOT_SUPPORTED = -1, RV32I_ADD = 0, RV32I_ADDI, RV32I_AND, RV32I_ANDI, RV32I_AUIPC, RV32I_BEQ, RV32I_BGE, RV32I_BGEU, RV32I_BLT,
    RV32I_BLTU, RV32I_BNE, RV32I_ECALL, RV32I_JAL, RV32I_JALR, RV32I_LB, RV32I_LBU, RV32I_LH, RV32I_LHU, RV32I_LUI, RV32I_LW, RV32I_OR,
    RV32I_ORI, RV32I_SB, RV32I_SH, RV32I_SLL, RV32I_SLLI, RV32I_SLT, RV32I_SLTI, RV32I_SLTIU, RV32I_SLTU, RV32I_SRA, RV32I_SRAI, RV32I_SRL,
    RV32I_SRLI, RV32I_SUB, RV32I_SW, RV32I_XOR, RV32I_XORI,
} rv32i_instruct_t;

typedef enum rv32i_opcodeTypes_t
{
    RV32I_OPCODE_TYPE_UNKNOWN = -1, RV32I_OPCODE_TYPE_R = 0, RV32I_OPCODE_TYPE_I, RV32I_OPCODE_TYPE_S,
    RV32I_OPCODE_TYPE_B, RV32I_OPCODE_TYPE_U, RV32I_OPCODE_TYPE_J
} rv32i_opcodeTypes_t;


enum rv32i_instruct_t rv32iDecodeInstructType(int32_t instruct);
int32_t rv32iGenerateImmediate(int32_t instruct);
uint8_t rv32iGetFunct3(int32_t instruct);
uint8_t rv32iGetFunct7(int32_t instruct);
uint8_t rv32iGetOpcode(int32_t instruct);
uint8_t rv32iGetRd (int32_t instruct);
uint8_t rv32iGetRs1(int32_t instruct);
uint8_t rv32iGetRs2(int32_t instruct);
enum rv32i_opcodeTypes_t rv32iOpcodeToOpcodeType (uint8_t opcode);



#endif //RV32I_H