CC = /usr/bin/g++

# Coverage flags
COVERAGE_FLAGS = -g -O0 --coverage
COVERAGE_LDFLAGS = --coverage

# Default CFLAGS without coverage
CFLAGS = -Wall -Wextra -std=c++17 -Iinclude -O2 -g

# Coverage-enabled CFLAGS
COVERAGE_CFLAGS = $(CFLAGS) $(COVERAGE_FLAGS)

BUILD_DIR = build
TEST_DIR = tests
GTEST_FLAGS = -lgtest -lgtest_main -pthread

VM_SRCS = src/lc3.cpp src/memory.cpp src/terminal_input.cpp
VM_TEST_SRCS = $(VM_SRCS)

TEST_MAIN_OBJ = $(BUILD_DIR)/test_main.o
TEST_FILES = tests/test_initialization.cpp \
             tests/test_opcode_execution.cpp \
             tests/test_disassembly.cpp \
             tests/test_integration.cpp
TEST_OBJS = $(TEST_FILES:tests/%.cpp=$(BUILD_DIR)/tests/%.o)

.PHONY: all clean test docs coverage coverage-clean

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
	$(CC) $(CFLAGS) $(TEST_MAIN_OBJ) $(TEST_OBJS) $(VM_SRCS) -o $(BUILD_DIR)/test_runner $(GTEST_FLAGS)

docs:
	@echo "Generating documentation with Doxygen..."
	@doxygen Doxyfile

# Coverage targets
coverage: CFLAGS := $(COVERAGE_CFLAGS)
coverage: LDFLAGS := $(COVERAGE_LDFLAGS)
coverage: clean test
	@echo "Generating coverage report..."
	@mkdir -p $(BUILD_DIR)/coverage
	@echo "Coverage Summary:"
	@gcovr --root . \
		--exclude "tests/.*" \
		--exclude "build/.*" \
		--print-summary
	@echo "Coverage report generated at $(BUILD_DIR)/coverage/index.html"

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR) 