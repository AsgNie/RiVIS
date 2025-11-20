#include <gtest/gtest.h>
#include <stdint.h>
extern "C" {
    #include <rv32i.h>
}

// Manually constructed test instructions. See rv32i.h for litterature reference.
#define INSTRUCT_ADDI_RD_13_RS1_31_IMM_NEG1 (0b111111111111'11111'000'01101'0010011)
#define INSTRUCT_BGE_RS1_2_RS2_17_IMM_MIN   (0b1000000'10001'00010'101'00000'1100011)
#define INSTRUCT_SUB_RD_21_RS1_10_RS2_0     (0b0100000'00000'01010'000'10101'0110011)

TEST(rv32i, GetOpcode)
{
    EXPECT_EQ(rv32iGetOpcode(0xffffffff), 0b1111111);
    EXPECT_EQ(rv32iGetOpcode(0xffffff80), 0b0000000);
    EXPECT_EQ(rv32iGetOpcode(RV32I_OPCODE_BRANCH), RV32I_OPCODE_BRANCH);
    EXPECT_EQ(rv32iGetOpcode(INSTRUCT_ADDI_RD_13_RS1_31_IMM_NEG1), RV32I_OPCODE_ALU_IMM);
}

TEST(rv32i, GetFunct3)
{
    EXPECT_EQ(rv32iGetFunct3(0xffffffff), 0b111);
    EXPECT_EQ(rv32iGetFunct3(0xffff8fff), 0b000);
    EXPECT_EQ(rv32iGetFunct3(INSTRUCT_ADDI_RD_13_RS1_31_IMM_NEG1), 0b000);
    EXPECT_EQ(rv32iGetFunct3(INSTRUCT_BGE_RS1_2_RS2_17_IMM_MIN), 0b101);
}

TEST(rv32i, GetFunct7)
{
    EXPECT_EQ(rv32iGetFunct7(0xffffffff), 0b1111111);
    EXPECT_EQ(rv32iGetFunct7(0x01ffffff), 0b0000000);
    EXPECT_EQ(rv32iGetFunct7(INSTRUCT_SUB_RD_21_RS1_10_RS2_0), 0b0100000);
}

// The only usecase in this implementation is for funct12 = 1. Test that, and that any other case does not leak.
TEST(rv32i, GetFunc12)
{
    int32_t instruct = 0b00000000000100000000000000000000;
    EXPECT_EQ(rv32iGetFunct12(instruct), 1);
    EXPECT_EQ(rv32iGetFunct12(!instruct), 0);
}

TEST(rv32i, GetRd)
{
    EXPECT_EQ(rv32iGetRd(0xffffffff), 0b11111);
    EXPECT_EQ(rv32iGetRd(0xfffff07f), 0b00000);
    EXPECT_EQ(rv32iGetRd(INSTRUCT_ADDI_RD_13_RS1_31_IMM_NEG1), 13);
    EXPECT_EQ(rv32iGetRd(INSTRUCT_SUB_RD_21_RS1_10_RS2_0), 21);
}

TEST(rv32i, GetRs1)
{
    EXPECT_EQ(rv32iGetRs1(0xffffffff), 0b11111);
    EXPECT_EQ(rv32iGetRs1(0xfff07fff), 0b00000);
    EXPECT_EQ(rv32iGetRs1(INSTRUCT_ADDI_RD_13_RS1_31_IMM_NEG1), 31);
    EXPECT_EQ(rv32iGetRs1(INSTRUCT_BGE_RS1_2_RS2_17_IMM_MIN), 2);
    EXPECT_EQ(rv32iGetRs1(INSTRUCT_SUB_RD_21_RS1_10_RS2_0), 10);
}

TEST(rv32i, GetRs2)
{
    EXPECT_EQ(rv32iGetRs2(0xffffffff), 0b11111);
    EXPECT_EQ(rv32iGetRs2(0xfe0fffff), 0b00000);
    EXPECT_EQ(rv32iGetRs2(INSTRUCT_BGE_RS1_2_RS2_17_IMM_MIN), 17);
    EXPECT_EQ(rv32iGetRs2(INSTRUCT_SUB_RD_21_RS1_10_RS2_0), 0);
}

TEST(rv32i, OpcodeToOpcodeType)
{
    EXPECT_EQ(rv32iOpcodeToOpcodeType(RV32I_OPCODE_JALR), RV32I_OPCODE_TYPE_I);
    EXPECT_EQ(rv32iOpcodeToOpcodeType(0b0000000), RV32I_OPCODE_TYPE_UNKNOWN);
    EXPECT_EQ(rv32iOpcodeToOpcodeType(0b0110111), RV32I_OPCODE_TYPE_U);
    EXPECT_EQ(rv32iOpcodeToOpcodeType(0b1101111), RV32I_OPCODE_TYPE_J);
}

TEST(rv32i, DecodeInstructionType)
{
    EXPECT_EQ(rv32iDecodeInstructType(INSTRUCT_ADDI_RD_13_RS1_31_IMM_NEG1), RV32I_ADDI);
    EXPECT_EQ(rv32iDecodeInstructType(INSTRUCT_BGE_RS1_2_RS2_17_IMM_MIN), RV32I_BGE);
    EXPECT_EQ(rv32iDecodeInstructType(INSTRUCT_SUB_RD_21_RS1_10_RS2_0), RV32I_SUB);

    int32_t instruct = 0;
    instruct |= RV32I_OPCODE_ALU;
    instruct |= (0b000 << 7);       // Funct3 for add/sub
    instruct |= (0b0000001 << 25);  // Illegal funct7 for given opcode and funct3
    EXPECT_EQ(rv32iDecodeInstructType(instruct), RV32I_NOT_SUPPORTED);
}

/*
NOTICE: This is by far the most complex logic making it error prone.
It switches over 5 distict types with different logic. They should all be tested.
*/
TEST(rv32i, GenerateImmediate)
{
    int32_t fakeInstruct, expectedVal;

    /*** Test fake instructions
    Testing on constructed instructions with only immediate value and opcode filled out. Opcode is added
    by OR'ing with an opcode belonging to the instruction type under test
    ***/

    // B-Type, simple sign extension
    fakeInstruct = 0b1000000'0000000000000'00000'0000000|RV32I_OPCODE_BRANCH;
    expectedVal  = 0b1111111111111111111'100000000000'0;
    EXPECT_EQ(rv32iGenerateImmediate(fakeInstruct), expectedVal);

    // B-Type
    fakeInstruct = 0b0010101'0000000000000'01011'0000000|RV32I_OPCODE_BRANCH;
    expectedVal  = 0b0000000000000000000'010101010101'0;
    EXPECT_EQ(rv32iGenerateImmediate(fakeInstruct), expectedVal);

    // I-Type
    fakeInstruct = 0b101010101010'00000000000000000000|RV32I_OPCODE_ALU_IMM;
    expectedVal  = 0b11111111111111111111'101010101010;
    EXPECT_EQ(rv32iGenerateImmediate(fakeInstruct), expectedVal);

    // S-Type
    fakeInstruct = 0b1011011'0000000000000'01101'0000000|RV32I_OPCODE_STORE;
    expectedVal  = 0b11111111111111111111'101101101101;
    EXPECT_EQ(rv32iGenerateImmediate(fakeInstruct), expectedVal);

    // U-Type
    fakeInstruct = 0b10101110001110110110'000000000000|RV32I_OPCODE_LUI;
    expectedVal  = 0b10101110001110110110'000000000000;
    EXPECT_EQ(rv32iGenerateImmediate(fakeInstruct), expectedVal);

    // J-Type
    fakeInstruct = 0b1'0110011001'1'01111101'000000000000|RV32I_OPCODE_JAL;
    expectedVal  = 0b11111111111'1'01111101'1'0110011001'0;
    EXPECT_EQ(rv32iGenerateImmediate(fakeInstruct), expectedVal);

    /*** Testing real instructions ***/
    EXPECT_EQ(rv32iGenerateImmediate(INSTRUCT_ADDI_RD_13_RS1_31_IMM_NEG1), -1);         // I-Type
    EXPECT_EQ(rv32iGenerateImmediate(INSTRUCT_BGE_RS1_2_RS2_17_IMM_MIN), -(0x1000));    // B-Type
}

TEST(rv32i, SignExtentByte)
{
    EXPECT_EQ(rv32iSignExtentByte((int8_t) 0b11001100), 0b111111111111111111111111'11001100);
    EXPECT_EQ(rv32iSignExtentByte((int8_t) 0b01001100), 0b000000000000000000000000'01001100);
}

TEST(rv32i, SignExtentHalfWord)
{
    EXPECT_EQ(rv32iSignExtentHalfWord((int16_t) 0b11001100'00000000), 0b1111111111111111'11001100'00000000);
    EXPECT_EQ(rv32iSignExtentHalfWord((int16_t) 0b01001100'00000000), 0b0000000000000000'01001100'00000000);
}

TEST(rv32i, LoadByte)
{
    uint8_t A[5] = {0b11111111, 0b00000010, 0b10101010, 0b01010101, 0b10000000};
    EXPECT_EQ(rv32iLoadByte(A+0), (int32_t) 0b11111111);
    EXPECT_NE(rv32iLoadByte(A+0), (int8_t)  0b11111111); // Check for previous sign extension bug
    EXPECT_EQ(rv32iLoadByte(A+1), (int32_t) 0b00000010);
    EXPECT_EQ(rv32iLoadByte(A+2), (int32_t) 0b10101010);
    EXPECT_EQ(rv32iLoadByte(A+3), (int32_t) 0b01010101);
    EXPECT_EQ(rv32iLoadByte(A+4), (int32_t) 0b10000000);
}

TEST(rv32i, LoadHalfWord)
{
    uint8_t A[5] = {0b11111111, 0b00000010, 0b10101010, 0b01010101, 0b10000000};
    EXPECT_EQ(rv32iLoadHalfWord(A+0), (int32_t) 0b00000010'11111111);
    EXPECT_EQ(rv32iLoadHalfWord(A+1), (int32_t) 0b10101010'00000010);
    EXPECT_NE(rv32iLoadHalfWord(A+1), (int16_t) 0b10101010'00000010); // Check for previous sign extension bug
    EXPECT_EQ(rv32iLoadHalfWord(A+2), (int32_t) 0b01010101'10101010);
    EXPECT_EQ(rv32iLoadHalfWord(A+3), (int32_t) 0b10000000'01010101);
}

TEST(rv32i, LoadWord)
{
    uint8_t A[5] = {0b11111111, 0b00000010, 0b10101010, 0b01010101, 0b10000000};
    EXPECT_EQ(rv32iLoadWord(A+0), (int32_t) 0b01010101'10101010'00000010'11111111);
    EXPECT_EQ(rv32iLoadWord(A+1), (int32_t) 0b10000000'01010101'10101010'00000010);
}

TEST(rv32i, StoreByte)
{
    uint8_t A[5] = {};
    rv32iStoreByte(A+0, 0b11111111);
    rv32iStoreByte(A+1, 0b00000001);
    rv32iStoreByte(A+2, 0b10101010);
    rv32iStoreByte(A+3, 0b01010101);
    rv32iStoreByte(A+4, 0b00110011);
    EXPECT_EQ(A[0], 0b11111111);
    EXPECT_EQ(A[1], 0b00000001);
    EXPECT_EQ(A[2], 0b10101010);
    EXPECT_EQ(A[3], 0b01010101);
    EXPECT_EQ(A[4], 0b00110011);

    rv32iStoreByte(A+0, 0b10111111);
    EXPECT_EQ(A[0], 0b10111111);
}

TEST(rv32i, StoreHalfWord)
{
    uint8_t A[5] = {};
    rv32iStoreHalfWord(A+0, 0b00000001'11111111);
    rv32iStoreHalfWord(A+2, 0b01010101'10101010);
    EXPECT_EQ(A[0], 0b11111111);
    EXPECT_EQ(A[1], 0b00000001);
    EXPECT_EQ(A[2], 0b10101010);
    EXPECT_EQ(A[3], 0b01010101);
    EXPECT_EQ(A[4], 0b00000000);

    rv32iStoreHalfWord(A+2, 0b11111111'11111111);
    EXPECT_EQ(A[2], 0b11111111);
    EXPECT_EQ(A[3], 0b11111111);
}

TEST(rv32i, StoreWord)
{
    uint8_t A[5] = {};
    rv32iStoreWord(A+0, 0b01010101'10101010'00000001'11111111);
    EXPECT_EQ(A[0], 0b11111111);
    EXPECT_EQ(A[1], 0b00000001);
    EXPECT_EQ(A[2], 0b10101010);
    EXPECT_EQ(A[3], 0b01010101);
    EXPECT_EQ(A[4], 0b00000000);

    rv32iStoreWord(A+1, 0b11111111'11111111'11111111'11111111);
    EXPECT_EQ(A[0], 0b11111111);
    EXPECT_EQ(A[1], 0b11111111);
    EXPECT_EQ(A[2], 0b11111111);
    EXPECT_EQ(A[3], 0b11111111);
    EXPECT_EQ(A[4], 0b11111111);
}
