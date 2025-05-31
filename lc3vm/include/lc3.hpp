/**
 * @file lc3.hpp
 * @brief Defines the main LC3State class for the LC-3 virtual machine.
 * 
 * This file contains the definition of the LC3State class, which encapsulates
 * the entire state of the LC-3 VM, including memory, registers, and control logic.
 */
#ifndef LC3_H
#define LC3_H

#include "memory.hpp"
#include "registers.hpp"
#include "flags.hpp"
#include "opcodes.hpp"
#include <string>
#include <array>

/**
 * @brief Represents the state of an LC-3 virtual machine.
 *
 * This class holds all components of the LC-3 architecture, including memory,
 * registers, the program counter (PC), and condition flags. It provides methods
 * to load programs, run the VM, step through instructions, and inspect/modify its state.
 */
class LC3State {
    private:
        Memory memory;
        /**
         * @brief Array of 16-bit registers.
         * This array holds the values of the 16 registers in the LC-3 architecture.
         * The size of the array is determined by the R_COUNT macro.
         */
        std::array<std::uint16_t, R_COUNT> reg;
        
        /**
         * @brief Updates the condition flags (R_COND) based on the value of a register.
         * @param r The index of the register whose value will be used to set the condition flags.
         *          It is expected to be one of the general-purpose registers (R0-R7).
         */
        void update_flags(std::uint16_t r);
        bool running; ///< Flag indicating whether the LC-3 VM is currently running.

     
        /**
         * @brief Table of function pointers for dispatching LC-3 opcodes.
         * Each entry corresponds to an opcode and points to its handler function.
         */
        static const std::array<void(*)(LC3State&, std::uint16_t), 16> op_table;

    public:
        /**
         * @brief Executes a specific LC-3 instruction.
         * This is a template function specialized for each opcode.
         * @tparam op The opcode to execute (e.g., OP_ADD, OP_LD).
         * @param state Reference to the current LC3State.
         * @param instr The 16-bit instruction word.
         */
        template <unsigned op>
        static void ins(LC3State& state, std::uint16_t instr);

        /**
         * @brief Constructs a new LC3State object.
         * Initializes registers (PC to 0x3000, COND to FL_ZRO) and sets the VM to running.
         */
        LC3State();
        /**
         * @brief Destroys the LC3State object.
         */
        ~LC3State();
        /**
         * @brief Loads an LC-3 program image into memory.
         * @param filename The path to the .obj file to load.
         * @throw std::runtime_error if the file cannot be opened or is malformed.
         */
        void load_image(const std::string& filename);
        /**
         * @brief Runs the LC-3 virtual machine until halted.
         * Continuously fetches, decodes, and executes instructions.
         */
        void run();
        /**
         * @brief Executes a single LC-3 instruction.
         * Fetches the instruction at PC, increments PC, and executes the instruction.
         * @throw std::runtime_error if an illegal or unsupported opcode is encountered.
         */
        void step();
        /**
         * @brief Disassembles the instruction at a given memory address.
         * (Currently not implemented)
         * @param address The memory address of the instruction to disassemble.
         */
        void disassemble(std::uint16_t address);
        /**
         * @brief Disassembles all instructions in memory.
         * (Currently not implemented)
         */
        void disassemble_all();
        /**
         * @brief Requests the VM to halt execution after the current instruction.
         * Sets the internal running flag to false.
         */
        void request_halt() { running = false; }

        // Test helper methods
        /**
         * @brief Retrieves the value of a specified register.
         * Used for testing purposes.
         * @param r The register to read (from Registers enum).
         * @return The 16-bit value of the register.
         * @throw std::out_of_range if the register index is invalid.
         */
        std::uint16_t get_register_value(Registers r) const { 
            if (r >= R_COUNT) throw std::out_of_range("Register index out of bounds");
            return reg[r]; 
        }
        /**
         * @brief Sets the value of a specified register.
         * Used for testing purposes.
         * @param r The register to write to (from Registers enum).
         * @param value The 16-bit value to set.
         * @throw std::out_of_range if the register index is invalid.
         */
        void set_register_value(Registers r, std::uint16_t value) {
            if (r >= R_COUNT) throw std::out_of_range("Register index out of bounds");
            reg[r] = value; 
        }
        /**
         * @brief Writes a value to a specified memory address.
         * Used for testing purposes.
         * @param address The memory address to write to.
         * @param value The 16-bit value to write.
         */
        void write_memory(std::uint16_t address, std::uint16_t value) { 
            memory.write(address, value); 
        }
        /**
         * @brief Reads a value from a specified memory address.
         * Used for testing purposes.
         * @param address The memory address to read from.
         * @return The 16-bit value at the specified address.
         */
        std::uint16_t read_memory(std::uint16_t address) { // Added for completeness
            return memory.read(address);
        }

        /**
         * @brief Sign-extends a number to 16 bits.
         * @param x The number to sign-extend.
         * @param bit_count The number of bits in the original number (e.g., 5 for IMM5, 9 for PCoffset9).
         * @return The 16-bit sign-extended value.
         */
        static std::uint16_t sign_extend(std::uint16_t x, int bit_count) {
            if ((x >> (bit_count - 1)) & 1) {
                x |= (0xFFFF << bit_count);
            }
            return x;
        }
};


#endif // LC3_H