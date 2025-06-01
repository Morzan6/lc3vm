#include <gtest/gtest.h>
#include "lc3.hpp"
#include "registers.hpp"
#include "flags.hpp"

TEST(LC3VMTest, BasicInitialization) {
    LC3State vm;
    EXPECT_EQ(vm.get_register_value(R_PC), 0x3000);
    EXPECT_EQ(vm.get_register_value(R_COND), FL_ZRO);

    for (int i = R_R0; i <= R_R7; ++i) {
        EXPECT_EQ(vm.get_register_value(static_cast<Registers>(i)), 0);
    }
} 