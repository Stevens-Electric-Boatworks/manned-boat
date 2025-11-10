//
// Created by ishaan on 11/7/25.
//

#pragma once
#include "ILoggableData.h"
#include <functional>
#include <memory>
#include <rclcpp/node.hpp>
#include <rclcpp/subscription_base.hpp>
#include <string>
#include <nlohmann/json.hpp>

template<typename T>
class LoggableData : public ILoggableData {
public:
    LoggableData(
            const std::string& topic_name,
            const rclcpp::Node::SharedPtr& node,
            const std::function<void(nlohmann::json)>& add_data)
        {
            subscription = node->create_subscription<T>(
                topic_name,
                10,
                [this, &add_data](const typename T::SharedPtr msg) {
                    this->on_data_receive(*msg, add_data);
                }
            );
        }
    virtual void on_data_receive(T data, const std::function<void(nlohmann::json)> add_data);
    std::string getTopicName();
    rclcpp::Node::SharedPtr getNode();

protected:
    rclcpp::Node::SharedPtr node;
    std::shared_ptr<rclcpp::SubscriptionBase> subscription;
    std::string topicName;
};