//
// Created by ishaan on 11/17/25.
//

#include <gtest/gtest.h>

#include "shore_comms_cpp_test/mock/MockDataReceiver.hpp"
#include "shore_comms_cpp_test/mock/MockDataTransmitter.hpp"
#include "shore_comms_cpp/data_loggers/MotorDataLogger.hpp"
#include "boat_data_interfaces/msg/can_motor_data.hpp"
#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"

TEST(package_name, can_bus_state_logger_test) {
    auto rec = std::make_shared<MockDataReceiver<boat_data_interfaces::msg::CANMotorData>>();
    auto trans = std::make_shared<MockDataTransmitter>();
    auto logger = MotorDataLogger(rec, trans, false);
    auto msg = boat_data_interfaces::msg::CANMotorData();
    msg.current = 500;
    msg.motor_temp = 30;
    msg.power = 52;
    msg.rpm = 90;
    msg.throttle_mv = 5;
    msg.voltage = 51;
    rec->on_data(msg);
    ASSERT_TRUE(trans->has(
        {
        {"voltage", 51}
        }
    ));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
