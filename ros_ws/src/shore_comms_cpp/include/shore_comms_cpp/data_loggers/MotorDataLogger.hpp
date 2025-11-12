#pragma once

#include "boat_data_interfaces/msg/motor_data.hpp"
#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <boat_data_interfaces/msg/detail/can_motor_data__struct.hpp>
#include <rclcpp/rclcpp.hpp>

using type = boat_data_interfaces::msg::CANMotorData;

class MotorDataLogger : public IDataLogger<type> {
public:
    explicit MotorDataLogger(const std::shared_ptr<IDataReceiver<type> > &data, const std::shared_ptr<IDataTransmitter>& transmitter);

    void on_data(type data);
};
