#pragma once

#include <rclcpp/node.hpp>
#include <string>
class ILoggableData {
public:
    virtual rclcpp::Node::SharedPtr getNode();
    virtual std::string getTopicName();
};