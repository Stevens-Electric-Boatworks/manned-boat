#pragma once

#include <rclcpp/rclcpp.hpp>
#include <memory>
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <string>

template<typename T>
class ROSDataReceiver : public IDataReceiver<T> {
public:
    explicit ROSDataReceiver(const std::string &topic_name,
                             rclcpp::Node::SharedPtr &node)
        : IDataReceiver<T>(topic_name),
          node_(node) {
        // Create ROS subscription
        RCLCPP_INFO(node->get_logger(), "Logging topic '%s'", topic_name.c_str());
        sub_ = node_->create_subscription<T>(
            topic_name, 10,
            [this](T data_msg) {
                this->on_data(data_msg);
            });
    }

    void on_data(T data_msg) override {
        if (this->callback) {
            this->callback(data_msg);
        }
    }

private:
    rclcpp::Node::SharedPtr node_;
    typename rclcpp::Subscription<T>::SharedPtr sub_;
};
