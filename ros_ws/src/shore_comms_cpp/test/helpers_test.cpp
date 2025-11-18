//
// Created by ishaan on 11/17/25.
//

#include <gtest/gtest.h>

#include "shore_comms_cpp_test/LoggerTestHelper.hpp"
#include "shore_comms_cpp/Helpers.hpp"
#include "shore_comms_cpp_test/mock/MockDataTransmitter.hpp"

TEST(shore_comms_cpp, helpers_get_time_to_msg_test) {
    auto time = builtin_interfaces::msg::Time();
    time.sec = 1763440019;
    time.nanosec = 373400000;
    constexpr double correct = 1763440019373.3999;
    ASSERT_EQ(get_time_from_msg(time), correct);
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
