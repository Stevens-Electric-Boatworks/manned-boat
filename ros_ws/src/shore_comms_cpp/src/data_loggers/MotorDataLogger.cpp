#include "shore_comms_cpp/data_loggers/MotorDataLogger.hpp"
#include <rclcpp/rclcpp.hpp>

MotorDataLogger::MotorDataLogger(const std::shared_ptr<IDataReceiver<type> > &data, const std::shared_ptr<IDataTransmitter>& data_transmitter, bool replay_mode)
    : IDataLogger(data, data_transmitter, replay_mode) {
    data->set_callback([this](const type &data_msg) {
        on_data(data_msg);
    });
}

void MotorDataLogger::on_data(const type data) const {
    nlohmann::json const j = {
        {"rpm", data.rpm},
        {"voltage", data.voltage},
        {"current", data.current},
        {"throttle_mv", data.throttle_mv},
        {"throttle_percentage", data.throttle_percentage},
        {"torque", data.torque},
        {"motor_temp", data.motor_temp},
        {"power", data.power}
    };
    this->data_transmitter->send_data(j);
}
