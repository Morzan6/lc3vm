/**
 * @file opcodes.hpp
 * @brief Defines the opcodes for the LC-3 instruction set.
 */
#ifndef LC3_OPCODES_H
#define LC3_OPCODES_H

/**
 * @brief Enumeration of LC-3 instruction opcodes.
 * Each opcode corresponds to a specific operation in the LC-3 instruction set.
 */
enum Opcodes {
    OP_BR = 0,      ///< Branch instruction (BR)
    OP_ADD = 1,     ///< Add instruction (ADD)
    OP_LD = 2,      ///< Load instruction (LD)
    OP_ST = 3,      ///< Store instruction (ST)
    OP_JSR = 4,     ///< Jump to Subroutine instruction (JSR/JSRR)
    OP_AND = 5,     ///< Bitwise AND instruction (AND)
    OP_LDR = 6,     ///< Load Base+offset instruction (LDR)
    OP_STR = 7,     ///< Store Base+offset instruction (STR)
    OP_RTI = 8,     ///< Return from Interrupt instruction (RTI) - Currently unused
    OP_NOT = 9,     ///< Bitwise NOT instruction (NOT)
    OP_LDI = 10,    ///< Load Indirect instruction (LDI)
    OP_STI = 11,    ///< Store Indirect instruction (STI)
    OP_JMP = 12,    ///< Jump instruction (JMP), also handles RET (JMP R7)
    OP_RES = 13,    ///< Reserved opcode - Currently unused
    OP_LEA = 14,    ///< Load Effective Address instruction (LEA)
    OP_TRAP = 15    ///< Execute Trap / System Call instruction (TRAP)
};

#endif // LC3_OPCODES_H