//
// Created by ishaan on 11/13/25.
//

#include "shore_comms_cpp/data_loggers/AlarmsLogger.hpp"
#include "shore_comms_cpp/Helpers.hpp"

AlarmsLogger::AlarmsLogger(const std::shared_ptr<IDataReceiver<type> > &data,
                           const std::shared_ptr<IDataTransmitter> &transmitter, bool replay_mode)
    : IDataLogger(data, transmitter, replay_mode) {
    data->set_callback([this](const type &data_msg) {
        on_data(data_msg);
    });
}

void AlarmsLogger::on_data(const type data) const {
    auto const alarm = Alarm{
        data.error_code, get_time_from_msg(data.timestamp), data.message
    };
    this->data_transmitter->send_alarm(alarm);
}