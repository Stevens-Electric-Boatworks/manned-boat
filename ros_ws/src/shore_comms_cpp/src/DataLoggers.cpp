#include "shore_comms_cpp/DataLoggers.hpp"

void DataLoggers::addDataLogger(rclcpp::Node::SharedPtr node)
{
    using type = boat_data_interfaces::msg::CANBusStatus;

    auto data_receiver = std::make_shared<ROSDataReceiver<type>>("/motors/can_bus_state", node);
    auto data_logger = std::make_shared<CANBusLogger>(data_receiver);

    data_loggers_.push_back(data_logger);
}
