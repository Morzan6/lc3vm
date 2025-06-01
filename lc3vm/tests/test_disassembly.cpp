#include <gtest/gtest.h>
#include "lc3.hpp"
#include "opcodes.hpp"
#include "registers.hpp"
#include "flags.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>


TEST(LC3DisassembleTest, ADD_Reg) {
    LC3State vm;
    std::uint16_t addr = 0x3000;
    // ADD R1, R2, R3   (0001 001 010 000 011 => 0x1283)
    vm.write_memory(addr, 0x1283);
    EXPECT_EQ(vm.disassemble(addr), "0x3000: ADD R1, R2, R3");
}

TEST(LC3DisassembleTest, ADD_Imm) {
    LC3State vm;
    std::uint16_t addr = 0x3001;
    // ADD R1, R2, #-1 (0001 001 010 1 11111 => 0x12BF)
    vm.write_memory(addr, 0x12BF);
    EXPECT_EQ(vm.disassemble(addr), "0x3001: ADD R1, R2, #-1");
    // ADD R3, R4, #5 (0001 011 100 1 00101 => 0x1725)
    vm.write_memory(addr + 1, 0x1725);
    EXPECT_EQ(vm.disassemble(addr + 1), "0x3002: ADD R3, R4, #5");
}

TEST(LC3DisassembleTest, AND_Reg) {
    LC3State vm;
    std::uint16_t addr = 0x3100;
    // AND R5, R6, R7 (Op=0101 DR=101 SR1=110 Mode=0 MustBeZero=00 SR2=111 => 0101 101 110 000 111 => 0x5B87)
    vm.write_memory(addr, 0x5B87); 
    EXPECT_EQ(vm.disassemble(addr), "0x3100: AND R5, R6, R7");
}

TEST(LC3DisassembleTest, AND_Imm) {
    LC3State vm;
    std::uint16_t addr = 0x3150;
    // AND R0, R1, #0   (0101 000 001 1 00000 => 0x5060)
    vm.write_memory(addr, 0x5060);
    EXPECT_EQ(vm.disassemble(addr), "0x3150: AND R0, R1, #0");
    // AND R2, R3, #-16 (0101 010 011 1 10000 => 0x54F0)
    vm.write_memory(addr + 1, 0x54F0);
    EXPECT_EQ(vm.disassemble(addr + 1), "0x3151: AND R2, R3, #-16");
}

TEST(LC3DisassembleTest, NOT) {
    LC3State vm;
    std::uint16_t addr = 0x3200;
    // NOT R3, R4       (1001 011 100 111111 => 0x973F)
    vm.write_memory(addr, 0x973F);
    EXPECT_EQ(vm.disassemble(addr), "0x3200: NOT R3, R4");
}

TEST(LC3DisassembleTest, BR) {
    LC3State vm;
    std::uint16_t addr = 0x3300;
    // BRnzp 0x330A (PC = 0x3300, PCOffset9 = 9. Target = 0x3300+1+9 = 0x330A)
    vm.write_memory(addr, 0x0E09);
    EXPECT_EQ(vm.disassemble(addr), "0x3300: BRnzp 0x330a"); 
    // BRn (PC=0x3301, PCOffset9 = -10 Target = 0x3301+1-10 = 0x32F8)
    vm.write_memory(addr + 1, 0x09F6);
    EXPECT_EQ(vm.disassemble(addr+1), "0x3301: BRn 0x32f8");
}

TEST(LC3DisassembleTest, JMP_RET) {
    LC3State vm;
    std::uint16_t addr = 0x3400;
    vm.write_memory(addr, 0xC0C0); // JMP R3
    EXPECT_EQ(vm.disassemble(addr), "0x3400: JMP R3");
    vm.write_memory(addr + 1, 0xC1C0); // RET (JMP R7)
    EXPECT_EQ(vm.disassemble(addr + 1), "0x3401: RET");
}

TEST(LC3DisassembleTest, JSR_JSRR) {
    LC3State vm;
    std::uint16_t addr = 0x3500;
    // JSR 0x358a. For PC=0x3500, Target=0x358a => PCOffset11 = 0x358a - 0x3500 - 1 = 0x89.
    // Instruction: 0100 1 (JSR bit) 00010001001 (0x089) => 0x4889
    vm.write_memory(addr, 0x4889); 
    EXPECT_EQ(vm.disassemble(addr), "0x3500: JSR 0x358a");

    // JSRR R5 (0100 0 00 101 000000 => 0x4140)
    vm.write_memory(addr + 1, 0x4140); 
    EXPECT_EQ(vm.disassemble(addr + 1), "0x3501: JSR R5");
}

TEST(LC3DisassembleTest, LD_LDI_LDR_LEA) {
    LC3State vm;
    std::uint16_t addr = 0x3600;
    vm.write_memory(addr, 0x200F); // LD R0, 0x3610
    EXPECT_EQ(vm.disassemble(addr), "0x3600: LD R0, 0x3610");
    vm.write_memory(addr + 1, 0xA206); // LDI R1, 0x3608
    EXPECT_EQ(vm.disassemble(addr + 1), "0x3601: LDI R1, 0x3608");
    vm.write_memory(addr + 2, 0x64C5); // LDR R2, R3, #5
    EXPECT_EQ(vm.disassemble(addr + 2), "0x3602: LDR R2, R3, #5");

    // LEA R4, 0x35f0. For PC=0x3603, Target=0x35f0 => PCOffset9 = 0x35f0 - 0x3603 - 1 = -20 (0x1EC).
    // Instruction: 1110 (LEA) 100 (R4) 111101100 (0x1EC) => 0xE9EC
    vm.write_memory(addr + 3, 0xE9EC); 
    EXPECT_EQ(vm.disassemble(addr + 3), "0x3603: LEA R4, 0x35f0");
}

TEST(LC3DisassembleTest, ST_STI_STR) {
    LC3State vm;
    std::uint16_t addr = 0x3700;
    vm.write_memory(addr, 0x3A1F); // ST R5, 0x3720
    EXPECT_EQ(vm.disassemble(addr), "0x3700: ST R5, 0x3720");
    vm.write_memory(addr + 1, 0xBDF0); // STI R6, 0x36f2
    EXPECT_EQ(vm.disassemble(addr + 1), "0x3701: STI R6, 0x36f2");
    vm.write_memory(addr + 2, 0x7E3E); // STR R7, R0, #-2
    EXPECT_EQ(vm.disassemble(addr + 2), "0x3702: STR R7, R0, #-2");
}

TEST(LC3DisassembleTest, TRAP) {
    LC3State vm;
    std::uint16_t addr = 0x3800;
    vm.write_memory(addr, 0xF025); // TRAP x25
    EXPECT_EQ(vm.disassemble(addr), "0x3800: TRAP x25");
    vm.write_memory(addr+1, 0xF020); // TRAP x20
    EXPECT_EQ(vm.disassemble(addr+1), "0x3801: TRAP x20");
}

TEST(LC3DisassembleTest, BAD_OPCODE) {
    LC3State vm;
    std::uint16_t addr = 0x3900;
    vm.write_memory(addr, 0x8000); // RTI (Opcode 8)
    EXPECT_EQ(vm.disassemble(addr), "0x3900: BAD OPCODE"); 
    vm.write_memory(addr + 1, 0xD000); // RES (Opcode 13)
    EXPECT_EQ(vm.disassemble(addr + 1), "0x3901: BAD OPCODE");
}
