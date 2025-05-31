/**
 * @file memory.cpp
 * @brief Implements the Memory class methods for the LC-3 virtual machine.
 * 
 * This file contains the definitions for memory read and write operations,
 * including handling for memory-mapped keyboard input.
 */
#include "memory.hpp"
#include "keyboard.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>


void Memory::write(std::uint16_t address, std::uint16_t value) {
    memory[address] = value;
}


std::uint16_t check_key() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

std::uint16_t Memory::read(std::uint16_t address) {
    if (address == Keyboard::MR_KBSR) {
        if (check_key()) {
            memory[Keyboard::MR_KBSR] = (1 << 15);
            char c_in;
            std::cin.get(c_in);
            memory[Keyboard::MR_KBDR] = static_cast<std::uint16_t>(c_in);
        } else {
            memory[Keyboard::MR_KBSR] = 0;
        }
    }
    return memory[address];
}