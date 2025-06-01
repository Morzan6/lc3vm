#include <gtest/gtest.h>
#include "lc3.hpp"
#include "registers.hpp"
#include "flags.hpp"

TEST(LC3VMTest, BasicInitialization) {
    LC3State vm;
    // PC should be initialized to 0x3000
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3000);
    // Condition code should be FL_ZRO
    EXPECT_EQ(vm.get_register_value(R_COND), FL_ZRO);

    // General purpose registers R0-R7 should be 0
    for (int i = R_R0; i <= R_R7; ++i) {
        EXPECT_EQ(vm.get_register_value(static_cast<Registers>(i)), 0);
    }
} 