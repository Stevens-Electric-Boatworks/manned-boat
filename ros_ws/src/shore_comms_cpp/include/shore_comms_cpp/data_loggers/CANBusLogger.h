// MotorDataLogger.h
#pragma once

#include <boat_data_interfaces/msg/can_motor_data.hpp>
#include "boat_data_interfaces/msg/can_motor_data.hpp"
#include "rclcpp/node.hpp"
#include "shore_comms_cpp/LoggableData.h"
#include "nlohmann/json.hpp"

class CANBusLogger : public LoggableData<boat_data_interfaces::msg::CANMotorData> {
public:

    CANBusLogger(const rclcpp::Node::SharedPtr& node, const std::function<void(nlohmann::json)>& add_data)
        : LoggableData("motor_data", node, add_data) {}

    void on_data_receive(boat_data_interfaces::msg::CANMotorData data, const std::function<void (nlohmann::json)> add_data) override {
        add_data({
            {"voltage", data.voltage}
        });
    }
};