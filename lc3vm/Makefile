CC = /usr/bin/g++
# CFLAGS = -Wall -Wextra -std=c++14 -Iinclude # Original CFLAGS
# Add a way to switch CFLAGS for coverage
ifeq ($(COVERAGE),1)
  CFLAGS = -Wall -Wextra -std=c++14 -Iinclude -fprofile-arcs -ftest-coverage --coverage -O0 -g
  LDFLAGS_COVERAGE = --coverage # For linking step if needed explicitly
else
  CFLAGS = -Wall -Wextra -std=c++14 -Iinclude -O2 -g # Normal build with optimization
endif

BUILD_DIR = build
TEST_DIR = tests
GTEST_FLAGS = -lgtest -lgtest_main -pthread

# Explicitly list VM source files needed for tests (excluding main.cpp with the actual main() )
VM_TEST_SRCS = src/ls3.cpp src/memory.cpp
# Add main.cpp as it's part of the test_runner build due to -Dmain=disabled_main
TEST_MAIN_SRC = src/main.cpp

all: $(BUILD_DIR)/lc3vm

$(BUILD_DIR)/lc3vm: $(VM_TEST_SRCS) src/main.cpp # main.cpp contains the actual main for the VM
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(VM_TEST_SRCS) src/main.cpp -o $(BUILD_DIR)/lc3vm

# Test target that will run tests compiled with or without coverage based on COVERAGE flag
test: $(BUILD_DIR)/test_runner
	$(BUILD_DIR)/test_runner

$(BUILD_DIR)/test_runner: $(TEST_DIR)/test_basic.cpp $(VM_TEST_SRCS) $(TEST_MAIN_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -Dmain=disabled_main $(TEST_DIR)/test_basic.cpp $(VM_TEST_SRCS) $(TEST_MAIN_SRC) -o $(BUILD_DIR)/test_runner $(GTEST_FLAGS) $(LDFLAGS_COVERAGE)


clean:
	rm -rf $(BUILD_DIR) 