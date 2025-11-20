#include "rv32i.h"
#include <assert.h>
#include <stdio.h>

rv32i_instruct_t rv32iDecodeInstructType(int32_t instruct)
{
    uint8_t opcode, funct3, funct7;
    uint16_t funct12;

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
        funct7 = rv32iGetFunct7(instruct);
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
        case 0b001:
            return RV32I_SLLI;
        case 0b101:
            switch (funct7)
            {
            case 0b0000000:
                return RV32I_SRLI;
            case 0b0100000:
                return RV32I_SRAI;
            default:
                return RV32I_NOT_SUPPORTED;
            }
        default:
            assert(0); // This condition should be unreachable.
            return RV32I_NOT_SUPPORTED;
        }
    case RV32I_OPCODE_AUIPC:
        return RV32I_AUIPC;
    case RV32I_OPCODE_BRANCH:
        funct3 = rv32iGetFunct3(instruct);
        switch (funct3)
        {
        case 0b000:
            return RV32I_BGE;
        case 0b001:
            return RV32I_BNE;
        case 0b100:
            return RV32I_BLT;
        case 0b101:
            return RV32I_BGE;
        case 0b110:
            return RV32I_BLTU;
        case 0b111:
            return RV32I_BGEU;
        default:
            return RV32I_NOT_SUPPORTED;
        }
    case RV32I_OPCODE_ENV:
        funct12 = rv32iGetFunct12(instruct);
        switch (funct12)
        {
        case 0b000000000000:
            return RV32I_ECALL;
        default:
            return RV32I_OPCODE_TYPE_UNKNOWN;
        }
    case RV32I_OPCODE_FEN_PAUS:
        return RV32I_NOT_SUPPORTED;
    case RV32I_OPCODE_JAL:
        return RV32I_JAL;
    case RV32I_OPCODE_JALR:
        return RV32I_JALR;
    case RV32I_OPCODE_LOAD:
        funct3 = rv32iGetFunct3(instruct);
        switch (funct3)
        {
        case 0b000:
            return RV32I_LB;
        case 0b001:
            return RV32I_LH;
        case 0b010:
            return RV32I_LW;
        case 0b100:
            return RV32I_LBU;
        case 0b101:
            return RV32I_LHU;
        default:
            return RV32I_NOT_SUPPORTED;
        }
    case RV32I_OPCODE_LUI:
        return RV32I_LUI;
    case RV32I_OPCODE_STORE:
        funct3 = rv32iGetFunct3(instruct);
        switch (funct3)
        {
        case 0b000:
            return RV32I_SB;
        case 0b001:
            return RV32I_SH;
        case 0b010:
            return RV32I_SW;
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

uint16_t rv32iGetFunct12(int32_t instruct)
{
    return (instruct >> 20) & 0b00000000000000000000111111111111;
}

uint8_t rv32iGetFunct3(int32_t instruct)
{
    return (instruct >> 12) & 0b00000000000000000000000000000111;
}

uint8_t rv32iGetFunct7(int32_t instruct)
{
    return (instruct >> 25) & 0b00000000000000000000000001111111;
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

int8_t rv32iLoadByte(uint8_t *adr)
{
    return *adr;
}

int16_t rv32iLoadHalfWord(uint8_t *adr)
{
    // TODO: Make solution for both endianness - Currently little endian is assumed
    return ( (int16_t) *adr | ( (int16_t) *(adr+1) << 8 ) );
}

int32_t rv32iLoadWord(uint8_t *adr)
{
    // TODO: Make solution for both endianness - Currently little endian is assumed
    return ( (int16_t) *adr | ( (int16_t) *(adr+1) << 8 ) | ( (int16_t) *(adr+2) << 16 ) | ( (int16_t) *(adr+3) << 24 ) );
}

rv32i_opcodeTypes_t rv32iOpcodeToOpcodeType(uint8_t opcode)
{
    switch (opcode)
    {
    case RV32I_OPCODE_BRANCH:
        return RV32I_OPCODE_TYPE_B;
    case RV32I_OPCODE_AUIPC:    // Fallthrough
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

int32_t rv32iSignExtentByte(uint8_t input)
{
    uint8_t signBit = input >> 7;

    if (signBit)
    {
        return (0b11111111111111111111111100000000 | input);
    }
    else
    {
        return input;
    }
}

int32_t rv32iSignExtentHalfWord(uint16_t input)
{
    uint16_t signBit = input >> 15;

    if (signBit)
    {
        return (0b11111111111111110000000000000000 | input);
    }
    else
    {
        return input;
    }
}

void rv32iStoreByte(uint8_t *adr, uint8_t value)
{
    *adr = value;
    return;
}

void rv32iStoreHalfWord(uint8_t *adr, uint16_t value)
{
    // TODO: Make solution for both endianness - Currently little endian is assumed
    *(adr  ) = value;
    *(adr+1) = value >> 8;
    return;
}

void rv32iStoreWord(uint8_t *adr, uint32_t value)
{
    // TODO: Make solution for both endianness - Currently little endian is assumed
    *(adr  ) = value;
    *(adr+1) = value >>  8;
    *(adr+2) = value >> 16;
    *(adr+3) = value >> 24;
    return;
}
