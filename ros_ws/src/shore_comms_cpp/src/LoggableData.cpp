//
// Created by ishaan on 11/7/25.
//

#include "include/LoggableData.h"
#include <rclcpp/node.hpp>
#include <string>


template<typename T>
std::string LoggableData<T>::getTopicName() {
    return this->topicName;
}

template<typename T>
rclcpp::Node::SharedPtr LoggableData<T>::getNode() {
    return this->node;
}
