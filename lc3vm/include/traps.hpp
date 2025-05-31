#ifndef LC3_TRAPS_H
#define LC3_TRAPS_H

enum Traps {
    TRAP_GETC = 0x20, // Get character from keyboard
    TRAP_OUT = 0x21, // Print character
    TRAP_PUTS = 0x22, // Print string
    TRAP_IN = 0x23, // Read integer
    TRAP_PUTSP = 0x24, // Print string with newline
    TRAP_HALT = 0x25, // Halt the program
};
#endif // LC3_TRAPS_H