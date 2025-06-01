#include "terminal_input.hpp"
#include <termios.h> 
#include <unistd.h>    
#include <stdexcept> 
#include <iostream>  

static struct termios original_termios;
static bool raw_mode_enabled = false;

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        perror("tcgetattr");
        throw std::runtime_error("Failed to get terminal attributes.");
    }

    struct termios raw = original_termios;

    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);

    raw.c_cflag |= (CS8);

    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);

    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        throw std::runtime_error("Failed to set terminal to raw mode.");
    }
    raw_mode_enabled = true;
}

void disable_raw_mode() {
    if (raw_mode_enabled) {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) {
            perror("disable_raw_mode: tcsetattr");
        }
        raw_mode_enabled = false;
    }
} 