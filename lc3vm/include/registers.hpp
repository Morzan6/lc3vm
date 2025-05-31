/**
 * @file registers.hpp
 * @brief Defines the register set for the LC-3 VM.
 */
#include <cstdint>
#ifndef LC3_REGISTERS_H
#define LC3_REGISTERS_H

/**
 * @brief Enumeration of LC-3 registers.
 * Includes general-purpose registers (R0-R7), Program Counter (PC),
 * and Condition Flags register (COND).
 */
enum Registers {
    R_R0 = 0, ///< General purpose register R0
    R_R1,     ///< General purpose register R1
    R_R2,     ///< General purpose register R2
    R_R3,     ///< General purpose register R3
    R_R4,     ///< General purpose register R4
    R_R5,     ///< General purpose register R5
    R_R6,     ///< General purpose register R6
    R_R7,     ///< General purpose register R7 (also used as subroutine linkage)
    R_PC,     ///< Program Counter register
    R_COND,   ///< Condition Flags register (stores N, Z, or P flag)
    R_COUNT   ///< Total number of registers (used for array sizing)
};


#endif // LC3_REGISTERS_H