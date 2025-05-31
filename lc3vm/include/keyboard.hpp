/**
 * @file keyboard.hpp
 * @brief Defines constants and functions related to LC-3 keyboard input.
 */
#ifndef LC3_KEYBOARD_H
#define LC3_KEYBOARD_H

#include <cstdint>

/**
 * @brief Enumeration for LC-3 keyboard related memory mapped registers and values.
 */
enum Keyboard {

    MR_KBSR = 0xFE00, // Keyboard Status Register
    MR_KBDR = 0xFE02, // Keyboard Data Register
    MR_KBSR_SHIFT = 15, // Keyboard Status Register Shift
    MR_KBDR_SHIFT = 0, // Keyboard Data Register Shift

};

/**
 * @brief Gets a key from standard input (non-blocking if possible, platform dependent).
 * This function is intended to be called when MR_KBSR indicates a key is ready.
 * @return The ASCII value of the key pressed as a std::uint16_t.
 * @note The original C implementation used custom terminal handling.
 *       This version might rely on standard input buffering.
 */
std::uint16_t get_key();

#endif // LC3_KEYBOARD_H