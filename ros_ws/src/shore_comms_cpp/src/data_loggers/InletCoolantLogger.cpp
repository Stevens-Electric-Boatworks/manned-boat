#include "shore_comms_cpp/data_loggers/InletCoolantLogger.hpp"
#include <rclcpp/rclcpp.hpp>
#include "shore_comms_cpp/Helpers.hpp"

InletCoolantLogger::InletCoolantLogger(const std::shared_ptr<IDataReceiver<type>> &data, const std::shared_ptr<IDataTransmitter>& transmitter, bool replay_mode)
    : IDataLogger(data, transmitter, replay_mode) {
    data->set_callback([this](const type &data_msg) {
        on_data(data_msg);
    });
}

void InletCoolantLogger::on_data(const type data) const {
    this->data_transmitter->send_data({
        {"inlet_temp", data.inlet_temp}
    });
}


