#include <iostream>
#include <string>
#include <vector>
#include <csignal>
#include "lc3.hpp"
#include "terminal_io.hpp"

// Global pointer to the VM for the signal handler
LC3State* g_vm_ptr = nullptr;

void handle_sigint(int sig) {
    if (sig == SIGINT) {
        std::cerr << "\nInterrupt signal received. Halting VM..." << std::endl;
        if (g_vm_ptr) {
            g_vm_ptr->request_halt();
        }
        // The TerminalIO destructor will handle restoring terminal settings.
        // Forcing an exit here might prevent cleanup by RAII objects.
        // If immediate exit is desired, consider std::quick_exit or other mechanisms
        // after ensuring resources are released.
    }
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_file1> [image_file2] ..." << std::endl;
        return 1;
    }

    TerminalIO term_io;
    if (!term_io.is_active()) {
        std::cerr << "Failed to initialize terminal settings. Exiting." << std::endl;
        return 1;
    }

    LC3State vm;
    g_vm_ptr = &vm;

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        perror("sigaction");
        g_vm_ptr = nullptr;
        return 1;
    }

    try {
        for (int i = 1; i < argc; ++i) {
            std::string filename = argv[i];
            std::cout << "Loading image: " << filename << std::endl;
            vm.load_image(filename);
        }

        std::cout << "Starting LC-3 VM..." << std::endl;
        vm.run();
        std::cout << "LC-3 VM halted." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "VM Runtime Error: " << e.what() << std::endl;
        g_vm_ptr = nullptr;
        return 1;
    }

    g_vm_ptr = nullptr;
    return 0;
}
