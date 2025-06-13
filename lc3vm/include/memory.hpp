/**
 * @file memory.hpp
 * @brief Defines the Memory class for the LC-3 virtual machine.
 */
#ifndef LC3_MEMORY_H
#define LC3_MEMORY_H

#include <cstdint>

/** @brief Maximum memory addressable by the LC-3 (2^16 locations). */
#define MEMORY_MAX 65536

/**
 * @brief Represents the memory unit of the LC-3 VM.
 *
 * This class manages the 65536 x 16-bit word addressable memory space.
 * It provides methods for reading from and writing to memory, and handles
 * memory-mapped I/O like keyboard status and data registers.
 */
class Memory {
    public:
        /** 
         * @brief The main memory array.
         * Stores 65536 16-bit words.
         */
        std::uint16_t memory[MEMORY_MAX];

        /**
         * @brief Flag to indicate if we're in test mode.
         * When true, keyboard input is simulated using memory values.
         */
        bool test_mode = false;

        /**
         * @brief Reads a 16-bit word from the specified memory address.
         * Handles memory-mapped I/O for keyboard status (MR_KBSR) and 
         * data (MR_KBDR) registers.
         * @param address The 16-bit memory address to read from.
         * @return The 16-bit value stored at the given address.
         * @see MR_KBSR, MR_KBDR
         */
        std::uint16_t read(std::uint16_t address);
        /**
         * @brief Writes a 16-bit word to the specified memory address.
         * @param address The 16-bit memory address to write to.
         * @param value The 16-bit value to write.
         */
        void write(std::uint16_t address, std::uint16_t value);
};

#endif // LC3_MEMORY_H
