cmake_minimum_required(VERSION 3.10)

project(lc3vm CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE)
    add_compile_options(-g -O0 --coverage)
    add_link_options(--coverage)
endif()

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
    src/lc3.cpp
    src/memory.cpp
    src/terminal_input.cpp
)

target_link_libraries(test_runner ${GTEST_LIBRARIES} pthread)

find_package(Doxygen REQUIRED)

if (DOXYGEN_FOUND)
    doxygen_add_docs(docs Doxyfile)
endif()

if(ENABLE_COVERAGE)
    find_program(GCOVR_PATH gcovr)
    if(GCOVR_PATH)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage Summary:"
            COMMAND ${GCOVR_PATH} --root ${CMAKE_SOURCE_DIR}
                    --exclude "tests/.*"
                    --exclude "build/.*"
                    --print-summary
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report..."
        )
    endif()
endif()

install(TARGETS lc3vm DESTINATION bin) 