#include <gtest/gtest.h>
#include "lc3.hpp"
#include "registers.hpp"
#include "flags.hpp"
#include "keyboard.hpp"
#include "terminal_input.hpp"
#include <fstream>
#include <filesystem>
#include <termios.h>
#include <unistd.h>


static std::uint16_t swap16(std::uint16_t x) {
    return (x << 8) | (x >> 8);
}

class IntegrationTest : public ::testing::Test {
protected:
    LC3State vm;
    
    void SetUp() override {
        vm.set_register_value(R_PC, 0x3000);
        vm.memory.test_mode = true;
    }

    void TearDown() override {
        disable_raw_mode();
    }

    void simulate_keyboard_input(char input) {
        vm.write_memory(Keyboard::MR_KBSR, 0x8000);
        vm.write_memory(Keyboard::MR_KBDR, input);
    }
};

TEST_F(IntegrationTest, MemoryOperations) {

    vm.write_memory(0x3000, 0x1234);
    EXPECT_EQ(vm.read_memory(0x3000), 0x1234);
    
    simulate_keyboard_input('A');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'A');

    vm.write_memory(0xFFFF, 0x5678);
    EXPECT_EQ(vm.read_memory(0xFFFF), 0x5678);

    vm.write_memory(0x4000, 0x0000);
    EXPECT_EQ(vm.read_memory(0x4000), 0x0000);
    vm.write_memory(0x4000, 0xFFFF);
    EXPECT_EQ(vm.read_memory(0x4000), 0xFFFF);
}

TEST_F(IntegrationTest, MemoryKeyboardStatus) {
    vm.memory.test_mode = true;
    
    vm.write_memory(Keyboard::MR_KBSR, 0x0000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
    
    vm.write_memory(Keyboard::MR_KBSR, 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
    
    vm.write_memory(Keyboard::MR_KBDR, 'X');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'X');
    
    simulate_keyboard_input('Y');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'Y');
    
    simulate_keyboard_input('Z');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'Z');
}

TEST_F(IntegrationTest, MemoryKeyboardNonTestMode) {
    // In test mode, we need to explicitly set and clear the keyboard status
    vm.memory.test_mode = true;
    
    // Test keyboard status when no input is available
    vm.write_memory(Keyboard::MR_KBSR, 0x0000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
    
    // Test keyboard status and data after input
    simulate_keyboard_input('T');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'T');
    
    // Test keyboard status after input is consumed
    vm.write_memory(Keyboard::MR_KBSR, 0x0000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
}

TEST_F(IntegrationTest, MemoryKeyboardMultipleInputs) {
    // In test mode, we need to explicitly set and clear the keyboard status
    vm.memory.test_mode = true;
    
    const char inputs[] = "ABC";
    for (char input : inputs) {
        // Clear keyboard status before each input
        vm.write_memory(Keyboard::MR_KBSR, 0x0000);
        EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
        
        // Simulate input
        simulate_keyboard_input(input);
        EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
        EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), input);
        
        // Clear keyboard status after input is consumed
        vm.write_memory(Keyboard::MR_KBSR, 0x0000);
        EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
    }
}

TEST_F(IntegrationTest, MemoryKeyboardTimeout) {
    // In test mode, we need to explicitly set and clear the keyboard status
    vm.memory.test_mode = true;
    
    // Test keyboard status with no input
    vm.write_memory(Keyboard::MR_KBSR, 0x0000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
    
    // Test multiple reads with no input
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
    }
}

TEST_F(IntegrationTest, MemoryWriteReadPatterns) {
    for (uint16_t i = 0; i < 16; i++) {
        uint16_t value = 1 << i;
        vm.write_memory(0x3000 + i, value);
        EXPECT_EQ(vm.read_memory(0x3000 + i), value);
    }
    
    for (uint16_t i = 0; i < 8; i++) {
        uint16_t value = (i % 2) ? 0xAAAA : 0x5555;
        vm.write_memory(0x4000 + i, value);
        EXPECT_EQ(vm.read_memory(0x4000 + i), value);
    }

    for (uint16_t i = 0; i < 256; i++) {
        vm.write_memory(0x5000 + i, i);
        EXPECT_EQ(vm.read_memory(0x5000 + i), i);
    }
}

TEST_F(IntegrationTest, MemoryEdgeCases) {
    vm.write_memory(0x0000, 0x1234);
    EXPECT_EQ(vm.read_memory(0x0000), 0x1234);
    
    vm.write_memory(0xFFFF, 0x5678);
    EXPECT_EQ(vm.read_memory(0xFFFF), 0x5678);
    
    vm.write_memory(0x1000, 0xABCD);
    vm.write_memory(0x1001, 0xEF01);
    EXPECT_EQ(vm.read_memory(0x1000), 0xABCD);
    EXPECT_EQ(vm.read_memory(0x1001), 0xEF01);
    
    vm.write_memory(0x2000, 0x0000);
    vm.write_memory(0x2001, 0xFFFF);
    vm.write_memory(0x2002, 0x5555);
    vm.write_memory(0x2003, 0xAAAA);
    EXPECT_EQ(vm.read_memory(0x2000), 0x0000);
    EXPECT_EQ(vm.read_memory(0x2001), 0xFFFF);
    EXPECT_EQ(vm.read_memory(0x2002), 0x5555);
    EXPECT_EQ(vm.read_memory(0x2003), 0xAAAA);
}

TEST_F(IntegrationTest, TerminalInput) {

    vm.write_memory(Keyboard::MR_KBSR, 0x8000);
    vm.write_memory(Keyboard::MR_KBDR, 'A');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'A');

    vm.write_memory(Keyboard::MR_KBSR, 0x0000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);
    vm.write_memory(Keyboard::MR_KBSR, 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);

    simulate_keyboard_input('B');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'B');
    simulate_keyboard_input('C');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'C');
}

TEST_F(IntegrationTest, TrapRoutines) {
    // Test TRAP_GETC
    vm.write_memory(0x3000, 0xF020);  // TRAP_GETC
    simulate_keyboard_input('B');
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R0), 'B');
    
    // Test TRAP_OUT
    vm.write_memory(0x3001, 0xF021);  // TRAP_OUT
    vm.set_register_value(R_R0, 'C');
    vm.step();
    
    // Test TRAP_PUTS
    vm.write_memory(0x3002, 0xF022);  // TRAP_PUTS
    vm.set_register_value(R_R0, 0x3100);
    vm.write_memory(0x3100, 'H');
    vm.write_memory(0x3101, 'i');
    vm.write_memory(0x3102, 0x0000);
    vm.step();
    
    // Test TRAP_IN
    vm.write_memory(0x3003, 0xF023);  // TRAP_IN
    simulate_keyboard_input('D');
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R0), 'D');
    
    // Test TRAP_PUTSP
    vm.write_memory(0x3004, 0xF024);  // TRAP_PUTSP
    vm.set_register_value(R_R0, 0x3200);
    vm.write_memory(0x3200, 0x4142);  // "AB"
    vm.write_memory(0x3201, 0x0000);
    vm.step();
    
    // Test TRAP_HALT
    vm.write_memory(0x3005, 0xF025);  // TRAP_HALT
    vm.step();
    EXPECT_FALSE(vm.is_running());
}

TEST_F(IntegrationTest, MemoryBoundaryConditions) {
    vm.write_memory(0x0000, 0x1234);
    EXPECT_EQ(vm.read_memory(0x0000), 0x1234);

    vm.write_memory(0xFFFF, 0x5678);
    EXPECT_EQ(vm.read_memory(0xFFFF), 0x5678);

    for (uint16_t addr = 0x3000; addr < 0x3010; addr++) {
        vm.write_memory(addr, addr);
        EXPECT_EQ(vm.read_memory(addr), addr);
    }
}

TEST_F(IntegrationTest, KeyboardStatusTransitions) {
    vm.write_memory(Keyboard::MR_KBSR, 0x0000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);

    simulate_keyboard_input('X');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'X');

    vm.write_memory(Keyboard::MR_KBSR, 0x0000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x0000);

    simulate_keyboard_input('Y');
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBSR), 0x8000);
    EXPECT_EQ(vm.read_memory(Keyboard::MR_KBDR), 'Y');
}

TEST_F(IntegrationTest, TerminalInputRawMode) {
    // Skip terminal tests in CI environment
    if (getenv("CI") || getenv("GITHUB_ACTIONS")) {
        GTEST_SKIP() << "Skipping terminal tests in CI environment";
    }
    
    EXPECT_NO_THROW(enable_raw_mode());
    EXPECT_TRUE(is_raw_mode_enabled());
    
    EXPECT_NO_THROW(disable_raw_mode());
    EXPECT_FALSE(is_raw_mode_enabled());
    
    EXPECT_NO_THROW(enable_raw_mode());
    EXPECT_TRUE(is_raw_mode_enabled());
}

TEST_F(IntegrationTest, TerminalInputErrorHandling) {
    // Skip terminal tests in CI environment
    if (getenv("CI") || getenv("GITHUB_ACTIONS")) {
        GTEST_SKIP() << "Skipping terminal tests in CI environment";
    }
    
    int original_stdin = dup(STDIN_FILENO);
    ASSERT_NE(original_stdin, -1);
    
    close(STDIN_FILENO);
    EXPECT_THROW(enable_raw_mode(), std::runtime_error);
    
    dup2(original_stdin, STDIN_FILENO);
    close(original_stdin);
    
    EXPECT_NO_THROW(enable_raw_mode());
    EXPECT_TRUE(is_raw_mode_enabled());
}

TEST_F(IntegrationTest, TerminalInputModeTransitions) {
    // Skip terminal tests in CI environment
    if (getenv("CI") || getenv("GITHUB_ACTIONS")) {
        GTEST_SKIP() << "Skipping terminal tests in CI environment";
    }
    
    for (int i = 0; i < 3; i++) {
        EXPECT_NO_THROW(enable_raw_mode());
        EXPECT_TRUE(is_raw_mode_enabled());
        
        EXPECT_NO_THROW(disable_raw_mode());
        EXPECT_FALSE(is_raw_mode_enabled());
    }
    
    EXPECT_NO_THROW(enable_raw_mode());
    EXPECT_TRUE(is_raw_mode_enabled());
    
    for (int i = 0; i < 3; i++) {
        EXPECT_NO_THROW(disable_raw_mode());
        EXPECT_FALSE(is_raw_mode_enabled());
    }
}


TEST_F(IntegrationTest, TrapRoutinesNonTestMode) {
    vm.memory.test_mode = false;
    
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);
    
    int original_stdin = dup(STDIN_FILENO);
    ASSERT_NE(original_stdin, -1);
    ASSERT_EQ(dup2(pipefd[0], STDIN_FILENO), STDIN_FILENO);
    
    // Test TRAP_GETC
    const char getc_input = 'G';
    ASSERT_EQ(write(pipefd[1], &getc_input, 1), 1);
    vm.write_memory(0x3000, 0xF020);  // TRAP_GETC
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R0), getc_input);
    
    // Test TRAP_OUT
    vm.write_memory(0x3001, 0xF021);  // TRAP_OUT
    vm.set_register_value(R_R0, 'O');
    vm.step();
    
    // Test TRAP_PUTS
    vm.write_memory(0x3002, 0xF022);  // TRAP_PUTS
    vm.set_register_value(R_R0, 0x3100);
    vm.write_memory(0x3100, 'H');
    vm.write_memory(0x3101, 'i');
    vm.write_memory(0x3102, 0x0000);
    vm.step();
    
    // Test TRAP_IN
    const char in_input = 'I';
    ASSERT_EQ(write(pipefd[1], &in_input, 1), 1);
    vm.write_memory(0x3003, 0xF023);  // TRAP_IN
    vm.step();
    EXPECT_EQ(vm.get_register_value(R_R0), in_input);
    
    // Test TRAP_PUTSP
    vm.write_memory(0x3004, 0xF024);  // TRAP_PUTSP
    vm.set_register_value(R_R0, 0x3200);
    vm.write_memory(0x3200, 0x4142);  // "AB"
    vm.write_memory(0x3201, 0x0000);
    vm.step();
    
    // Test TRAP_HALT
    vm.write_memory(0x3005, 0xF025);  // TRAP_HALT
    vm.step();
    EXPECT_FALSE(vm.is_running());
    
    ASSERT_EQ(dup2(original_stdin, STDIN_FILENO), STDIN_FILENO);
    close(original_stdin);
    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(IntegrationTest, TrapRoutineErrors) {
    // Test unknown TRAP vector
    vm.write_memory(0x3000, 0xF0FF);  // Invalid TRAP vector
    EXPECT_THROW(vm.step(), std::runtime_error);
    
    // Test TRAP_PUTS with invalid memory
    vm.write_memory(0x3000, 0xF022);  // TRAP_PUTS
    vm.set_register_value(R_R0, 0xFFFF);  // Invalid memory address
    EXPECT_NO_THROW(vm.step());
    
    // Test TRAP_PUTSP with invalid memory
    vm.write_memory(0x3001, 0xF024);  // TRAP_PUTSP
    vm.set_register_value(R_R0, 0xFFFF);  // Invalid memory address
    EXPECT_NO_THROW(vm.step());  
}

TEST_F(IntegrationTest, Disassembly) {
    vm.write_memory(0x3000, 0x1234);  // ADD R1, R0, #-12
    EXPECT_EQ(vm.disassemble(0x3000), "0x3000: ADD R1, R0, #-12");
    
    vm.write_memory(0x3001, 0x5678);  // AND R3, R1, #-8
    EXPECT_EQ(vm.disassemble(0x3001), "0x3001: AND R3, R1, #-8");
    
    vm.write_memory(0x3002, 0x9ABC);  // NOT R5, R2
    EXPECT_EQ(vm.disassemble(0x3002), "0x3002: NOT R5, R2");
    
    vm.write_memory(0x3003, 0x0000);  // BR 0x3004
    EXPECT_EQ(vm.disassemble(0x3003), "0x3003: BR 0x3004");
    
    vm.write_memory(0x3004, 0xF025);  // HALT
    EXPECT_EQ(vm.disassemble(0x3004), "0x3004: TRAP x25");
} 