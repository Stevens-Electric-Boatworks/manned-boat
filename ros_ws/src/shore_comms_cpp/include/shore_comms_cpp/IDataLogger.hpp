//
// Created by ishaan on 11/11/25.
//

#pragma once
#include <memory>
#include <rclcpp/node.hpp>
#include <rclcpp/rclcpp.hpp>

#include "IDataReceiver.hpp"

struct IDataLoggerBase {
    virtual ~IDataLoggerBase() = default;
};


template<typename T>
struct IDataLogger : public IDataLoggerBase {
    IDataLogger(std::shared_ptr<IDataReceiver<T> > data_receiver) : data_Receiver(data_receiver) {
    }

    std::shared_ptr<IDataReceiver<T> > data_Receiver;
};