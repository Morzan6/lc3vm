#ifndef LC3_OPCODES_H
#define LC3_OPCODES_H

enum Opcodes {
    OP_BR = 0,      // Branch
    OP_ADD = 1,      // Add
    OP_LD = 2,       // Load
    OP_ST = 3,       // Store
    OP_JSR = 4,      // Jump to Subroutine 
    OP_AND = 5,      // Bitwise AND (C code: 5)
    OP_LDR = 6,      // Load Register (C code: 6)
    OP_STR = 7,      // Store Register (C code: 7)
    OP_RTI = 8,      // Return from Interrupt (C code: 8, unused)
    OP_NOT = 9,      // Bitwise NOT (C code: 9)
    OP_LDI = 10,     // Load Indirect (C code: 10)
    OP_STI = 11,     // Store Indirect (C code: 11)
    OP_JMP = 12,     // Jump (C code: 12, distinct from JSR)
    OP_RES = 13,     // Reserved (C code: 13, unused)
    OP_LEA = 14,     // Load Effective Address (C code: 14)
    OP_TRAP = 15     // Trap/System Call (C code: 15)
};

#endif // LC3_OPCODES_H