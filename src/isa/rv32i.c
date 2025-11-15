#include "rv32i.h"
#include <assert.h>
#include <stdio.h>

rv32i_instruct_t rv32iDecodeInstructType(int32_t instruct)
{
    uint8_t opcode, funct3, funct7;

    opcode = rv32iGetOpcode(instruct);

    switch (opcode)
    {
    case RV32I_OPCODE_ALU:
        funct3 = rv32iGetFunct3(instruct);
        funct7 = rv32iGetFunct7(instruct);
        switch (funct3)
        {
        case 0b000:
            switch (funct7)
            {
            case 0b0000000:
                return RV32I_ADD;
            case 0b0100000:
                return RV32I_SUB;
            default:
                return RV32I_NOT_SUPPORTED;
            }
        case 0b001:
            return RV32I_SLL;
        case 0b010:
            return RV32I_SLT;
        case 0b011:
            return RV32I_SLTU;
        case 0b100:
            return RV32I_XOR;
        case 0b101:
            switch (funct7)
            {
            case 0b0000000:
                return RV32I_SRL;
            case 0b0100000:
                return RV32I_SRA;
            default:
                return RV32I_NOT_SUPPORTED;
            }
        case 0b110:
            return RV32I_OR;
        case 0b111:
            return RV32I_AND;
        default:
            assert(0); // This condition should be unreachable.
            return RV32I_NOT_SUPPORTED;
        }
        break;
    case RV32I_OPCODE_ALU_IMM:
        funct3 = rv32iGetFunct3(instruct);
        switch (funct3)
        {
        case 0b000:
            return RV32I_ADDI;
        case 0b010:
            return RV32I_SLTI;
        case 0b011:
            return RV32I_SLTIU;
        case 0b100:
            return RV32I_XORI;
        case 0b110:
            return RV32I_ORI;
        case 0b111:
            return RV32I_ANDI;
        default:
            return RV32I_NOT_SUPPORTED;
        }
    default:
        return RV32I_NOT_SUPPORTED;
    }
}

int32_t rv32iGenerateImmediate(int32_t instruct)
{
    int32_t imm = 0;
    uint8_t opcode = rv32iGetOpcode(instruct);
    switch (rv32iOpcodeToOpcodeType(opcode))
    {
    case RV32I_OPCODE_TYPE_I:
        imm  = (instruct >> 20);
        break;
    case RV32I_OPCODE_TYPE_S:
        imm  = (instruct >>  7) & 0b00000000000000000000000000011111;   // imm[4:0]
        imm |= (instruct >> 20) & 0b11111111111111111111111111100000;   // imm[11:5] with sign extension
        break;
    case RV32I_OPCODE_TYPE_B:
        imm  = (instruct >>  7) & 0b00000000000000000000000000011110;   // imm[4:1]
        imm |= (instruct >> 20) & 0b00000000000000000000011111100000;   // imm[10:5];
        imm |= (instruct <<  4) & 0b00000000000000000000100000000000;   // imm[11]
        imm |= (instruct >> 19) & 0b11111111111111111111000000000000;   // imm[12] with sign extension
        break;
    case RV32I_OPCODE_TYPE_U:
        imm  = (instruct      ) & 0b11111111111111111111000000000000;   // imm[31:12]
        break;
    case RV32I_OPCODE_TYPE_J:
        imm  = (instruct >>  0) & 0b00000000000011111111000000000000;   // imm[19:12]
        imm |= (instruct >>  9) & 0b00000000000000000000100000000000;   // imm[11]
        imm |= (instruct >> 20) & 0b00000000000000000000011111111110;   // imm[10:1]
        imm |= (instruct >> 11) & 0b11111111111100000000000000000000;   // imm[20] with sign extension
        break;
    case RV32I_OPCODE_TYPE_R:
        imm  = -1;  // Don't care condition
        break;
    case RV32I_OPCODE_TYPE_UNKNOWN:
        imm  = -2;  // Don't care condition
        break;
    default:
        assert(0); // This condition should be unreachable
        imm  = -3;
        break;
    };

    return imm;
}

uint8_t rv32iGetFunct3(int32_t instruct)
{
    return (instruct >> 12) & 0b00000000000000000000000000000111;
}

uint8_t rv32iGetFunct7(int32_t instruct)
{
    return (instruct >> 25) & 0b00000000000000000000000000000111;
}

uint8_t rv32iGetOpcode(int32_t instruct)
{
    return (instruct >>  0) & 0b00000000000000000000000001111111;
}

uint8_t rv32iGetRd(int32_t instruct)
{
    return (instruct >>  7) & 0b00000000000000000000000000011111;
}

uint8_t rv32iGetRs1(int32_t instruct)
{
    return (instruct >> 15) & 0b00000000000000000000000000011111;
}

uint8_t rv32iGetRs2(int32_t instruct)
{
    return (instruct >> 20) & 0b00000000000000000000000000011111;
}

rv32i_opcodeTypes_t rv32iOpcodeToOpcodeType(uint8_t opcode)
{
    switch (opcode)
    {
    case RV32I_OPCODE_BRANCH:
        return RV32I_OPCODE_TYPE_B;
    case RV32I_OPCODE_LUI:
        return RV32I_OPCODE_TYPE_U;
    case RV32I_OPCODE_JAL:
        return RV32I_OPCODE_TYPE_J;
    case RV32I_OPCODE_JALR:     // Fallthrough
    case RV32I_OPCODE_LOAD:     // Fallthrough
    case RV32I_OPCODE_ALU_IMM:  // Fallthrough
    case RV32I_OPCODE_ENV:
        return RV32I_OPCODE_TYPE_I;
    case RV32I_OPCODE_ALU:
        return RV32I_OPCODE_TYPE_R;
    case RV32I_OPCODE_STORE:
        return RV32I_OPCODE_TYPE_S;
    default:
        return RV32I_OPCODE_TYPE_UNKNOWN;
    }
}
