#pragma once

#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <boat_data_interfaces/msg/detail/can_motor_data__struct.hpp>
#include <rclcpp/rclcpp.hpp>


class MotorDataLogger : public IDataLogger<boat_data_interfaces::msg::CANMotorData> {
public:
    using type = boat_data_interfaces::msg::CANMotorData;
    explicit MotorDataLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(const type data);
};
