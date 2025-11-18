//
// Created by ishaan on 11/17/25.
//

#include <gtest/gtest.h>

#include "shore_comms_cpp_test/mock/MockDataReceiver.hpp"
#include "shore_comms_cpp_test/mock/MockDataTransmitter.hpp"
#include "shore_comms_cpp/data_loggers/MotorDataLogger.hpp"
#include "boat_data_interfaces/msg/can_motor_data.hpp"
#include "shore_comms_cpp/Helpers.hpp"
#include "shore_comms_cpp/data_loggers/BoatTimeLogger.hpp"
#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"
#include "shore_comms_cpp/data_loggers/GPSVTGLogger.hpp"
#include "shore_comms_cpp_test/LoggerTestHelper.hpp"

TEST(shore_comms_cpp, can_bus_state_logger_test) {
    const auto can_logger = LoggerTestHelper<boat_data_interfaces::msg::CANMotorData, MotorDataLogger>(false);

    auto msg = boat_data_interfaces::msg::CANMotorData();
    msg.current = 500;
    msg.motor_temp = 30;
    msg.power = 52;
    msg.rpm = 90;
    msg.throttle_mv = 5;
    msg.voltage = 51;
    msg.torque = 92;
    msg.throttle_percentage = 12;
    can_logger.rec->on_data(msg);
    can_logger.transmitter->assert_has_data({
        {"current", 500},
        {"motor_temp", 30},
        {"power", 52},
        {"rpm", 90},
        {"throttle_mv", 5},
        {"voltage", 51},
        {"torque", 92},
        {"throttle_percentage", 12}
    });

}
TEST(shore_comms_cpp, boat_time_logger_test) {
    const auto time_logger = LoggerTestHelper<builtin_interfaces::msg::Time, BoatTimeLogger>(false);
    auto msg = builtin_interfaces::msg::Time();
    msg.nanosec = 508282839;
    msg.sec = 2717281;
    time_logger.rec->on_data(msg);
    time_logger.transmitter->assert_has_data({
        {"boat_time", get_time_from_msg(msg)}
    });
}

TEST(shore_comms_cpp, can_bus_status_logger_test) {
    const auto can_state_logger = LoggerTestHelper<boat_data_interfaces::msg::CANBusStatus, CANBusStateLogger>(false);
    auto msg = boat_data_interfaces::msg::CANBusStatus();
    msg.bus_state = msg.ONLINE;
    can_state_logger.rec->on_data(msg);
    can_state_logger.transmitter->assert_has_can_status(msg.ONLINE);
}

TEST(shore_comms_cpp, gps_vtg_logger_test) {
    const auto can_state_logger = LoggerTestHelper<boat_data_interfaces::msg::GPSVTGData, GPSVTGLogger>(false);
    auto msg = boat_data_interfaces::msg::GPSVTGData();
    msg.speed = -5;
    msg.true_track = 97;
    can_state_logger.rec->on_data(msg);
    can_state_logger.transmitter->assert_has_data({
    {"speed", -5},
    {"heading", 97}
    });
}
