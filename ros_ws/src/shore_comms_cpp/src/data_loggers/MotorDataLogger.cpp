#include "shore_comms_cpp/data_loggers/MotorDataLogger.hpp"
#include <rclcpp/rclcpp.hpp>

MotorDataLogger::MotorDataLogger(const std::shared_ptr<IDataReceiver<type> > &data)
    : IDataLogger(data) {
    data->set_callback([](const type &data_msg) {
        on_data(data_msg);
    });
}

void MotorDataLogger::on_data(const type data) {
    rclcpp::Logger my_logger = rclcpp::get_logger("Motor");
    RCLCPP_INFO(my_logger, "Got data RPM: %d", data.rpm);
}
