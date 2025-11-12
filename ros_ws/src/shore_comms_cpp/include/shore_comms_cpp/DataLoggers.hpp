#pragma once

#include "boat_data_interfaces/msg/can_bus_status.hpp"
#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/data_loggers/CANBusLogger.hpp"
#include "shore_comms_cpp/data_receivers/ROSDataReceiver.hpp"
#include <memory>
#include <vector>
#include <memory_resource>
#include <rclcpp/rclcpp.hpp>

class DataLoggers {
public:
    void addDataLogger(rclcpp::Node::SharedPtr node);

private:
    std::pmr::vector<std::shared_ptr<IDataLoggerBase>> data_loggers_;
};
