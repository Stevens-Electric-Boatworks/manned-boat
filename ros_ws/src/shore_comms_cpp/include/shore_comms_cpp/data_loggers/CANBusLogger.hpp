#pragma once

#include "boat_data_interfaces/msg/can_bus_status.hpp"
#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <rclcpp/rclcpp.hpp>

class CANBusLogger : public IDataLogger<boat_data_interfaces::msg::CANBusStatus> {
public:
    using type = boat_data_interfaces::msg::CANBusStatus;

    explicit CANBusLogger(std::shared_ptr<IDataReceiver<type>> data);

    void on_data(type data);
};
