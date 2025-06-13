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

## Building the Project

The project can be built using either CMake or Make. Choose your preferred method below.

### Using CMake

1. **Create a build directory and navigate to it**:

    ```bash
    mkdir build
    cd build
    ```

2. **Generate build files**:

    ```bash
    cmake ..
    ```

3. **Build the project**:

    ```bash
    cmake --build .
    ```

    This will create the following executables in the `build` directory:
    * `lc3vm`: The main VM executable
    * `test_runner`: The test executable

4. **Run tests** (optional):

    ```bash
    ./test_runner
    ```

5. **Generate documentation** (optional):

    ```bash
    cmake --build . --target docs
    ```

### Using Make

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

## Disassembling Object Files

The LC-3 VM can also disassemble `.obj` files, showing you the LC-3 assembly instructions corresponding to the machine code in the file. This is useful for inspecting programs or debugging.

**How to use:**

To disassemble one or more object files, use the `-d` or `--disassemble` flag followed by the path(s) to your `.obj` file(s):

```bash
./lc3vm/build/lc3vm -d path/to/your_program.obj
```

Or for multiple files:

```bash
./lc3vm/build/lc3vm --disassemble program1.obj program2.obj
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
