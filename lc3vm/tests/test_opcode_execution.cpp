#include <gtest/gtest.h>
#include "lc3.hpp"
#include "registers.hpp"
#include "flags.hpp"
#include "opcodes.hpp"

TEST(LC3VMTest, OP_ADD_Immediate) {
    LC3State vm;
    // Setup: R1 = 5, R2 will be destination
    vm.set_register_value(R_R1, 5);
    vm.set_register_value(R_R2, 0); // Clear destination
    vm.set_register_value(R_PC, 0x3000); // Start PC

    // Instruction: ADD R2, R1, #10 (0001 010 001 10010 -> 0x14A2)
    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | (1 << 5) | 10;
    vm.write_memory(0x3000, instr);

    vm.step(); // Execute the ADD instruction

    EXPECT_EQ(vm.get_register_value(R_R2), 15); // 5 + 10 = 15
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS); // Result is positive
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001); // PC should increment
}

TEST(LC3VMTest, OP_ADD_Register) {
    LC3State vm;
    // Setup: R1 = 5, R3 = 7, R2 will be destination
    vm.set_register_value(R_R1, 5);
    vm.set_register_value(R_R3, 7);
    vm.set_register_value(R_R2, 0); // Clear destination
    vm.set_register_value(R_PC, 0x3000); // Start PC

    // Instruction: ADD R2, R1, R3 (0001 010 001 000 011 -> 0x14C3)
    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | R_R3;
    vm.write_memory(0x3000, instr);

    vm.step(); // Execute the ADD instruction

    EXPECT_EQ(vm.get_register_value(R_R2), 12); // 5 + 7 = 12
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS); // Result is positive
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001); // PC should increment
}

TEST(LC3VMTest, OP_LD_LoadDirect) {
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

TEST(LC3VMTest, OP_ST_StoreDirect) {
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R2, 456);
    std::uint16_t instr = (Opcodes::OP_ST << 12) | (R_R2 << 9) | 5;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.read_memory(0x3001 + 5), 456);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_AND_Register) {
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

TEST(LC3VMTest, OP_NOT) {
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

TEST(LC3VMTest, OP_JMP) {
    LC3State vm;
    vm.set_register_value(R_R3, 0x4000);
    vm.set_register_value(R_PC, 0x3000);
    std::uint16_t instr = (Opcodes::OP_JMP << 12) | (R_R3 << 6);
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x4000);
}

TEST(LC3VMTest, OP_LDI_LoadIndirect) {
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

TEST(LC3VMTest, OP_STI_StoreIndirect) {
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

TEST(LC3VMTest, OP_LDR_LoadBaseOffset) {
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

TEST(LC3VMTest, OP_STR_StoreBaseOffset) {
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

TEST(LC3VMTest, OP_LEA_LoadEffectiveAddress) {
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    std::uint16_t instr = (Opcodes::OP_LEA << 12) | (R_R2 << 9) | 7;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R2), 0x3001 + 7);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS); 
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_BR_BranchIfPositive) {
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_POS);
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_POS << 9) | 10;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001 + 10);
}

TEST(LC3VMTest, OP_BR_BranchNotTaken) {
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_ZRO);
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_POS << 9) | 10;
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

// Test for BR instruction offsets and conditions
TEST(LC3VMTest, OP_BR_BranchIfZero) {
    LC3State vm;
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_ZRO);
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_ZRO << 9) | 10; // BRz #10
    vm.write_memory(0x3000, instr);
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001 + 10);
} 