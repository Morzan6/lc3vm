cmake_minimum_required(VERSION 3.10)

project(lc3vm CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include)


add_executable(lc3vm
    src/ls3.cpp
    src/memory.cpp
    src/terminal_input.cpp
    src/main.cpp
)


find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_runner
    tests/test_main.cpp
    tests/test_initialization.cpp
    tests/test_opcode_execution.cpp
    tests/test_disassembly.cpp
    src/ls3.cpp
    src/memory.cpp
    src/terminal_input.cpp
)

target_link_libraries(test_runner ${GTEST_LIBRARIES} pthread)

find_package(Doxygen REQUIRED)

if (DOXYGEN_FOUND)
    doxygen_add_docs(docs Doxyfile)
endif()

install(TARGETS lc3vm DESTINATION bin) 