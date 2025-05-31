/**
 * @file flags.hpp
 * @brief Defines the condition flags for the LC-3 VM.
 */
#ifndef LC3_FLAGS_H
#define LC3_FLAGS_H

/**
 * @brief Enumeration for LC-3 condition flags.
 * These flags are set by arithmetic and logical operations.
 */
enum StatusFlags {
    FL_POS = 1 << 0,  ///< Positive condition flag (P)
    FL_ZRO = 1 << 1,  ///< Zero condition flag (Z)
    FL_NEG = 1 << 2   ///< Negative condition flag (N)
};

#endif // LC3_FLAGS_H