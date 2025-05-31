#ifndef TERMINAL_IO_HPP
#define TERMINAL_IO_HPP

#include <termios.h>
#include <unistd.h> // For STDIN_FILENO, tcgetattr, tcsetattr
#include <cstdio>   // For perror

// RAII class for managing terminal state (input buffering, echo)
class TerminalIO {
public:
    TerminalIO() : active(false) {
        if (tcgetattr(STDIN_FILENO, &original_tio) == -1) {
            perror("tcgetattr");
            return; // Constructor failed, active remains false
        }

        struct termios new_tio = original_tio;
        // Disable canonical mode (line buffering) and echo
        new_tio.c_lflag &= ~(ICANON | ECHO);
        // Set to apply changes immediately
        if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tio) == -1) {
            perror("tcsetattr");
            // Try to restore original settings if setup failed mid-way
            tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
            return; // Constructor failed, active remains false
        }
        active = true;
    }

    ~TerminalIO() {
        if (active) {
            if (tcsetattr(STDIN_FILENO, TCSANOW, &original_tio) == -1) {
                perror("tcsetattr (restore)");
            }
        }
    }

    // Deleted copy constructor and assignment operator to prevent issues
    TerminalIO(const TerminalIO&) = delete;
    TerminalIO& operator=(const TerminalIO&) = delete;

    bool is_active() const { return active; }

private:
    struct termios original_tio;
    bool active; // To track if constructor succeeded and restoration is needed
};

#endif // TERMINAL_IO_HPP 