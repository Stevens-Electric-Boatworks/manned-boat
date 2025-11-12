#include "shore_comms_cpp/data_loggers/CANBusLogger.hpp"
#include <rclcpp/rclcpp.hpp>

CANBusLogger::CANBusLogger(std::shared_ptr<IDataReceiver<type>> data)
    : IDataLogger<type>(data)
{
    data->set_callback([this](type data_msg) {
        on_data(data_msg);
    });
}

void CANBusLogger::on_data(type data)
{
    rclcpp::Logger my_logger = rclcpp::get_logger("CANBusLogger");
    RCLCPP_INFO(my_logger, "Got data : %d", data.bus_state);
}
