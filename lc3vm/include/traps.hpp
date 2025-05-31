/**
 * @file traps.hpp
 * @brief Defines the trap vector numbers for LC-3 system calls.
 */
#ifndef LC3_TRAPS_H
#define LC3_TRAPS_H

/**
 * @brief Enumeration of LC-3 trap vector numbers.
 * These vectors are used by the TRAP instruction to invoke system service routines.
 */
enum Traps {
    TRAP_GETC = 0x20, ///< Trap vector for getting a character from the keyboard (not echoed).
    TRAP_OUT = 0x21,  ///< Trap vector for printing a character to the console.
    TRAP_PUTS = 0x22, ///< Trap vector for printing a null-terminated string to the console.
    TRAP_IN = 0x23,   ///< Trap vector for getting a character from the keyboard (echoed) and prompting.
    TRAP_PUTSP = 0x24,///< Trap vector for printing a null-terminated string of packed characters (2 per word).
    TRAP_HALT = 0x25  ///< Trap vector for halting the program execution.
};
#endif // LC3_TRAPS_H