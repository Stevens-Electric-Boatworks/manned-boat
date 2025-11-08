// MotorDataLogger.h
#pragma once

#include <boat_data_interfaces/msg/can_motor_data.hpp>
#include "rclcpp/node.hpp"
#include "../include/LoggableData.h"

class CANBusLogger : public LoggableData<boat_data_interfaces::msg::CANMotorData> {
public:
    using Msg = boat_data_interfaces::msg::CANMotorData;

    CANBusLogger(const rclcpp::Node::SharedPtr& node)
        : LoggableData("motor_data", node) {}

    void on_data_receive(const Msg& data, const AddDataFn& add_data) override {
        nlohmann::json j;
        j["rpm"] = data.rpm;
        j["current"] = data.current;
        j["voltage"] = data.voltage;
        add_data(j);
    }
};

#endif // BUILD_MOTORDATALOGGER_H
