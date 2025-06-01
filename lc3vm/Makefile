CC = /usr/bin/g++

CFLAGS = -Wall -Wextra -std=c++14 -Iinclude -O2 -g

BUILD_DIR = build
TEST_DIR = tests
GTEST_FLAGS = -lgtest -lgtest_main -pthread

VM_SRCS = src/ls3.cpp src/memory.cpp src/terminal_input.cpp
VM_TEST_SRCS = $(VM_SRCS)

TEST_MAIN_OBJ = $(BUILD_DIR)/test_main.o
TEST_FILES = tests/test_initialization.cpp tests/test_opcode_execution.cpp tests/test_disassembly.cpp
TEST_OBJS = $(TEST_FILES:tests/%.cpp=$(BUILD_DIR)/tests/%.o)

.PHONY: all clean test docs

all: $(BUILD_DIR)/lc3vm

$(BUILD_DIR)/lc3vm: $(VM_SRCS) src/main.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(VM_SRCS) src/main.cpp -o $(BUILD_DIR)/lc3vm

test: $(BUILD_DIR)/test_runner
	$(BUILD_DIR)/test_runner

$(TEST_MAIN_OBJ): tests/test_main.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c tests/test_main.cpp -o $(TEST_MAIN_OBJ)

$(BUILD_DIR)/tests/%.o: tests/%.cpp
	mkdir -p $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_runner: $(TEST_MAIN_OBJ) $(TEST_OBJS) $(VM_SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_MAIN_OBJ) $(TEST_OBJS) $(VM_SRCS) -o $(BUILD_DIR)/test_runner $(GTEST_FLAGS) $(LDFLAGS_COVERAGE)

docs:
	@echo "Generating documentation with Doxygen..."
	@doxygen Doxyfile

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR) 