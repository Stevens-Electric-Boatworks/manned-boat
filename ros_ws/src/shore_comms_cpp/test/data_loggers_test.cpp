//
// Created by ishaan on 11/17/25.
//

#include <gtest/gtest.h>

#include "shore_comms_cpp_test/mock/FakeDataReceiver.hpp"
#include "shore_comms_cpp_test/mock/FakeDataTransmitter.hpp"
#include "shore_comms_cpp/data_loggers/MotorDataLogger.hpp"
#include "boat_data_interfaces/msg/can_motor_data.hpp"
#include "shore_comms_cpp/Helpers.hpp"
#include "shore_comms_cpp/data_loggers/AlarmsLogger.hpp"
#include "shore_comms_cpp/data_loggers/BoatTimeLogger.hpp"
#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"
#include "shore_comms_cpp/data_loggers/GPSLogger.hpp"
#include "shore_comms_cpp/data_loggers/GPSVTGLogger.hpp"
#include "shore_comms_cpp/data_loggers/InletCoolantLogger.hpp"
#include "shore_comms_cpp/data_loggers/OutletCoolantLogger.hpp"
#include "shore_comms_cpp/data_loggers/ROSOutLogger.hpp"
#include "shore_comms_cpp_test/LoggerTestHelper.hpp"

builtin_interfaces::msg::Time create_test_time_msg() {
    auto msg = builtin_interfaces::msg::Time();
    msg.nanosec = 508282839;
    msg.sec = 2717281;
    return msg;
}

TEST(shore_comms_cpp, can_motor_data_logger_test) {
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
    auto msg = create_test_time_msg();
    time_logger.rec->on_data(msg);
    time_logger.transmitter->assert_has_data({
        {"boat_time", get_time_from_msg(msg)}
    });
}

TEST(shore_comms_cpp, can_bus_status_logger_test) {
    const auto can_state_logger = LoggerTestHelper<boat_data_interfaces::msg::CANBusStatus, CANBusStateLogger>(false);
    auto msg = boat_data_interfaces::msg::CANBusStatus();
    msg.bus_state = boat_data_interfaces::msg::CANBusStatus::ONLINE;
    can_state_logger.rec->on_data(msg);
    can_state_logger.transmitter->assert_has_can_status(boat_data_interfaces::msg::CANBusStatus::ONLINE);
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

TEST(shore_comms_cpp, gps_logger_test) {
    const auto gps_logger = LoggerTestHelper<boat_data_interfaces::msg::GPSData, GPSLogger>(false);
    auto msg = boat_data_interfaces::msg::GPSData();
    msg.lat = -27.2712636;
    msg.lon = 17.283127129;
    gps_logger.rec->on_data(msg);
    gps_logger.transmitter->assert_has_data({
        {"lat", msg.lat},
        {"long", msg.lon}
    });
}

TEST(shore_comms_cpp, inlet_coolant_logger_test) {
    const auto inlet_logger = LoggerTestHelper<boat_data_interfaces::msg::InletCoolantData, InletCoolantLogger>(false);
    auto msg = boat_data_interfaces::msg::InletCoolantData();
    msg.inlet_temp = 27.273;
    inlet_logger.rec->on_data(msg);
    inlet_logger.transmitter->assert_has_data({
        {"inlet_temp", msg.inlet_temp},
    });
}

TEST(shore_comms_cpp, outlet_coolant_logger_test) {
    const auto outlet_logger = LoggerTestHelper<boat_data_interfaces::msg::OutletCoolantData,
        OutletCoolantLogger>(false);
    auto msg = boat_data_interfaces::msg::OutletCoolantData();
    msg.outlet_temp = 83.273;
    outlet_logger.rec->on_data(msg);
    outlet_logger.transmitter->assert_has_data({
        {"outlet_temp", msg.outlet_temp},
    });
}

TEST(shore_comms_cpp, alarms_publisher_test) {
    const auto boat_alarm_logger = LoggerTestHelper<boat_data_interfaces::msg::BoatAlarm, AlarmsLogger>(false);
    auto msg = boat_data_interfaces::msg::BoatAlarm();
    msg.error_code = 5;
    msg.timestamp = create_test_time_msg();
    boat_alarm_logger.rec->on_data(msg);

    auto alarm1 = Alarm();
    alarm1.timestamp = get_time_from_msg(create_test_time_msg());
    alarm1.id = 5;

    boat_alarm_logger.transmitter->assert_has_alarm(alarm1, 1);
}

TEST(shore_comms_cpp, logger_publisher_test) {
    const auto log_logger = LoggerTestHelper<rcl_interfaces::msg::Log, ROSOutLogger>(false);
    auto msg = rcl_interfaces::msg::Log();
    msg.file = "/src/mkail";
    msg.msg = "This is an example log message";
    msg.function = "test_method";
    msg.line = 67;
    msg.name = "TestLogger";
    msg.stamp = create_test_time_msg();
    msg.level = 40;
    log_logger.rec->on_data(msg);

    auto log1 = LogData();
    log1.filename = "/src/mkail";
    log1.msg = "This is an example log message";
    log1.function = "test_method";
    log1.line = 67;
    log1.timestamp = get_time_from_msg(create_test_time_msg());
    log1.level = 40;
    log_logger.transmitter->assert_has_log(log1, 1);
}

