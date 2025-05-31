CC = /usr/bin/g++

CFLAGS = -Wall -Wextra -std=c++14 -Iinclude -O2 -g

BUILD_DIR = build
TEST_DIR = tests
GTEST_FLAGS = -lgtest -lgtest_main -pthread

VM_TEST_SRCS = src/ls3.cpp src/memory.cpp

TEST_MAIN_SRC = src/main.cpp

.PHONY: all clean test docs

all: $(BUILD_DIR)/lc3vm

$(BUILD_DIR)/lc3vm: $(VM_TEST_SRCS) src/main.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(VM_TEST_SRCS) src/main.cpp -o $(BUILD_DIR)/lc3vm

test: $(BUILD_DIR)/test_runner
	$(BUILD_DIR)/test_runner

$(BUILD_DIR)/test_runner: $(TEST_DIR)/test_basic.cpp $(VM_TEST_SRCS) $(TEST_MAIN_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -Dmain=disabled_main $(TEST_DIR)/test_basic.cpp $(VM_TEST_SRCS) $(TEST_MAIN_SRC) -o $(BUILD_DIR)/test_runner $(GTEST_FLAGS) $(LDFLAGS_COVERAGE)

docs:
	@echo "Generating documentation with Doxygen..."
	@doxygen Doxyfile

clean:
	rm -rf $(BUILD_DIR) 