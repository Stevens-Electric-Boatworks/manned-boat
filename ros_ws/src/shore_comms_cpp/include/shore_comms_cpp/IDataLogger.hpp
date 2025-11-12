//
// Created by ishaan on 11/11/25.
//

#pragma once
#include <memory>
#include <rclcpp/node.hpp>
#include <rclcpp/rclcpp.hpp>

#include "IDataReceiver.hpp"
#include "IDataTransmitter.h"

struct IDataLoggerBase {
    virtual ~IDataLoggerBase() = default;
};


template<typename T>
struct IDataLogger : public IDataLoggerBase {
    IDataLogger(std::shared_ptr<IDataReceiver<T> > data_receiver, const std::shared_ptr<IDataTransmitter>& data_transmitter) : data_receiver(data_receiver), data_transmitter(data_transmitter) {
    }

    std::shared_ptr<IDataReceiver<T> > data_receiver;
    std::shared_ptr<IDataTransmitter> data_transmitter;

};