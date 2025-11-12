#pragma once

#include <rclcpp/rclcpp.hpp>
#include <memory>
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <string>

template <typename T>
class ROSDataReceiver : public IDataReceiver<T> {
public:
    explicit ROSDataReceiver(const std::string& topic_name,
                             rclcpp::Node::SharedPtr& node)
        : IDataReceiver<T>(topic_name),
          node_(node)
    {
        // Create ROS subscription
        sub_ = node_->create_subscription<T>(
            topic_name, 10,
            [this](T data_msg) {
                this->on_data(data_msg);
            });
    }

    void on_data(T data_msg) override {
        RCLCPP_INFO(this->node_->get_logger(),
                  "(ROS DataReceiver) Calling callback!");
        if (this->callback) {
            this->callback(data_msg);
        }
    }

private:
    rclcpp::Node::SharedPtr node_;
    typename rclcpp::Subscription<T>::SharedPtr sub_;
};
