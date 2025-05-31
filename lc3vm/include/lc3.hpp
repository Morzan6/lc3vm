#ifndef LC3_H
#define LC3_H

#include "memory.hpp"
#include "registers.hpp"
#include "flags.hpp"
#include "opcodes.hpp"
#include <string>
#include <array>

class LC3State {
    private:
        Memory memory;
        std::array<std::uint16_t, R_COUNT> reg;
        // StatusFlags flags; // Removed as R_COND in reg is used directly
        // void (*opcodes[16])(std::uint16_t); // Removed as op_table in ls3.cpp is used
        void update_flags(std::uint16_t r);
        bool running;

        // op_table will be moved here as a static member
        static const std::array<void(*)(LC3State&, std::uint16_t), 16> op_table;

    public:
        template <unsigned op>
        static void ins(LC3State& state, std::uint16_t instr);

        LC3State();
        ~LC3State();
        void load_image(const std::string& filename);
        void run();
        void step();
        void disassemble(std::uint16_t address);
        void disassemble_all();
        void request_halt() { running = false; }

        // Test helper methods
        std::uint16_t get_register_value(Registers r) const { 
            if (r >= R_COUNT) throw std::out_of_range("Register index out of bounds");
            return reg[r]; 
        }
        void set_register_value(Registers r, std::uint16_t value) {
            if (r >= R_COUNT) throw std::out_of_range("Register index out of bounds");
            reg[r] = value; 
        }
        void write_memory(std::uint16_t address, std::uint16_t value) { 
            memory.write(address, value); 
        }
        std::uint16_t read_memory(std::uint16_t address) { // Added for completeness
            return memory.read(address);
        }

        static std::uint16_t sign_extend(std::uint16_t x, int bit_count) {
            if ((x >> (bit_count - 1)) & 1) {
                x |= (0xFFFF << bit_count);
            }
            return x;
        }
};


#endif // LC3_H