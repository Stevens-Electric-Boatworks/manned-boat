#include "shore_comms_cpp/data_loggers/GPSVTGLogger.hpp"
#include <rclcpp/rclcpp.hpp>
#include "shore_comms_cpp/Helpers.hpp"

GPSVTGLogger::GPSVTGLogger(const std::shared_ptr<IDataReceiver<type>> &data, const std::shared_ptr<IDataTransmitter>& transmitter, bool replay_mode)
    : IDataLogger(data, transmitter, replay_mode) {
    data->set_callback([this](const type &data_msg) {
        on_data(data_msg);
    });
}

void GPSVTGLogger::on_data(const type data) const {
    this->data_transmitter->send_data({
        {"speed", data.speed},
        {"heading", data.true_track}
    });
}


