#include <gtest/gtest.h>
#include "lc3.hpp"      // Include the main VM header
#include "registers.hpp" // For R_PC, R_COND, R_R0 etc.
#include "flags.hpp"     // For FL_ZRO

// Helper to get register value if LC3State does not expose 'reg' directly publicly
// For now, assuming 'reg' is accessible for testing or we add a getter.
// Our LC3State.reg is private. We need a way to inspect registers for tests.
// Let's add a simple public getter in LC3State for tests for now.

TEST(LC3VMTest, BasicInitialization) {
    LC3State vm;
    // PC should be initialized to 0x3000
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3000);
    // Condition code should be FL_ZRO
    EXPECT_EQ(vm.get_register_value(R_COND), FL_ZRO);

    // General purpose registers R0-R7 should be 0
    for (int i = R_R0; i <= R_R7; ++i) {
        EXPECT_EQ(vm.get_register_value(static_cast<Registers>(i)), 0);
    }
}

// Placeholder for OP_ADD test - we'll need to write to memory
TEST(LC3VMTest, OP_ADD_Immediate) {
    LC3State vm;
    // Setup: R1 = 5, R2 will be destination
    vm.set_register_value(R_R1, 5);
    vm.set_register_value(R_R2, 0); // Clear destination
    vm.set_register_value(R_PC, 0x3000); // Start PC

    // Instruction: ADD R2, R1, #10 (0001 010 001 10010 -> 0x14A2)
    // R2 is DR (010), R1 is SR1 (001), imm_flag = 1, imm5 = 10 (01010)
    // Opcode: 0001 (ADD)
    // DR: 010 (R2)
    // SR1: 001 (R1)
    // Imm Flag: 1
    // Imm5: 01010 (10 decimal)
    // instr = (OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | (1 << 5) | 10;
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
    // R2 is DR (010), R1 is SR1 (001), imm_flag = 0, SR2 = R3 (011)
    // Opcode: 0001 (ADD)
    // DR: 010 (R2)
    // SR1: 001 (R1)
    // Imm Flag: 0
    // Reserved: 00
    // SR2: 011 (R3)
    // instr = (OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | (0 << 5) | R_R3;
    std::uint16_t instr = (Opcodes::OP_ADD << 12) | (R_R2 << 9) | (R_R1 << 6) | R_R3;
    vm.write_memory(0x3000, instr);

    vm.step(); // Execute the ADD instruction

    EXPECT_EQ(vm.get_register_value(R_R2), 12); // 5 + 7 = 12
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS); // Result is positive
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001); // PC should increment
}

TEST(LC3VMTest, OP_LD_LoadDirect) {
    LC3State vm;
    // Setup: PC = 0x3000. Memory[PC_after_fetch + offset(10)] = 123. R2 is destination.
    // LD R2, LABEL_VAL (PCoffset9 = 10)
    // PC_after_fetch = 0x3001. Effective Address = 0x3001 + 10 = 0x300B
    vm.set_register_value(R_PC, 0x3000);
    vm.write_memory(0x300B, 123); // Value to be loaded (Corrected address)

    // Instruction: LD R2, #10 (0010 010 000001010 -> 0x240A)
    // R2 is DR (010), PCoffset9 = 10
    std::uint16_t instr = (Opcodes::OP_LD << 12) | (R_R2 << 9) | 10;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 123);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS); // 123 is positive
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_ST_StoreDirect) {
    LC3State vm;
    // Setup: PC = 0x3000. R2 = 456. Store R2 to Memory[PC_after_fetch + offset(5)]
    // ST R2, LABEL_STORE (PCoffset9 = 5)
    // PC_after_fetch = 0x3001. Effective Address = 0x3001 + 5 = 0x3006
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R2, 456);

    // Instruction: ST R2, #5 (0011 010 000000101 -> 0x3405)
    // R2 is SR (010), PCoffset9 = 5
    std::uint16_t instr = (Opcodes::OP_ST << 12) | (R_R2 << 9) | 5;
    vm.write_memory(0x3000, instr);

    vm.step();

    // Effective address calculated in OP_ST is state.reg[R_PC] (which is 0x3001) + offset (5) = 0x3006
    EXPECT_EQ(vm.read_memory(0x3001 + 5), 456); // Corrected expectation

    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001); // PC just increments
    // ST does not change flags
}

TEST(LC3VMTest, OP_AND_Register) {
    LC3State vm;
    // Setup: R1 = 0b1100, R3 = 0b1010. R2 is DR.
    vm.set_register_value(R_R1, 0xC); // 12
    vm.set_register_value(R_R3, 0xA); // 10
    vm.set_register_value(R_PC, 0x3000);

    // Instruction: AND R2, R1, R3 (0101 010 001 000 011 -> 0x54C3)
    std::uint16_t instr = (Opcodes::OP_AND << 12) | (R_R2 << 9) | (R_R1 << 6) | R_R3;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 0x8); // 0b1000 (12 & 10 = 8)
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_NOT) {
    LC3State vm;
    // Setup: R1 = 0xFF00. R2 is DR.
    vm.set_register_value(R_R1, 0xFF00);
    vm.set_register_value(R_PC, 0x3000);

    // Instruction: NOT R2, R1 (1001 010 001 111111 -> 0x94BF)
    std::uint16_t instr = (Opcodes::OP_NOT << 12) | (R_R2 << 9) | (R_R1 << 6) | 0x3F; // last 6 bits are 1s
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), (std::uint16_t)~0xFF00); // 0x00FF
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_JMP) {
    LC3State vm;
    // Setup: R3 contains target address 0x4000
    vm.set_register_value(R_R3, 0x4000);
    vm.set_register_value(R_PC, 0x3000);

    // Instruction: JMP R3 (1100 000 011 000000 -> 0xC0C0)
    std::uint16_t instr = (Opcodes::OP_JMP << 12) | (R_R3 << 6);
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_PC), 0x4000);
}

TEST(LC3VMTest, OP_LDI_LoadIndirect) {
    LC3State vm;
    // Setup: PC = 0x3000. R2 is DR.
    // LDI R2, ADDR_PTR (PCoffset9 = 10)
    // ADDR_PTR is at 0x3001 (PC after fetch) + 10 = 0x300B
    // Memory[0x300B] contains 0x4000 (the effective address)
    // Memory[0x4000] contains 789 (the value to be loaded)
    vm.set_register_value(R_PC, 0x3000);
    vm.write_memory(0x300B, 0x4000); // Pointer to effective address
    vm.write_memory(0x4000, 789);    // Value at effective address

    // Instruction: LDI R2, #10 (1010 010 000001010 -> 0xA40A)
    std::uint16_t instr = (Opcodes::OP_LDI << 12) | (R_R2 << 9) | 10;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 789);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_STI_StoreIndirect) {
    LC3State vm;
    // Setup: PC = 0x3000. R2 = 987 (value to store).
    // STI R2, ADDR_PTR (PCoffset9 = 7)
    // ADDR_PTR is at 0x3001 (PC after fetch) + 7 = 0x3008
    // Memory[0x3008] contains 0x5000 (the effective address where R2 will be stored)
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R2, 987);
    vm.write_memory(0x3008, 0x5000); // Pointer to effective address

    // Instruction: STI R2, #7 (1011 010 000000111 -> 0xB407)
    std::uint16_t instr = (Opcodes::OP_STI << 12) | (R_R2 << 9) | 7;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.read_memory(0x5000), 987);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001); // PC just increments
    // STI does not change flags
}

TEST(LC3VMTest, OP_LDR_LoadBaseOffset) {
    LC3State vm;
    // Setup: PC = 0x3000. R1 (BaseR) = 0x4000. Memory[0x4000 + offset(5)] = 222. R2 is DR.
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R1, 0x4000); // Base register
    vm.write_memory(0x4000 + 5, 222);   // Value at BaseR + offset

    // Instruction: LDR R2, R1, #5 (0110 010 001 000101 -> 0x6445)
    // R2 is DR (010), R1 is BaseR (001), offset6 = 5
    std::uint16_t instr = (Opcodes::OP_LDR << 12) | (R_R2 << 9) | (R_R1 << 6) | 5;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 222);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_STR_StoreBaseOffset) {
    LC3State vm;
    // Setup: PC = 0x3000. R1 (BaseR) = 0x5000. R2 (SR) = 333.
    // Store R2 to Memory[R1 + offset(3)]
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_R1, 0x5000); // Base register
    vm.set_register_value(R_R2, 333);   // Source register

    // Instruction: STR R2, R1, #3 (0111 010 001 000011 -> 0x7443)
    // R2 is SR (010), R1 is BaseR (001), offset6 = 3
    std::uint16_t instr = (Opcodes::OP_STR << 12) | (R_R2 << 9) | (R_R1 << 6) | 3;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.read_memory(0x5000 + 3), 333);
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
    // STR does not change flags
}

TEST(LC3VMTest, OP_LEA_LoadEffectiveAddress) {
    LC3State vm;
    // Setup: PC = 0x3000. R2 is DR.
    // LEA R2, #7 (PCoffset9 = 7)
    // Effective address = PC_after_fetch + 7 = 0x3001 + 7 = 0x3008
    vm.set_register_value(R_PC, 0x3000);

    // Instruction: LEA R2, #7 (1110 010 000000111 -> 0xE407)
    std::uint16_t instr = (Opcodes::OP_LEA << 12) | (R_R2 << 9) | 7;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_R2), 0x3001 + 7);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_POS); // Address 0x3008 is positive
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

TEST(LC3VMTest, OP_BR_BranchIfPositive) {
    LC3State vm;
    // Setup: PC = 0x3000. R_COND = FL_POS (1).
    // BRp #10 (PCoffset9 = 10)
    // Target PC = PC_after_fetch + 10 = 0x3001 + 10 = 0x300B
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_POS);

    // Instruction: BRp #10 (0000 001 000001010 -> 0x020A)
    // nzp = 001 (Positive), PCoffset9 = 10
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_POS << 9) | 10;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001 + 10);
    // BR does not change flags
}

TEST(LC3VMTest, OP_BR_BranchNotTaken) {
    LC3State vm;
    // Setup: PC = 0x3000. R_COND = FL_ZRO (2).
    // BRp #10 (PCoffset9 = 10)
    // Condition (Positive) is not met, so PC should just increment.
    vm.set_register_value(R_PC, 0x3000);
    vm.set_register_value(R_COND, FL_ZRO);

    // Instruction: BRp #10 (0000 001 000001010 -> 0x020A)
    std::uint16_t instr = (Opcodes::OP_BR << 12) | (FL_POS << 9) | 10;
    vm.write_memory(0x3000, instr);

    vm.step();

    EXPECT_EQ(vm.get_register_value(R_PC), 0x3001);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 