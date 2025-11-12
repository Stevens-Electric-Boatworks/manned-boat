//
// Created by ishaan on 11/11/25.
//

#pragma once
#include <rclcpp/rclcpp.hpp>

#include "IDataReceiver.hpp"


template<typename T>
struct IDataLogger {
    IDataLogger(rclcpp::Node node, std::string topic_name, IDataReceiver<T> data_receiver){}

    virtual void on_data_recieve(T data) = 0;

    virtual rclcpp::Node::SharedPtr get_node() = 0;
};
