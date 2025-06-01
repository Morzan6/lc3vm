#ifndef LC3_TERMINAL_INPUT_H
#define LC3_TERMINAL_INPUT_H

/**
 * @file terminal_input.hpp
 * @brief Provides functions to control terminal input mode (raw/canonical).
 */

/**
 * @brief Enables raw input mode for the terminal.
 * 
 * In raw mode:
 * - Input is unbuffered (character by character).
 * - Input is not echoed to the screen.
 * - Special character processing (like Ctrl+C for SIGINT) might be altered,
 *   so signal handling needs to be robust.
 * @throw std::runtime_error if enabling raw mode fails.
 */
void enable_raw_mode();

/**
 * @brief Disables raw input mode, restoring original terminal settings.
 * This function should be called before the program exits to ensure the
 * terminal is left in a usable state.
 */
void disable_raw_mode();

#endif // LC3_TERMINAL_INPUT_H 