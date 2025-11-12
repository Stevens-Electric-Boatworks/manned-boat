#include "shore_comms_cpp/data_loggers/MotorDataLogger.hpp"
#include <rclcpp/rclcpp.hpp>

MotorDataLogger::MotorDataLogger(const std::shared_ptr<IDataReceiver<type> > &data, const std::shared_ptr<IDataTransmitter>& data_transmitter)
    : IDataLogger(data, data_transmitter) {
    data->set_callback([this](const type &data_msg) {
        on_data(data_msg);
    });
}

void MotorDataLogger::on_data(const type data) {
    rclcpp::Logger my_logger = rclcpp::get_logger("Motor");
    nlohmann::json j = {
        {"rpm", data.rpm}
    };
    this->data_transmitter->send_data(j);
}
