#ifndef LC3_TERMINAL_INPUT_H
#define LC3_TERMINAL_INPUT_H

/**
 * @file terminal_input.hpp
 * @brief Provides functions to control terminal input mode (raw/canonical).
 */

/**
 * @brief Enables raw mode for terminal input.
 * This disables echo, canonical mode, and other terminal features
 * to allow for direct character input.
 * @throw std::runtime_error if terminal attributes cannot be modified.
 */
void enable_raw_mode();

/**
 * @brief Disables raw mode and restores original terminal settings.
 * This should be called before the program exits to ensure the terminal
 * is left in a usable state.
 */
void disable_raw_mode();

/**
 * @brief Checks if raw mode is currently enabled.
 * @return true if raw mode is enabled, false otherwise.
 */
bool is_raw_mode_enabled();

#endif // LC3_TERMINAL_INPUT_H 