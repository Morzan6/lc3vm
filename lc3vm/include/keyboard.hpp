#ifndef LC3_KEYBOARD_H
#define LC3_KEYBOARD_H

#include <cstdint>

enum Keyboard {

    MR_KBSR = 0xFE00, // Keyboard Status Register
    MR_KBDR = 0xFE02, // Keyboard Data Register
    MR_KBSR_SHIFT = 15, // Keyboard Status Register Shift
    MR_KBDR_SHIFT = 0, // Keyboard Data Register Shift

};

std::uint16_t get_key();

#endif // LC3_KEYBOARD_H