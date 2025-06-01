/**
 * @file main.cpp
 * @brief Main entry point for the LC-3 virtual machine.
 * 
 * This program loads one or more LC-3 object files into the VM's memory
 * and starts execution. It also handles SIGINT for graceful shutdown.
 */
#include <iostream>
#include <string>
#include <vector>
#include <csignal>
#include "lc3.hpp"
#include "terminal_input.hpp" // For raw terminal mode
#include <cstdlib> // For std::atexit

/** 
 * @brief Global pointer to the LC3State instance.
 * Used by the signal handler to request the VM to halt.
 * This is null when no VM is active.
 */
LC3State* g_vm_ptr = nullptr;

/**
 * @brief Signal handler for SIGINT (Ctrl+C).
 * Requests the LC-3 VM to halt its execution gracefully.
 * @param sig The signal number (expected to be SIGINT).
 */
void handle_sigint(int sig) {
    if (sig == SIGINT) {
        std::cerr << "\nInterrupt signal received. Halting VM..." << std::endl;
        if (g_vm_ptr) {
            g_vm_ptr->request_halt();
        }
        disable_raw_mode();
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }
}

/**
 * @brief Main function for the LC-3 virtual machine.
 * 
 * Parses command-line arguments for LC-3 object image files, loads them into
 * the VM, sets up a SIGINT handler, and runs the VM.
 * @param argc The number of command-line arguments.
 * @param argv An array of C-style strings representing the command-line arguments.
 *             The first argument (argv[0]) is the program name.
 *             Subsequent arguments (argv[1]...) are paths to LC-3 object files.
 * @return 0 on successful execution and halt, 1 on error (e.g., file not found, runtime error).
 */
int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [-d|--disassemble] <image_file1> [image_file2] ..." << std::endl;
        return 1;
    }

    LC3State vm;
    g_vm_ptr = &vm;

    try {
        enable_raw_mode();
        std::atexit(disable_raw_mode);
    } catch (const std::exception& e) {
        std::cerr << "Terminal Setup Error: " << e.what() << std::endl;
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        perror("sigaction");
        g_vm_ptr = nullptr;
        return 1;
    }

    bool disassemble_mode = false;
    int first_image_arg_index = 1;

    if (argc > 1) {
        std::string first_arg = argv[1];
        if (first_arg == "-d" || first_arg == "--disassemble") {
            disassemble_mode = true;
            first_image_arg_index = 2;
            if (argc < 3) {
                 std::cerr << "Usage: " << argv[0] << " [-d|--disassemble] <image_file1> [image_file2] ..." << std::endl;
                 std::cerr << "Error: At least one image file is required for disassembly." << std::endl;
                 g_vm_ptr = nullptr;
                 return 1;
            }
        }
    }

    try {
        for (int i = first_image_arg_index; i < argc; ++i) {
            std::string filename = argv[i];
            std::cout << "Loading image: " << filename << std::endl;
            vm.load_image(filename);
        }

        if (disassemble_mode) {
            std::cout << "Disassembling memory..." << std::endl;
            vm.disassemble_all();
        } else {
            std::cout << "Starting LC-3 VM..." << std::endl;
            vm.run();
            std::cout << "LC-3 VM halted." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "VM Runtime Error: " << e.what() << std::endl;
        g_vm_ptr = nullptr;
        return 1;
    }

    g_vm_ptr = nullptr;
    return 0;
}
