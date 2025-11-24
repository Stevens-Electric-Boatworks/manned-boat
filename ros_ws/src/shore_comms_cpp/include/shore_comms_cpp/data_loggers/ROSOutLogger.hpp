#pragma once

#include "builtin_interfaces/msg/time.hpp"
#include "rcl_interfaces/msg/log.hpp"
#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <rclcpp/rclcpp.hpp>

class ROSOutLogger : public IDataLogger<rcl_interfaces::msg::Log> {
public:
    using type = rcl_interfaces::msg::Log;
    explicit ROSOutLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(type data) const;
};
