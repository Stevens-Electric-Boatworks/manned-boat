#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"
#include <rclcpp/rclcpp.hpp>

CANBusStateLogger::CANBusStateLogger(const std::shared_ptr<IDataReceiver<type> > &data)
    : IDataLogger(data) {
    data->set_callback([](const type &data_msg) {
        on_data(data_msg);
    });
}

void CANBusStateLogger::on_data(const type data) {
    rclcpp::Logger my_logger = rclcpp::get_logger("CANBusLogger");
    RCLCPP_INFO(my_logger, "Got data : %d", data.bus_state);
}
