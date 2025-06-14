/**
 * @file lc3.cpp
 * @brief Implements the LC3State class methods for the LC-3 virtual machine.
 * 
 * This file contains the definitions for the LC-3 instruction set, trap handlers,
 * program loading, and the main execution loop of the LC-3 VM.
 */
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "lc3.hpp"
#include "memory.hpp"
#include "registers.hpp"
#include "opcodes.hpp"
#include <iostream>
#include <array>
#include "traps.hpp"
#include "flags.hpp"
#include "keyboard.hpp"
#include <unistd.h>
#include <sstream>
#include <iomanip>

/**
 * @brief Swaps the endianness of a 16-bit unsigned integer.
 * Useful for reading LC-3 object files which are typically big-endian.
 * @param x The 16-bit value to swap.
 * @return The endian-swapped 16-bit value.
 */
static std::uint16_t swap16(std::uint16_t x) {
    return (x << 8) | (x >> 8);
}

template <unsigned op>
void LC3State::ins(LC3State& state, std::uint16_t instr) {
    if (op == OP_BR) {
        std::uint16_t cond_flag_from_instr = (instr >> 9) & 0x7;
        if (cond_flag_from_instr & state.reg[R_COND]) {
            state.reg[R_PC] += sign_extend(instr & 0x1FF, 9);
        }
    }
    else if (op == OP_ADD) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t r1 = (instr >> 6) & 0x7;
        std::uint16_t imm_flag = (instr >> 5) & 0x1;
        if (imm_flag) {
            std::uint16_t imm5 = sign_extend(instr & 0x1F, 5);
            state.reg[r0] = state.reg[r1] + imm5;
        } else {
            std::uint16_t r2 = instr & 0x7;
            state.reg[r0] = state.reg[r1] + state.reg[r2];
        }
        state.update_flags(r0);
    }
    else if (op == OP_LD) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t pc_offset9 = sign_extend(instr & 0x1FF, 9);
        state.reg[r0] = state.memory.read(state.reg[R_PC] + pc_offset9);
        state.update_flags(r0);
    }
    else if (op == OP_ST) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t pc_offset9 = sign_extend(instr & 0x1FF, 9);
        state.memory.write(state.reg[R_PC] + pc_offset9, state.reg[r0]);
    }
    else if (op == OP_JSR) {
        state.reg[R_R7] = state.reg[R_PC];
        std::uint16_t long_flag = (instr >> 11) & 1;
        if (long_flag) {
            std::uint16_t long_pc_offset = sign_extend(instr & 0x7FF, 11);
            state.reg[R_PC] += long_pc_offset;
        } else {
            std::uint16_t r1 = (instr >> 6) & 0x7;
            state.reg[R_PC] = state.reg[r1];
        }
    }
    else if (op == OP_AND) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t r1 = (instr >> 6) & 0x7;
        std::uint16_t imm_flag = (instr >> 5) & 0x1;
        if (imm_flag) {
            std::uint16_t imm5 = sign_extend(instr & 0x1F, 5);
            state.reg[r0] = state.reg[r1] & imm5;
        } else {
            std::uint16_t r2 = instr & 0x7;
            state.reg[r0] = state.reg[r1] & state.reg[r2];
        }
        state.update_flags(r0);
    }
    else if (op == OP_LDR) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t r1 = (instr >> 6) & 0x7;
        std::uint16_t offset6 = sign_extend(instr & 0x3F, 6);
        state.reg[r0] = state.memory.read(state.reg[r1] + offset6);
        state.update_flags(r0);
    }
    else if (op == OP_STR) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t r1 = (instr >> 6) & 0x7;
        std::uint16_t offset6 = sign_extend(instr & 0x3F, 6);
        state.memory.write(state.reg[r1] + offset6, state.reg[r0]);
    }
    else if (op == OP_NOT) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t r1 = (instr >> 6) & 0x7;
        state.reg[r0] = ~state.reg[r1];
        state.update_flags(r0);
    }
    else if (op == OP_LDI) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t pc_offset9 = sign_extend(instr & 0x1FF, 9);
        std::uint16_t effective_address_location = state.reg[R_PC] + pc_offset9;
        state.reg[r0] = state.memory.read(state.memory.read(effective_address_location));
        state.update_flags(r0);
    }
    else if (op == OP_STI) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t pc_offset9 = sign_extend(instr & 0x1FF, 9);
        std::uint16_t effective_address_location = state.reg[R_PC] + pc_offset9;
        state.memory.write(state.memory.read(effective_address_location), state.reg[r0]);
    }
    else if (op == OP_JMP) {
        std::uint16_t r1 = (instr >> 6) & 0x7;
        state.reg[R_PC] = state.reg[r1];
    }
    else if (op == OP_LEA) {
        std::uint16_t r0 = (instr >> 9) & 0x7;
        std::uint16_t pc_offset9 = sign_extend(instr & 0x1FF, 9);
        state.reg[r0] = state.reg[R_PC] + pc_offset9;
        state.update_flags(r0);
    }
    else if (op == OP_TRAP) {
        state.reg[R_R7] = state.reg[R_PC];
        switch (instr & 0xFF) {
            case TRAP_GETC:
                {
                    if (state.memory.test_mode) {
                        state.reg[R_R0] = state.memory.read(Keyboard::MR_KBDR);
                    } else {
                        char c_in = 0;
                        if (read(STDIN_FILENO, &c_in, 1) == 1) {
                            state.reg[R_R0] = static_cast<std::uint16_t>(c_in);
                        }
                    }
                }
                state.update_flags(R_R0);
                break;
            case TRAP_OUT:
                if (!state.memory.test_mode) {
                    std::cout.put(static_cast<char>(state.reg[R_R0]));
                    std::cout.flush();
                }
                break;
            case TRAP_PUTS: {
                if (!state.memory.test_mode) {
                    std::uint16_t current_char_addr = state.reg[R_R0];
                    std::uint16_t val = state.memory.read(current_char_addr);
                    while (val != 0) {
                        std::cout.put(static_cast<char>(val));
                        current_char_addr++;
                        val = state.memory.read(current_char_addr);
                    }
                    std::cout.flush();
                }
                break;
            }
            case TRAP_IN: {
                if (state.memory.test_mode) {
                    state.reg[R_R0] = state.memory.read(Keyboard::MR_KBDR);
                } else {
                    std::cout << "Enter a character: ";
                    std::cout.flush();
                    char c_in_trap = 0;
                    if (read(STDIN_FILENO, &c_in_trap, 1) == 1) {
                        std::cout.put(c_in_trap);
                        std::cout.flush();
                        state.reg[R_R0] = static_cast<std::uint16_t>(c_in_trap);
                    }
                }
                state.update_flags(R_R0);
                break;
            }
            case TRAP_PUTSP: {
                if (!state.memory.test_mode) {
                    std::uint16_t current_addr = state.reg[R_R0];
                    std::uint16_t word = state.memory.read(current_addr);
                    while (word != 0) {
                        char char1 = word & 0xFF;
                        std::cout.put(char1);
                        char char2 = (word >> 8) & 0xFF;
                        if (char2) {
                            std::cout.put(char2);
                        }
                        current_addr++;
                        word = state.memory.read(current_addr);
                    }
                    std::cout.flush();
                }
                break;
            }
            case TRAP_HALT:
                if (!state.memory.test_mode) {
                    std::cout << "HALT" << std::endl;
                }
                state.running = false;
                break;
            default:
                throw std::runtime_error("Unknown TRAP vector: " + std::to_string(instr & 0xFF));
                break;
        }
    }
}

const std::array<void(*)(LC3State&, std::uint16_t), 16> LC3State::op_table = {
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_BR>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_ADD>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_LD>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_ST>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_JSR>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_AND>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_LDR>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_STR>(s,i); },
    nullptr,
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_NOT>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_LDI>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_STI>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_JMP>(s,i); },
    nullptr,
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_LEA>(s,i); },
    [](LC3State& s, std::uint16_t i){ LC3State::ins<OP_TRAP>(s,i); }
};

void LC3State::load_image(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open image file: " + filename);
    }

    std::uint16_t origin;
    file.read(reinterpret_cast<char*>(&origin), sizeof(origin));
    if (file.gcount() != sizeof(origin)) {
        throw std::runtime_error("Failed to read origin from image file: " + filename);
    }
    origin = swap16(origin);

    std::uint16_t max_words_to_read = MEMORY_MAX - origin;
    std::vector<std::uint16_t> image_data(max_words_to_read);

    file.read(reinterpret_cast<char*>(image_data.data()), max_words_to_read * sizeof(std::uint16_t));
    size_t words_actually_read = file.gcount() / sizeof(std::uint16_t);

    for (size_t i = 0; i < words_actually_read; ++i) {
        if (origin + i < MEMORY_MAX) {
            this->memory.write(origin + i, swap16(image_data[i]));
        } else {
            throw std::runtime_error("Image too large for memory at specified origin.");
        }
    }
    if (words_actually_read > 0) {
        loaded_code_segments.push_back({origin, static_cast<std::uint16_t>(words_actually_read)});
    }
}

LC3State::LC3State() : memory(), reg{}, running(true) {
    this->reg[R_PC] = 0x3000;
    this->reg[R_COND] = FL_ZRO;
}

LC3State::~LC3State() {
}

void LC3State::run() {
    this->running = true;
    while (this->running) {
        step();
    }
}

void LC3State::step() {
    if (!this->running) return;

    std::uint16_t current_pc = this->reg[R_PC];
    this->reg[R_PC]++;
    
    std::uint16_t instruction = this->memory.read(current_pc);
    std::uint16_t opcode_val = instruction >> 12;

    if (opcode_val >= op_table.size() || !op_table[opcode_val]) {
        throw std::runtime_error("Illegal or unsupported opcode: " + std::to_string(opcode_val) + 
                                 " at PC: " + std::to_string(current_pc));
    }
    
    op_table[opcode_val](*this, instruction);
}

void LC3State::update_flags(std::uint16_t r_idx) {
    if (this->reg[r_idx] == 0) {
        this->reg[R_COND] = FL_ZRO;
    } else if (this->reg[r_idx] >> 15) {
        this->reg[R_COND] = FL_NEG;
    } else {
        this->reg[R_COND] = FL_POS;
    }
}

std::string LC3State::disassemble(std::uint16_t address) {
    std::uint16_t instr = memory.read(address);
    std::uint16_t opcode = instr >> 12;
    std::ostringstream oss;

    oss << "0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << address << ": ";

    switch (opcode) {
        case OP_ADD: {
            std::uint16_t dr = (instr >> 9) & 0x7;
            std::uint16_t sr1 = (instr >> 6) & 0x7;
            oss << "ADD R" << dr << ", R" << sr1 << ", ";
            if ((instr >> 5) & 0x1) {
                std::int16_t imm5 = static_cast<std::int16_t>(sign_extend(instr & 0x1F, 5));
                oss << "#" << std::dec << imm5;
            } else {
                std::uint16_t sr2 = instr & 0x7;
                oss << "R" << sr2;
            }
            break;
        }
        case OP_AND: {
            std::uint16_t dr = (instr >> 9) & 0x7;
            std::uint16_t sr1 = (instr >> 6) & 0x7;
            oss << "AND R" << dr << ", R" << sr1 << ", ";
            if ((instr >> 5) & 0x1) {
                std::int16_t imm5 = static_cast<std::int16_t>(sign_extend(instr & 0x1F, 5));
                oss << "#" << std::dec << imm5;
            } else {
                std::uint16_t sr2 = instr & 0x7;
                oss << "R" << sr2;
            }
            break;
        }
        case OP_NOT: {
            std::uint16_t dr = (instr >> 9) & 0x7;
            std::uint16_t sr = (instr >> 6) & 0x7;
            oss << "NOT R" << dr << ", R" << sr;
            break;
        }
        case OP_BR: {
            std::uint16_t n = (instr >> 11) & 0x1;
            std::uint16_t z = (instr >> 10) & 0x1;
            std::uint16_t p = (instr >> 9) & 0x1;
            std::int16_t pc_offset9 = static_cast<std::int16_t>(sign_extend(instr & 0x1FF, 9));
            oss << "BR" << (n ? "n" : "") << (z ? "z" : "") << (p ? "p" : "") << " 0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << (address + 1 + pc_offset9);
            break;
        }
        case OP_JMP: {
            std::uint16_t base_r = (instr >> 6) & 0x7;
            if (base_r == 7) {
                 oss << "RET";
            } else {
                 oss << "JMP R" << base_r;
            }
            break;
        }
        case OP_JSR: {
            oss << "JSR ";
            if ((instr >> 11) & 1) {
                std::int16_t pc_offset11 = static_cast<std::int16_t>(sign_extend(instr & 0x7FF, 11));
                oss << "0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << (address + 1 + pc_offset11);
            } else {
                std::uint16_t base_r = (instr >> 6) & 0x7;
                oss << "R" << base_r;
            }
            break;
        }
        case OP_LD: {
            std::uint16_t dr = (instr >> 9) & 0x7;
            std::int16_t pc_offset9 = static_cast<std::int16_t>(sign_extend(instr & 0x1FF, 9));
            oss << "LD R" << dr << ", 0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << (address + 1 + pc_offset9);
            break;
        }
        case OP_LDI: {
            std::uint16_t dr = (instr >> 9) & 0x7;
            std::int16_t pc_offset9 = static_cast<std::int16_t>(sign_extend(instr & 0x1FF, 9));
            oss << "LDI R" << dr << ", 0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << (address + 1 + pc_offset9);
            break;
        }
        case OP_LDR: {
            std::uint16_t dr = (instr >> 9) & 0x7;
            std::uint16_t base_r = (instr >> 6) & 0x7;
            std::int16_t offset6 = static_cast<std::int16_t>(sign_extend(instr & 0x3F, 6));
            oss << "LDR R" << dr << ", R" << base_r << ", #" << std::dec << offset6;
            break;
        }
        case OP_LEA: {
            std::uint16_t dr = (instr >> 9) & 0x7;
            std::int16_t pc_offset9 = static_cast<std::int16_t>(sign_extend(instr & 0x1FF, 9));
            oss << "LEA R" << dr << ", 0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << (address + 1 + pc_offset9);
            break;
        }
        case OP_ST: {
            std::uint16_t sr = (instr >> 9) & 0x7;
            std::int16_t pc_offset9 = static_cast<std::int16_t>(sign_extend(instr & 0x1FF, 9));
            oss << "ST R" << sr << ", 0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << (address + 1 + pc_offset9);
            break;
        }
        case OP_STI: {
            std::uint16_t sr = (instr >> 9) & 0x7;
            std::int16_t pc_offset9 = static_cast<std::int16_t>(sign_extend(instr & 0x1FF, 9));
            oss << "STI R" << sr << ", 0x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(4) << (address + 1 + pc_offset9);
            break;
        }
        case OP_STR: {
            std::uint16_t sr = (instr >> 9) & 0x7;
            std::uint16_t base_r = (instr >> 6) & 0x7;
            std::int16_t offset6 = static_cast<std::int16_t>(sign_extend(instr & 0x3F, 6));
            oss << "STR R" << sr << ", R" << base_r << ", #" << std::dec << offset6;
            break;
        }
        case OP_TRAP: {
            oss << "TRAP x" << std::nouppercase << std::hex << std::setfill('0') << std::setw(2) << (instr & 0xFF);
            break;
        }
        case OP_RES:
        case OP_RTI:
        default:
            oss << "BAD OPCODE";
            break;
    }
    return oss.str();
}

void LC3State::disassemble_all() {
    if (loaded_code_segments.empty()) {
        std::cout << "No program images loaded. Nothing to disassemble." << std::endl;
        return;
    }

    for (const auto& segment : loaded_code_segments) {
        for (std::uint16_t i = 0; i < segment.size; ++i) {
            std::uint16_t current_address = segment.start_address + i;
            std::cout << disassemble(current_address) << std::endl;
        }
    }
}