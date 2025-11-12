//
// Created by ishaan on 11/11/25.
//

#include <rclcpp/node.hpp>

#include "shore_comms_cpp/IDataReceiver.h"

template <typename T>
class ROSDataReceiver : public IDataReceiver<T> {
public:
    explicit ROSDataReceiver(std::string& topic_name, rclcpp::Node::SharedPtr node, IDataLogger<T>& logger) : IDataReceiver<T>(topic_name, logger) {
        this->node = node;
        this->sub = node->create_subscription<T>(topic_name, 10, [this](T data) {
            this->send_data(data);
        });

    }

    void send_data(T data) override {
        this->callback(data);
    }
private:
    rclcpp::Node::SharedPtr node;
    std::shared_ptr<rclcpp::SubscriptionBase> sub;
    std::function<void(T)> callback;
};
