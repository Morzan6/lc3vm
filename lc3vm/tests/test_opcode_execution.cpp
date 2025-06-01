#include <gtest/gtest.h>
#include "lc3.hpp"
#include "registers.hpp"
#include "flags.hpp"
#include "opcodes.hpp"

TEST(LC3VMTest, OP_ADD_Immediate)
{
    LC3State vm;
    vm.set_register_value(R_R1, 5);
    vm.set_register_value(R_R2, 0);
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | (1 << 5) | 10;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 15);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_ADD_Register)
{
    LC3State vm;
    vm.set_register_value(R_R1, 5);
    vm.set_register_value(R_R3, 7);
    vm.set_register_value(R_R2, 0);

    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | R_R3;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 12);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_LD_LoadDirect)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.write_memory(0x300B, 123);
    std::uint16_t instr = (Opcodes::OP_LD << 12) | (R_R2 << 9) | 10;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R2), 123);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_ST_StoreDirect)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R2, 456);
    std::uint16_t instr = (Opcodes::OP_ST << 12) | (R_R2 << 9) | 5;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.read_memory(0x3001 + 5), 456);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_AND_Register)
{
    LC3State vm;
    vm.set_register_value(R_R1, 0xC);
    vm.set_register_value(R_R3, 0xA);
    vm.set_register_value(R_PC, 0x3000);
    std::uint16_t instr = (Opcodes::OP_AND << 12) | (R_R2 << 9) | (R_R1 << 6) | R_R3;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R2), 0x8);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_NOT)
{
    LC3State vm;
    vm.set_register_value(R_R1, 0xFF00);
    vm.set_register_value(R_PC, 0x3000);
    std::uint16_t instr = (Opcodes::OP_NOT << 12) | (R_R2 << 9) | (R_R1 << 6) | 0x3F;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R2), (std::uint16_t)~0xFF00);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_JMP)
{
    LC3State vm;
    vm.set_register_value(R_R3, 0x4000);
    vm.set_register_value(R_PC, 0x3000);
    std::uint16_t instr = (Opcodes::OP_JMP << 12) | (R_R3 << 6);
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x4000);
}

TEST(LC3VMTest, OP_LDI_LoadIndirect)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.write_memory(0x300B, 0x4000);
    vm.write_memory(0x4000, 789);
    std::uint16_t instr = (Opcodes::OP_LDI << 12) | (R_R2 << 9) | 10;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R2), 789);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_STI_StoreIndirect)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R2, 987);
    vm.write_memory(0x3008, 0x5000);
    std::uint16_t instr = (Opcodes::OP_STI << 12) | (R_R2 << 9) | 7;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.read_memory(0x5000), 987);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_LDR_LoadBaseOffset)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R1, 0x4000);
    vm.write_memory(0x4000 + 5, 222);
    std::uint16_t instr = (Opcodes::OP_LDR << 12) | (R_R2 << 9) | (R_R1 << 6) | 5;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R2), 222);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_STR_StoreBaseOffset)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R1, 0x5000);
    vm.set_register_value(R_R2, 333);
    std::uint16_t instr = (Opcodes::OP_STR << 12) | (R_R2 << 9) | (R_R1 << 6) | 3;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.read_memory(0x5000 + 3), 333);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_LEA_LoadEffectiveAddress)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    std::uint16_t instr = (Opcodes::OP_LEA << 12) | (R_R2 << 9) | 7;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R2), 0x3001 + 7);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_BR_BranchIfPositive)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_POS);
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_POS << 9) | 10;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001 + 10);
}

TEST(LC3VMTest, OP_BR_BranchNotTaken)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_ZRO);
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_POS << 9) | 10;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_BR_BranchIfZero)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_ZRO);
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_ZRO << 9) | 10;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001 + 10);
}

TEST(LC3VMTest, OP_ADD_Immediate_NegativeResult)
{
    LC3State vm;
    vm.set_register_value(R_R1, 10);
    vm.set_register_value(R_R2, 0);
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | (1 << 5) | (0b11111 & 0x1F);
    std::uint16_t instr_neg = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | (1 << 5) | (0b11011 & 0x1F);
    vm.write_memory(0x3000, instr_neg);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 5);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_ADD_Immediate_NegativeResult_Corrected)
{
    LC3State vm;
    vm.set_register_value(R_R1, 10);
    vm.set_register_value(R_R2, 0);
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | (1 << 5) | (0b10100 & 0x1F);
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 0xFFFE);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_NEG);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_ADD_Register_PositiveOverflow)
{
    LC3State vm;
    vm.set_register_value(R_R1, 0x7000);
    vm.set_register_value(R_R2, 0x7000);
    vm.set_register_value(R_R3, 0);
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R3 << 9) | (R_R1 << 6) | R_R2;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R3), 0xE000);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_NEG);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_ADD_Register_NegativeOverflow_WrapAround)
{
    LC3State vm;
    vm.set_register_value(R_R1, 0xA000);
    vm.set_register_value(R_R2, 0xA000);
    vm.set_register_value(R_R3, 0);
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R3 << 9) | (R_R1 << 6) | R_R2;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R3), 0x4000);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_LD_InvalidAddress)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_PC, 0xFFFE);
    std::uint16_t instr_wrap = (Opcodes::OP_LD << 12) | (R_R0 << 9) | (3 & 0x1FF);
    vm.write_memory(0xFFFE, instr_wrap);
    vm.write_memory(0x0002, 0xABCD);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R0), 0xABCD);
    EXPECT_EQ(vm.get_register_value(R_PC), 0xFFFF);
}

TEST(LC3VMTest, InvalidOpcodeExecution)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t invalid_instr = (Opcodes::OP_RTI << 12);
    vm.write_memory(0x3000, invalid_instr);
    std::uint16_t illegal_opcode = (0x8 << 12);
    vm.write_memory(0x3000, illegal_opcode);

    std::uint16_t trap_undefined = (Opcodes::OP_TRAP << 12) | 0xFF;
    vm.write_memory(0x3000, trap_undefined);
}
TEST(LC3VMTest, IllegalOpcodeExecution_DoesNotIncrementPC)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t illegal_opcode = (0x8 << 12);
    vm.write_memory(0x3000, illegal_opcode);

    EXPECT_THROW(vm.step(), std::runtime_error);
}

TEST(LC3VMTest, UndefinedTrapVector_DoesNotIncrementPC)
{
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);

    std::uint16_t trap_undefined = (Opcodes::OP_TRAP << 12) | 0xFF;
    vm.write_memory(0x3000, trap_undefined);

    EXPECT_THROW(vm.step(), std::runtime_error);
}
