#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"
#include <rclcpp/rclcpp.hpp>

CANBusStateLogger::CANBusStateLogger(const std::shared_ptr<IDataReceiver<type> > &data,
                                     const std::shared_ptr<IDataTransmitter> &transmitter, bool replay_mode)
    : IDataLogger(data, transmitter, replay_mode) {
    data->set_callback([this](const type &data_msg) {
        on_data(data_msg);
    });
}

void CANBusStateLogger::on_data(const type data) const {
    rclcpp::Logger my_logger = rclcpp::get_logger("CANBusLogger");
    this->data_transmitter->send_can_bus_state(data.bus_state);
    this->data_transmitter->send_data({
        {"can_bus_data", data.bus_state}
    });
}