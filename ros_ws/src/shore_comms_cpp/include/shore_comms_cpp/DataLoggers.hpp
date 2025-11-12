#pragma once

#include "boat_data_interfaces/msg/can_bus_status.hpp"
#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"
#include "shore_comms_cpp/data_receivers/ROSDataReceiver.hpp"
#include <memory>
#include <vector>
#include <memory_resource>
#include <rclcpp/rclcpp.hpp>

#include "data_transmitter/WebsocketDataTransmitter.hpp"

class DataLoggers {
public:
    template<typename T, typename L>
    void addDataLogger(std::string topic_name, rclcpp::Node::SharedPtr node) {
        auto data_receiver = std::make_shared<ROSDataReceiver<T> >(topic_name, node);
        auto data_logger = std::make_shared<L>(data_receiver, this->data_transmitter);

        data_loggers_.push_back(data_logger);
    }

    void use_websockets(const rclcpp::Node::SharedPtr& node) {
        this->data_transmitter = std::make_shared<WebsocketDataTransmitter>(node);
    }

private:
    std::pmr::vector<std::shared_ptr<IDataLoggerBase> > data_loggers_;
    std::shared_ptr<IDataTransmitter> data_transmitter;
};
