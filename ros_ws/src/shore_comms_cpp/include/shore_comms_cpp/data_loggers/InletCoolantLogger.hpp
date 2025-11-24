#pragma once

#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <boat_data_interfaces/msg/inlet_coolant_data.hpp>
#include <rclcpp/rclcpp.hpp>


class InletCoolantLogger : public IDataLogger<boat_data_interfaces::msg::InletCoolantData> {
public:
    using type = boat_data_interfaces::msg::InletCoolantData;
    explicit InletCoolantLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(const type data) const;
};
