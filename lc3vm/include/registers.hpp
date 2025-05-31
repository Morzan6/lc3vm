#include <cstdint>
#ifndef LC3_REGISTERS_H
#define LC3_REGISTERS_H

enum Registers {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,    // Program Counter
    R_COND,  // Condition Flags
    R_COUNT  // Total register count
};

extern std::uint16_t registers[R_COUNT];

#endif // LC3_REGISTERS_H