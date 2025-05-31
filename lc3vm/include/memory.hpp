#ifndef LC3_MEMORY_H
#define LC3_MEMORY_H

#include <cstdint>
#define MEMORY_MAX 65536

class Memory {
    public:
        std::uint16_t memory[MEMORY_MAX];
        std::uint16_t read(std::uint16_t address);
        void write(std::uint16_t address, std::uint16_t value);;
};

#endif // LC3_MEMORY_H
