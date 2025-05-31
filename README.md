# LC-3 Virtual Machine (lc3vm)

HSE Student project for an LC-3 (Little Computer 3) Virtual Machine implementation in C++.

## Overview

This project implements a functional LC-3 virtual machine that can load and execute LC-3 assembly programs (`.obj` files). It includes a CPU, memory, and support for various LC-3 instructions and trap routines for basic I/O.

## Features

* **Instruction Set**: Implements the standard LC-3 opcodes:
  * `ADD`, `AND`, `NOT`
  * `BR` (Branch)
  * `JMP` (Jump), `RET` (Return from Subroutine via `JMP R7`)
  * `JSR`, `JSRR` (Jump to Subroutine)
  * `LD`, `LDI`, `LDR` (Load)
  * `ST`, `STI`, `STR` (Store)
  * `LEA` (Load Effective Address)
  * `RTI` (Return from Interrupt - placeholder, not fully implemented)
  * `RES` (Reserved - placeholder)
* **Trap Routines**: Supports common TRAP routines:
  * `TRAP_GETC`: Get character from keyboard (not echoed).
  * `TRAP_OUT`: Output a character to the console.
  * `TRAP_PUTS`: Output a null-terminated string.
  * `TRAP_IN`: Get character from keyboard (echoed) with prompt.
  * `TRAP_PUTSP`: Output a null-terminated string of packed characters.
  * `TRAP_HALT`: Halt the program.
* **Memory-mapped I/O**: Keyboard Status Register (`KBSR`) and Keyboard Data Register (`KBDR`) are implemented.
* **Unit Tests**: Includes a suite of unit tests using Google Test to verify instruction behavior.
* **Documentation**: Source code documentation can be generated using Doxygen.
* **CI/CD**: Basic GitHub Actions workflow for building and testing on push/pull request.

## Dependencies

To build and run this project, you will need:

* `g++` (with C++14 support or newer)
* `make`
* Google Test (`libgtest-dev` on Debian/Ubuntu systems)
* `doxygen` (optional, for generating documentation)
* `graphviz` (optional, for Doxygen diagrams)

## Building the Project

The project uses a Makefile located in the `lc3vm/` directory.

1. **Navigate to the `lc3vm` directory**:

    ```bash
    cd lc3vm
    ```

2. **Build the VM**:

    ```bash
    make
    ```

    This will create the executable `build/lc3vm` inside the `lc3vm` directory.
3. **Build the tests**:
    The test executable `build/test_runner` (inside `lc3vm`) is also built by the default `make` target or specifically with `make test_runner`.

4. **Clean build files**:

    ```bash
    make clean
    ```

## Running the VM

To run an LC-3 object file:

```bash
./lc3vm/build/lc3vm path/to/your_program.obj [path/to/another_program.obj ...]
```

## Running Tests

To compile and run the unit tests (from within the `lc3vm` directory):

```bash
make test
```

## Project Structure

```sh
.
├── .github/            # GitHub Actions workflows
├── docs/               # Generated Doxygen documentation (after 'make docs')
├── lc3vm/              # Main directory for the VM
│   ├── Doxyfile        # Doxygen configuration file
│   ├── Makefile        # Makefile for building the VM and tests
│   ├── build/          # Build output directory (executables, object files)
│   ├── include/        # Header files for the LC-3 VM
│   ├── src/            # Source files for the LC-3 VM
│   └── tests/          # Unit test source files (using Google Test)
├── README.md           # This file
└── LICENSE             # Project License
```
