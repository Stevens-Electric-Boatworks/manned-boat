#pragma once

#include "builtin_interfaces/msg/time.hpp"
#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <rclcpp/rclcpp.hpp>

class BoatTimeLogger : public IDataLogger<builtin_interfaces::msg::Time> {
public:
    using type = builtin_interfaces::msg::Time;
    explicit BoatTimeLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(type data) const;
};
