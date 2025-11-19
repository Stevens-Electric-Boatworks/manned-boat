#include "shore_comms_cpp/data_loggers/ROSOutLogger.hpp"
#include <rclcpp/rclcpp.hpp>
#include "shore_comms_cpp/Helpers.hpp"
#include "shore_comms_cpp/IDataTransmitter.h"

ROSOutLogger::ROSOutLogger(const std::shared_ptr<IDataReceiver<type> > &data,
                           const std::shared_ptr<IDataTransmitter> &transmitter, bool replay_mode)
    : IDataLogger(data, transmitter, replay_mode) {
    data->set_callback([this](const type &data_msg) {
        on_data(data_msg);
    });
}

void ROSOutLogger::on_data(const type data) const {
    this->data_transmitter->send_log(LogData{
        get_time_from_msg(data.stamp), data.msg, data.file,
        data.function, data.line, data.level
    });
}